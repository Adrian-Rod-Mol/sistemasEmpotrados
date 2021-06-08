/*
 * command.h
 *
 *  Created on: 30 abr. 2021
 *      Author: edavidfs
 */

#ifndef APPLICATION_USER_COMMAND_H_
#define APPLICATION_USER_COMMAND_H_

#define CLI_UNKNOWN_CODE    0
#define CLI_HELP_CODE       1
#define CLI_CAM_ON_CODE		2
#define CLI_CAM_OFF_CODE	3
#define CLI_CAM_TEMP_CODE   4
#define CLI_CAM_FRAME_CODE  5
#define CLI_RAW_FRAME_CODE  6
#define CLI_CAM_SRATE_CODE	7
#define CLI_CAM_GRATE_CODE	8
#define CLI_SMAX_TEMP_CODE	9
#define CLI_GMAX_TEMP_CODE	10
#define CLI_SMIN_TEMP_CODE	11
#define CLI_GMIN_TEMP_CODE	12
#define CLI_CPU_TEMP_CODE	13
#define CLI_ECHO_ON_CODE	14
#define CLI_ECHO_OFF_CODE	15

#define APP_UNKNOWN_COMMAND 0
#define APP_HELP_COMMAND    1
#define APP_TURN_ON_CAM		2
#define APP_TURN_OFF_CAM	3
#define APP_SEND_CAM_TEMP   4
#define APP_SEND_CAM_FRAME  5
#define APP_SEND_RAW_FRAME  6
#define APP_SET_CAM_RATE	7
#define APP_GET_CAM_RATE	8
#define APP_SET_MAX_TEMP	9
#define APP_GET_MAX_TEMP	10
#define APP_SET_MIN_TEMP	11
#define APP_GET_MIN_TEMP	12
#define APP_SEND_CPU_TEMP	13
#define APP_ECHO_ON			14
#define APP_ECHO_OFF		15

typedef void *(SFPointer)(void);

typedef struct{
  char *key;                  /**< cadena de caracteres que describe al comando */
  int command_code;           /**< Codigo identificativo del comando */
  int has_parameter;          /**< 1 si posee datos adicionales 0 si no */
  char *description;          /**< Descripción del comando mostrado al solicitar la ayuda */
  SFPointer *service_routine; /**< Rutina de servicio que se ejecuta al recibir el comando */
} CLI_Command;


/** @brief  Mensaje interno de la Aplicación.
 *          Esta estructura representa los comandos que realiza la aplicación.
 *          El Modelo recibe estos mensajes desde la cola de entrada y los procesa
 */
typedef struct{
    int8_t  message_code;   /**< Código del mensaje */
    uint8_t data[20];       /**< Atributos */
}App_Message;


extern CLI_Command cli_command_list[];

void unknown_cli_cmd(void);
void PrintSeparator(void);
void PrintHeader(char section);
void PrintPointer(void);

#endif /* APPLICATION_USER_COMMAND_H_ */
