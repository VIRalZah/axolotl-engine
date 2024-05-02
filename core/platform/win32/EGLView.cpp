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

#include "EGLView.h"
#include "ccMacros.h"
#include "base/Director.h"
#include "base/Touch.h"
#include "text_input_node/IMEDispatcher.h"
#include "keypad_dispatcher/KeypadDispatcher.h"
#include "base/EventDispatcher.h"
#include "Application.h"
#include "cocoa/String.h"
#include "support/StringUtils.h"

NS_AX_BEGIN

static bool glew_dynamic_binding();

static EGLView* _mainWindow = nullptr;

EGLView::EGLView()
    : _monitor(nullptr)
    , _window(nullptr)
    , _captured(false)
    , _accelerometerKeyHook(nullptr)
{
    _viewName = "AxWin32";

    AXAssert(_mainWindow == nullptr, "Only one instance of EGLView can be created");

    _mainWindow = this;

    glfwSetErrorCallback(
        [](int errorCode, const char* desc)
        {
            _mainWindow->onGLFWError(errorCode, desc);
        }
    );

    AXAssert(glfwInit() == GLFW_TRUE, "GLFW init falied!");
}

EGLView::~EGLView()
{
    AX_ASSERT(_mainWindow == this);
    glfwTerminate();
}

EGLView* EGLView::createWithFrameSize(const std::string& viewName, float width, float height)
{
    auto ret = new EGLView();
    if (ret && ret->initWithFrameSize(viewName, width, height))
    {
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool EGLView::initWithFrameSize(const std::string& viewName, float width, float height)
{
    bool ret = false;
    do
    {
        EGLViewProtocol::setViewName(viewName);
        EGLViewProtocol::setFrameSize(width, height);

        _window = glfwCreateWindow((int)width, (int)height, viewName.c_str(), _monitor, nullptr);

        AX_BREAK_IF(!_window);

        glfwMakeContextCurrent(_window);

        AX_BREAK_IF(!initGL());

        centerWindow();

        setupCallbacks();

        ret = true;
    } while (0);
    return ret;
}

bool EGLView::windowShouldClose() const
{
    if (_window)
    {
        return glfwWindowShouldClose(_window) == GLFW_TRUE;
    }
    return true;
}

void EGLView::setAccelerometerKeyHook(LPFN_ACCELEROMETER_KEYHOOK accelerometerKeyHook)
{
    _accelerometerKeyHook = accelerometerKeyHook;
}

bool EGLView::isOpenGLReady()
{
    return _window != nullptr;
}

void EGLView::end()
{
    if (_window)
    {
        glfwDestroyWindow(_window);
        _window = nullptr;
    }
    release();
}

void EGLView::swapBuffers()
{
    if (_window)
    {
        glfwSwapBuffers(_window);
    }
}

void EGLView::pollEvents()
{
    glfwPollEvents();
}

void EGLView::setIMEKeyboardState(bool open)
{
    AX_UNUSED_PARAM(open);
}

GLFWmonitor* EGLView::getMonitor() const
{
    if (_monitor)
    {
        return _monitor;
    }
    return glfwGetPrimaryMonitor();
}

void EGLView::setFrameSize(float width, float height)
{
    EGLViewProtocol::setFrameSize(width, height);

    resize((int)width, (int)height);
    centerWindow();
}

void EGLView::setViewPortInPoints(float x , float y , float w , float h)
{
    glViewport((GLint)(x * _scaleX + _viewPortRect.origin.x),
        (GLint)(y * _scaleY + _viewPortRect.origin.y),
        (GLsizei)(w * _scaleX),
        (GLsizei)(h * _scaleY));
}

void EGLView::setScissorInPoints(float x , float y , float w , float h)
{
    glScissor((GLint)(x * _scaleX + _viewPortRect.origin.x),
              (GLint)(y * _scaleY + _viewPortRect.origin.y ),
              (GLsizei)(w * _scaleX),
              (GLsizei)(h * _scaleY));
}

EGLView* EGLView::sharedEGLView()
{
    return _mainWindow;
}

void EGLView::onGLFWError(int errorCode, const char* desc)
{
    axMessageBox(String::createWithFormat(
        "Error %d\nMore info:\n%s", errorCode, desc
    )->getCString(), "GLFW error");
}

void EGLView::onWindowResized(int width, int height)
{
    if (width && height)
    {
        auto baseDesignResolution = _designResolutionSize;
        auto resolutionPolicy = _resolutionPolicy;

        EGLViewProtocol::setFrameSize(width, height);

        setDesignResolutionSize(baseDesignResolution.width, baseDesignResolution.height, resolutionPolicy);

        Director::sharedDirector()->setViewport();
    }
}

void EGLView::onMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (GLFW_PRESS == action)
        {
            _captured = true;
            if (getViewPortRect().equals(CCRectZero) ||
                getViewPortRect().containsPoint(_cursorPosition))
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

void EGLView::onMouseMove(float x, float y)
{
    _cursorPosition.setPoint(x, y);

    if (_captured)
    {
        int id = 0;
        handleTouchesMove(1, &id, &_cursorPosition.x, &_cursorPosition.y);
    }
}

static KeyboardEventType keyboardEventTypeWithGLFWAction(int glfwAction);
static KeyCode keyCodeWithGLFWKey(int glfwKey);

void EGLView::onKey(int key, int action)
{
    if (action != GLFW_RELEASE)
    {
        if (key == GLFW_KEY_BACKSPACE)
        {
            IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
        }
    }

    EventKeyboard event(keyboardEventTypeWithGLFWAction(action), keyCodeWithGLFWKey(key));
    Director::sharedDirector()->getEventDispatcher()->dispatchEvent(&event);
}

void EGLView::onChar(unsigned int codepoint)
{
    IMEDispatcher::sharedDispatcher()->dispatchInsertText(StringUtils::codePointToUTF8(codepoint));
}

bool EGLView::initGL()
{
    auto glVersion = (char*)glGetString(GL_VERSION);
    if (atof(glVersion) < 1.5)
    {
        axMessageBox("OpenGL version too old.\nPlease upgrade the driver of your video card.", "OpenGL error");
        return false;
    }

    log("OpenGL %s", glVersion);

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

void EGLView::resize(int width, int height)
{
    if (_window)
    {
        glfwSetWindowSize(_window, width, height);
    }
}

void EGLView::centerWindow()
{
    if (!_window) return;

    auto monitor = getMonitor();
    auto vidMode = glfwGetVideoMode(monitor);
    if (vidMode)
    {
        int x, y;
        glfwGetMonitorPos(getMonitor(), &x, &y);

        x += (int)((vidMode->width - _screenSize.width) / 2);
        y += (int)((vidMode->height - _screenSize.height) / 2);

        glfwSetWindowPos(_window, x, y);
    }
}

void EGLView::setupCallbacks()
{
    if (_window)
    {
        glfwSetWindowSizeCallback(
            _window,
            [](GLFWwindow* window, int width, int height)
            {
                if (_mainWindow->getWindow() == window)
                {
                    _mainWindow->onWindowResized(width, height);
                }
            }
        );
        glfwSetMouseButtonCallback(
            _window,
            [](GLFWwindow* window, int button, int action, int mods)
            {
                if (_mainWindow->getWindow() == window)
                {
                    _mainWindow->onMouseButton(button, action, mods);
                }
            }
        );
        glfwSetCursorPosCallback(
            _window,
            [](GLFWwindow* window, double x, double y)
            {
                if (_mainWindow->getWindow() == window)
                {
                    _mainWindow->onMouseMove((float)x, (float)y);
                }
            }
        );
        glfwSetKeyCallback(
            _window,
            [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                if (_mainWindow->getWindow() == window)
                {
                    _mainWindow->onKey(key, action);
                }
            }
        );
        glfwSetCharCallback(
            _window,
            [](GLFWwindow* window, unsigned int codepoint)
            {
                if (_mainWindow->getWindow() == window)
                {
                    _mainWindow->onChar(codepoint);
                }
            }
        );
    }
}

bool glew_dynamic_binding()
{
    const char* glExtensions = (const char*)glGetString(GL_EXTENSIONS);

    if (glGenFramebuffers == nullptr)
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
        else
            if (strstr(glExtensions, "EXT_framebuffer_object"))
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

Size getMonitorResolution(GLFWmonitor* monitor)
{
    auto vidMode = glfwGetVideoMode(monitor);
    if (vidMode)
    {
        return Size(vidMode->width, vidMode->height);
    }
    return CCSizeZero;
}

static std::map<int, KeyCode> _keys = {
    { GLFW_KEY_ESCAPE, KeyCode::KEY_ESCAPE }
};

KeyboardEventType keyboardEventTypeWithGLFWAction(int glfwAction)
{
    if (glfwAction == GLFW_REPEAT)
    {
        return KeyboardEventType::KEY_REPEAT;
    }
    else if (glfwAction == GLFW_RELEASE)
    {
        return KeyboardEventType::KEY_UP;
    }
    return KeyboardEventType::KEY_DOWN;
}

KeyCode keyCodeWithGLFWKey(int glfwKey)
{
    auto find = _keys.find(glfwKey);
    if (find != _keys.end())
    {
        return find->second;
    }
    return KeyCode::KEY_NONE;
}

NS_AX_END