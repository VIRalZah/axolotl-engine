/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Erawppa
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

#include "NotificationCenter.h"
#include "base/Array.h"
#include <string>

using namespace std;

NS_AX_BEGIN

static NotificationCenter *s_sharedNotifCenter = NULL;

NotificationCenter::NotificationCenter()
{
    m_observers = Array::createWithCapacity(3);
    m_observers->retain();
}

NotificationCenter::~NotificationCenter()
{
    m_observers->release();
}

NotificationCenter *NotificationCenter::sharedNotificationCenter(void)
{
    if (!s_sharedNotifCenter)
    {
        s_sharedNotifCenter = new NotificationCenter;
    }
    return s_sharedNotifCenter;
}

void NotificationCenter::purgeNotificationCenter(void)
{
    AX_SAFE_RELEASE_NULL(s_sharedNotifCenter);
}

//
// internal functions
//
bool NotificationCenter::observerExisted(Object *target,const char *name)
{
    Object* obj = NULL;
    AXARRAY_FOREACH(m_observers, obj)
    {
        NotificationObserver* observer = (NotificationObserver*) obj;
        if (observer && observer->getTarget() == target && !strcmp(observer->getName(),name))
            return true;
    }
    return false;
}

//
// observer functions
//
void NotificationCenter::addObserver(Object *target, 
                                       SEL_CallFuncO selector,
                                       const char *name,
                                       Object *obj)
{
    if (this->observerExisted(target, name))
        return;
    
    NotificationObserver *observer = new NotificationObserver(target, selector, name, obj);
    if (!observer)
        return;
    
    observer->autorelease();
    m_observers->addObject(observer);
}

void NotificationCenter::removeObserver(Object *target,const char *name)
{
    Object* obj = NULL;
    AXARRAY_FOREACH(m_observers, obj)
    {
        NotificationObserver* observer = (NotificationObserver*) obj;
        if (!observer)
            continue;
        
        if (!strcmp(observer->getName(),name) && observer->getTarget() == target)
        {
            m_observers->removeObject(observer);
            return;
        }
    }
}

int NotificationCenter::removeAllObservers(Object *target)
{
    Object *obj = NULL;
    Array *toRemove = Array::create();

    AXARRAY_FOREACH(m_observers, obj)
    {
        NotificationObserver *observer = (NotificationObserver *)obj;
        if (!observer)
            continue;

        if (observer->getTarget() == target)
        {
            toRemove->addObject(observer);
        }
    }

    m_observers->removeObjectsInArray(toRemove);
    return toRemove->count();
}

void NotificationCenter::postNotification(const char *name, Object *object)
{
    Array* ObserversCopy = Array::createWithCapacity(m_observers->count());
    ObserversCopy->addObjectsFromArray(m_observers);
    Object* obj = NULL;
    AXARRAY_FOREACH(ObserversCopy, obj)
    {
        NotificationObserver* observer = (NotificationObserver*) obj;
        if (!observer)
            continue;
        
        if ((observer->getObject() == object || observer->getObject() == NULL || object == NULL) && !strcmp(name,observer->getName()))
        {
            observer->performSelector(object);
        }
    }
}

void NotificationCenter::postNotification(const char *name)
{
    this->postNotification(name,NULL);
}

////////////////////////////////////////////////////////////////////////////////
///
/// NotificationObserver
///
////////////////////////////////////////////////////////////////////////////////
NotificationObserver::NotificationObserver(Object *target, 
                                               SEL_CallFuncO selector,
                                               const char *name,
                                               Object *obj)
{
    m_target = target;
    m_selector = selector;
    m_object = obj;
    
    m_name = new char[strlen(name)+1];
    memset(m_name,0,strlen(name)+1);
    
    string orig (name);
    orig.copy(m_name,strlen(name),0);
}

NotificationObserver::~NotificationObserver()
{
    AX_SAFE_DELETE_ARRAY(m_name);
}

void NotificationObserver::performSelector(Object *obj)
{
    if (m_target)
    {
		if (obj) {
			(m_target->*m_selector)(obj);
		} else {
			(m_target->*m_selector)(m_object);
		}
    }
}

Object *NotificationObserver::getTarget()
{
    return m_target;
}

SEL_CallFuncO NotificationObserver::getSelector()
{
    return m_selector;
}

char *NotificationObserver::getName()
{
    return m_name;
}

Object *NotificationObserver::getObject()
{
    return m_object;
}

NS_AX_END
