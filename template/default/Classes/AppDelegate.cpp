﻿#include "AppDelegate.h"
#include "HelloWorldScene.h"

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
}

static Size _designResolutionSize = Size(960, 640);
static Size _frameSize = Size(960, 640);

bool AppDelegate::applicationDidFinishLaunching()
{
    auto director = Director::sharedDirector();
    if (!director->getOpenGLView())
    {
        auto eglView = EGLView::createWithFrameSize("Hello Cpp", _frameSize);
        director->setOpenGLView(eglView);

        eglView->setAspectRatio(_frameSize);

        eglView->setDesignResolutionSize(_designResolutionSize.width, _designResolutionSize.height, kResolutionNoBorder);
    }

    director->setDisplayStats(true);
    director->setAnimationInterval(1.0 / 60);

    auto scene = HelloWorld::scene();
    director->runWithScene(scene);

    return true;
}

void AppDelegate::applicationDidEnterBackground()
{
    Director::sharedDirector()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground()
{
    Director::sharedDirector()->startAnimation();
}
