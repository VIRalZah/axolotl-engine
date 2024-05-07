#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "includes.h"

class AppDelegate : private Application
{
public:
    AppDelegate(); // constructor
    virtual ~AppDelegate(); // virtual destructor

    virtual bool applicationDidFinishLaunching(); // init opengl stuff

    virtual void applicationDidEnterBackground();
    virtual void applicationWillEnterForeground();
};

#endif // __APP_DELEGATE_H__