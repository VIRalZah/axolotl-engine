#ifndef __AX_APPLICATION_ANDROID_H__
#define __AX_APPLICATION_ANDROID_H__

#include "platform/Common.h"
#include "platform/ApplicationProtocol.h"

NS_AX_BEGIN

class Rect;

class AX_DLL Application : public ApplicationProtocol
{
public:
    Application();
    virtual ~Application();

    /**
    @brief    Callback by Director to limit FPS.
    @interval       The time, expressed in seconds, between current frame and next. 
    */
    void setAnimationInterval(double interval);

    /**
    @brief    Run the message loop.
    */
    int run();

    /**
    @brief    Get current application instance.
    @return Current application instance pointer.
    */
    static Application* sharedApplication();

    /**
    @brief Get current language config
    @return Current language config
    */
    virtual ccLanguageType getCurrentLanguage();
    
    /**
     @brief Get target platform
     */
    virtual TargetPlatform getTargetPlatform();

protected:
    static Application * sm_pSharedApplication;
};

NS_AX_END

#endif    // __AX_APPLICATION_ANDROID_H__
