/*
 * board.c
 *
 *  Created on: 7 jun. 2021
 *      Author: Admar
 */

/* Includes */
/***************************************************************/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "main.h"
#include "cmsis_os2.h"
#include <stdio.h>  // Para el printf


/* Defines */
/***************************************************************/
#define BUTTON_FLAG 			(0x01<<2)	/**< @brief Flag que indica que se ha pulsado el botón */


/* Handlers */
/***************************************************************/
extern ADC_HandleTypeDef 	hadc1;
extern osMessageQueueId_t 	cpuTempQueueHandle;
extern osEventFlagsId_t  	userButtonEventHandle;


/* Function Declaration */
/***************************************************************/
HAL_StatusTypeDef ReadTemperatureCPU(uint16_t *rawTemperature);


/* Tasks */
/***************************************************************/
/**
 * @brief Función que implementa la medida de la temperatura de la cpu
 * @param argument: Not used
 * @retval None
 */
void CpuTemp(void *argument)
{
  uint16_t  rawTemperature 	= 0;
  uint32_t  ticks           = 0;
  float     temperature     = 0.0;

  HAL_StatusTypeDef halStatus;

  ticks = osKernelGetTickCount();

  for (;;) {

	halStatus = ReadTemperatureCPU(&rawTemperature);

	if (halStatus == HAL_OK){
	  // V medida = adc value * (vlotaje alimentación/adc total bits)
	  temperature = ((float) rawTemperature)*(3.3/4095);
	  // Temperatura corregida con los datos de la hoja de características del sensor
	  temperature = ((temperature - 0.76)/0.0025) + 25;

	  osMessageQueuePut(cpuTempQueueHandle,(void*) &temperature, 0, 0);
	}

	ticks += 500;
	osDelayUntil(ticks);
  }
}

/* Function */
/***************************************************************/
/**
 * @brief Función que lee la temperatura de laCPU a través del ADC
 * @param rawTemperature: 	Not used
 * @retval opSuccess:		Estado de la operación de la medida
 */

HAL_StatusTypeDef ReadTemperatureCPU(uint16_t *rawTemperature)
{
	HAL_StatusTypeDef initStatus;
	HAL_StatusTypeDef opSuccess;

	initStatus = HAL_ADC_Start(&hadc1);

	if (initStatus == HAL_OK) {
		opSuccess = HAL_ADC_PollForConversion(&hadc1, 100);

		if (opSuccess == HAL_OK) {
			*rawTemperature = HAL_ADC_GetValue(&hadc1);
		}

	} else {
		return initStatus;
	}

	HAL_ADC_Stop(&hadc1);
	return opSuccess;
}


/* Callback */
/***************************************************************/
/**
 * @brief Función que se activa con el boton de usuario, activa el evento de cámara
 * 		  por puerto serie
 *
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (HAL_GPIO_ReadPin(USR_BTN_GPIO_Port, USR_BTN_Pin) == GPIO_PIN_SET) {
		osEventFlagsSet(userButtonEventHandle, BUTTON_FLAG);
	}
}
