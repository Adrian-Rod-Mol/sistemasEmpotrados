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

	void	SetCurrentScreen(uint8_t screen);

	bool	GetCamState();
	bool	GetTargetState();

	uint8_t GetFPS();
	uint8_t GetMaxTemp();
	uint8_t GetMinTemp();

	void	ChangeBitmapState	(bool state);
	void	SendScreenshot();
	void	ChangeTargetState	(bool state);

	void ChangeFPSValue		(bool operation);
	void ChangeMaxTempValue	(bool operation);
	void ChangeMinTempValue	(bool operation);


protected:
    ModelListener* modelListener;

    uint8_t 	currentScreen;
    bool 		echo;

    float		sensorTemp;
    float		cpuTemp;
    float		screenFrames;

    //this->previousTicks;
    uint8_t		count;

	bool 		camState;
	bool		targetState;

	float		frame[64];
	uint32_t 	frameDelay;

	uint8_t 	fps;
	uint8_t 	maxTemp;
	uint8_t 	minTemp;

};

#endif // MODEL_HPP
