
#ifndef __AXEVENT_TYPE_H__
#define __AXEVENT_TYPE_H__

/**
 * This header is used for defining event types using in NotificationCenter
 */

// The application will come to foreground.
// This message is used for reloading resources before come to foreground on Android.
// This message is posted in main.cpp.
#define EVENT_COME_TO_FOREGROUND    "event_come_to_foreground"

// The application will come to background.
// This message is used for doing something before coming to background, such as save RenderTexture.
// This message is posted in cocos2dx/platform/android/jni/MessageJni.cpp.
#define EVENT_COME_TO_BACKGROUND    "event_come_to_background"

#endif // __AXEVENT_TYPE_H__
