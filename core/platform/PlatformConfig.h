/****************************************************************************
Copyright (c) 2010 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.

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

#ifndef __AX_PLATFORM_CONFIG_H__
#define __AX_PLATFORM_CONFIG_H__

/**
Config of cocos2d-x project, per target platform.
*/

//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro which CC uses.
#define AX_PLATFORM_UNKNOWN            0
#define AX_PLATFORM_IOS                1
#define AX_PLATFORM_ANDROID            2
#define AX_PLATFORM_WIN32              3
#define AX_PLATFORM_MARMALADE          4
#define AX_PLATFORM_LINUX              5
#define AX_PLATFORM_BADA               6
#define AX_PLATFORM_BLACKBERRY         7
#define AX_PLATFORM_MAC                8
#define AX_PLATFORM_NACL               9
#define AX_PLATFORM_EMSCRIPTEN        10
#define AX_PLATFORM_TIZEN             11
#define AX_PLATFORM_WINRT             12
#define AX_PLATFORM_WP8               13

// Determine target platform by compile environment macro.
#define AX_TARGET_PLATFORM             AX_PLATFORM_UNKNOWN

// mac
#if defined(AX_TARGET_OS_MAC)
#undef  AX_TARGET_PLATFORM
#define AX_TARGET_PLATFORM         AX_PLATFORM_MAC
#endif

// iphone
#if defined(AX_TARGET_OS_IPHONE)
    #undef  AX_TARGET_PLATFORM
    #define AX_TARGET_PLATFORM         AX_PLATFORM_IOS
#endif

// android
#if defined(ANDROID)
    #undef  AX_TARGET_PLATFORM
    #define AX_TARGET_PLATFORM         AX_PLATFORM_ANDROID
#endif

// WinRT (Windows Store App)
#if defined(WINRT) && defined(_WINRT)
    #undef  AX_TARGET_PLATFORM
    #define AX_TARGET_PLATFORM			AX_PLATFORM_WINRT
#endif

// WP8 (Windows Phone 8 App)
#if defined(WP8) && defined(_WP8)
    #undef  AX_TARGET_PLATFORM
    #define AX_TARGET_PLATFORM			AX_PLATFORM_WP8
#endif

// win32
#if defined(WIN32) && defined(_WINDOWS)
    #undef  AX_TARGET_PLATFORM
    #define AX_TARGET_PLATFORM         AX_PLATFORM_WIN32
#endif

// linux
#if defined(LINUX)
    #undef  AX_TARGET_PLATFORM
    #define AX_TARGET_PLATFORM         AX_PLATFORM_LINUX
#endif

// marmalade
#if defined(MARMALADE)
#undef  AX_TARGET_PLATFORM
#define AX_TARGET_PLATFORM         AX_PLATFORM_MARMALADE
#endif

// bada
#if defined(SHP)
#undef  AX_TARGET_PLATFORM
#define AX_TARGET_PLATFORM         AX_PLATFORM_BADA
#endif

// qnx
#if defined(__QNX__)
    #undef  AX_TARGET_PLATFORM
    #define AX_TARGET_PLATFORM     AX_PLATFORM_BLACKBERRY
#endif

// native client
#if defined(__native_client__)
    #undef  AX_TARGET_PLATFORM
    #define AX_TARGET_PLATFORM     AX_PLATFORM_NACL
#endif

// Emscripten
#if defined(EMSCRIPTEN)
    #undef  AX_TARGET_PLATFORM
    #define AX_TARGET_PLATFORM     AX_PLATFORM_EMSCRIPTEN
#endif

// tizen
#if defined(TIZEN)
    #undef  AX_TARGET_PLATFORM
    #define AX_TARGET_PLATFORM     AX_PLATFORM_TIZEN
#endif

//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set platform
#if ! AX_TARGET_PLATFORM
    #error  "Cannot recognize the target platform; are you targeting an unsupported platform?"
#endif 

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
#pragma warning (disable:4127)  
#endif  // AX_PLATFORM_WIN32

#endif  // __AX_PLATFORM_CONFIG_H__

