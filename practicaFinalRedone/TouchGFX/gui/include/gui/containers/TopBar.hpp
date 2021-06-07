#ifndef TOPBAR_HPP
#define TOPBAR_HPP

#include <gui_generated/containers/TopBarBase.hpp>

class TopBar : public TopBarBase
{
public:
    TopBar();
    virtual ~TopBar() {}

    virtual void initialize();

    void SetCPUTemp		(float temperature);
    void SetSensorTemp	(float temperature);
    void SetScreenFPS	(float fps);

protected:
};

#endif // TOPBAR_HPP
