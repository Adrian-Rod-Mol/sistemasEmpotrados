#ifndef MODEL_HPP
#define MODEL_HPP
#include <stdint.h>

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

    /* Puerto Serie */
    /***************************************************************/
    void cli_cam_temp();
	void send_cam_temp();

	void Unknow				(uint8_t index);
	void SendHelp();
	void TurnCam			(bool state);
	void SendCamFrame		(float *frame, bool raw);

	void SetCamRate			(int8_t value);
	void GetCamRate();

	void SerialSetMaxTemp	(int8_t value);
	void SerialGetMaxTemp();

	void SerialSetMinTemp	(int8_t value);
	void SerialGetMinTemp();

	void SendCPUTemp();

	/* Getters y setters */
	/***************************************************************/
	void	SetCurrentScreen(uint8_t screen);

	bool	GetCamState();
	bool	GetTargetState();

	uint8_t GetFPS();
	uint8_t GetMaxTemp();
	uint8_t GetMinTemp();

	/* Gestión de variables de estado */
	/***************************************************************/
	// Main Screen
	void	ChangeBitmapState	(bool state);
	void	SendScreenshot();
	void	ChangeTargetState	(bool state);

	// Conf Screen
	void 	ChangeFPSValue		(bool operation);
	void 	ChangeMaxTempValue	(bool operation);
	void 	ChangeMinTempValue	(bool operation);


protected:
    ModelListener* modelListener;

    uint8_t 	currentScreen;	// Vista que se está mostrando en la pantalla
    bool 		echo;			// Echo de cpu activado o desactivado

    float		sensorTemp;		// Temperatura del termistor de la cámara
    float		cpuTemp;		// Temperatura de la cpu
    float		screenFrames;	// Frames por segundo de la cámara

    uint8_t		count;			// Numero de veces que se ha completado el tick (se resetea cada ~250 ms)
    uint32_t 	value;			// Sumatoria de valores del timer (se resetea cada ~250 ms)

	bool 		camState;		// Variable de control de cámara apagada/encendida
	bool		targetState;	// Variable de control de puntero apagado/encendido

	float		frame[64];		// Frame de la cámara
	uint32_t 	frameDelay;		// Delay que se espera entre medidas del frame

	uint8_t 	fps;			// Frames por segundo de la cámara
	uint8_t 	maxTemp;		// Temperatura superior de saturación de la cámara
	uint8_t 	minTemp;		// Temperatura inferior de saturación de la cámara

};

#endif // MODEL_HPP
