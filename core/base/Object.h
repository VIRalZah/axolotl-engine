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

#ifndef __AXOBJECT_H__
#define __AXOBJECT_H__

#include "DataVisitor.h"
#include <functional>

#ifdef EMSCRIPTEN
#include <GLES2/gl2.h>
#endif // EMSCRIPTEN

NS_AX_BEGIN

/**
 * @addtogroup base
 * @{
 */

class Zone;
class Object;
class Node;
class Event;

/**
 * @js NA
 * @lua NA
 */
class AX_DLL Copying
{
public:
    virtual Object* copyWithZone(Zone* pZone);
};

/**
 * @js NA
 */
class AX_DLL Object : public Copying
{
public:
    // object id, CCScriptSupport need public m_uID
    unsigned int        m_uID;
    // Lua reference id
    int                 m_nLuaID;
protected:
    // count of references
    unsigned int        m_uReference;
    // count of autorelease
    unsigned int        m_uAutoReleaseCount;
public:
    Object(void);
    /**
     *  @lua NA
     */
    virtual ~Object(void);
    
    void release(void);
    void retain(void);
    Object* autorelease(void);
    Object* copy(void);
    bool isSingleReference(void) const;
    unsigned int retainCount(void) const;
    virtual bool isEqual(const Object* pObject);

    virtual void acceptVisitor(DataVisitor &visitor);

    virtual void update(float dt) {AX_UNUSED_PARAM(dt);};
    
    friend class AutoreleasePool;
};


typedef void (Object::*SEL_SCHEDULE)(float);
typedef void (Object::*SEL_CallFunc)();
typedef void (Object::*SEL_CallFuncN)(Node*);
typedef void (Object::*SEL_CallFuncND)(Node*, void*);
typedef void (Object::*SEL_CallFuncO)(Object*);
typedef void (Object::*SEL_EventHandler)(Event*);
typedef int (Object::*SEL_Compare)(Object*);

typedef std::function<void(Object*)> axMenuCallback;

#define selector(_SELECTOR, _TARGET, ...) std::bind(&_SELECTOR, _TARGET, ##__VA_ARGS__)

#define menu_selector(_SELECTOR, _TARGET) selector(_SELECTOR, _TARGET, std::placeholders::_1)

#define schedule_selector(_SELECTOR) (SEL_SCHEDULE)(&_SELECTOR)
#define callfunc_selector(_SELECTOR) (SEL_CallFunc)(&_SELECTOR)
#define callfuncN_selector(_SELECTOR) (SEL_CallFuncN)(&_SELECTOR)
#define callfuncND_selector(_SELECTOR) (SEL_CallFuncND)(&_SELECTOR)
#define callfuncO_selector(_SELECTOR) (SEL_CallFuncO)(&_SELECTOR)
#define event_selector(_SELECTOR) (SEL_EventHandler)(&_SELECTOR)
#define compare_selector(_SELECTOR) (SEL_Compare)(&_SELECTOR)

// end of base group
/// @}

NS_AX_END

#endif // __AXOBJECT_H__
