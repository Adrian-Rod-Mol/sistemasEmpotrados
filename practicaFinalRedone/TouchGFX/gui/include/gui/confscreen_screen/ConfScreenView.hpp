#ifndef CONFSCREENVIEW_HPP
#define CONFSCREENVIEW_HPP

#include <gui_generated/confscreen_screen/ConfScreenViewBase.hpp>
#include <gui/confscreen_screen/ConfScreenPresenter.hpp>

class ConfScreenView : public ConfScreenViewBase
{
public:
    ConfScreenView();
    virtual ~ConfScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void PlusFPS();
	void MinusFPS();

	void PlusMaxTemp();
	void MinusMaxTemp();

	void PlusMinTemp();
	void MinusMinTemp();

	void HandleTopBarData	(float cpuTemp, float sensorTemp, float fps);

	void UpdateFPS		(uint8_t fps);
	void UpdateMaxTemp	(uint8_t maxTemp);
	void UpdateMinTemp	(uint8_t minTemp);

protected:
};

#endif // CONFSCREENVIEW_HPP
