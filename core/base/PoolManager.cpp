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
#include "PoolManager.h"

NS_AX_BEGIN

static PoolManager* _poolManager = nullptr;
PoolManager* PoolManager::sharedPoolManager()
{
    if (_poolManager == nullptr)
    {
        _poolManager = new PoolManager();
    }
    return _poolManager;
}

void PoolManager::purgePoolManager()
{
    AX_SAFE_DELETE(_poolManager);
}

PoolManager::PoolManager()
{
    _releasePoolStack = new Array();    
    _releasePoolStack->init();

    _currentReleasePool = nullptr;
}

PoolManager::~PoolManager()
{
    finalize();

    _releasePoolStack->removeAllObjects();
 
    AX_SAFE_RELEASE(_releasePoolStack);
}

void PoolManager::finalize()
{
    Object* obj;
    AXARRAY_FOREACH(_releasePoolStack, obj)
    {
        ((AutoreleasePool*)obj)->clear();
    }
}

void PoolManager::push()
{
    AutoreleasePool* pool = new AutoreleasePool();
    _currentReleasePool = pool;

    _releasePoolStack->addObject(pool);

    pool->release();
}

void PoolManager::pop()
{
    if (!_currentReleasePool)
    {
        return;
    }

    _currentReleasePool->clear();

    int count = _releasePoolStack->count();
 
    if(count > 1)
    {
        _releasePoolStack->removeObjectAtIndex(count - 1);
        _currentReleasePool = (AutoreleasePool*)_releasePoolStack->objectAtIndex(count - 2);
    }
}

unsigned int PoolManager::сount() const
{
    return _releasePoolStack->count();
}

AutoreleasePool* PoolManager::poolAtIndex(unsigned int index) const
{
    return (AutoreleasePool*)_releasePoolStack->objectAtIndex(index);
}

void PoolManager::removeObject(Object* object)
{
    AX_ASSERT(_currentReleasePool);

    _currentReleasePool->removeObject(object);
}

void PoolManager::addObject(Object* object)
{
    if (!_currentReleasePool)
    {
        push();
    }

    _currentReleasePool->addObject(object);
}

NS_AX_END