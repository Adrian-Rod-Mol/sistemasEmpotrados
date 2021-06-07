#ifndef CONFSCREENPRESENTER_HPP
#define CONFSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ConfScreenView;

class ConfScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ConfScreenPresenter(ConfScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~ConfScreenPresenter() {};

    void SetConfTopBar		(float cpuTemp, float sensorTemp, float fps);

    void ChangeFPSValue		(bool operation);
	void ChangeMaxTempValue	(bool operation);
	void ChangeMinTempValue	(bool operation);

	void SetFPSValue		(uint8_t fps);
	void SetMaxTempValue	(uint8_t maxTemp);
	void SetMinTempValue	(uint8_t minTemp);


private:
    ConfScreenPresenter();

    ConfScreenView& view;
};

#endif // CONFSCREENPRESENTER_HPP
