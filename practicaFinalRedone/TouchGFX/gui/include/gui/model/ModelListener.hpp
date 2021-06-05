#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    virtual void set_new_temperature(float temperature){}

	virtual void SetFPSValue		(uint8_t fps) 		{}
	virtual void SetMaxTempValue	(uint8_t maxTemp)	{}
	virtual void SetMinTempValue	(uint8_t minTemp)	{}


    void bind(Model* m)
    {
        model = m;
    }
protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
