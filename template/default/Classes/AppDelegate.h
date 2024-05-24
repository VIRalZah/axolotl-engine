#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "includes.h"

class AppDelegate : private Application
{
public:
    AppDelegate(); // constructor
    ~AppDelegate(); // virtual destructor

    bool applicationDidFinishLaunching() override; // init opengl stuff
    void applicationWillTerminate() override;

    void applicationWillResignActive() override;
    void applicationDidBecomeActive() override;

    void applicationDidEnterBackground() override;
    void applicationWillEnterForeground() override;
};

#endif // __APP_DELEGATE_H__