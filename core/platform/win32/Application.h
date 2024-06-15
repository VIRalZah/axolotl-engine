#ifndef __AX_APPLICATION_WIN32_H__
#define __AX_APPLICATION_WIN32_H__

#include "platform/ApplicationProtocol.h"
#include <string>

NS_AX_BEGIN

class AX_DLL Application : public ApplicationProtocol
{
public:
    Application();
    ~Application();

    static Application* sharedApplication();

    virtual int run();

    void setAnimationInterval(double interval) override;
    
    TargetPlatform getTargetPlatform() override;
protected:
    double _animationInterval;
};

NS_AX_END

#endif // __AX_APPLICATION_WIN32_H__
