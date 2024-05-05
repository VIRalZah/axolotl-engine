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

#ifndef __AXNOTIFICATIONCENTER_H__
#define __AXNOTIFICATIONCENTER_H__

#include "base/Object.h"
#include "base/Array.h"

NS_AX_BEGIN
/**
 * @js NA
 */
class AX_DLL NotificationCenter : public Object
{
public:
    /** NotificationCenter constructor */
    NotificationCenter();

    /** NotificationCenter destructor */
    ~NotificationCenter();
    
    /** Gets the single instance of NotificationCenter. */
    static NotificationCenter *sharedNotificationCenter(void);

    /** Destroys the single instance of NotificationCenter. */
    static void purgeNotificationCenter(void);

    /** @brief Adds an observer for the specified target.
     *  @param target The target which wants to observe notification events.
     *  @param selector The callback function which will be invoked when the specified notification event was posted.
     *  @param name The name of this notification.
     *  @param obj The extra parameter which will be passed to the callback function.
     */
    void addObserver(Object *target, 
                     SEL_CallFuncO selector,
                     const char *name,
                     Object *obj);

    /** @brief Removes the observer by the specified target and name.
     *  @param target The target of this notification.
     *  @param name The name of this notification. 
     */
    void removeObserver(Object *target,const char *name);
    
    /** @brief Removes all notifications registered by this target
     *  @param target The target of this notification.
     *  @returns the number of observers removed
     */
    int removeAllObservers(Object *target);

    /** @brief Posts one notification event by name.
     *  @param name The name of this notification.
     */
    void postNotification(const char *name);

    /** @brief Posts one notification event by name.
     *  @param name The name of this notification.
     *  @param object The extra parameter.
     */
    void postNotification(const char *name, Object *object);
private:
    // internal functions

    // Check whether the observer exists by the specified target and name.
    bool observerExisted(Object *target,const char *name);
    
    // variables
    //
    Array* m_observers;
};

/**
 * @js NA
 * @lua NA
 */
class AX_DLL NotificationObserver : public Object
{
public:
    /** @brief NotificationObserver constructor
     *  @param target The target which wants to observer notification events.
     *  @param selector The callback function which will be invoked when the specified notification event was posted.
     *  @param name The name of this notification.
     *  @param obj The extra parameter which will be passed to the callback function.
     */
    NotificationObserver(Object *target, 
                           SEL_CallFuncO selector,
                           const char *name,
                           Object *obj);

    /** NotificationObserver destructor function */
    ~NotificationObserver();      
    
    /** Invokes the callback function of this observer */
    void performSelector(Object *obj);
private:
    AX_PROPERTY_READONLY(Object *, m_target, Target);
    AX_PROPERTY_READONLY(SEL_CallFuncO, m_selector, Selector);
    AX_PROPERTY_READONLY(char *, m_name, Name);
    AX_PROPERTY_READONLY(Object *, m_object, Object);
};

NS_AX_END

#endif//__AXNOTIFICATIONCENTER_H__
