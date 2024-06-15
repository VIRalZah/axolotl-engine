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

#include "GLViewImpl.h"
#include "base/Director.h"
#include "text_input_node/IMEDispatcher.h"
#include "base/EventDispatcher.h"
#include "Application.h"
#include "support/StringUtils.h"
#include <map>

NS_AX_BEGIN

bool glew_dynamic_binding()
{
    const char* glExtensions = (const char*)glGetString(GL_EXTENSIONS);

    if (glExtensions == nullptr)
    {
        log("OpenGL: glGenFramebuffers is nullptr, try to detect an extension");
        if (strstr(glExtensions, "ARB_framebuffer_object"))
        {
            log("OpenGL: ARB_framebuffer_object is supported");

            glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)wglGetProcAddress("glIsRenderbuffer");
            glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer");
            glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffers");
            glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers");
            glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage");
            glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetRenderbufferParameteriv");
            glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)wglGetProcAddress("glIsFramebuffer");
            glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
            glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
            glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
            glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
            glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)wglGetProcAddress("glFramebufferTexture1D");
            glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
            glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)wglGetProcAddress("glFramebufferTexture3D");
            glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer");
            glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
            glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
        }
        else if (strstr(glExtensions, "EXT_framebuffer_object"))
        {
            log("OpenGL: EXT_framebuffer_object is supported");

            glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)wglGetProcAddress("glIsRenderbufferEXT");
            glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbufferEXT");
            glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
            glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffersEXT");
            glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorageEXT");
            glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
            glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)wglGetProcAddress("glIsFramebufferEXT");
            glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebufferEXT");
            glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
            glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffersEXT");
            glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
            glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
            glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
            glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
            glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
            glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
            glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmapEXT");
        }
        else
        {
            log("OpenGL: No framebuffers extension is supported");
            return false;
        }
    }
    return true;
}

static int _counter = 0;

GLViewImpl::GLViewImpl()
    : _window(nullptr)
    , _captured(false)
{
    _viewName = "AxWin32";

    glfwSetErrorCallback(
        [](int errorCode, const char* desc)
        {
            axMessageBox(StringUtils::format("Error %d\nMore info:\n%s", errorCode, desc).c_str(), "GLFW error");
        }
    );

    if (++_counter == 1)
    {
        AXAssert(glfwInit() == GLFW_TRUE, "GLFW init falied!");

        // set open gl version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    }
}

GLViewImpl::~GLViewImpl()
{
    AXLOG("axolotl: deallocing GLViewImpl %p", this);
    if (--_counter == 0)
    {
        glfwTerminate();
    }
}

GLViewImpl* GLViewImpl::create(const std::string& viewName, float width, float height)
{
    auto ret = new GLViewImpl();
    if (ret && ret->init(viewName, width, height))
    {
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl* GLViewImpl::create(const std::string& viewName, const Size& size)
{
    return GLViewImpl::create(viewName, size.width, size.height);
}

bool GLViewImpl::init(const std::string& viewName, float width, float height)
{
    bool ret = false;
    do
    {
        GLView::setViewName(viewName);
        GLView::setFrameSize(width, height);

        _window = glfwCreateWindow((int)width, (int)height, viewName.c_str(), nullptr, nullptr);

        AX_BREAK_IF(!initGL());

        centerWindow();

        // for callbacks
        glfwSetWindowUserPointer(_window, this);
        setupCallbacks();

        ret = true;
    } while (0);
    return ret;
}

bool GLViewImpl::windowShouldClose() const
{
    if (_window)
    {
        return glfwWindowShouldClose(_window) == GLFW_TRUE;
    }
    return true;
}

bool GLViewImpl::isOpenGLReady()
{
    return _window != nullptr;
}

void GLViewImpl::end()
{
    if (_window)
    {
        glfwDestroyWindow(_window);
        _window = nullptr;
    }
    release();
}

void GLViewImpl::swapBuffers()
{
    if (_window)
    {
        glfwSwapBuffers(_window);
    }
}

void GLViewImpl::pollEvents()
{
    glfwPollEvents();
}

void GLViewImpl::setIMEKeyboardState(bool open)
{
    AX_UNUSED_PARAM(open);
}

void GLViewImpl::setFrameSize(float width, float height)
{
    GLView::setFrameSize(width, height);
    updateFrameSize();
}

void GLViewImpl::setViewPortInPoints(float x , float y , float w , float h)
{
    glViewport((GLint)(x * _scaleX + _viewPortRect.origin.x),
        (GLint)(y * _scaleY + _viewPortRect.origin.y),
        (GLsizei)(w * _scaleX),
        (GLsizei)(h * _scaleY));
}

void GLViewImpl::setScissorInPoints(float x , float y , float w , float h)
{
    glScissor((GLint)(x * _scaleX + _viewPortRect.origin.x),
        (GLint)(y * _scaleY + _viewPortRect.origin.y),
        (GLsizei)(w * _scaleX),
        (GLsizei)(h * _scaleY));
}

void GLViewImpl::windowResized(int width, int height)
{
    GLView::setFrameSize(width, height);
    updateDesignResolutionSize();
}

void GLViewImpl::mouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (GLFW_PRESS == action)
        {
            _captured = true;
            auto& viewPort = getViewPortRect();
            if (viewPort.equals(Rect::ZERO) ||
                viewPort.containsPoint(_cursorPosition))
            {
                int id = 0;
                handleTouchesBegin(1, &id, &_cursorPosition.x, &_cursorPosition.y);
            }
        }
        else if (GLFW_RELEASE == action)
        {
            if (_captured)
            {
                _captured = false;
                int id = 0;
                handleTouchesEnd(1, &id, &_cursorPosition.x, &_cursorPosition.y);
            }
        }
    }
}

void GLViewImpl::cursorMove(float x, float y)
{
    _cursorPosition.setVec2(x, y);

    if (_captured)
    {
        int id = 0;
        handleTouchesMove(1, &id, &_cursorPosition.x, &_cursorPosition.y);
    }
}

void GLViewImpl::key(int key, int scancode, int action, int mods)
{
    AX_UNUSED_PARAM(scancode);
    AX_UNUSED_PARAM(mods);

    auto director = Director::getInstance();
    auto eventDispatcher = director->getEventDispatcher();
    
    if (action != GLFW_RELEASE)
    {
        switch (key)
        {
        case GLFW_KEY_BACKSPACE:
            eventDispatcher->dispatchEvent(&EventIME(IMEEventType::DELETE_BACKWARD));
            break;
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_F1:
        case GLFW_KEY_F2:
            eventDispatcher->dispatchEvent(&EventKeypad(
                key == GLFW_KEY_ESCAPE || key == GLFW_KEY_F1
                ? KeypadEventType::KEY_BACK_CLICKED : KeypadEventType::KEY_MENU_CLICKED));
            break;
        case GLFW_KEY_V:
        {
            if (auto clipboardText = glfwGetClipboardString(_window);
                (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                glfwGetKey(_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
            {
                eventDispatcher->dispatchEvent(&EventIME(IMEEventType::INPUT_TEXT, clipboardText));
            }
        }
            break;
        }
    }

    static std::map<int, KeyCode> keys = {
        { GLFW_KEY_ESCAPE, KeyCode::ESCAPE },
        { GLFW_KEY_Q, KeyCode::Q },
        { GLFW_KEY_W, KeyCode::W },
        { GLFW_KEY_E, KeyCode::E },
        { GLFW_KEY_R, KeyCode::R },
        { GLFW_KEY_T, KeyCode::T },
        { GLFW_KEY_Y, KeyCode::Y },
        { GLFW_KEY_U, KeyCode::U },
        { GLFW_KEY_I, KeyCode::I },
        { GLFW_KEY_O, KeyCode::O },
        { GLFW_KEY_P, KeyCode::P },
        { GLFW_KEY_A, KeyCode::A },
        { GLFW_KEY_S, KeyCode::S },
        { GLFW_KEY_D, KeyCode::D }
    };

    auto find = keys.find(key);
    EventKeyboard event(action == GLFW_PRESS || action == GLFW_REPEAT ? KeyboardEventType::KEY_DOWN : KeyboardEventType::KEY_UP,
        find != keys.end() ? find->second : KeyCode::NONE);
    director->getEventDispatcher()->dispatchEvent(&event);
}

void GLViewImpl::text(unsigned int codepoint)
{
    EventIME event(IMEEventType::INPUT_TEXT, reinterpret_cast<wchar_t*>(&codepoint));
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GLViewImpl::iconified(int iconified)
{
    auto application = Application::sharedApplication();
    if (iconified == GLFW_TRUE)
    {
        application->applicationDidEnterBackground();
    }
    else
    {
        application->applicationWillEnterForeground();
    }
}

void GLViewImpl::focused(int focused)
{
    auto application = Application::sharedApplication();
    if (focused == GLFW_TRUE)
    {
        application->applicationDidBecomeActive();
    }
    else
    {
        application->applicationWillResignActive();
    }
}

bool GLViewImpl::initGL()
{
    if (!_window) return false;

    glfwMakeContextCurrent(_window);

    auto result = glewInit();
    if (result != GLEW_OK)
    {
        axMessageBox((char*)glewGetErrorString(result), "OpenGL error");
        return false;
    }

    if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
    {
        log("Ready for GLSL");
    }
    else
    {
        log("Not totally ready :(");
    }

    if (glewIsSupported("GL_VERSION_2_0"))
    {
        log("Ready for OpenGL 2.0");
    }
    else
    {
        log("OpenGL 2.0 not supported");
    }

    if (!glew_dynamic_binding())
    {
        axMessageBox("No OpenGL framebuffer support. Please upgrade the driver of your video card.", "OpenGL error");
        return false;
    }

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    return true;
}

void GLViewImpl::updateFrameSize()
{
    resize(_screenSize.width, _screenSize.height);
    centerWindow();
}

void GLViewImpl::resize(float width, float height)
{
    if (_window)
    {
        glfwSetWindowSize(_window, width, height);
    }
}

static GLFWmonitor* getWindowMonitor(GLFWwindow* window)
{
    if (window)
    {
        int x, y, width, height;
        glfwGetWindowPos(window, &x, &y);
        glfwGetWindowSize(window, &width, &height);

        int monitorCount;
        auto monitors = glfwGetMonitors(&monitorCount);

        for (int i = 0; i < monitorCount; i++)
        {
            auto monitor = monitors[i];

            int monitorX, monitorY;
            glfwGetMonitorPos(monitor, &monitorX, &monitorY);

            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            if (x + width > monitorX &&
                x < monitorX + mode->width &&
                y + height > monitorY &&
                y < monitorY + mode->height)
            {
                return monitor;
            }
        }
    }
    return nullptr;
}

void GLViewImpl::centerWindow()
{
    if (_window)
    {
        auto monitor = getWindowMonitor(_window);
        auto mode = glfwGetVideoMode(monitor);
        if (mode)
        {
            int x, y;
            glfwGetMonitorPos(monitor, &x, &y);

            int windowWidth, windowHeight;
            glfwGetWindowSize(_window, &windowWidth, &windowHeight);

            x += (mode->width - windowWidth) / 2;
            y += (mode->height - windowHeight) / 2;

            glfwSetWindowPos(_window, x, y);
        }
    }
}

void GLViewImpl::setupCallbacks()
{
    if (_window)
    {
        glfwSetWindowSizeCallback(
            _window,
            [](GLFWwindow* window, int width, int height)
            {
                auto eglView = static_cast<GLViewImpl*>(glfwGetWindowUserPointer(window));
                if (eglView)
                {
                    eglView->windowResized(width, height);
                }
            }
        );
        glfwSetMouseButtonCallback(
            _window,
            [](GLFWwindow* window, int button, int action, int mods)
            {
                auto eglView = static_cast<GLViewImpl*>(glfwGetWindowUserPointer(window));
                if (eglView)
                {
                    eglView->mouseButton(button, action, mods);
                }
            }
        );
        glfwSetCursorPosCallback(
            _window,
            [](GLFWwindow* window, double x, double y)
            {
                auto eglView = static_cast<GLViewImpl*>(glfwGetWindowUserPointer(window));
                if (eglView)
                {
                    eglView->cursorMove((float)x, (float)y);
                }
            }
        );
        glfwSetKeyCallback(
            _window,
            [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                auto eglView = static_cast<GLViewImpl*>(glfwGetWindowUserPointer(window));
                if (eglView)
                {
                    eglView->key(key, scancode, action, mods);
                }
            }
        );
        glfwSetCharCallback(
            _window,
            [](GLFWwindow* window, unsigned int codepoint)
            {
                auto eglView = static_cast<GLViewImpl*>(glfwGetWindowUserPointer(window));
                if (eglView)
                {
                    eglView->text(codepoint);
                }
            }
        );
        glfwSetWindowIconifyCallback(
            _window,
            [](GLFWwindow* window, int iconified)
            {
                auto eglView = static_cast<GLViewImpl*>(glfwGetWindowUserPointer(window));
                if (eglView)
                {
                    eglView->iconified(iconified);
                }
            }
        );
        glfwSetWindowFocusCallback(
            _window,
            [](GLFWwindow* window, int focused)
            {
                auto eglView = static_cast<GLViewImpl*>(glfwGetWindowUserPointer(window));
                if (eglView)
                {
                    eglView->focused(focused);
                }
            }
        );
    }
}

NS_AX_END