#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>  // Para el printf


#define FRAME_FLAG 				(0x01<<1)	/**< @brief Flag que inicia lectura del frame de la cámara */

extern "C" {

  #include "command.h"
  #include "cli.h"

  extern osMessageQueueId_t cpuTempQueueHandle;
  extern osMessageQueueId_t	temp_queueHandle;
  extern TIM_HandleTypeDef 	htim2;

  extern osMessageQueueId_t messageQueueHandle;
  extern osMemoryPoolId_t 	command_MemPool;

  extern osMessageQueueId_t	frameQueueHandle;
  extern osMessageQueueId_t framerateQueueHandle;
  extern osEventFlagsId_t 	readFrameEventHandle;
  extern osMemoryPoolId_t 	frame_MemPool;


}

Model::Model() : modelListener(0)
{
	this->fps			= 10;
	this->maxTemp		= 40;
	this->minTemp		= 20;

	this->camState 		= false;
	this->targetState 	= false;

	this->screenFrames 	= 60.0;
	this->count = 0;


}

/**
 * @brief Metodo que se ejecuta antes de cada refresco de pantalla. En el se observan las colas de mensajes
 *        y en caso de recibir alguno se procesa el mensaje correspondiente.
 *
 */
void Model::tick()
{
  /* Definición de variables */
  /***************************************************************/
  float		sensorTemp 		= 30.5;
  float		cpuTemp 		= 30.5;
  uint32_t 	value			= 0;



  float*	frame 			= NULL;
  float		midTemp			= 22.3;

  osStatus_t  		os_status;
  osStatus_t  		os_msg_status;
  osStatus_t  		os_pool_status;

  App_Message *app_message = NULL;

  /* Top Bar */
  /***************************************************************/
  // Se obtienen los valores


  __HAL_TIM_SET_COUNTER(&htim2, 0);

  os_status = osMessageQueueGet(cpuTempQueueHandle, &cpuTemp, 0, 2);
  if (os_status == osOK) {
	this->cpuTemp = cpuTemp;
  }

  os_status = osMessageQueueGet(temp_queueHandle, &sensorTemp, 0, 2);
  if (os_status == osOK) {
  	this->sensorTemp = sensorTemp;
  }

  /***************************************************************/
  // Se muestran en la pantalla correspondiente
  if (this->currentScreen == 1) {
	  this->modelListener->SetMainTopBar(this->cpuTemp, this->sensorTemp, this->screenFrames);
  } else if (this->currentScreen == 2) {
	  this->modelListener->SetConfTopBar(this->cpuTemp, this->sensorTemp, this->screenFrames);
  }


  /* Frame de la cámara */
  /***************************************************************/
  if (this->camState) {
    os_status = osMessageQueueGet(frameQueueHandle, &frame, 0, 4);

	if (os_status == osOK) {

		for (int i = 0; i < 64; i++) { this->frame[i] = frame[i]; }

		this->modelListener->SetBitmapValues(frame);

		os_pool_status = osMemoryPoolFree(frame_MemPool, frame);
		if (os_pool_status != osOK) printf("Pool Failure Correcto\r\n");
	}

  }

  if (this->targetState && this->camState) {
	  midTemp = ((this->frame[27] + this->frame[28] + this->frame[35] + this->frame[36]) / 4);
	  this->modelListener->SetTargetValue(midTemp);
  }


  /* Mensajes del puerto serie */
  /***************************************************************/
  os_msg_status = osMessageQueueGet(messageQueueHandle, &app_message, 0, 2);
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

  value = __HAL_TIM_GET_COUNTER(&htim2);
  this->count++;
  if (count > 3) {
	  this->screenFrames = (float)1000000 / value;
	  this->count = 0;
  }


}

/***************************************************************/
/*** GESTIÓN DE MENSAJES ***/

/***************************************************************/
void Model::send_cam_temp()
{
	printf("    CAM TEMPERATURE: %3.3f %cC.\r\n", this->sensorTemp, 0xB0);
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

	for (int i = 0; i < 64; i++) { this->frame[i] = frame[i]; }


	printf("    CAM FRAME (%cC):", 0xB0);

	for (int i = 0; i < 8; i++) {
		printf(" \r\n  ");
		for (int j = 0; j < 8; j++) {
			printf("%3.2f  ", this->frame[ (i * 8) + j ]);
		}
	}
	printf(" \r\n");

  }

  os_pool_status = osMemoryPoolFree(frame_MemPool, frame);
  if (os_pool_status != osOK) printf("Pool Failure\r\n");

  if (!(this->camState)) {
	  osEventFlagsClear(readFrameEventHandle, FRAME_FLAG);
  }

}

/***************************************************************/
/*** MÉTODOS DE LA PANTALLA PRINCIPAL ***/

/***************************************************************/
void Model::ChangeBitmapState(bool state)
{
	osStatus_t osCamStatus;
	this->camState = !(this->camState);

	if (this->camState) {

		this->frameDelay = 1000 / this->fps;
		osCamStatus = osMessageQueuePut(framerateQueueHandle, ((void*) &this->frameDelay), 0, 0);

		if (osCamStatus == osOK) {
			osEventFlagsSet(readFrameEventHandle, FRAME_FLAG);
		}

		this->modelListener->SetTargetState(this->targetState);
	} else {
		osEventFlagsClear(readFrameEventHandle, FRAME_FLAG);

		this->modelListener->SetTargetState(false);
	}
}

/***************************************************************/
void Model::SendScreenshot()
{
	if (this->camState) {
		float* frame = NULL;
		printf("\r\n");
		this->SendCamFrame(frame);
		PrintPointer();
	} else {
		printf("Camara desactivada, activela para usar esta funcion.\r\n");
		PrintPointer();
	}
}

/***************************************************************/
void Model::ChangeTargetState(bool state)
{
	if (this->camState) {
		this->targetState = !(this->targetState);
	}
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
/*** GETTERS Y SETTERS ***/

/* Variables comunes */
/***************************************************************/
void Model::SetCurrentScreen(uint8_t screen)
{
	this->currentScreen = screen;
}

/* Pantalla Principal */
/***************************************************************/
bool Model::GetCamState()
{
	return this->camState;
}
/***************************************************************/
bool Model::GetTargetState()
{
	return this->targetState;
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
