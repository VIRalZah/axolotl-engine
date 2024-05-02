#include "text_input_node/IMEDispatcher.h"
#include "base/Director.h"
#include "../Application.h"
#include "platform/FileUtils.h"
#include "EventType.h"
#include "support/NotificationCenter.h"
#include "JniHelper.h"
#include <jni.h>

using namespace axolotl;

extern "C" {
    JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeRender(JNIEnv* env) {
        axolotl::Director::sharedDirector()->mainLoop();
    }

    JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeOnPause() {
        if (Director::sharedDirector()->getOpenGLView()) {
            Application::sharedApplication()->applicationDidEnterBackground();

            NotificationCenter::sharedNotificationCenter()->postNotification(EVENT_COME_TO_BACKGROUND, NULL);
        }
    }

    JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeOnResume() {
        if (Director::sharedDirector()->getOpenGLView()) {
            Application::sharedApplication()->applicationWillEnterForeground();
        }
    }

    JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInsertText(JNIEnv* env, jobject thiz, jstring text) {
        const char* pszText = env->GetStringUTFChars(text, NULL);
        axolotl::IMEDispatcher::sharedDispatcher()->dispatchInsertText(pszText, strlen(pszText));
        env->ReleaseStringUTFChars(text, pszText);
    }

    JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeDeleteBackward(JNIEnv* env, jobject thiz) {
        axolotl::IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
    }

    JNIEXPORT jstring JNICALL Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeGetContentText() {
        JNIEnv * env = 0;

        if (JniHelper::getJavaVM()->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK || ! env) {
            return 0;
        }
        const char * pszText = axolotl::IMEDispatcher::sharedDispatcher()->getContentText();
        return env->NewStringUTF(pszText);
    }
}
