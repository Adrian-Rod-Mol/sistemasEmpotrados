/*
 * parser.c
 *
 *  Created on: 25 abr. 2021
 *      Author: edavidfs
 */

#include "stm32f4xx_hal.h"
#include <cmsis_os2.h>

#include "cli.h"
#include "command.h"

#include <string.h> // Para las comparaciones de los comandos
#include <stdlib.h> // Para el atoi
#include <stdio.h>  // Para el printf


extern UART_HandleTypeDef huart1;

extern osMessageQueueId_t charQueueHandle;
extern osMessageQueueId_t messageQueueHandle;

osStatus_t  os_status;
osStatus_t  os_queue_status;


uint8_t input_buffer[20];
uint8_t index_buffer = 0;
uint8_t input_char;

osMemoryPoolId_t command_MemPool; /**< Pool de memoria para los comandos **/

/**
 * @brief Inicio de la funcionalidad de Command Line Interface.
 *
 */
void cli_init(){

  PrintHeader(1);

  // [x] 1: Crear el Pool de memoria y almacenar el identificador en la variable global command_MemPool
  command_MemPool = osMemoryPoolNew(MEMPOOL_OBJECTS, sizeof(App_Message), NULL);

  // [x] 2: Inicia la lectura caracteres de la UART por interrupción
  HAL_UART_Receive_IT(&huart1, &input_char, sizeof(input_char));
  PrintPointer();

}


/**
 * @brief UART Receive Complete Callback
 */
// [x] 3: Crear la Rutina de servicio de la interrucpión de la UART al recibir un caracter.
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

  // [x] 4: Almacenar el valor recibido
	input_buffer[index_buffer] = input_char;
  // [x] 5: Introducirlo a la Cola de caracteres
	osMessageQueuePut(charQueueHandle, (void *) &input_buffer[index_buffer], 0, 0);
  // [x] 6: Enviar el Caracter por la UART para que el usuario tenga feedback de lo que está escribiendo
	printf("%c", input_buffer[index_buffer]);
	fflush(stdout);
	index_buffer++;
  // [x] 7: Solicitar un nuevo caracter a la UART
	HAL_UART_Receive_IT(&huart1, &input_char, sizeof(input_char));


}


/**
 * @brief Compara la cadena de texto recibida con la lista de comandos existentes
 *        Si la encuentra complenta el mensaje de aplicación, de lo contrario envia un codigo de mensaje desconocido
 *
 * @param temporal_command
 * @param size_command
 * @return
 */
int process_cli_command(uint8_t temporal_command[], App_Message *app_command) {

  int command_index = 0;
  uint8_t commandFind = 0;

  // [x] 13: Recorrer el array de comandos cli_command_list comparando el parámetro key con la cadena recibida,
  while (cli_command_list[command_index].key[0] != 0) {
	  // [x] 14: En caso de encontrar el comando completar el mensaje con el código/identidicador correspondiente
	  //          Si no se encuentra el comando se devuelve el mensaje de Comando desconocido.
	  if ( (memcmp(temporal_command, cli_command_list[command_index].key, strlen(cli_command_list[command_index].key)) == 0)) {
		  app_command->message_code = (int8_t)cli_command_list[command_index].command_code;

		  if (cli_command_list[command_index].has_parameter) {
			  uint8_t count = 0;

			  // Como las str acaban con un \0, al sumar 1 se salta el carácter espacio y pasa al comando
			  int inicio 	= strlen(cli_command_list[command_index].key) + 1;

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

  if (!commandFind) {
	  app_command->message_code = (int8_t)cli_command_list[command_index].command_code;
	  app_command->data[0] = command_index;
  }

  // [x] 16: Opcionalmente se puede devolver el código de comando o de mensaje encontrado si se desea realizar
  //          alguna operación extra.
  return commandFind;
}

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

    // [x] 8: Esperar hasta recibir un caracter de la cola char_queue que almacenaremos en la variable new_char
	  os_status = osMessageQueueGet(charQueueHandle, &new_char, 0, -1);
	// [x] 9: Almacenar el caracter en el array temporal_command hasta recibir el '\n'
	//         es conveniente contar el número de caracteres recibidos en la variable command_size
	  if (os_status == osOK) {
		  temporal_command[command_size] = new_char;
		  command_size++;

	  }

    if (new_char == '\n' && command_size > 1) {
    	index_buffer = 0;
    	command_size = 0;
      // [x] 10: Reservar espacio en el Pool con osMemoryPoolAlloc
    	app_message = osMemoryPoolAlloc(command_MemPool, 10000);

      // [x] 11: Procesar la cadena recibida en una función a la que le pasamos la cadena y el puntero al espacio de
      //          Memoria reservado en el Pool
        process_cli_command(temporal_command, app_message);

      // [x] 12: Enviar el mensaje generado por la funcion anterior a la cola de mensajes para el Modelo.
		os_queue_status = osMessageQueuePut(messageQueueHandle, (void*)&app_message, 0, 0);
		if (os_queue_status != osOK) { printf("Queue Failure\r\n"); }
    }
  }
}

/**
 * @brief Esta función se encarga de transformar los datos de la cola de mensajes a un valor asignable.
 *
 * @param message: puntero a los datos del mensaje
 */

int8_t MessageToValue(uint8_t *message)
{
	uint8_t count = 0;
	int8_t	value = 0;

	for (int i = 0; i < 20; i++) {

		if (message[i] == ';') {
			break;

		} else {
			count++;
		}
	}

	if (count != 0) {

		for (int i = 0; i < count; i++) {
			value += (message[i] - 0x30)*10*(count - 1 - i);
		}
		value += (message[(count - 1)] - 0x30);
	} else { value = -1; }

	return value;
}

