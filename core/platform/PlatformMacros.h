/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org
 
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
#ifndef __AX_PLATFORM_MACROS_H__
#define __AX_PLATFORM_MACROS_H__

/**
 * define some platform specific macros
 */
#include "ccConfig.h"
#include "PlatformConfig.h"
#include "PlatformDefine.h"

/**
 * define a create function for a specific type, such as Layer
 * @__TYPE__ class type to add create(), such as Layer
 */
#define CREATE_FUNC(__TYPE__) \
static __TYPE__* create() \
{ \
    __TYPE__ *pRet = new __TYPE__(); \
    if (pRet && pRet->init()) \
    { \
        pRet->autorelease(); \
        return pRet; \
    } \
    else \
    { \
        delete pRet; \
        pRet = NULL; \
        return NULL; \
    } \
}

/**
 * define a node function for a specific type, such as Layer
 * @__TYPE__ class type to add node(), such as Layer
 * @deprecated: This interface will be deprecated sooner or later.
 */
#define NODE_FUNC(__TYPE__) \
AX_DEPRECATED_ATTRIBUTE static __TYPE__* node() \
{ \
    __TYPE__ *pRet = new __TYPE__(); \
    if (pRet && pRet->init()) \
    { \
        pRet->autorelease(); \
        return pRet; \
    } \
    else \
    { \
        delete pRet; \
        pRet = NULL; \
        return NULL; \
    } \
}

/** @def AX_ENABLE_CACHE_TEXTURE_DATA
Enable it if you want to cache the texture data.
Basically, it's only enabled for Emscripten.

It's new in cocos2d-x since v0.99.5
*/
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || (AX_TARGET_PLATFORM == AX_PLATFORM_EMSCRIPTEN) || (AX_TARGET_PLATFORM == AX_PLATFORM_WP8)
    #define AX_ENABLE_CACHE_TEXTURE_DATA       1
#else
    #define AX_ENABLE_CACHE_TEXTURE_DATA       0
#endif

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) || (AX_TARGET_PLATFORM == AX_PLATFORM_EMSCRIPTEN)
    /* Application will crash in glDrawElements function on some win32 computers and some android devices.
       Indices should be bound again while drawing to avoid this bug.
     */
    #define AX_REBIND_INDICES_BUFFER  1
#else
    #define AX_REBIND_INDICES_BUFFER  0
#endif

// generic macros

// namespace axolotl {}
#ifdef __cplusplus
    #define NS_AX_BEGIN                     namespace axolotl {
    #define NS_AX_END                       }
    #define USING_NS_AX                     using namespace axolotl
#else
    #define NS_AX_BEGIN 
    #define NS_AX_END 
    #define USING_NS_AX 
#endif 

/** AX_PROPERTY_READONLY is used to declare a protected variable.
 We can use getter to read the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 @warning : The getter is a public virtual function, you should rewrite it first.
 The variables and methods declared after AX_PROPERTY_READONLY are all public.
 If you need protected or private, please declare.
 */
#define AX_PROPERTY_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void);

#define AX_PROPERTY_READONLY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void);

/** AX_PROPERTY is used to declare a protected variable.
 We can use getter to read the variable, and use the setter to change the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 "set + funName" is the name of the setter.
 @warning : The getter and setter are public virtual functions, you should rewrite them first.
 The variables and methods declared after AX_PROPERTY are all public.
 If you need protected or private, please declare.
 */
#define AX_PROPERTY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void);\
public: virtual void set##funName(varType var);

#define AX_PROPERTY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void);\
public: virtual void set##funName(const varType& var);

/** AX_SYNTHESIZE_READONLY is used to declare a protected variable.
 We can use getter to read the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 @warning : The getter is a public inline function.
 The variables and methods declared after AX_SYNTHESIZE_READONLY are all public.
 If you need protected or private, please declare.
 */
#define AX_SYNTHESIZE_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }

#define AX_SYNTHESIZE_READONLY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const { return varName; }

/** AX_SYNTHESIZE is used to declare a protected variable.
 We can use getter to read the variable, and use the setter to change the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 "set + funName" is the name of the setter.
 @warning : The getter and setter are public  inline functions.
 The variables and methods declared after AX_SYNTHESIZE are all public.
 If you need protected or private, please declare.
 */
#define AX_SYNTHESIZE(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }\
public: virtual void set##funName(varType var){ varName = var; }

#define AX_SYNTHESIZE_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const { return varName; }\
public: virtual void set##funName(const varType& var){ varName = var; }

#define AX_SYNTHESIZE_RETAIN(varType, varName, funName)    \
private: varType varName; \
public: virtual varType get##funName(void) const { return varName; } \
public: virtual void set##funName(varType var)   \
{ \
    if (varName != var) \
    { \
        AX_SAFE_RETAIN(var); \
        AX_SAFE_RELEASE(varName); \
        varName = var; \
    } \
} 

#define AX_SAFE_DELETE(p)            do { if(p) { delete (p); (p) = 0; } } while(0)
#define AX_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = 0; } } while(0)
#define AX_SAFE_FREE(p)                do { if(p) { free(p); (p) = 0; } } while(0)
#define AX_SAFE_RELEASE(p)            do { if(p) { (p)->release(); } } while(0)
#define AX_SAFE_RELEASE_NULL(p)        do { if(p) { (p)->release(); (p) = 0; } } while(0)
#define AX_SAFE_RETAIN(p)            do { if(p) { (p)->retain(); } } while(0)
#define AX_BREAK_IF(cond)            if(cond) break
#define AX_CONTINUE_IF(cond)            if(cond) continue

#define __AXLOGWITHFUNCTION(s, ...) \
    log("%s : %s",__FUNCTION__, String::createWithFormat(s, ##__VA_ARGS__)->getCString())

// cocos2d debug
#if !defined(AXOLOTL_DEBUG) || AXOLOTL_DEBUG == 0
#define AXLOG(...)       do {} while (0)
#define AXLOGINFO(...)   do {} while (0)
#define AXLOGERROR(...)  do {} while (0)
#define AXLOGWARN(...)   do {} while (0)

#elif AXOLOTL_DEBUG == 1
#define AXLOG(format, ...)      axolotl::log(format, ##__VA_ARGS__)
#define AXLOGERROR(format,...)  axolotl::log(format, ##__VA_ARGS__)
#define AXLOGINFO(format,...)   do {} while (0)
#define AXLOGWARN(...) __AXLOGWITHFUNCTION(__VA_ARGS__)

#elif AXOLOTL_DEBUG > 1
#define AXLOG(format, ...)      axolotl::log(format, ##__VA_ARGS__)
#define AXLOGERROR(format,...)  axolotl::log(format, ##__VA_ARGS__)
#define AXLOGINFO(format,...)   axolotl::log(format, ##__VA_ARGS__)
#define AXLOGWARN(...) __AXLOGWITHFUNCTION(__VA_ARGS__)
#endif // AXOLOTL_DEBUG

// Lua engine debug
#if !defined(AXOLOTL_DEBUG) || AXOLOTL_DEBUG == 0 || AX_LUA_ENGINE_DEBUG == 0
#define LUALOG(...)
#else
#define LUALOG(format, ...)     axolotl::log(format, ##__VA_ARGS__)
#endif // Lua engine debug

#if defined(__GNUC__) && ((__GNUC__ >= 5) || ((__GNUG__ == 4) && (__GNUC_MINOR__ >= 4))) \
    || (defined(__clang__) && (__clang_major__ >= 3))
#define AX_DISABLE_COPY(Class) \
private: \
    Class(const Class &) = delete; \
    Class &operator =(const Class &) = delete;
#else
#define AX_DISABLE_COPY(Class) \
private: \
    Class(const Class &); \
    Class &operator =(const Class &);
#endif

/*
 * only certain compilers support __attribute__((deprecated))
 */
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
    #define AX_DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#elif _MSC_VER >= 1400 //vs 2005 or higher
    #define AX_DEPRECATED_ATTRIBUTE __declspec(deprecated) 
#else
    #define AX_DEPRECATED_ATTRIBUTE
#endif 

/*
 * only certain compiler support __attribute__((format))
 * formatPos - 1-based position of format string argument
 * argPos - 1-based position of first format-dependent argument
 */
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define AX_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#elif defined(__has_attribute)
  #if __has_attribute(format)
  #define AX_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
  #endif // __has_attribute(format)
#else
#define AX_FORMAT_PRINTF(formatPos, argPos)
#endif

#if defined(_MSC_VER)
#define AX_FORMAT_PRINTF_SIZE_T "%08lX"
#else
#define AX_FORMAT_PRINTF_SIZE_T "%08zX"
#endif

#ifdef __GNUC__
#define AX_UNUSED __attribute__ ((unused))
#else
#define AX_UNUSED
#endif

#endif // __AX_PLATFORM_MACROS_H__
