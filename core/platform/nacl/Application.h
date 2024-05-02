/****************************************************************************
Copyright (c) 2013 The Chromium Authors

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __AXAPLICATION_H__
#define __AXAPLICATION_H__

#include "platform/Common.h"
#include "platform/ApplicationProtocol.h"

NS_AX_BEGIN
class Rect;

class Application : public ApplicationProtocol
{
public:
    Application();
    virtual ~Application();

    /**
     @brief Callback by Director for limit FPS.
     @interval       The time, which expressed in second in second, between current frame and next.
     */
    void setAnimationInterval(double interval);

    /**
     @brief Run the message loop.
     */
    int run();

    /**
     @brief Get current applicaiton instance.
     @return Current application instance pointer.
     */
    static Application* sharedApplication();

    /* override functions */
    virtual ccLanguageType getCurrentLanguage();

    /**
     @brief Get target platform
     */
    virtual TargetPlatform getTargetPlatform();

    static bool isRunning() { return s_running; }
protected:
    long m_nAnimationInterval;  // microseconds

    static bool s_running; // is the application running
    static Application* sm_pSharedApplication;
};

NS_AX_END

#endif /* __AXAPLICATION_H__ */
