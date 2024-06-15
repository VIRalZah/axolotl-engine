/****************************************************************************
Copyright (c) 2024 zahann.ru

http://www.zahann.ru

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

#ifndef __AX_GLVIEW_IMPL_WIN32_H__
#define __AX_GLVIEW_IMPL_WIN32_H__

#include "StdC.h"
#include "platform/Common.h"
#include "platform/GLView.h"
#include "GLFW/glfw3.h"

NS_AX_BEGIN

class AX_DLL GLViewImpl : public GLView
{
public:
    GLViewImpl();
    virtual ~GLViewImpl();

    static GLViewImpl* create(const std::string& viewName, float width, float height);
    static GLViewImpl* create(const std::string& viewName, const Size& size);

    virtual bool init(const std::string& viewName, float width, float height);

    bool windowShouldClose() const override;

    bool isOpenGLReady() override;
    void end() override;
    void swapBuffers() override;
    void pollEvents() override;
    void setFrameSize(float width, float height) override;
    void setIMEKeyboardState(bool open) override;

    GLFWwindow* getWindow() const { return _window; }

    void setViewPortInPoints(float x, float y, float w, float h) override;
    void setScissorInPoints(float x, float y, float w, float h) override;
protected:
    bool initGL();

    void updateFrameSize();

    void resize(float width, float height);
    void centerWindow();

    void setupCallbacks();

    void windowResized(int width, int height);

    void mouseButton(int button, int action, int mods);
    void cursorMove(float x, float y);

    void key(int key, int scancode, int action, int mods);

    void text(unsigned int codepoint);

    void iconified(int iconified);

    void focused(int focused);

    GLFWwindow* _window;

    Vec2 _cursorPosition;
    bool _captured;
};

NS_AX_END

#endif // __AX_GLVIEW_IMPL_WIN32_H__