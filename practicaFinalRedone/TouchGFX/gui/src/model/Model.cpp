#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>  // Para el printf


#define FRAME_FLAG 				(0x01<<1)	/**< @brief Flag que inicia lectura del frame de la cámara */
#define BUTTON_FLAG 			(0x01<<2)	/**< @brief Flag que indica que se ha pulsado el botón */

extern "C" {

  #include "command.h"
  #include "cli.h"

  extern osMessageQueueId_t cpuTempQueueHandle;
  extern osMessageQueueId_t	temp_queueHandle;
  extern TIM_HandleTypeDef 	htim2;

  extern osMutexId_t 		serialPortMutexHandle;
  extern osMessageQueueId_t messageQueueHandle;
  extern osMemoryPoolId_t 	command_MemPool;

  extern osMessageQueueId_t	frameQueueHandle;
  extern osMessageQueueId_t framerateQueueHandle;
  extern osEventFlagsId_t 	readFrameEventHandle;
  extern osMemoryPoolId_t 	frame_MemPool;

  extern osEventFlagsId_t 	userButtonEventHandle;



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

	this->echo = false;


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
  osStatus_t  		osMutexStatus;
  uint32_t  		eventStatus;

  App_Message *app_message = NULL;

  /* Top Bar */
  /***************************************************************/
  // Se obtienen los valores


  __HAL_TIM_SET_COUNTER(&htim2, 0);

  os_status = osMessageQueueGet(cpuTempQueueHandle, &cpuTemp, 0, 2);
  if (os_status == osOK) {
	this->cpuTemp = cpuTemp;

	if (this->echo) {
		osMutexStatus = osMutexAcquire(serialPortMutexHandle, 5);

		if (osMutexStatus == osOK) {

			this->SendCPUTemp();
			PrintPointer();
			osMutexRelease(serialPortMutexHandle);
		}
	}
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
	  case APP_TURN_ON_CAM:
		  this->TurnCam(true);
		  break;
	  case APP_TURN_OFF_CAM:
		  this->TurnCam(false);
		  break;
	  case APP_SEND_CAM_TEMP:
		  this->send_cam_temp();
		  break;
	  case APP_SEND_CAM_FRAME:
		  this->SendCamFrame(frame, false);
		  break;
	  case APP_SEND_RAW_FRAME:
		  this->SendCamFrame(frame, true);
		  break;
	  case APP_SET_CAM_RATE:
		  this->SetCamRate(MessageToValue(app_message->data));
		  break;
	  case APP_GET_CAM_RATE:
		  this->GetCamRate();
		  break;
	  case APP_SET_MAX_TEMP:
		  this->SerialSetMaxTemp(MessageToValue(app_message->data));
		  break;
	  case APP_GET_MAX_TEMP:
		  this->SerialGetMaxTemp();
		  break;
	  case APP_SET_MIN_TEMP:
		  this->SerialSetMinTemp(MessageToValue(app_message->data));
		  break;
	  case APP_GET_MIN_TEMP:
		  this->SerialGetMinTemp();
		  break;
	  case APP_SEND_CPU_TEMP:
		  this->SendCPUTemp();
		  break;
	  case APP_ECHO_ON:
		  this->echo = true;
		  printf("    VALUE SET CORRECTLY\r\n");
		  break;
	  case APP_ECHO_OFF:
		  this->echo = false;
		  printf("    VALUE SET CORRECTLY\r\n");
		  break;
	  default:
		  break;
	  }

	  os_pool_status = osMemoryPoolFree(command_MemPool, app_message);
	  if (os_pool_status != osOK) printf("Pool Failure\r\n");
	  PrintPointer();

  }

  eventStatus = osEventFlagsWait(userButtonEventHandle, BUTTON_FLAG, osFlagsWaitAll, 2);
  if (eventStatus == BUTTON_FLAG) {
	  printf(" \r\n");
	  this->SendCamFrame(frame, false);
	  PrintPointer();
	  eventStatus = 0;
  }

  value = __HAL_TIM_GET_COUNTER(&htim2);
  this->count++;
  if (count > 3) {
	  this->screenFrames = (float)1000000 / value;
	  this->count = 0;
  }
}


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
void Model::TurnCam(bool state)
{
	this->ChangeBitmapState(!state);
	this->modelListener->SetBitmapVisibility(this->camState);
}

/***************************************************************/
void Model::SendCamFrame(float *frame, bool raw)
{

  osStatus_t  os_status;
  osStatus_t  os_pool_status;
  osEventFlagsSet(readFrameEventHandle, FRAME_FLAG);

  os_status = osMessageQueueGet(frameQueueHandle, &frame, 0, 3000);

  if (os_status == osOK) {

	for (int i = 0; i < 64; i++) { this->frame[i] = frame[i]; }

	if (raw) {
		uint16_t rawFrame;
		printf("    RAW FRAME (%cC):", 0xB0);

		for (int i = 0; i < 8; i++) {
			printf(" \r\n  ");
			for (int j = 0; j < 8; j++) {
				rawFrame = (uint16_t)(this->frame[ (i * 8) + j ] / 0.25);
				printf("%5d  ", rawFrame);
			}
		}
		printf(" \r\n");

	} else {
		printf("    CAM FRAME (%cC):", 0xB0);

		for (int i = 0; i < 8; i++) {
			printf(" \r\n  ");
			for (int j = 0; j < 8; j++) {
				printf("%3.2f  ", this->frame[ (i * 8) + j ]);
			}
		}
		printf(" \r\n");
	}
  }

  os_pool_status = osMemoryPoolFree(frame_MemPool, frame);
  if (os_pool_status != osOK) printf("Pool Failure\r\n");

  if (!(this->camState)) {
	  osEventFlagsClear(readFrameEventHandle, FRAME_FLAG);
  }

}

/***************************************************************/
void Model::SetCamRate(int8_t value)
{
	if (value > 0 && value < 11) {
		this->fps = value;
		this->modelListener->SetFPSValue(this->fps);
		printf("    VALUE SET CORRECTLY\r\n");

	} else {
		printf("Invalid value: Please introduce a valid value\r\n");
	}

}

/***************************************************************/
void Model::GetCamRate()
{
	printf("    CAM FRAME RATE: %2d FPS.\r\n", this->fps);
}

/***************************************************************/
void Model::SerialSetMaxTemp(int8_t value)
{
	if (value > (this->minTemp + 5) && value < 60) {
		this->maxTemp = value;
		this->modelListener->SetMaxTempValue(this->maxTemp);
		printf("    VALUE SET CORRECTLY\r\n");

	} else {
		printf("Invalid value: Please introduce a valid value\r\n");
	}

}

/***************************************************************/
void Model::SerialGetMaxTemp()
{
	printf("    IMAGE SCALE MAX TEMP (%cC): %2d .\r\n", 0xB0, this->maxTemp);
}

/***************************************************************/
void Model::SerialSetMinTemp(int8_t value)
{
	if (value > 0 && value < (this->maxTemp - 5)) {
		this->minTemp = value;
		this->modelListener->SetMinTempValue(this->minTemp);
		printf("    VALUE SET CORRECTLY\r\n");

	} else {
		printf("Invalid value: Please introduce a valid value\r\n");
	}

}

/***************************************************************/
void Model::SerialGetMinTemp()
{
	printf("    IMAGE SCALE MIN TEMP (%cC): %2d .\r\n", 0xB0, this->minTemp);
}

/***************************************************************/
void Model::SendCPUTemp()
{
	printf("    CPU TEMPERATURE: %3.3f %cC.\r\n", this->cpuTemp, 0xB0);
}

/***************************************************************/
/*** MÉTODOS DE LA PANTALLA PRINCIPAL ***/

/***************************************************************/
void Model::ChangeBitmapState(bool state)
{
	osStatus_t osCamStatus;
	this->camState = !(state);

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
		this->SendCamFrame(frame, false);
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

	if (this->camState) {
		osStatus_t osEmptyQueue;
		this->frameDelay = 1000 / this->fps;

		osEmptyQueue = osMessageQueueReset(framerateQueueHandle);

		if (osEmptyQueue == osOK) {
			osMessageQueuePut(framerateQueueHandle, ((void*) &this->frameDelay), 0, 0);
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
