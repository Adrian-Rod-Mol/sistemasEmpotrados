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
protected:
};

#endif // CONFSCREENVIEW_HPP
