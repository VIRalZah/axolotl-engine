#ifndef __AXPLATFORMDEFINE_H__
#define __AXPLATFORMDEFINE_H__

#include "android/log.h"

#define AX_DLL

#define AX_NO_MESSAGE_PSEUDOASSERT(cond)                        \
    if (!(cond)) {                                              \
        __android_log_print(ANDROID_LOG_ERROR,                  \
                            "cocos2d-x assert",                 \
                            "%s function:%s line:%d",           \
                            __FILE__, __FUNCTION__, __LINE__);  \
    }

#define AX_MESSAGE_PSEUDOASSERT(cond, msg)                          \
    if (!(cond)) {                                                  \
        __android_log_print(ANDROID_LOG_ERROR,                      \
                            "cocos2d-x assert",                     \
                            "file:%s function:%s line:%d, %s",      \
                            __FILE__, __FUNCTION__, __LINE__, msg); \
    }

#define AX_ASSERT(cond) AX_NO_MESSAGE_PSEUDOASSERT(cond)

#define AX_UNUSED_PARAM(unusedparam) (void)unusedparam

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#endif /* __AXPLATFORMDEFINE_H__*/
