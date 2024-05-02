#include "cocoa/Geometry.h"
#include "platform/android/Accelerometer.h"
#include "../EGLView.h"
#include "JniHelper.h"
#include <jni.h>
#include "base/Director.h"

using namespace axolotl;

extern "C" {
    JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxAccelerometer_onSensorChanged(JNIEnv*  env, jobject thiz, jfloat x, jfloat y, jfloat z, jlong timeStamp) {
        Director* pDirector = Director::sharedDirector();
        pDirector->getAccelerometer()->update(x, y, z, timeStamp);
    }    
}
