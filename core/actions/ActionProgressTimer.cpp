/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (C) 2010      Lam Pham
 
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
#include "ActionProgressTimer.h"
#include "misc_nodes/ProgressTimer.h"
#include "cocoa/Zone.h"

NS_AX_BEGIN

#define kProgressTimerCast ProgressTimer*

// implementation of ProgressTo

ProgressTo* ProgressTo::create(float duration, float fPercent)
{
    ProgressTo *pProgressTo = new ProgressTo();
    pProgressTo->initWithDuration(duration, fPercent);
    pProgressTo->autorelease();

    return pProgressTo;
}

bool ProgressTo::initWithDuration(float duration, float fPercent)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_fTo = fPercent;

        return true;
    }

    return false;
}

Object* ProgressTo::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    ProgressTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (ProgressTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new ProgressTo();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_fTo);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void ProgressTo::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_fFrom = ((kProgressTimerCast)(pTarget))->getPercentage();

    // XXX: Is this correct ?
    // Adding it to support Repeat
    if (m_fFrom == 100)
    {
        m_fFrom = 0;
    }
}

void ProgressTo::update(float time)
{
    ((kProgressTimerCast)(m_pTarget))->setPercentage(m_fFrom + (m_fTo - m_fFrom) * time);
}

// implementation of ProgressFromTo

ProgressFromTo* ProgressFromTo::create(float duration, float fFromPercentage, float fToPercentage)
{
    ProgressFromTo *pProgressFromTo = new ProgressFromTo();
    pProgressFromTo->initWithDuration(duration, fFromPercentage, fToPercentage);
    pProgressFromTo->autorelease();

    return pProgressFromTo;
}

bool ProgressFromTo::initWithDuration(float duration, float fFromPercentage, float fToPercentage)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_fTo = fToPercentage;
        m_fFrom = fFromPercentage;

        return true;
    }

    return false;
}

Object* ProgressFromTo::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    ProgressFromTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (ProgressFromTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new ProgressFromTo();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_fFrom, m_fTo);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

ActionInterval* ProgressFromTo::reverse(void)
{
    return ProgressFromTo::create(m_fDuration, m_fTo, m_fFrom);
}

void ProgressFromTo::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
}

void ProgressFromTo::update(float time)
{
    ((kProgressTimerCast)(m_pTarget))->setPercentage(m_fFrom + (m_fTo - m_fFrom) * time);
}

NS_AX_END

