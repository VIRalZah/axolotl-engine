/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2009 Jason Booth

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

/*
 * Elastic, Back and Bounce actions based on code from:
 * http://github.com/NikhilK/silverlightfx/
 *
 * by http://github.com/NikhilK
 */

#include "ActionEase.h"
#include "cocoa/Zone.h"

NS_AX_BEGIN

#ifndef M_PI_X_2
#define M_PI_X_2 (float)M_PI * 2.0f
#endif

//
// EaseAction
//

ActionEase* ActionEase::create(ActionInterval *pAction)
{
    ActionEase *pRet = new ActionEase();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet;
}

bool ActionEase::initWithAction(ActionInterval *pAction)
{
    AXAssert(pAction != NULL, "");

    if (ActionInterval::initWithDuration(pAction->getDuration()))
    {
        m_pInner = pAction;
        pAction->retain();

        return true;
    }

    return false;
}

Object* ActionEase::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    ActionEase* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (ActionEase*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new ActionEase();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

ActionEase::~ActionEase(void)
{
    AX_SAFE_RELEASE(m_pInner);
}

void ActionEase::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_pInner->startWithTarget(m_pTarget);
}

void ActionEase::stop(void)
{
    m_pInner->stop();
    ActionInterval::stop();
}

void ActionEase::update(float time)
{
    m_pInner->update(time);
}

ActionInterval* ActionEase::reverse(void)
{
    return ActionEase::create(m_pInner->reverse());
}

ActionInterval* ActionEase::getInnerAction()
{
    return m_pInner;
}

//
// EaseRateAction
//

EaseRateAction* EaseRateAction::create(ActionInterval *pAction, float fRate)
{
    EaseRateAction *pRet = new EaseRateAction();
    if (pRet)
    {
        if (pRet->initWithAction(pAction, fRate))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet;
}

bool EaseRateAction::initWithAction(ActionInterval *pAction, float fRate)
{
    if (ActionEase::initWithAction(pAction))
    {
        m_fRate = fRate;
        return true;
    }

    return false;
}

Object* EaseRateAction::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseRateAction* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseRateAction*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseRateAction();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval*)(m_pInner->copy()->autorelease()), m_fRate);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

EaseRateAction::~EaseRateAction(void)
{
}

ActionInterval* EaseRateAction::reverse(void)
{
    return EaseRateAction::create(m_pInner->reverse(), 1 / m_fRate);
}

//
// EeseIn
//

EaseIn* EaseIn::create(ActionInterval *pAction, float fRate)
{
    EaseIn *pRet = new EaseIn();
    if (pRet)
    {
        if (pRet->initWithAction(pAction, fRate))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet;
}

Object* EaseIn::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseIn* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseIn*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseIn();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval*)(m_pInner->copy()->autorelease()), m_fRate);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseIn::update(float time)
{
    m_pInner->update(powf(time, m_fRate));
}

ActionInterval* EaseIn::reverse(void)
{
    return EaseIn::create(m_pInner->reverse(), 1 / m_fRate);
}

//
// EaseOut
//
EaseOut* EaseOut::create(ActionInterval *pAction, float fRate)
{
    EaseOut *pRet = new EaseOut();
    if (pRet)
    {
        if (pRet->initWithAction(pAction, fRate))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet;   
}

Object* EaseOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseOut();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval*)(m_pInner->copy()->autorelease()), m_fRate);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseOut::update(float time)
{
    m_pInner->update(powf(time, 1 / m_fRate));
}

ActionInterval* EaseOut::reverse()
{
    return EaseOut::create(m_pInner->reverse(), 1 / m_fRate);
}

//
// EaseInOut
//
EaseInOut* EaseInOut::create(ActionInterval *pAction, float fRate)
{
    EaseInOut *pRet = new EaseInOut();
    if (pRet)
    {
        if (pRet->initWithAction(pAction, fRate))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object* EaseInOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseInOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseInOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseInOut();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval*)(m_pInner->copy()->autorelease()), m_fRate);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseInOut::update(float time)
{
    time *= 2;
    if (time < 1)
    {
        m_pInner->update(0.5f * powf(time, m_fRate));
    }
    else
    {
        m_pInner->update(1.0f - 0.5f * powf(2-time, m_fRate));
    }
}

// InOut and OutIn are symmetrical
ActionInterval* EaseInOut::reverse(void)
{
    return EaseInOut::create(m_pInner->reverse(), m_fRate);
}

//
// EaseExponentialIn
//
EaseExponentialIn* EaseExponentialIn::create(ActionInterval* pAction)
{
    EaseExponentialIn *pRet = new EaseExponentialIn();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet;    
}

Object* EaseExponentialIn::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseExponentialIn* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseExponentialIn*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseExponentialIn();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseExponentialIn::update(float time)
{
    m_pInner->update(time == 0 ? 0 : powf(2, 10 * (time/1 - 1)) - 1 * 0.001f);
}

ActionInterval* EaseExponentialIn::reverse(void)
{
    return EaseExponentialOut::create(m_pInner->reverse());
}

//
// EaseExponentialOut
//
EaseExponentialOut* EaseExponentialOut::create(ActionInterval* pAction)
{
    EaseExponentialOut *pRet = new EaseExponentialOut();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object* EaseExponentialOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseExponentialOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseExponentialOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseExponentialOut();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseExponentialOut::update(float time)
{
    m_pInner->update(time == 1 ? 1 : (-powf(2, -10 * time / 1) + 1));
}

ActionInterval* EaseExponentialOut::reverse(void)
{
    return EaseExponentialIn::create(m_pInner->reverse());
}

//
// EaseExponentialInOut
//

EaseExponentialInOut* EaseExponentialInOut::create(ActionInterval *pAction)
{
    EaseExponentialInOut *pRet = new EaseExponentialInOut();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object* EaseExponentialInOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseExponentialInOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseExponentialInOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseExponentialInOut();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseExponentialInOut::update(float time)
{
    time /= 0.5f;
    if (time < 1)
    {
        time = 0.5f * powf(2, 10 * (time - 1));
    }
    else
    {
        time = 0.5f * (-powf(2, -10 * (time - 1)) + 2);
    }

    m_pInner->update(time);
}

ActionInterval* EaseExponentialInOut::reverse()
{
    return EaseExponentialInOut::create(m_pInner->reverse());
}

//
// EaseSineIn
//

EaseSineIn* EaseSineIn::create(ActionInterval* pAction)
{
    EaseSineIn *pRet = new EaseSineIn();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object* EaseSineIn::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseSineIn* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (EaseSineIn*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseSineIn();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseSineIn::update(float time)
{
    m_pInner->update(-1 * cosf(time * (float)M_PI_2) + 1);
}

ActionInterval* EaseSineIn::reverse(void)
{
    return EaseSineOut::create(m_pInner->reverse());
}

//
// EaseSineOut
//

EaseSineOut* EaseSineOut::create(ActionInterval* pAction)
{
    EaseSineOut *pRet = new EaseSineOut();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object* EaseSineOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseSineOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseSineOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseSineOut();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseSineOut::update(float time)
{
    m_pInner->update(sinf(time * (float)M_PI_2));
}

ActionInterval* EaseSineOut::reverse(void)
{
    return EaseSineIn::create(m_pInner->reverse());
}

//
// EaseSineInOut
//

EaseSineInOut* EaseSineInOut::create(ActionInterval* pAction)
{
    EaseSineInOut *pRet = new EaseSineInOut();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object* EaseSineInOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseSineInOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseSineInOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseSineInOut();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseSineInOut::update(float time)
{
    m_pInner->update(-0.5f * (cosf((float)M_PI * time) - 1));
}

ActionInterval* EaseSineInOut::reverse()
{
    return EaseSineInOut::create(m_pInner->reverse());
}

//
// EaseElastic
//

EaseElastic* EaseElastic::create(ActionInterval *pAction)
{
    return EaseElastic::create(pAction, 0.3f);
}

EaseElastic* EaseElastic::create(ActionInterval *pAction, float fPeriod/* = 0.3f*/)
{
    EaseElastic *pRet = new EaseElastic();
    if (pRet)
    {
        if (pRet->initWithAction(pAction, fPeriod))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

bool EaseElastic::initWithAction(ActionInterval *pAction, float fPeriod/* = 0.3f*/)
{
    if (ActionEase::initWithAction(pAction))
    {
        m_fPeriod = fPeriod;
        return true;
    }

    return false;
}

Object* EaseElastic::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseElastic* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseElastic*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseElastic();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()), m_fPeriod);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

ActionInterval* EaseElastic::reverse(void)
{
    AXAssert(0, "Override me");

    return NULL;
}

//
// EaseElasticIn
//

EaseElasticIn* EaseElasticIn::create(ActionInterval *pAction)
{
    return EaseElasticIn::create(pAction, 0.3f);
}

EaseElasticIn* EaseElasticIn::create(ActionInterval *pAction, float fPeriod/* = 0.3f*/)
{
    EaseElasticIn *pRet = new EaseElasticIn();
    if (pRet)
    {
        if (pRet->initWithAction(pAction, fPeriod))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object* EaseElasticIn::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseElasticIn* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseElasticIn*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseElasticIn();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()), m_fPeriod);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseElasticIn::update(float time)
{
    float newT = 0;
    if (time == 0 || time == 1)
    {
        newT = time;
    }
    else
    {
        float s = m_fPeriod / 4;
        time = time - 1;
        newT = -powf(2, 10 * time) * sinf((time - s) * M_PI_X_2 / m_fPeriod);
    }

    m_pInner->update(newT);
}

ActionInterval* EaseElasticIn::reverse(void)
{
    return EaseElasticOut::create(m_pInner->reverse(), m_fPeriod);
}

//
// EaseElasticOut
//

EaseElasticOut* EaseElasticOut::create(ActionInterval *pAction)
{
    return EaseElasticOut::create(pAction, 0.3f);
}

EaseElasticOut* EaseElasticOut::create(ActionInterval *pAction, float fPeriod/* = 0.3f*/)
{
    EaseElasticOut *pRet = new EaseElasticOut();
    if (pRet)
    {
        if (pRet->initWithAction(pAction, fPeriod))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object *EaseElasticOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseElasticOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseElasticOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseElasticOut();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()), m_fPeriod);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseElasticOut::update(float time)
{
    float newT = 0;
    if (time == 0 || time == 1)
    {
        newT = time;
    }
    else
    {
        float s = m_fPeriod / 4;
        newT = powf(2, -10 * time) * sinf((time - s) * M_PI_X_2 / m_fPeriod) + 1;
    }

    m_pInner->update(newT);
}

ActionInterval* EaseElasticOut::reverse(void)
{
    return EaseElasticIn::create(m_pInner->reverse(), m_fPeriod);
}

//
// EaseElasticInOut
//

EaseElasticInOut* EaseElasticInOut::create(ActionInterval *pAction)
{
    return EaseElasticInOut::create(pAction, 0.3f);
}

EaseElasticInOut* EaseElasticInOut::create(ActionInterval *pAction, float fPeriod/* = 0.3f*/)
{
    EaseElasticInOut *pRet = new EaseElasticInOut();
    if (pRet)
    {
        if (pRet->initWithAction(pAction, fPeriod))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object* EaseElasticInOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseElasticInOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseElasticInOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseElasticInOut();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()), m_fPeriod);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;

}

void EaseElasticInOut::update(float time)
{
    float newT = 0;
    if (time == 0 || time == 1)
    {
        newT = time;
    }
    else
    {
        time = time * 2;
        if (! m_fPeriod)
        {
            m_fPeriod = 0.3f * 1.5f;
        }

        float s = m_fPeriod / 4;

        time = time - 1;
        if (time < 0)
        {
            newT = -0.5f * powf(2, 10 * time) * sinf((time -s) * M_PI_X_2 / m_fPeriod);
        }
        else
        {
            newT = powf(2, -10 * time) * sinf((time - s) * M_PI_X_2 / m_fPeriod) * 0.5f + 1;
        }
    }

    m_pInner->update(newT);
}

ActionInterval* EaseElasticInOut::reverse(void)
{
    return EaseElasticInOut::create(m_pInner->reverse(), m_fPeriod);
}

//
// EaseBounce
//

EaseBounce* EaseBounce::create(ActionInterval* pAction)
{
    EaseBounce *pRet = new EaseBounce();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object* EaseBounce::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseBounce* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseBounce*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseBounce();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

float EaseBounce::bounceTime(float time)
{
    if (time < 1 / 2.75)
    {
        return 7.5625f * time * time;
    } else 
    if (time < 2 / 2.75)
    {
        time -= 1.5f / 2.75f;
        return 7.5625f * time * time + 0.75f;
    } else
    if(time < 2.5 / 2.75)
    {
        time -= 2.25f / 2.75f;
        return 7.5625f * time * time + 0.9375f;
    }

    time -= 2.625f / 2.75f;
    return 7.5625f * time * time + 0.984375f;
}

ActionInterval* EaseBounce::reverse()
{
    return EaseBounce::create(m_pInner->reverse());
}

//
// EaseBounceIn
//

EaseBounceIn* EaseBounceIn::create(ActionInterval* pAction)
{
    EaseBounceIn *pRet = new EaseBounceIn();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object* EaseBounceIn::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseBounceIn* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseBounceIn*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseBounceIn();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseBounceIn::update(float time)
{
    float newT = 1 - bounceTime(1 - time);
    m_pInner->update(newT);
}

ActionInterval* EaseBounceIn::reverse(void)
{
    return EaseBounceOut::create(m_pInner->reverse());
}

//
// EaseBounceOut
//

EaseBounceOut* EaseBounceOut::create(ActionInterval* pAction)
{
    EaseBounceOut *pRet = new EaseBounceOut();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object* EaseBounceOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseBounceOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (EaseBounceOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseBounceOut();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseBounceOut::update(float time)
{
    float newT = bounceTime(time);
    m_pInner->update(newT);
}

ActionInterval* EaseBounceOut::reverse(void)
{
    return EaseBounceIn::create(m_pInner->reverse());
}

//
// EaseBounceInOut
//

EaseBounceInOut* EaseBounceInOut::create(ActionInterval* pAction)
{
    EaseBounceInOut *pRet = new EaseBounceInOut();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet; 
}

Object* EaseBounceInOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseBounceInOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseBounceInOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseBounceInOut();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseBounceInOut::update(float time)
{
    float newT = 0;
    if (time < 0.5f)
    {
        time = time * 2;
        newT = (1 - bounceTime(1 - time)) * 0.5f;
    }
    else
    {
        newT = bounceTime(time * 2 - 1) * 0.5f + 0.5f;
    }

    m_pInner->update(newT);
}

ActionInterval* EaseBounceInOut::reverse()
{
    return EaseBounceInOut::create(m_pInner->reverse());
}

//
// EaseBackIn
//

EaseBackIn* EaseBackIn::create(ActionInterval *pAction)
{
    EaseBackIn *pRet = new EaseBackIn();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet;
}

Object* EaseBackIn::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseBackIn* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseBackIn*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseBackIn();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseBackIn::update(float time)
{
    float overshoot = 1.70158f;
    m_pInner->update(time * time * ((overshoot + 1) * time - overshoot));
}

ActionInterval* EaseBackIn::reverse(void)
{
    return EaseBackOut::create(m_pInner->reverse());
}

//
// EaseBackOut
//

EaseBackOut* EaseBackOut::create(ActionInterval* pAction)
{
    EaseBackOut *pRet = new EaseBackOut();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet;
}

Object* EaseBackOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseBackOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseBackOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseBackOut();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseBackOut::update(float time)
{
    float overshoot = 1.70158f;

    time = time - 1;
    m_pInner->update(time * time * ((overshoot + 1) * time + overshoot) + 1);
}

ActionInterval* EaseBackOut::reverse(void)
{
    return EaseBackIn::create(m_pInner->reverse());
}

//
// EaseBackInOut
//

EaseBackInOut* EaseBackInOut::create(ActionInterval* pAction)
{
    EaseBackInOut *pRet = new EaseBackInOut();
    if (pRet)
    {
        if (pRet->initWithAction(pAction))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pRet);
        }
    }

    return pRet;
}

Object* EaseBackInOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    EaseBackInOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (EaseBackInOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new EaseBackInOut();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAction((ActionInterval *)(m_pInner->copy()->autorelease()));
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void EaseBackInOut::update(float time)
{
    float overshoot = 1.70158f * 1.525f;

    time = time * 2;
    if (time < 1)
    {
        m_pInner->update((time * time * ((overshoot + 1) * time - overshoot)) / 2);
    }
    else
    {
        time = time - 2;
        m_pInner->update((time * time * ((overshoot + 1) * time + overshoot)) / 2 + 1);
    }
}

ActionInterval* EaseBackInOut::reverse()
{
    return EaseBackInOut::create(m_pInner->reverse());
}

NS_AX_END
