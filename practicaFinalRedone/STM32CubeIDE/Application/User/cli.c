/*
 * cli.c
 *
 *  Created on: 25 abr. 2021
 *      Author: edavidfs
 *
 *  Modified by: Adrian-Rod-Mol
 * 
 *  @brief este archivo contiene las tareas que se encargan de gestionar los valores recibidos por el puerto serie. 
 *         En el se inicializa la interrupción que se ctiva cuando llega un carácter por el puerto serie, se crea el 
 *         pool de memoria en el que se guardan los mensajes que se envían al modelo y se procesan los mensajes llegados 
 *         por el puerto serie.
 */

/* Includes */
/***************************************************************/
#include "stm32f4xx_hal.h"
#include <cmsis_os2.h>

#include "cli.h"
#include "command.h"

#include <string.h> // Para las comparaciones de los comandos
#include <stdlib.h> // Para el atoi
#include <stdio.h>  // Para el printf


/* Handlers del hardware */
/***************************************************************/
extern UART_HandleTypeDef	huart1;

extern osMessageQueueId_t 	charQueueHandle;
extern osMessageQueueId_t 	messageQueueHandle;
extern osMutexId_t 			serialPortMutexHandle;

osStatus_t  os_status;
osStatus_t  os_queue_status;


/* Variables de control */
/***************************************************************/
uint8_t input_buffer[20];
uint8_t index_buffer = 0;
uint8_t input_char;

/** @brief Controla que no se intenten adquirir varias veces el mutex de puerto serie **/
uint8_t mutex = 0;

osMemoryPoolId_t command_MemPool; 	/**< @brief Pool de memoria para los comandos **/


/* Funciones y tareas */
/***************************************************************/
/**
 * @brief Función de inicio de la funcionalidad de Command Line Interface.
 *
 */
void cli_init()
{
  PrintHeader(1);

  // Crear el Pool de memoria y almacenar el identificador en la variable global command_MemPool
  command_MemPool = osMemoryPoolNew(MEMPOOL_OBJECTS, sizeof(App_Message), NULL);

  // Inicia la lectura caracteres de la UART por interrupción
  HAL_UART_Receive_IT(&huart1, &input_char, sizeof(input_char));
  PrintPointer();

}

/***************************************************************/
/**
 * @brief UART Receive Complete Callback
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  input_buffer[index_buffer] = input_char;

  osMessageQueuePut(charQueueHandle, (void *) &input_buffer[index_buffer], 0, 0);

  // Enviar el Caracter por la UART para que el usuario tenga feedback de lo que está escribiendo
  printf("%c", input_buffer[index_buffer]);
  fflush(stdout);
  index_buffer++;

  HAL_UART_Receive_IT(&huart1, &input_char, sizeof(input_char)); // Solicita un nuevo caracter a la UART

}

/***************************************************************/
/**
 * @brief Compara la cadena de texto recibida con la lista de comandos existentes
 *        Si la encuentra complenta el mensaje de aplicación, de lo contrario envia un codigo de mensaje desconocido
 *
 * @param temporal_command: comando que ha llegado por el puerto serie
 * @param app_command:		pool de memoria en el que se almacena el mensaje
 * @return
 */
void process_cli_command(uint8_t temporal_command[], App_Message *app_command)
{
  int command_index = 0;
  uint8_t commandFind = 0;

  // Recorre el array de comandos cli_command_list comparando el parámetro key con la cadena recibida
  while (cli_command_list[command_index].key[0] != 0) {
	  // En caso de encontrar el comando completar el mensaje con el código/identidicador correspondiente
	  // Si no se encuentra el comando se devuelve el mensaje de Comando desconocido.
	  if ( (memcmp(temporal_command, cli_command_list[command_index].key, strlen(cli_command_list[command_index].key)) == 0)) {
		  app_command->message_code = (int8_t)cli_command_list[command_index].command_code;

		  // Si al comando va asociado un parámetro lo almacena
		  if (cli_command_list[command_index].has_parameter) {
			  uint8_t count = 0;

			  // Como las str acaban con un \0, al sumar 1 se salta el carácter espacio y pasa al comando
			  int inicio 	= strlen(cli_command_list[command_index].key) + 1;

			  // Almacena todos los carácteres hasta el '\r'
			  for (int i = inicio; i < 20; i++) {
				  if (temporal_command[i] != '\r') {
					  app_command->data[count] = temporal_command[i];
					  count++;
				  } else { break;}
			  }

			  app_command->data[count] = ';';		// Carácter para indicar el final del dato
		  }

		  commandFind = 1;
		  break;
	  }

    command_index++;
  }

  // Si no se encuentra el comando se envía el código de comando desconocido
  if (!commandFind) {
	  app_command->message_code = (int8_t)cli_command_list[command_index].command_code;
	  app_command->data[0] = command_index;		// Se usa para llamar a la rutina de servicio de comando desconocido
  }

}

/***************************************************************/
/**
 * @brief Esta tarea recibe caracteres por una cola y va formando un comando. Cuando se recibe el caracter que indica una nueva línea
 * se procesa la cadena recibida para crear un mensaje interno de la aplicación y se envia por la cola al Modelo para que procese el
 * mensaje correspondiente.
 *
 * @param argument
 */
void cli_processing_task(void *argument)
{
  uint8_t new_char     = 0;
  uint8_t command_size = 0;
  uint8_t temporal_command[20];


  App_Message *app_message = NULL;

  for(;;)
  {

	os_status = osMessageQueueGet(charQueueHandle, &new_char, 0, osWaitForever);

	if (mutex == 0) {
		// Mutex usado para que no se interrumpa la escritura del mensaje
		osMutexAcquire(serialPortMutexHandle, 10000);
		mutex = 1;
	}

	if (os_status == osOK) {
		temporal_command[command_size] = new_char;
		command_size++;

	}

	if (new_char == '\n' && command_size > 1) {
		index_buffer = 0;
		command_size = 0;

		// Cuando se ha recibido el mensaje completo se procesa y se envía al modelo
		app_message = osMemoryPoolAlloc(command_MemPool, 10000);

		process_cli_command(temporal_command, app_message);

		// Se libera el mutex del puerto serie
		osMutexRelease(serialPortMutexHandle);
		mutex = 0;

		os_queue_status = osMessageQueuePut(messageQueueHandle, (void*)&app_message, 0, 0);
		if (os_queue_status != osOK) { printf("Queue Failure\r\n"); }
	}
  }
}

/***************************************************************/
/**
 * @brief Esta función se encarga de transformar los datos de la cola de mensajes a un valor asignable.
 *
 * @param message: puntero a los datos del mensaje
 */
int8_t MessageToValue(uint8_t *message)
{
  uint8_t 	count = 0;	// Tamaño del mensaje
  int8_t	value = 0;	// Valor del parámetro extraído del mensaje

  // Comprueba el tamaño del mensaje
  for (int i = 0; i < 20; i++) {

    if (message[i] == ';') {  break; } // ';' indica fin del parámetro
    else { count++; }

  }

  if (count != 0) {

	for (int i = 0; i < count; i++) {
	  // Modifica el valor de la variable recibida en función de los carácteres recibidos
	  value += (message[i] - 0x30)*10*(count - 1 - i);

	}

	value += (message[(count - 1)] - 0x30);		// Unidades del mensaje (char ascii - 0x30 = número que le corresponde)

  } else { value = -1; }	// Indica mensaje sin valor

  return value;
}

