/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

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

#ifndef __AX_EGLVIEW_WIN32_H__
#define __AX_EGLVIEW_WIN32_H__

#include "StdC.h"
#include "platform/Common.h"
#include "cocoa/Geometry.h"
#include "platform/EGLViewProtocol.h"
#include "glfw/include/GLFW/glfw3.h"

NS_AX_BEGIN

class AX_DLL EGLView : public EGLViewProtocol
{
public:
    EGLView();
    virtual ~EGLView();

    static EGLView* createWithFrameSize(const std::string& viewName, float width, float height);

    virtual bool initWithFrameSize(const std::string& viewName, float width, float height);

    virtual bool windowShouldClose() const;

    virtual bool isOpenGLReady();
    virtual void end();
    virtual void swapBuffers();
    virtual void pollEvents();
    virtual void setFrameSize(float width, float height);
    virtual void setIMEKeyboardState(bool open);

    virtual GLFWwindow* getWindow() const { return _window; }
    virtual GLFWmonitor* getMonitor() const;

    typedef void (*LPFN_ACCELEROMETER_KEYHOOK)(UINT message, WPARAM wParam, LPARAM lParam);
    void setAccelerometerKeyHook(LPFN_ACCELEROMETER_KEYHOOK accelerometerKeyHook);

    virtual void setViewPortInPoints(float x, float y, float w, float h);
    virtual void setScissorInPoints(float x, float y, float w, float h);
    
    static EGLView* sharedEGLView();

    // callbacks

    virtual void onGLFWError(int errorCode, const char* desc);

    virtual void onWindowResized(int width, int height);

    virtual void onMouseButton(int button, int action, int mods);
    virtual void onMouseMove(float x, float y);

    virtual void onKey(int key, int action);

    virtual void onChar(unsigned int codepoint);
protected:
    virtual bool initGL();

    virtual void resize(int width, int height);
    virtual void centerWindow();

    virtual void setupCallbacks();

    GLFWmonitor* _monitor;
    GLFWwindow* _window;

    Point _cursorPosition;
    bool _captured;

    LPFN_ACCELEROMETER_KEYHOOK _accelerometerKeyHook;
};

NS_AX_END

#endif    // end of __AX_EGLVIEW_WIN32_H__
