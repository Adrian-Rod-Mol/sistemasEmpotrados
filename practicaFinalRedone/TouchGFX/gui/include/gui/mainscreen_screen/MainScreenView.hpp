#ifndef MAINSCREENVIEW_HPP
#define MAINSCREENVIEW_HPP

#include <gui_generated/mainscreen_screen/MainScreenViewBase.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

class MainScreenView : public MainScreenViewBase
{
public:
    MainScreenView();
    virtual ~MainScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void ToggleCamBitmap();

	void HandleCamBitmap();
	void HandleCamState	(bool state);

	uint8_t* GetImageBitmap();

protected:
};

#endif // MAINSCREENVIEW_HPP
