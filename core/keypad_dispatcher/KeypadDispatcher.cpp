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

#include "KeypadDispatcher.h"
#include "support/data_support/axCArray.h"

NS_AX_BEGIN

//------------------------------------------------------------------
//
// KeypadDispatcher
//
//------------------------------------------------------------------
KeypadDispatcher::KeypadDispatcher()
: m_bLocked(false)
, m_bToAdd(false)
, m_bToRemove(false)
{
    m_pDelegates = Array::create();
    m_pDelegates->retain();

    m_pHandlersToAdd    = axCArrayNew(8);
    m_pHandlersToRemove = axCArrayNew(8);
}

KeypadDispatcher::~KeypadDispatcher()
{
    AX_SAFE_RELEASE(m_pDelegates);
    if (m_pHandlersToAdd)
    {
        axCArrayFree(m_pHandlersToAdd);
    }
    
    if (m_pHandlersToRemove)
    {
        axCArrayFree(m_pHandlersToRemove);
    }    
}

void KeypadDispatcher::removeDelegate(KeypadDelegate* pDelegate)
{
    if (!pDelegate)
    {
        return;
    }
    if (! m_bLocked)
    {
        forceRemoveDelegate(pDelegate);
    }
    else
    {
        axCArrayAppendValue(m_pHandlersToRemove, pDelegate);
        m_bToRemove = true;
    }
}

void KeypadDispatcher::addDelegate(KeypadDelegate* pDelegate)
{
    if (!pDelegate)
    {
        return;
    }

    if (! m_bLocked)
    {
        forceAddDelegate(pDelegate);
    }
    else
    {
        axCArrayAppendValue(m_pHandlersToAdd, pDelegate);
        m_bToAdd = true;
    }
}

void KeypadDispatcher::forceAddDelegate(KeypadDelegate* pDelegate)
{
    KeypadHandler* pHandler = KeypadHandler::handlerWithDelegate(pDelegate);

    if (pHandler)
    {
        m_pDelegates->addObject(pHandler);
    }
}

void KeypadDispatcher::forceRemoveDelegate(KeypadDelegate* pDelegate)
{
    KeypadHandler* pHandler = NULL;
    Object* pObj = NULL;
    AXARRAY_FOREACH(m_pDelegates, pObj)
    {
        pHandler = (KeypadHandler*)pObj;
        if (pHandler && pHandler->getDelegate() == pDelegate)
        {
            m_pDelegates->removeObject(pHandler);
            break;
        }
    }
}

bool KeypadDispatcher::dispatchKeypadMSG(ccKeypadMSGType nMsgType)
{
    KeypadHandler*  pHandler = NULL;
    KeypadDelegate* pDelegate = NULL;

    m_bLocked = true;

    if (m_pDelegates->count() > 0)
    {
        Object* pObj = NULL;
        AXARRAY_FOREACH(m_pDelegates, pObj)
        {
            AX_BREAK_IF(!pObj);

            pHandler = (KeypadHandler*)pObj;
            pDelegate = pHandler->getDelegate();

            switch (nMsgType)
            {
            case kTypeBackClicked:
                pDelegate->keyBackClicked();
                break;
            case kTypeMenuClicked:
                pDelegate->keyMenuClicked();
                break;
            default:
                break;
            }
        }
    }

    m_bLocked = false;
    if (m_bToRemove)
    {
        m_bToRemove = false;
        for (unsigned int i = 0; i < m_pHandlersToRemove->num; ++i)
        {
            forceRemoveDelegate((KeypadDelegate*)m_pHandlersToRemove->arr[i]);
        }
        axCArrayRemoveAllValues(m_pHandlersToRemove);
    }

    if (m_bToAdd)
    {
        m_bToAdd = false;
        for (unsigned int i = 0; i < m_pHandlersToAdd->num; ++i)
        {
            forceAddDelegate((KeypadDelegate*)m_pHandlersToAdd->arr[i]);
        }
        axCArrayRemoveAllValues(m_pHandlersToAdd);
    }

    return true;
}

NS_AX_END
