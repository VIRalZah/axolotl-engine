#include "AppDelegate.h"
#include "HelloWorldScene.h"

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
    auto director = Director::getInstance();
    if (!director->getGLView())
    {
        auto eglView = GLViewImpl::create("Hello Axolotl", 960, 640);
        director->setGLView(eglView);

        eglView->setDesignResolutionSize(960, 640, kResolutionNoBorder);
    }

    director->setAnimationInterval(1.0 / 60);

    auto scene = HelloWorld::scene();
    director->runWithScene(scene);

    return true;
}

void AppDelegate::applicationWillTerminate()
{
    AXLOG(":)");
}

void AppDelegate::applicationWillResignActive()
{
}

void AppDelegate::applicationDidBecomeActive()
{
}

void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();
}
