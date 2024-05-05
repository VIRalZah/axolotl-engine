#include "AppDelegate.h"
#include "axolotl.h"
#include "EventType.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace axolotl;

extern "C"
{
    
jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JniHelper::setJavaVM(vm);

    return JNI_VERSION_1_4;
}

void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit(JNIEnv*  env, jobject thiz, jint w, jint h)
{
    if (!Director::sharedDirector()->getOpenGLView())
    {
        EGLView *view = EGLView::sharedOpenGLView();
        view->setFrameSize(w, h);

        AppDelegate *pAppDelegate = new AppDelegate();
        Application::sharedApplication()->run();
    }
    else
    {
        ccGLInvalidateStateCache();
        ShaderCache::sharedShaderCache()->reloadDefaultShaders();
        ccDrawInit();
        TextureCache::reloadAllTextures();
        NotificationCenter::sharedNotificationCenter()->postNotification(EVENT_COME_TO_FOREGROUND, NULL);
        Director::sharedDirector()->setGLDefaultValues(); 
    }
}

}
