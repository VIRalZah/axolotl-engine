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
#include "AutoreleasePool.h"
#include "ccMacros.h"

NS_AX_BEGIN

AutoreleasePool::AutoreleasePool(void)
{
    _managedObjectArray = new Array();
    _managedObjectArray->init();
}

AutoreleasePool::~AutoreleasePool(void)
{
    AX_SAFE_RELEASE(_managedObjectArray);
}

void AutoreleasePool::addObject(Object* pObject)
{
    _managedObjectArray->addObject(pObject);

    AXAssert(pObject->m_uReference > 1, "reference count should be greater than 1");
    pObject->m_uAutoReleaseCount++;
    pObject->release();
}

void AutoreleasePool::removeObject(Object* pObject)
{
    for (unsigned int i = 0; i < pObject->m_uAutoReleaseCount; ++i)
    {
        _managedObjectArray->removeObject(pObject, false);
    }
}

void AutoreleasePool::clear()
{
    if(_managedObjectArray->count() > 0)
    {
        Object* obj;
        AXARRAY_FOREACH_REVERSE(_managedObjectArray, obj)
        {
            obj->m_uAutoReleaseCount--;
        }

        _managedObjectArray->removeAllObjects();
    }
}

NS_AX_END