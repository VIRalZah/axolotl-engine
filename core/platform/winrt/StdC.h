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

#ifndef __AX_STD_C_H__
#define __AX_STD_C_H__

#include "platform/PlatformMacros.h"
#include <float.h>

// for math.h on win32 platform

#if !defined(_USE_MATH_DEFINES)
    #define _USE_MATH_DEFINES       // make M_PI can be use
#endif

#if !defined(isnan)
    #define isnan   _isnan
#endif

#ifndef snprintf
#define snprintf _snprintf
#endif

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// for MIN MAX and sys/time.h on win32 platform

#define MIN     min
#define MAX     max

#if _MSC_VER >= 1600
    #include <stdint.h>
#else
    #include "./compat/stdint.h"
#endif

//typedef SSIZE_T ssize_t;
// ssize_t was redefined as int in libwebsockets.h.
// Therefore, to avoid conflict, we needs the same definition.
typedef int ssize_t;

#define _WINSOCKAPI_
// Structure timeval has define in winsock.h, include windows.h for it.
#if AX_TARGET_PLATFORM == AX_PLATFORM_WP8
#include <WinSock2.h>
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#include <Windows.h>
//#include <WinSock2.h>

#undef timeval
struct timeval
{
	long tv_sec;		// seconds
	long tv_usec;    // microSeconds
};
#endif // AX_TARGET_PLATFORM == AX_PLATFORM_WP8

struct timezone
{
    int tz_minuteswest;
    int tz_dsttime;
};

int AX_DLL gettimeofday(struct timeval *, struct timezone *);

#endif  // __AX_STD_C_H__

