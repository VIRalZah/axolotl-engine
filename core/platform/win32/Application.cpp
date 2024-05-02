#include "Application.h"
#include "EGLView.h"
#include "base/Director.h"
#include <algorithm>
#include "platform/FileUtils.h"
/**
@brief    This function change the PVRFrame show/hide setting in register.
@param  bEnable If true show the PVRFrame window, otherwise hide.
*/
static void PVRFrameEnableControlWindow(bool bEnable);

NS_AX_BEGIN

// sharedApplication pointer
Application * Application::sm_pSharedApplication = 0;

Application::Application()
: m_hInstance(NULL)
, m_hAccelTable(NULL)
{
    m_hInstance    = GetModuleHandle(NULL);
    m_nAnimationInterval.QuadPart = 0;
    AX_ASSERT(! sm_pSharedApplication);
    sm_pSharedApplication = this;
}

Application::~Application()
{
    AX_ASSERT(this == sm_pSharedApplication);
    sm_pSharedApplication = NULL;
}

int Application::run()
{
    PVRFrameEnableControlWindow(false);

    LARGE_INTEGER nFreq;
    LARGE_INTEGER nLast;
    LARGE_INTEGER nNow;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&nLast);

    // Initialize instance and cocos2d.
    if (!applicationDidFinishLaunching())
    {
        return 0;
    }

    EGLView* glView = EGLView::sharedEGLView();
    glView->retain();

    while (!glView->windowShouldClose())
    {
        QueryPerformanceCounter(&nNow);

        if (nNow.QuadPart - nLast.QuadPart > m_nAnimationInterval.QuadPart)
        {
            nLast.QuadPart = nNow.QuadPart;
            Director::sharedDirector()->mainLoop();
            glView->pollEvents();
        }
        else
        {
            Sleep(0);
        }
    }

    glView->release();

    return 0;
}

void Application::setAnimationInterval(double interval)
{
    LARGE_INTEGER nFreq;
    QueryPerformanceFrequency(&nFreq);
    m_nAnimationInterval.QuadPart = (LONGLONG)(interval * nFreq.QuadPart);
}

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
Application* Application::sharedApplication()
{
    AX_ASSERT(sm_pSharedApplication);
    return sm_pSharedApplication;
}

ccLanguageType Application::getCurrentLanguage()
{
    ccLanguageType ret = kLanguageEnglish;

    LCID localeID = GetUserDefaultLCID();
    unsigned short primaryLanguageID = localeID & 0xFF;
    
    switch (primaryLanguageID)
    {
        case LANG_CHINESE:
            ret = kLanguageChinese;
            break;
        case LANG_ENGLISH:
            ret = kLanguageEnglish;
            break;
        case LANG_FRENCH:
            ret = kLanguageFrench;
            break;
        case LANG_ITALIAN:
            ret = kLanguageItalian;
            break;
        case LANG_GERMAN:
            ret = kLanguageGerman;
            break;
        case LANG_SPANISH:
            ret = kLanguageSpanish;
            break;
        case LANG_DUTCH:
            ret = kLanguageDutch;
            break;
        case LANG_RUSSIAN:
            ret = kLanguageRussian;
            break;
        case LANG_KOREAN:
            ret = kLanguageKorean;
            break;
        case LANG_JAPANESE:
            ret = kLanguageJapanese;
            break;
        case LANG_HUNGARIAN:
            ret = kLanguageHungarian;
            break;
        case LANG_PORTUGUESE:
            ret = kLanguagePortuguese;
            break;
        case LANG_ARABIC:
            ret = kLanguageArabic;
            break;
    }

    return ret;
}

TargetPlatform Application::getTargetPlatform()
{
    return kTargetWindows;
}

void Application::setResourceRootPath(const std::string& rootResDir)
{
    m_resourceRootPath = rootResDir;
    std::replace(m_resourceRootPath.begin(), m_resourceRootPath.end(), '\\', '/');
    if (m_resourceRootPath[m_resourceRootPath.length() - 1] != '/')
    {
        m_resourceRootPath += '/';
    }
    FileUtils* pFileUtils = FileUtils::sharedFileUtils();
    std::vector<std::string> searchPaths = pFileUtils->getSearchPaths();
    searchPaths.insert(searchPaths.begin(), m_resourceRootPath);
    pFileUtils->setSearchPaths(searchPaths);
}

const std::string& Application::getResourceRootPath(void)
{
    return m_resourceRootPath;
}

void Application::setStartupScriptFilename(const std::string& startupScriptFile)
{
    m_startupScriptFilename = startupScriptFile;
    std::replace(m_startupScriptFilename.begin(), m_startupScriptFilename.end(), '\\', '/');
}

NS_AX_END

//////////////////////////////////////////////////////////////////////////
// Local function
//////////////////////////////////////////////////////////////////////////
static void PVRFrameEnableControlWindow(bool bEnable)
{
    HKEY hKey = 0;

    // Open PVRFrame control key, if not exist create it.
    if(ERROR_SUCCESS != RegCreateKeyExW(HKEY_CURRENT_USER,
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
    const WCHAR* wszNewData = (bEnable) ? L"NO" : L"YES";
    WCHAR wszOldData[256] = {0};
    DWORD   dwSize = sizeof(wszOldData);
    LSTATUS status = RegQueryValueExW(hKey, wszValue, 0, NULL, (LPBYTE)wszOldData, &dwSize);
    if (ERROR_FILE_NOT_FOUND == status              // the key not exist
        || (ERROR_SUCCESS == status                 // or the hide_gui value is exist
        && 0 != wcscmp(wszNewData, wszOldData)))    // but new data and old data not equal
    {
        dwSize = sizeof(WCHAR) * (wcslen(wszNewData) + 1);
        RegSetValueEx(hKey, wszValue, 0, REG_SZ, (const BYTE *)wszNewData, dwSize);
    }

    RegCloseKey(hKey);
}
