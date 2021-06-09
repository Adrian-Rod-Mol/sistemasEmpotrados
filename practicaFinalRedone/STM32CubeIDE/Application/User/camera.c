/*
 * camera.c
 *
 *  Created on: 19 abr. 2021
 *      Author: edavidfs
 *
 *  Modified by: Adrian-Rod-Mol
 *  @brief este archivo contiene las tareas que se encargan de leer tanto la temperatura del termistor de la 
 *         cámara como el registro en el que se guardan los píxeles del sensor infrarojo de la cámara.
 */

/* Includes */
/***************************************************************/
#include "camera.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "cmsis_os2.h"
#include <stdio.h>  // Para el printf


/* Registros de la cámara */
/***************************************************************/
#define I2C3_TIMEOUT_MAX		0x3000 		/*<! The value of the maximal timeout for I2C waiting loops */
#define AMG8833_ADDR  			(0x69 << 1)	/**< @brief Dirección I2C del periferico */
#define THERMISTOR_REG 			0x0e		/**< @brief Dirección del Registro del valor del Termistor */
#define FRAME_REG 				0x80		/**< @brief Dirección del Registro del valor del frame de la cámara */


/* Flags */
/***************************************************************/
#define FRAME_FLAG 				(0x01<<1)	/**< @brief Flag que inicia lectura del frame de la cámara */


/* Handlers del hardware */
/***************************************************************/
extern I2C_HandleTypeDef    hi2c3;

extern osMutexId_t 			camAccessMutexHandle;

extern osMessageQueueId_t	temp_queueHandle;

extern osMessageQueueId_t	frameQueueHandle;
extern osMessageQueueId_t 	framerateQueueHandle;
extern osEventFlagsId_t 	readFrameEventHandle;

osMemoryPoolId_t frame_MemPool; /**< Pool de memoria para los comandos **/


/* Declaración de funciones */
/***************************************************************/
HAL_StatusTypeDef read_temp_register();
HAL_StatusTypeDef ReadFrameRegisters();


/***************************************************************/
/** @brief Inicio de la funcionalidad de Camera. */
void camInit()
{
  frame_MemPool = osMemoryPoolNew(MEMPOOL_FRAME, (sizeof(float)*64), NULL);
}


/* Tareas */
/***************************************************************/
/**
 * @brief Tarea que se encarga de leer la temperatura del termistor de la cámara cada 500 ticks
 *
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


/***************************************************************/
/**
 * @brief Tarea que se encarga de leer el frame de la cámara. Esta tarea se ejecuta solo
 * 		  una vez si se borra el flag o continuamente cada 'frameDelay' ticks del os
 *
 * @param argument: Not used
 * @retval None
 */
void FrameTask(void *argument) {

  uint16_t  frameRaw[64] 	= {0};		// Datos leídos del registro de la cámara
  float*    frame 			= NULL;		// Puntero al pool de memoria
  uint32_t  ticks           = 0;		// Ticks del os
  uint32_t	frameDelay		= 100;		// Delay en ticks del os
  uint32_t	recDelay		= 0;		// Delay recivido por la cola

  osStatus_t  osTickStatus;

  HAL_StatusTypeDef hal_status;

  for (;;) {

	// Flag que bloque/desbloquea la tarea para que realice la función de lectura
	// No se resetea automáticamente para que la tarea pueda ejecutarse en bucle
	osEventFlagsWait(readFrameEventHandle, FRAME_FLAG, osFlagsNoClear, osWaitForever);

	ticks = osKernelGetTickCount();

	// Cola por la que se recibe el delay de la tarea
	osTickStatus = osMessageQueueGet(framerateQueueHandle, &recDelay, 0, 100);
	if (osTickStatus == osOK) {
		frameDelay = recDelay;
	}

	// Acceso controlado a los registros de la cámara
	osMutexAcquire(camAccessMutexHandle, 2000);
    hal_status = ReadFrameRegisters(frameRaw);
    osMutexRelease(camAccessMutexHandle);

    frame = osMemoryPoolAlloc(frame_MemPool, 10000);

    if (hal_status == HAL_OK) {
    	for (int i = 0; i < 64; i++) frame[i] = frameRaw[i] * 0.25;		// Se ajustan los valores del frame a grados celcius
        osMessageQueuePut(frameQueueHandle, ((void*) &frame), 0, 0); 	// Se envía el frame al modelo
    }

    ticks += frameDelay;
    osDelayUntil(ticks);
  }
}


/* Funciones de lectura */
/***************************************************************/
/**
 * @brief Lectura del registro del termistor de la cámara AMG8833
 *
 * @param temperature_raw: Datos del registro del Termistor del AMG8833
 * @return Hal Status
 */
HAL_StatusTypeDef read_temp_register(uint16_t *temperature_raw) {

  HAL_StatusTypeDef hal_return;
  uint8_t buffer[2] = { 0, 0 };

  // Lee los registros de 8 bits por el I2C
  hal_return = HAL_I2C_Mem_Read(&hi2c3, (uint16_t)AMG8833_ADDR, (uint16_t) THERMISTOR_REG,
      I2C_MEMADD_SIZE_8BIT, buffer, 2, 100);

  // Los pasa a un entero de 16 bits (little endian)
  if (hal_return == HAL_OK) {
    *temperature_raw = (((buffer[1] & 0x07) << 8) | buffer[0]);

  }

  return hal_return;
}

/***************************************************************/
/**
 * @brief Lectura de los registros del frame de temperatura de la cámara
 *
 * @param frameRaw: Datos de lso píxeles del frame de los registros del sensor AMG8833
 * @return Hal Status
 */
HAL_StatusTypeDef ReadFrameRegisters(uint16_t *frameRaw)
{

  HAL_StatusTypeDef hal_return;
  uint8_t buffer[128] = { 0 };

  // Lee los registros de 8 bits por el I2C
  hal_return = HAL_I2C_Mem_Read(&hi2c3, (uint16_t)AMG8833_ADDR, (uint16_t) FRAME_REG,
      I2C_MEMADD_SIZE_8BIT, buffer, 128, 1000);

  // Los pasa a un buffer de enteros de 16 bits (little endian)
  if (hal_return == HAL_OK) {
	for (int i = 0; i < 64; i++) {
		frameRaw[i] = (uint16_t)(((buffer[(i*2 + 1)] & 0x07) << 8) | buffer[i*2]);
	}
  }

  return hal_return;
}
