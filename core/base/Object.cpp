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


#include "base/Object.h"
#include "base/PoolManager.h"
#include "ccMacros.h"

NS_AX_BEGIN

Object* Copying::copyWithZone(Zone *pZone)
{
    AX_UNUSED_PARAM(pZone);
    AXAssert(0, "not implement");
    return 0;
}

Object::Object(void)
: m_nLuaID(0)
, m_uReference(1) // when the object is created, the reference count of it is 1
, m_uAutoReleaseCount(0)
{
    static unsigned int uObjectCount = 0;

    m_uID = ++uObjectCount;
}

Object::~Object(void)
{
    // if the object is managed, we should remove it
    // from pool manager
    if (m_uAutoReleaseCount > 0)
    {
        PoolManager::sharedPoolManager()->removeObject(this);
    }
}

Object* Object::copy()
{
    return copyWithZone(0);
}

void Object::release(void)
{
    AXAssert(m_uReference > 0, "reference count should greater than 0");
    --m_uReference;

    if (m_uReference == 0)
    {
        delete this;
    }
}

void Object::retain(void)
{
    AXAssert(m_uReference > 0, "reference count should greater than 0");

    ++m_uReference;
}

Object* Object::autorelease(void)
{
    PoolManager::sharedPoolManager()->addObject(this);
    return this;
}

bool Object::isSingleReference(void) const
{
    return m_uReference == 1;
}

unsigned int Object::retainCount(void) const
{
    return m_uReference;
}

bool Object::isEqual(const Object *pObject)
{
    return this == pObject;
}

void Object::acceptVisitor(DataVisitor &visitor)
{
    visitor.visitObject(this);
}

NS_AX_END
