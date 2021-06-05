/*
 * camera.c
 *
 *  Created on: 19 abr. 2021
 *      Author: edavidfs
 */

#include "camera.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "cmsis_os2.h"
#include <stdio.h>  // Para el printf

#define I2C3_TIMEOUT_MAX		0x3000 		/*<! The value of the maximal timeout for I2C waiting loops */
#define AMG8833_ADDR  			(0x69 << 1)	/**< @brief Dirección I2C del periferico */
#define THERMISTOR_REG 			0x0e		/**< @brief Dirección del Registro del valor del Termistor */
#define FRAME_REG 				0x80		/**< @brief Dirección del Registro del valor del frame de la cámara */
#define FRAME_FLAG 				(0x01<<1)	/**< @brief Flag que inicia lectura del frame de la cámara */

extern I2C_HandleTypeDef    hi2c3;

extern osMutexId_t 			camAccessMutexHandle;

extern osMessageQueueId_t	temp_queueHandle;

extern osMessageQueueId_t	frameQueueHandle;
extern osMessageQueueId_t 	framerateQueueHandle;
extern osEventFlagsId_t 	readFrameEventHandle;

osMemoryPoolId_t frame_MemPool; /**< Pool de memoria para los comandos **/

HAL_StatusTypeDef read_temp_register();
HAL_StatusTypeDef ReadFrameRegisters();

/**
 * @brief Inicio de la funcionalidad de Camera.
 *
 */
void camInit(){

  frame_MemPool = osMemoryPoolNew(MEMPOOL_FRAME, (sizeof(float)*64), NULL);

}
/**
 * @brief Function implementing the temperature thread.
 * @param argument: Not used
 * @retval None
 */
void temperature_task(void *argument) {

  uint16_t  temperature_raw = 0;
  uint32_t  ticks           = 0;
  float     temperature     = 0.0;

  HAL_StatusTypeDef hal_status;

  ticks = osKernelGetTickCount();

  for (;;) {

	osMutexAcquire(camAccessMutexHandle, 2000);
    hal_status = read_temp_register(&temperature_raw);
    osMutexRelease(camAccessMutexHandle);

    if (hal_status == HAL_OK){
      temperature = temperature_raw * 0.0625;

      osMessageQueuePut(temp_queueHandle,(void*) &temperature, 0, 0);
    }
    ticks += 500;
    osDelayUntil(ticks);
  }
}

/**
 * @brief Function implementing the cam frame thread.
 * @param argument: Not used
 * @retval None
 */
void FrameTask(void *argument) {

  uint16_t  frameRaw[64] 	= {0};
  float*    frame 			= NULL;
  uint32_t  ticks           = 0;
  uint32_t	frameDelay		= 100;
  uint32_t	recDelay		= 0;

  osStatus_t  osTickStatus;

  HAL_StatusTypeDef hal_status;

  for (;;) {

	osEventFlagsWait(readFrameEventHandle, FRAME_FLAG, osFlagsNoClear, osWaitForever);

	ticks = osKernelGetTickCount();

	osTickStatus = osMessageQueueGet(framerateQueueHandle, &recDelay, 0, 100);

	if (osTickStatus == osOK) {
		frameDelay = recDelay;
	}

	osMutexAcquire(camAccessMutexHandle, 2000);
    hal_status = ReadFrameRegisters(frameRaw);
    osMutexRelease(camAccessMutexHandle);

    frame = osMemoryPoolAlloc(frame_MemPool, 10000);

    if (hal_status == HAL_OK) {
    	for (int i = 0; i < 64; i++) frame[i] = frameRaw[i] * 0.25;
        osMessageQueuePut(frameQueueHandle, ((void*) &frame), 0, 0);
    }

    ticks += frameDelay;
    osDelayUntil(ticks);




  }
}



/**
 * @brief Lectura de un Registro del AMG8833
 *
 * @param temperature_raw: Datos del registro del Termistor del AMG8833
 * @return Hal Status
 */
HAL_StatusTypeDef read_temp_register(uint16_t *temperature_raw) {

  HAL_StatusTypeDef hal_return;
  uint8_t buffer[2] = { 0, 0 };

  hal_return = HAL_I2C_Mem_Read(&hi2c3, (uint16_t)AMG8833_ADDR, (uint16_t) THERMISTOR_REG,
      I2C_MEMADD_SIZE_8BIT, buffer, 2, 100);

  if (hal_return == HAL_OK) {
    *temperature_raw = (((buffer[1] & 0x07) << 8) | buffer[0]);

  }

  return hal_return;
}

/**
 * @brief Lectura de un Registro del AMG8833
 *
 * @param temperature_raw: Datos del registro del Termistor del AMG8833
 * @return Hal Status
 */
HAL_StatusTypeDef ReadFrameRegisters(uint16_t *frameRaw)
{

  HAL_StatusTypeDef hal_return;
  uint8_t buffer[128] = { 0 };

  hal_return = HAL_I2C_Mem_Read(&hi2c3, (uint16_t)AMG8833_ADDR, (uint16_t) FRAME_REG,
      I2C_MEMADD_SIZE_8BIT, buffer, 128, 1000);

  if (hal_return == HAL_OK) {
	for (int i = 0; i < 64; i++) {
		frameRaw[i] = (uint16_t)(((buffer[(i*2 + 1)] & 0x07) << 8) | buffer[i*2]);
	}
  }

  return hal_return;
}
