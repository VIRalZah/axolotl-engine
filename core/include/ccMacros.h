/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

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

#ifndef __AXMACROS_H__
#define __AXMACROS_H__

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "platform/Common.h"
#include "StdC.h"

#ifndef AXAssert
#if AXOLOTL_DEBUG > 0
#define AXAssert(cond, msg) do {                              \
      if (!(cond)) {                                          \
        if (strlen(msg)) \
          axolotl::log("Assert failed: %s", msg);           \
        AX_ASSERT(cond);                                      \
      } \
    } while (0)
#else
#define AXAssert(cond, msg) ((void)(cond))
#endif
#endif  // AXAssert

#include "ccConfig.h"

/** @def AX_SWAP
simple macro that swaps 2 variables
*/
#define AX_SWAP(x, y, type)    \
{    type temp = (x);        \
    x = y; y = temp;        \
}


/** @def CCRANDOM_MINUS1_1
 returns a random float between -1 and 1
 */
#define CCRANDOM_MINUS1_1() ((2.0f*((float)rand()/RAND_MAX))-1.0f)

/** @def CCRANDOM_0_1
 returns a random float between 0 and 1
 */
#define CCRANDOM_0_1() ((float)rand()/RAND_MAX)

/** @def AX_DEGREES_TO_RADIANS
 converts degrees to radians
 */
#define AX_DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) * 0.01745329252f) // PI / 180

/** @def AX_RADIANS_TO_DEGREES
 converts radians to degrees
 */
#define AX_RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f) // PI * 180

#define kAXRepeatForever (UINT_MAX -1)

/** @def AX_BLEND_SRC
default gl blend src function. Compatible with premultiplied alpha images.
*/
#define AX_BLEND_SRC GL_ONE
#define AX_BLEND_DST GL_ONE_MINUS_SRC_ALPHA


/** @def AX_NODE_DRAW_SETUP
 Helpful macro that setups the GL server state, the correct GL program and sets the Model View Projection matrix
 @since v2.0
 */
#define AX_NODE_DRAW_SETUP() \
do { \
    ccGLEnable(m_eGLServerState); \
    AXAssert(getShaderProgram(), "No shader program set for this node"); \
    { \
        getShaderProgram()->use(); \
        getShaderProgram()->setUniformsForBuiltins(); \
    } \
} while(0)


 /** @def AX_DIRECTOR_END
  Stops and removes the director from memory.
  Removes the CCGLView from its parent

  @since v0.99.4
  */
#define AX_DIRECTOR_END()                                        \
do {                                                            \
    Director *__director = Director::sharedDirector();        \
    __director->end();                                            \
} while(0)

/** @def AX_CONTENT_SCALE_FACTOR
On Mac it returns 1;
On iPhone it returns 2 if RetinaDisplay is On. Otherwise it returns 1
*/
#define AX_CONTENT_SCALE_FACTOR() Director::sharedDirector()->getContentScaleFactor()

/****************************/
/** RETINA DISPLAY ENABLED **/
/****************************/

/** @def AX_RECT_PIXELS_TO_POINTS
 Converts a rect in pixels to points
 */
#define AX_RECT_PIXELS_TO_POINTS(__rect_in_pixels__)                                                                        \
    Rect( (__rect_in_pixels__).origin.x / AX_CONTENT_SCALE_FACTOR(), (__rect_in_pixels__).origin.y / AX_CONTENT_SCALE_FACTOR(),    \
            (__rect_in_pixels__).size.width / AX_CONTENT_SCALE_FACTOR(), (__rect_in_pixels__).size.height / AX_CONTENT_SCALE_FACTOR() )

/** @def AX_RECT_POINTS_TO_PIXELS
 Converts a rect in points to pixels
 */
#define AX_RECT_POINTS_TO_PIXELS(__rect_in_points_points__)                                                                        \
    Rect( (__rect_in_points_points__).origin.x * AX_CONTENT_SCALE_FACTOR(), (__rect_in_points_points__).origin.y * AX_CONTENT_SCALE_FACTOR(),    \
            (__rect_in_points_points__).size.width * AX_CONTENT_SCALE_FACTOR(), (__rect_in_points_points__).size.height * AX_CONTENT_SCALE_FACTOR() )

/** @def AX_POINT_PIXELS_TO_POINTS
 Converts a rect in pixels to points
 */
#define AX_POINT_PIXELS_TO_POINTS(__pixels__)                                                                        \
Vec2( (__pixels__).x / AX_CONTENT_SCALE_FACTOR(), (__pixels__).y / AX_CONTENT_SCALE_FACTOR())

/** @def AX_POINT_POINTS_TO_PIXELS
 Converts a rect in points to pixels
 */
#define AX_POINT_POINTS_TO_PIXELS(__points__)                                                                        \
Vec2( (__points__).x * AX_CONTENT_SCALE_FACTOR(), (__points__).y * AX_CONTENT_SCALE_FACTOR())

/** @def AX_POINT_PIXELS_TO_POINTS
 Converts a rect in pixels to points
 */
#define AX_SIZE_PIXELS_TO_POINTS(__size_in_pixels__)                                                                        \
Size( (__size_in_pixels__).width / AX_CONTENT_SCALE_FACTOR(), (__size_in_pixels__).height / AX_CONTENT_SCALE_FACTOR())

/** @def AX_POINT_POINTS_TO_PIXELS
 Converts a rect in points to pixels
 */
#define AX_SIZE_POINTS_TO_PIXELS(__size_in_points__)                                                                        \
Size( (__size_in_points__).width * AX_CONTENT_SCALE_FACTOR(), (__size_in_points__).height * AX_CONTENT_SCALE_FACTOR())


#ifndef FLT_EPSILON
#define FLT_EPSILON     1.192092896e-07F
#endif // FLT_EPSILON

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&);\
            void operator=(const TypeName&)

/**
Helper macros which converts 4-byte little/big endian 
integral number to the machine native number representation
 
It should work same as apples CFSwapInt32LittleToHost(..)
*/

/// when define returns true it means that our architecture uses big endian
#define AX_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100) 
#define AX_SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define AX_SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)   
#define AX_SWAP_INT32_LITTLE_TO_HOST(i) ((AX_HOST_IS_BIG_ENDIAN == true)? AX_SWAP32(i) : (i) )
#define AX_SWAP_INT16_LITTLE_TO_HOST(i) ((AX_HOST_IS_BIG_ENDIAN == true)? AX_SWAP16(i) : (i) )
#define AX_SWAP_INT32_BIG_TO_HOST(i)    ((AX_HOST_IS_BIG_ENDIAN == true)? (i) : AX_SWAP32(i) )
#define AX_SWAP_INT16_BIG_TO_HOST(i)    ((AX_HOST_IS_BIG_ENDIAN == true)? (i):  AX_SWAP16(i) )

/**********************/
/** Profiling Macros **/
/**********************/
#if AX_ENABLE_PROFILERS

#define AX_PROFILER_DISPLAY_TIMERS() Profiler::sharedProfiler()->displayTimers()
#define AX_PROFILER_PURGE_ALL() Profiler::sharedProfiler()->releaseAllTimers()

#define AX_PROFILER_START(__name__) AXProfilingBeginTimingBlock(__name__)
#define AX_PROFILER_STOP(__name__) AXProfilingEndTimingBlock(__name__)
#define AX_PROFILER_RESET(__name__) AXProfilingResetTimingBlock(__name__)

#define AX_PROFILER_START_CATEGORY(__cat__, __name__) do{ if(__cat__) AXProfilingBeginTimingBlock(__name__); } while(0)
#define AX_PROFILER_STOP_CATEGORY(__cat__, __name__) do{ if(__cat__) AXProfilingEndTimingBlock(__name__); } while(0)
#define AX_PROFILER_RESET_CATEGORY(__cat__, __name__) do{ if(__cat__) AXProfilingResetTimingBlock(__name__); } while(0)

#define AX_PROFILER_START_INSTANCE(__id__, __name__) do{ AXProfilingBeginTimingBlock( String::createWithFormat("%08X - %s", __id__, __name__)->getCString() ); } while(0)
#define AX_PROFILER_STOP_INSTANCE(__id__, __name__) do{ AXProfilingEndTimingBlock(    String::createWithFormat("%08X - %s", __id__, __name__)->getCString() ); } while(0)
#define AX_PROFILER_RESET_INSTANCE(__id__, __name__) do{ AXProfilingResetTimingBlock( String::createWithFormat("%08X - %s", __id__, __name__)->getCString() ); } while(0)


#else

#define AX_PROFILER_DISPLAY_TIMERS() do {} while (0)
#define AX_PROFILER_PURGE_ALL() do {} while (0)

#define AX_PROFILER_START(__name__)  do {} while (0)
#define AX_PROFILER_STOP(__name__) do {} while (0)
#define AX_PROFILER_RESET(__name__) do {} while (0)

#define AX_PROFILER_START_CATEGORY(__cat__, __name__) do {} while(0)
#define AX_PROFILER_STOP_CATEGORY(__cat__, __name__) do {} while(0)
#define AX_PROFILER_RESET_CATEGORY(__cat__, __name__) do {} while(0)

#define AX_PROFILER_START_INSTANCE(__id__, __name__) do {} while(0)
#define AX_PROFILER_STOP_INSTANCE(__id__, __name__) do {} while(0)
#define AX_PROFILER_RESET_INSTANCE(__id__, __name__) do {} while(0)

#endif

#if !defined(AXOLOTL_DEBUG) || AXOLOTL_DEBUG == 0
#define CHECK_GL_ERROR_DEBUG()
#else
#define CHECK_GL_ERROR_DEBUG() \
    do { \
        GLenum __error = glGetError(); \
        if(__error) { \
            log("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
        } \
    } while (false)
#endif

/** @def AX_INCREMENT_GL_DRAWS_BY_ONE
 Increments the GL Draws counts by one.
 The number of calls per frame are displayed on the screen when the Director's stats are enabled.
 */
extern unsigned int AX_DLL _numberOfDraws;
#define AX_INCREMENT_GL_DRAWS(__n__) _numberOfDraws += __n__

/*******************/
/** Notifications **/
/*******************/
/** @def CCAnimationFrameDisplayedNotification
 Notification name when a SpriteFrame is displayed
 */
#define CCAnimationFrameDisplayedNotification "CCAnimationFrameDisplayedNotification"

#endif // __AXMACROS_H__
