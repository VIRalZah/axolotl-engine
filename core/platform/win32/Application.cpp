#include "Application.h"
#include "GLViewImpl.h"
#include "base/Director.h"
#include <algorithm>
#include "platform/FileUtils.h"
#include <thread>

NS_AX_BEGIN

static void PVRFrameEnableControlWindow(bool enable)
{
    HKEY hKey = 0;

    // Open PVRFrame control key, if not exist create it.
    if (ERROR_SUCCESS != RegCreateKeyExW(HKEY_CURRENT_USER,
        L"Software\\Imagination Technologies\\PVRVFRame\\STARTUP\\",
        0,
        0,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        0,
        &hKey,
        NULL))
    {
        return;
    }

    const WCHAR* wszValue = L"hide_gui";
    const WCHAR* wszNewData = (enable) ? L"NO" : L"YES";
    WCHAR wszOldData[256] = { 0 };
    DWORD   dwSize = sizeof(wszOldData);
    LSTATUS status = RegQueryValueExW(hKey, wszValue, 0, NULL, (LPBYTE)wszOldData, &dwSize);
    if (ERROR_FILE_NOT_FOUND == status // the key not exist
        || (ERROR_SUCCESS == status // or the hide_gui value is exist
            && 0 != wcscmp(wszNewData, wszOldData))) // but new data and old data not equal
    {
        dwSize = sizeof(WCHAR) * (wcslen(wszNewData) + 1);
        RegSetValueEx(hKey, wszValue, 0, REG_SZ, (const BYTE*)wszNewData, dwSize);
    }

    RegCloseKey(hKey);
}

static Application* _sharedApplication = nullptr;
Application::Application()
    : _animationInterval(0.0)
{
    AX_ASSERT(!_sharedApplication);
    _sharedApplication = this;
}

Application::~Application()
{
    AX_ASSERT(_sharedApplication == this);
    _sharedApplication = nullptr;
}

Application* Application::sharedApplication()
{
    AX_ASSERT(_sharedApplication);
    return _sharedApplication;
}

int Application::run()
{
    PVRFrameEnableControlWindow(false);

    if (!applicationDidFinishLaunching())
    {
        return 1;
    }

    auto director = Director::getInstance();
    auto glView = director->getGLView();
    glView->retain();

    auto last = glfwGetTime();

    while (!glView->windowShouldClose())
    {
        auto now = glfwGetTime();
        auto interval = now - last;
        if (interval >= _animationInterval)
        {
            last = now;

            glView->pollEvents();
            director->mainLoop();
        }
    }

    if (glView->isOpenGLReady())
    {
        director->end();
        director->mainLoop();
    }

    glView->release();

    return 0;
}

void Application::setAnimationInterval(double interval)
{
    _animationInterval = interval;
}

TargetPlatform Application::getTargetPlatform()
{
    return TargetPlatform::WINDOWS;
}

NS_AX_END