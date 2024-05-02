#include "jni/JniHelper.h"
#include "jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#include "Application.h"
#include "base/Director.h"
#include "EGLView.h"
#include <android/log.h>
#include <jni.h>
#include <cstring>

#define  LOG_TAG    "CCApplication_android Debug"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

NS_AX_BEGIN

// sharedApplication pointer
Application * Application::sm_pSharedApplication = 0;

Application::Application()
{
    AXAssert(! sm_pSharedApplication, "");
    sm_pSharedApplication = this;
}

Application::~Application()
{
    AXAssert(this == sm_pSharedApplication, "");
    sm_pSharedApplication = NULL;
}

int Application::run()
{
    // Initialize instance and cocos2d.
    if (! applicationDidFinishLaunching())
    {
        return 0;
    }
    
    return -1;
}

void Application::setAnimationInterval(double interval)
{
    JniMethodInfo methodInfo;
    if (! JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/lib/Cocos2dxRenderer", "setAnimationInterval", 
        "(D)V"))
    {
        AXLOG("%s %d: error to get methodInfo", __FILE__, __LINE__);
    }
    else
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, interval);
    }
}

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
Application* Application::sharedApplication()
{
    AXAssert(sm_pSharedApplication, "");
    return sm_pSharedApplication;
}

ccLanguageType Application::getCurrentLanguage()
{
    std::string languageName = getCurrentLanguageJNI();
    const char* pLanguageName = languageName.c_str();
    ccLanguageType ret = kLanguageEnglish;

    if (0 == strcmp("zh", pLanguageName))
    {
        ret = kLanguageChinese;
    }
    else if (0 == strcmp("en", pLanguageName))
    {
        ret = kLanguageEnglish;
    }
    else if (0 == strcmp("fr", pLanguageName))
    {
        ret = kLanguageFrench;
    }
    else if (0 == strcmp("it", pLanguageName))
    {
        ret = kLanguageItalian;
    }
    else if (0 == strcmp("de", pLanguageName))
    {
        ret = kLanguageGerman;
    }
    else if (0 == strcmp("es", pLanguageName))
    {
        ret = kLanguageSpanish;
    }
    else if (0 == strcmp("nl", pLanguageName))
    {
        ret = kLanguageDutch;
    }
    else if (0 == strcmp("ru", pLanguageName))
    {
        ret = kLanguageRussian;
    }
    else if (0 == strcmp("ko", pLanguageName))
    {
        ret = kLanguageKorean;
    }
    else if (0 == strcmp("ja", pLanguageName))
    {
        ret = kLanguageJapanese;
    }
    else if (0 == strcmp("hu", pLanguageName))
    {
        ret = kLanguageHungarian;
    }
    else if (0 == strcmp("pt", pLanguageName))
    {
        ret = kLanguagePortuguese;
    }
    else if (0 == strcmp("ar", pLanguageName))
    {
        ret = kLanguageArabic;
    }
    
    return ret;
}

TargetPlatform Application::getTargetPlatform()
{
    return kTargetAndroid;
}

NS_AX_END
