#ifndef MAINSCREENPRESENTER_HPP
#define MAINSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class MainScreenView;

class MainScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    MainScreenPresenter(MainScreenView& v);

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

    virtual ~MainScreenPresenter() {};

    void HandleBitmapVisibility	(bool state);
    void TakeScreenshot			();
    void HandleTargetTemp		(bool state);

    void SetMainTopBar			(float cpuTemp, float sensorTemp, float fps);

    void SetBitmapValues		(float *frame);
	void SetBitmapVisibility	(bool state);
	void SetTargetState			(bool state);
	void SetTargetValue			(float temperature);


private:
    MainScreenPresenter();

    MainScreenView& view;
};

#endif // MAINSCREENPRESENTER_HPP
