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
	void SendHelp();
	void Unknow(uint8_t index);
	void SendCamFrame(float *frame);

	bool	GetCamState();

	uint8_t GetFPS();
	uint8_t GetMaxTemp();
	uint8_t GetMinTemp();

	void	ChangeBitmapState	(bool state);

	void ChangeFPSValue		(bool operation);
	void ChangeMaxTempValue	(bool operation);
	void ChangeMinTempValue	(bool operation);


protected:
    ModelListener* modelListener;

    float temperature;

	bool camState;

	uint8_t fps;
	uint8_t maxTemp;
	uint8_t minTemp;

};

#endif // MODEL_HPP
