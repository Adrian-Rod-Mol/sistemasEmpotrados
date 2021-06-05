/*
 * command.c
 *
 *  Created on: 30 abr. 2021
 *      Author: edavidfs
 */

#include "main.h"
#include "command.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>  // Para el printf
#include <stdlib.h>


#define UNKNOWN_CLI_COMMAND {0,0,0,"Comando desconocido",(void*)unknown_cli_cmd }
#define HELP_CLI_COMMAND {"help", CLI_HELP_CODE, 0,"Comandos del programa", (void*)help_cli_cmd }
#define CAM_TEMP_CLI_COMMAND {"cam temp" , CLI_CAM_TEMP_CODE , 0, "Temperatura de la cámara", (void*)cam_temp_cli_cmd}
#define CAM_FRAME_CLI_COMMAND {"cam frame", CLI_CAM_FRAME_CODE, 0, "Matriz de temperatura de la cámara", (void*)cam_frame_cli_cmd}


extern UART_HandleTypeDef huart1;
void help_cli_cmd(void);
void cam_temp_cli_cmd(void);
void cam_frame_cli_cmd(void);



CLI_Command cli_command_list[] = {
	HELP_CLI_COMMAND,
    CAM_TEMP_CLI_COMMAND,
    CAM_FRAME_CLI_COMMAND,
    UNKNOWN_CLI_COMMAND
};

void help_cli_cmd(void)
{
  printf("     Solicitar comandos del programa\r\n");
}

void cam_temp_cli_cmd(void)
{
  printf(" Solicitar datos Temperatura\r\n");
}

void cam_frame_cli_cmd(void)
{
  printf("Solicitar datos de la Matriz de temperatura\r\n");
}

void unknown_cli_cmd(void)
{
  printf("Comando desconocido\r\n");
}

void PrintSeparator(void)
{
	printf("###########################################################\r\n");
}

void PrintPointer(void)
{
	printf("setr-p4 >> ");
	fflush(stdout);
}

void PrintHeader(char section)
{
	printf("###########################################################\r\n");

	switch(section)
	{
	case 1:
		printf("###         Ejercicio de Practica 4 SETR 20-21          ###\r\n");
		break;
	case 2:
		printf("###         Comandos Aceptados por el programa          ###\r\n");
		break;
	default:
		break;
	}
	printf("###########################################################\r\n");
}
