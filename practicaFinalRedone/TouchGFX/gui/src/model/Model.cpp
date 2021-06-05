#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>  // Para el printf

#define FRAME_FLAG 				(0x01<<1)	/**< @brief Flag que inicia lectura del frame de la cámara */

extern "C" {

  #include "command.h"
  #include "cli.h"
  extern osMessageQueueId_t	temp_queueHandle;

  extern osMessageQueueId_t messageQueueHandle;
  extern osMemoryPoolId_t 	command_MemPool;

  extern osMessageQueueId_t	frameQueueHandle;
  extern osEventFlagsId_t 	readFrameEventHandle;
  extern osMemoryPoolId_t 	frame_MemPool;

}

Model::Model() : modelListener(0)
{
	this->fps		= 10;
	this->maxTemp	= 35;
	this->minTemp	= 20;

	this->camState = false;
}

/**
 * @brief Metodo que se ejecuta antes de cada refresco de pantalla. En el se observan las colas de mensajes
 *        y en caso de recibir alguno se procesa el mensaje correspondiente.
 *
 */
void Model::tick()
{
  float	temperature 	= 0.0;
  float *frame = NULL;

  osStatus_t  os_status;
  osStatus_t  os_msg_status;
  osStatus_t  os_pool_status;

  App_Message *app_message = NULL;

  this->frame[0] = 20;
  for (int i = 1; i < 64; i++) { this->frame[i] = this->frame[i - 1] + 0.25; }
  this->modelListener->SetBitmapValues(this->frame);

  os_status = osMessageQueueGet(temp_queueHandle, &temperature, 0, 10);
  if (os_status == osOK) {
	this->temperature = temperature;
	//this->modelListener->set_new_temperature(temperature);
  }

  os_msg_status = osMessageQueueGet(messageQueueHandle, &app_message, 0, 10);
  if (os_msg_status == osOK) {
	  switch(app_message->message_code)
	  {
	  case APP_UNKNOWN_COMMAND:
		  this->Unknow(app_message->data[0]);
		  break;
	  case APP_HELP_COMMAND:
		  this->SendHelp();
		  break;
	  case APP_SEND_CAM_TEMP:
		  this->send_cam_temp();
		  break;
	  case APP_SEND_CAM_FRAME:
		  this->SendCamFrame(frame);
		  break;
	  default:
		  break;
	  }

	  os_pool_status = osMemoryPoolFree(command_MemPool, app_message);
	  if (os_pool_status != osOK) printf("Pool Failure\r\n");
	  PrintPointer();

  }
}

/***************************************************************/
/*** GESTIÓN DE MENSAJES ***/

/***************************************************************/
void Model::send_cam_temp()
{
	printf("    CAM TEMPERATURE: %3.3f %cC.\r\n", this->temperature, 0xB0);
}

void Model::Unknow(uint8_t index)
{
  PrintPointer();
  cli_command_list[index].service_routine();
}

/***************************************************************/
void Model::SendHelp()
{
  int commandIndex = 0;
  PrintHeader(2);
  while (cli_command_list[commandIndex].key[0] != 0) {
	  size_t len = strlen(cli_command_list[commandIndex].key);
	  printf("    ");
	  for (size_t i = 0; i < len; i++) {
		  printf("%c",cli_command_list[commandIndex].key[i]);
	  }
	  printf("    ");
	  cli_command_list[commandIndex].service_routine();

	  commandIndex++;
  }
  PrintSeparator();
}

/***************************************************************/
void Model::SendCamFrame(float *frame)
{

  osStatus_t  os_status;
  osStatus_t  os_pool_status;
  osEventFlagsSet(readFrameEventHandle, FRAME_FLAG);

  os_status = osMessageQueueGet(frameQueueHandle, &frame, 0, 3000);

  if (os_status == osOK) {
	printf("    CAM FRAME (%cC):", 0xB0);

	for (int i = 0; i < 8; i++) {
		printf(" \r\n  ");
		for (int j = 0; j < 8; j++) {
			printf("%3.2f  ", frame[ (i * 8) + j ]);
		}
	}
	printf(" \r\n");
  }
  os_pool_status = osMemoryPoolFree(frame_MemPool, frame);
  if (os_pool_status != osOK) printf("Pool Failure\r\n");
}

/***************************************************************/
/*** MÉTODOS DE LA PANTALLA PRINCIPAL ***/

/***************************************************************/
void Model::ChangeBitmapState(bool state)
{
	this->camState = !(this->camState);
}

/***************************************************************/
/*** MÉTODOS DE LA PANTALLA DE CONFIGURACIÓN ***/

/***************************************************************/
/**
 * @brief Método que modifica el valor de los fps si se encuentran entre
 * 		  los límites establecidos (máximo y mínimo de la cámara)
 *
 * @param operation - true: incrementa el valor; false: lo decrementa
 */
void Model::ChangeFPSValue(bool operation)
{
	if(operation) {

		if (this->fps < 10) {
			this->fps = this->fps + 1;
		}
	} else {

		if (this->fps > 1) {
			this->fps = this->fps - 1;
		}
	}
}

/***************************************************************/
/**
 * @brief Método que modifica el valor de la máxima temperatura del
 * 		  bitmap si se encuentran entre los límites establecidos
 * 		  (máximo aproximado y mínimo 5 ud. superior a la mínima)
 *
 * @param operation - true: incrementa el valor; false: lo decrementa
 */
void Model::ChangeMaxTempValue(bool operation)
{
	if(operation) {

		if (this->maxTemp < 60) {
			this->maxTemp = this->maxTemp + 1;
		}
	} else {
		if (this->maxTemp > (this->minTemp + 5)) {
			this->maxTemp = this->maxTemp - 1;
		}
	}
}

/***************************************************************/
/**
 * @brief Método que modifica el valor de la mínima temperatura del
 * 		  bitmap si se encuentran entre los límites establecidos
 * 		  (mínimo aproximado y máximo 5 ud. inferior a la máxima)
 *
 * @param operation - true: incrementa el valor; false: lo decrementa
 */
void Model::ChangeMinTempValue(bool operation)
{
	if(operation) {

		if (this->minTemp < (this->maxTemp - 5)) {
			this->minTemp = this->minTemp + 1;
		}
	} else {

		if (this->minTemp > 5) {
			this->minTemp = this->minTemp - 1;
		}
	}
}

/***************************************************************/
/*** GETTERS ***/


/* Pantalla Principal */
/***************************************************************/
bool Model::GetCamState()
{
	return this->camState;
}

/* Pantalla Secundaria */
/***************************************************************/
uint8_t Model::GetFPS()
{
	return this->fps;
}

/***************************************************************/
uint8_t Model::GetMaxTemp()
{
	return this->maxTemp;
}

/***************************************************************/
uint8_t Model::GetMinTemp()
{
	return this->minTemp;
}
