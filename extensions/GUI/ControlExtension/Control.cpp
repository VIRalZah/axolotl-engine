/*
 * Copyright (c) 2012 cocos2d-x.org
 * http://www.cocos2d-x.org
 *
 * Copyright 2011 Yannick Loriot.
 * http://yannickloriot.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *
 * converted to c++ / cocos2d-x by Angus C
 */

#include "Control.h"
#include "base/Director.h"
#include "menu_nodes/Menu.h"
#include "base/Touch.h"

NS_AX_EXT_BEGIN

CCControl::CCControl()
: m_bIsOpacityModifyRGB(false)
, m_eState(CCControlStateNormal)
, m_hasVisibleParents(false)
, m_bEnabled(false)
, m_bSelected(false)
, m_bHighlighted(false)
, m_pDispatchTable(NULL)
{

}

CCControl* CCControl::create()
{
    CCControl* pRet = new CCControl();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        AX_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool CCControl::init()
{
    if (Layer::init())
    {
        //this->setTouchEnabled(true);
        //m_bIsTouchEnabled=true;
        // Initialise instance variables
        m_eState=CCControlStateNormal;
        setEnabled(true);
        setSelected(false);
        setHighlighted(false);

        // Set the touch dispatcher priority by default to 1
        this->setTouchPriority(1);
        // Initialise the tables
        m_pDispatchTable = new Dictionary(); 
        // Initialise the mapHandleOfControlEvents
        m_mapHandleOfControlEvent.clear();
        
        return true;
    }
    else
    {
        return false;
    }
}

CCControl::~CCControl()
{
    AX_SAFE_RELEASE(m_pDispatchTable);
}

void CCControl::onEnter()
{
    Layer::onEnter();
}

void CCControl::onExit()
{
    Layer::onExit();
}

void CCControl::sendActionsForControlEvents(CCControlEvent controlEvents)
{
    // For each control events
    for (int i = 0; i < kControlEventTotalNumber; i++)
    {
        // If the given controlEvents bitmask contains the curent event
        if ((controlEvents & (1 << i)))
        {
            // Call invocations
            // <CCInvocation*>
            Array* invocationList = this->dispatchListforControlEvent(1<<i);
            Object* pObj = NULL;
            AXARRAY_FOREACH(invocationList, pObj)
            {
                CCInvocation* invocation = (CCInvocation*)pObj;
                invocation->invoke(this);
            }
            //Call ScriptFunc
            if (kScriptTypeNone != m_eScriptType)
            {
                int nHandler = this->getHandleOfControlEvent(controlEvents);
                if (-1 != nHandler) {
                    Array* pArrayArgs = Array::createWithCapacity(3);
                    pArrayArgs->addObject(String::create(""));
                    pArrayArgs->addObject(this);
                    pArrayArgs->addObject(Integer::create(1 << i));
                    ScriptEngineManager::sharedManager()->getScriptEngine()->executeEventWithArgs(nHandler, pArrayArgs);
                }
            }
        }
    }
}
void CCControl::addTargetWithActionForControlEvents(Object* target, SEL_AXControlHandler action, CCControlEvent controlEvents)
{
    // For each control events
    for (int i = 0; i < kControlEventTotalNumber; i++)
    {
        // If the given controlEvents bitmask contains the curent event
        if ((controlEvents & (1 << i)))
        {
            this->addTargetWithActionForControlEvent(target, action, 1<<i);            
        }
    }
}



/**
 * Adds a target and action for a particular event to an internal dispatch 
 * table.
 * The action message may optionnaly include the sender and the event as 
 * parameters, in that order.
 * When you call this method, target is not retained.
 *
 * @param target The target object that is, the object to which the action 
 * message is sent. It cannot be nil. The target is not retained.
 * @param action A selector identifying an action message. It cannot be NULL.
 * @param controlEvent A control event for which the action message is sent.
 * See "CCControlEvent" for constants.
 */
void CCControl::addTargetWithActionForControlEvent(Object* target, SEL_AXControlHandler action, CCControlEvent controlEvent)
{    
    // Create the invocation object
    CCInvocation *invocation = CCInvocation::create(target, action, controlEvent);

    // Add the invocation into the dispatch list for the given control event
    Array* eventInvocationList = this->dispatchListforControlEvent(controlEvent);
    eventInvocationList->addObject(invocation);    
}

void CCControl::removeTargetWithActionForControlEvents(Object* target, SEL_AXControlHandler action, CCControlEvent controlEvents)
{
     // For each control events
    for (int i = 0; i < kControlEventTotalNumber; i++)
    {
        // If the given controlEvents bitmask contains the curent event
        if ((controlEvents & (1 << i)))
        {
            this->removeTargetWithActionForControlEvent(target, action, 1 << i);
        }
    }
}

void CCControl::removeTargetWithActionForControlEvent(Object* target, SEL_AXControlHandler action, CCControlEvent controlEvent)
{
    // Retrieve all invocations for the given control event
    //<CCInvocation*>
    Array *eventInvocationList = this->dispatchListforControlEvent(controlEvent);
    
    //remove all invocations if the target and action are null
    //TODO: should the invocations be deleted, or just removed from the array? Won't that cause issues if you add a single invocation for multiple events?
    bool bDeleteObjects=true;
    if (!target && !action)
    {
        //remove objects
        eventInvocationList->removeAllObjects();
    } 
    else
    {
            //normally we would use a predicate, but this won't work here. Have to do it manually
            Object* pObj = NULL;
            AXARRAY_FOREACH(eventInvocationList, pObj)
            {
                CCInvocation *invocation = (CCInvocation*)pObj;
                bool shouldBeRemoved=true;
                if (target)
                {
                    shouldBeRemoved=(target==invocation->getTarget());
                }
                if (action)
                {
                    shouldBeRemoved=(shouldBeRemoved && (action==invocation->getAction()));
                }
                // Remove the corresponding invocation object
                if (shouldBeRemoved)
                {
                    eventInvocationList->removeObject(invocation, bDeleteObjects);
                }
            }
    }
}


//CRGBA protocol
void CCControl::setOpacityModifyRGB(bool bOpacityModifyRGB)
{
    m_bIsOpacityModifyRGB=bOpacityModifyRGB;
    Object* child;
    Array* children=getChildren();
    AXARRAY_FOREACH(children, child)
    {
        RGBAProtocol* pNode = dynamic_cast<RGBAProtocol*>(child);        
        if (pNode)
        {
            pNode->setOpacityModifyRGB(bOpacityModifyRGB);
        }
    }
}

bool CCControl::isOpacityModifyRGB()
{
    return m_bIsOpacityModifyRGB;
}


Point CCControl::getTouchLocation(Touch* touch)
{
    Point touchLocation = touch->getLocation();            // Get the touch position
    touchLocation = this->convertToNodeSpace(touchLocation);  // Convert to the node space of this class
    
    return touchLocation;
}

bool CCControl::isTouchInside(Touch* touch)
{
    Point touchLocation = touch->getLocation(); // Get the touch position
    touchLocation = this->getParent()->convertToNodeSpace(touchLocation);
    Rect bBox=boundingBox();
    return bBox.containsPoint(touchLocation);
}

Array* CCControl::dispatchListforControlEvent(CCControlEvent controlEvent)
{
    Array* invocationList = static_cast<Array*>(m_pDispatchTable->objectForKey((int)controlEvent));

    // If the invocation list does not exist for the  dispatch table, we create it
    if (invocationList == NULL)
    {
        invocationList = Array::createWithCapacity(1);
        m_pDispatchTable->setObject(invocationList, controlEvent);
    }    
    return invocationList;
}

void CCControl::needsLayout()
{
}

void CCControl::setEnabled(bool bEnabled)
{
    m_bEnabled = bEnabled;
    if(m_bEnabled) {
        m_eState = CCControlStateNormal;
    } else {
        m_eState = CCControlStateDisabled;
    }

    this->needsLayout();
}

bool CCControl::isEnabled()
{
    return m_bEnabled;
}

void CCControl::setSelected(bool bSelected)
{
    m_bSelected = bSelected;
    this->needsLayout();
}

bool CCControl::isSelected()
{
    return m_bSelected;
}

void CCControl::setHighlighted(bool bHighlighted)
{
    m_bHighlighted = bHighlighted;
    this->needsLayout();
}

bool CCControl::isHighlighted()
{
    return m_bHighlighted;
}

bool CCControl::hasVisibleParents()
{
    Node* pParent = this->getParent();
    for( Node *c = pParent; c != NULL; c = c->getParent() )
    {
        if( !c->isVisible() )
        {
            return false;
        }
    }
    return true;
}

void CCControl::addHandleOfControlEvent(int nFunID,CCControlEvent controlEvent)
{
    m_mapHandleOfControlEvent[controlEvent] = nFunID;
}

void CCControl::removeHandleOfControlEvent(CCControlEvent controlEvent)
{
    std::map<int,int>::iterator Iter = m_mapHandleOfControlEvent.find(controlEvent);
    
    if (m_mapHandleOfControlEvent.end() != Iter)
    {
        m_mapHandleOfControlEvent.erase(Iter);
    }
    
}

int  CCControl::getHandleOfControlEvent(CCControlEvent controlEvent)
{
    std::map<int,int>::iterator Iter = m_mapHandleOfControlEvent.find(controlEvent);
    
    if (m_mapHandleOfControlEvent.end() != Iter)
        return Iter->second;
    
    return -1;
}
NS_AX_EXT_END
