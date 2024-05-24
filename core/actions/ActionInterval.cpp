/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011 Zynga Inc.
 
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

#include "ActionInterval.h"
#include "sprite_nodes/Sprite.h"
#include "base/Node.h"
#include "support/PointExtension.h"
#include "StdC.h"
#include "ActionInstant.h"
#include "base/Zone.h"
#include <stdarg.h>

NS_AX_BEGIN

// Extra action for making a Sequence or Spawn when only adding one action to it.
class ExtraAction : public FiniteTimeAction
{
public:
    static ExtraAction* create();
    virtual Object* copyWithZone(Zone* pZone);
    virtual ExtraAction* reverse(void);
    virtual void update(float time);
    virtual void step(float dt);
};

ExtraAction* ExtraAction::create()
{
    ExtraAction* pRet = new ExtraAction();
    if (pRet)
    {
        pRet->autorelease();
    }
    return pRet;
}

Object* ExtraAction::copyWithZone(Zone* pZone)
{
    AX_UNUSED_PARAM(pZone);
    ExtraAction* pRet = new ExtraAction();
    return pRet;
}

ExtraAction* ExtraAction::reverse(void)
{
    return ExtraAction::create();
}

void ExtraAction::update(float time)
{
    AX_UNUSED_PARAM(time);
}

void ExtraAction::step(float dt)
{
    AX_UNUSED_PARAM(dt);
}

//
// IntervalAction
//
ActionInterval* ActionInterval::create(float d)
{
    ActionInterval *pAction = new ActionInterval();
    pAction->initWithDuration(d);
    pAction->autorelease();

    return pAction;
}

bool ActionInterval::initWithDuration(float d)
{
    m_fDuration = d;

    // prevent division by 0
    // This comparison could be in step:, but it might decrease the performance
    // by 3% in heavy based action games.
    if (m_fDuration == 0)
    {
        m_fDuration = FLT_EPSILON;
    }

    m_elapsed = 0;
    m_bFirstTick = true;

    return true;
}

Object* ActionInterval::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    ActionInterval* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (ActionInterval*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new ActionInterval();
        pZone = pNewZone = new Zone(pCopy);
    }

    
    FiniteTimeAction::copyWithZone(pZone);

    AX_SAFE_DELETE(pNewZone);

    pCopy->initWithDuration(m_fDuration);

    return pCopy;
}

bool ActionInterval::isDone(void)
{
    return m_elapsed >= m_fDuration;
}

void ActionInterval::step(float dt)
{
    if (m_bFirstTick)
    {
        m_bFirstTick = false;
        m_elapsed = 0;
    }
    else
    {
        m_elapsed += dt;
    }
    
    this->update(MAX (0,                                  // needed for rewind. elapsed could be negative
                      MIN(1, m_elapsed /
                          MAX(m_fDuration, FLT_EPSILON)   // division by 0
                          )
                      )
                 );
}

void ActionInterval::setAmplitudeRate(float amp)
{
    AX_UNUSED_PARAM(amp);
    // Abstract class needs implementation
    AXAssert(0, "");
}

float ActionInterval::getAmplitudeRate(void)
{
    // Abstract class needs implementation
    AXAssert(0, "");

    return 0;
}

void ActionInterval::startWithTarget(Node *pTarget)
{
    FiniteTimeAction::startWithTarget(pTarget);
    m_elapsed = 0.0f;
    m_bFirstTick = true;
}

ActionInterval* ActionInterval::reverse(void)
{
    AXAssert(false, "CCIntervalAction: reverse not implemented.");
    return NULL;
}

//
// Sequence
//

Sequence* Sequence::createWithTwoActions(FiniteTimeAction *pActionOne, FiniteTimeAction *pActionTwo)
{
    Sequence *pSequence = new Sequence();
    pSequence->initWithTwoActions(pActionOne, pActionTwo);
    pSequence->autorelease();

    return pSequence;
}

Sequence* Sequence::create(FiniteTimeAction *pAction1, ...)
{
    va_list params;
    va_start(params, pAction1);

    Sequence *pRet = Sequence::createWithVariableList(pAction1, params);

    va_end(params);
    
    return pRet;
}

Sequence* Sequence::createWithVariableList(FiniteTimeAction *pAction1, va_list args)
{
    FiniteTimeAction *pNow;
    FiniteTimeAction *pPrev = pAction1;
    bool bOneAction = true;

    while (pAction1)
    {
        pNow = va_arg(args, FiniteTimeAction*);
        if (pNow)
        {
            pPrev = createWithTwoActions(pPrev, pNow);
            bOneAction = false;
        }
        else
        {
            // If only one action is added to Sequence, make up a Sequence by adding a simplest finite time action.
            if (bOneAction)
            {
                pPrev = createWithTwoActions(pPrev, ExtraAction::create());
            }
            break;
        }
    }
    
    return ((Sequence*)pPrev);
}

Sequence* Sequence::create(Array* arrayOfActions)
{
    Sequence* pRet = NULL;
    do 
    {
        unsigned  int count = arrayOfActions->count();
        AX_BREAK_IF(count == 0);

        FiniteTimeAction* prev = (FiniteTimeAction*)arrayOfActions->objectAtIndex(0);

        if (count > 1)
        {
            for (unsigned int i = 1; i < count; ++i)
            {
                prev = createWithTwoActions(prev, (FiniteTimeAction*)arrayOfActions->objectAtIndex(i));
            }
        }
        else
        {
            // If only one action is added to Sequence, make up a Sequence by adding a simplest finite time action.
            prev = createWithTwoActions(prev, ExtraAction::create());
        }
        pRet = (Sequence*)prev;
    }while (0);
    return pRet;
}

bool Sequence::initWithTwoActions(FiniteTimeAction *pActionOne, FiniteTimeAction *pActionTwo)
{
    AXAssert(pActionOne != NULL, "");
    AXAssert(pActionTwo != NULL, "");

    float d = pActionOne->getDuration() + pActionTwo->getDuration();
    ActionInterval::initWithDuration(d);

    m_pActions[0] = pActionOne;
    pActionOne->retain();

    m_pActions[1] = pActionTwo;
    pActionTwo->retain();

    return true;
}

Object* Sequence::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    Sequence* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (Sequence*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new Sequence();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithTwoActions((FiniteTimeAction*)(m_pActions[0]->copy()->autorelease()), 
                (FiniteTimeAction*)(m_pActions[1]->copy()->autorelease()));

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

Sequence::~Sequence(void)
{
    AX_SAFE_RELEASE(m_pActions[0]);
    AX_SAFE_RELEASE(m_pActions[1]);
}

void Sequence::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_split = m_pActions[0]->getDuration() / m_fDuration;
    m_last = -1;
}

void Sequence::stop(void)
{
    // Issue #1305
    if( m_last != - 1)
    {
        m_pActions[m_last]->stop();
    }

    ActionInterval::stop();
}

void Sequence::update(float t)
{
    int found = 0;
    float new_t = 0.0f;

    if( t < m_split ) {
        // action[0]
        found = 0;
        if( m_split != 0 )
            new_t = t / m_split;
        else
            new_t = 1;

    } else {
        // action[1]
        found = 1;
        if ( m_split == 1 )
            new_t = 1;
        else
            new_t = (t-m_split) / (1 - m_split );
    }

    if ( found==1 ) {

        if( m_last == -1 ) {
            // action[0] was skipped, execute it.
            m_pActions[0]->startWithTarget(m_pTarget);
            m_pActions[0]->update(1.0f);
            m_pActions[0]->stop();
        }
        else if( m_last == 0 )
        {
            // switching to action 1. stop action 0.
            m_pActions[0]->update(1.0f);
            m_pActions[0]->stop();
        }
    }
	else if(found==0 && m_last==1 )
	{
		// Reverse mode ?
		// XXX: Bug. this case doesn't contemplate when _last==-1, found=0 and in "reverse mode"
		// since it will require a hack to know if an action is on reverse mode or not.
		// "step" should be overriden, and the "reverseMode" value propagated to inner Sequences.
		m_pActions[1]->update(0);
		m_pActions[1]->stop();
	}
    // Last action found and it is done.
    if( found == m_last && m_pActions[found]->isDone() )
    {
        return;
    }

    // Last action found and it is done
    if( found != m_last )
    {
        m_pActions[found]->startWithTarget(m_pTarget);
    }

    m_pActions[found]->update(new_t);
    m_last = found;
}

ActionInterval* Sequence::reverse(void)
{
    return Sequence::createWithTwoActions(m_pActions[1]->reverse(), m_pActions[0]->reverse());
}

//
// Repeat
//

Repeat* Repeat::create(FiniteTimeAction *pAction, unsigned int times)
{
    Repeat* pRepeat = new Repeat();
    pRepeat->initWithAction(pAction, times);
    pRepeat->autorelease();

    return pRepeat;
}

bool Repeat::initWithAction(FiniteTimeAction *pAction, unsigned int times)
{
    float d = pAction->getDuration() * times;

    if (ActionInterval::initWithDuration(d))
    {
        m_uTimes = times;
        m_pInnerAction = pAction;
        pAction->retain();

        m_bActionInstant = dynamic_cast<ActionInstant*>(pAction) ? true : false;
        //an instant action needs to be executed one time less in the update method since it uses startWithTarget to execute the action
        if (m_bActionInstant) 
        {
            m_uTimes -=1;
        }
        m_uTotal = 0;

        return true;
    }

    return false;
}

Object* Repeat::copyWithZone(Zone *pZone)
{
    
    Zone* pNewZone = NULL;
    Repeat* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (Repeat*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new Repeat();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithAction((FiniteTimeAction*)(m_pInnerAction->copy()->autorelease()), m_uTimes);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

Repeat::~Repeat(void)
{
    AX_SAFE_RELEASE(m_pInnerAction);
}

void Repeat::startWithTarget(Node *pTarget)
{
    m_uTotal = 0;
    m_fNextDt = m_pInnerAction->getDuration()/m_fDuration;
    ActionInterval::startWithTarget(pTarget);
    m_pInnerAction->startWithTarget(pTarget);
}

void Repeat::stop(void)
{
    m_pInnerAction->stop();
    ActionInterval::stop();
}

// issue #80. Instead of hooking step:, hook update: since it can be called by any 
// container action like Repeat, Sequence, CCEase, etc..
void Repeat::update(float dt)
{
    if (dt >= m_fNextDt)
    {
        while (dt > m_fNextDt && m_uTotal < m_uTimes)
        {

            m_pInnerAction->update(1.0f);
            m_uTotal++;

            m_pInnerAction->stop();
            m_pInnerAction->startWithTarget(m_pTarget);
            m_fNextDt += m_pInnerAction->getDuration()/m_fDuration;
        }

        // fix for issue #1288, incorrect end value of repeat
        if(dt >= 1.0f && m_uTotal < m_uTimes) 
        {
            m_uTotal++;
        }

        // don't set an instant action back or update it, it has no use because it has no duration
        if (!m_bActionInstant)
        {
            if (m_uTotal == m_uTimes)
            {
                m_pInnerAction->update(1);
                m_pInnerAction->stop();
            }
            else
            {
                // issue #390 prevent jerk, use right update
                m_pInnerAction->update(dt - (m_fNextDt - m_pInnerAction->getDuration()/m_fDuration));
            }
        }
    }
    else
    {
        m_pInnerAction->update(fmodf(dt * m_uTimes,1.0f));
    }
}

bool Repeat::isDone(void)
{
    return m_uTotal == m_uTimes;
}

ActionInterval* Repeat::reverse(void)
{
    return Repeat::create(m_pInnerAction->reverse(), m_uTimes);
}

//
// RepeatForever
//
RepeatForever::~RepeatForever()
{
    AX_SAFE_RELEASE(m_pInnerAction);
}

RepeatForever *RepeatForever::create(ActionInterval *pAction)
{
    RepeatForever *pRet = new RepeatForever();
    if (pRet && pRet->initWithAction(pAction))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool RepeatForever::initWithAction(ActionInterval *pAction)
{
    AXAssert(pAction != NULL, "");
    pAction->retain();
    m_pInnerAction = pAction;
    return true;
}
Object* RepeatForever::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    RepeatForever* pRet = NULL;
    if(pZone && pZone->m_pCopyObject) //in case of being called at sub class
    {
        pRet = (RepeatForever*)(pZone->m_pCopyObject);
    }
    else
    {
        pRet = new RepeatForever();
        pZone = pNewZone = new Zone(pRet);
    }
    ActionInterval::copyWithZone(pZone);
    // win32 : use the m_pOther's copy object.
    pRet->initWithAction((ActionInterval*)(m_pInnerAction->copy()->autorelease())); 
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

void RepeatForever::startWithTarget(Node* pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_pInnerAction->startWithTarget(pTarget);
}

void RepeatForever::step(float dt)
{
    m_pInnerAction->step(dt);
    if (m_pInnerAction->isDone())
    {
        float diff = m_pInnerAction->getElapsed() - m_pInnerAction->getDuration();
        m_pInnerAction->startWithTarget(m_pTarget);
        // to prevent jerk. issue #390, 1247
        m_pInnerAction->step(0.0f);
        m_pInnerAction->step(diff);
    }
}

bool RepeatForever::isDone()
{
    return false;
}

ActionInterval *RepeatForever::reverse()
{
    return (ActionInterval*)(RepeatForever::create(m_pInnerAction->reverse()));
}

//
// Spawn
//

Spawn* Spawn::create(FiniteTimeAction *pAction1, ...)
{
    va_list params;
    va_start(params, pAction1);

    Spawn *pRet = Spawn::createWithVariableList(pAction1, params);

    va_end(params);
    
    return pRet;
}

Spawn* Spawn::createWithVariableList(FiniteTimeAction *pAction1, va_list args)
{
    FiniteTimeAction *pNow;
    FiniteTimeAction *pPrev = pAction1;
    bool bOneAction = true;

    while (pAction1)
    {
        pNow = va_arg(args, FiniteTimeAction*);
        if (pNow)
        {
            pPrev = createWithTwoActions(pPrev, pNow);
            bOneAction = false;
        }
        else
        {
            // If only one action is added to Spawn, make up a Spawn by adding a simplest finite time action.
            if (bOneAction)
            {
                pPrev = createWithTwoActions(pPrev, ExtraAction::create());
            }
            break;
        }
    }

    return ((Spawn*)pPrev);
}

Spawn* Spawn::create(Array *arrayOfActions)
{
    Spawn* pRet = NULL;
    do 
    {
        unsigned  int count = arrayOfActions->count();
        AX_BREAK_IF(count == 0);
        FiniteTimeAction* prev = (FiniteTimeAction*)arrayOfActions->objectAtIndex(0);
        if (count > 1)
        {
            for (unsigned int i = 1; i < arrayOfActions->count(); ++i)
            {
                prev = createWithTwoActions(prev, (FiniteTimeAction*)arrayOfActions->objectAtIndex(i));
            }
        }
        else
        {
            // If only one action is added to Spawn, make up a Spawn by adding a simplest finite time action.
            prev = createWithTwoActions(prev, ExtraAction::create());
        }
        pRet = (Spawn*)prev;
    }while (0);

    return pRet;
}

Spawn* Spawn::createWithTwoActions(FiniteTimeAction *pAction1, FiniteTimeAction *pAction2)
{
    Spawn *pSpawn = new Spawn();
    pSpawn->initWithTwoActions(pAction1, pAction2);
    pSpawn->autorelease();

    return pSpawn;
}

bool Spawn:: initWithTwoActions(FiniteTimeAction *pAction1, FiniteTimeAction *pAction2)
{
    AXAssert(pAction1 != NULL, "");
    AXAssert(pAction2 != NULL, "");

    bool bRet = false;

    float d1 = pAction1->getDuration();
    float d2 = pAction2->getDuration();

    if (ActionInterval::initWithDuration(MAX(d1, d2)))
    {
        m_pOne = pAction1;
        m_pTwo = pAction2;

        if (d1 > d2)
        {
            m_pTwo = Sequence::createWithTwoActions(pAction2, DelayTime::create(d1 - d2));
        } 
        else if (d1 < d2)
        {
            m_pOne = Sequence::createWithTwoActions(pAction1, DelayTime::create(d2 - d1));
        }

        m_pOne->retain();
        m_pTwo->retain();

        bRet = true;
    }

    
    return bRet;
}

Object* Spawn::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    Spawn* pCopy = NULL;

    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (Spawn*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new Spawn();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithTwoActions((FiniteTimeAction*)(m_pOne->copy()->autorelease()), 
                    (FiniteTimeAction*)(m_pTwo->copy()->autorelease()));

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

Spawn::~Spawn(void)
{
    AX_SAFE_RELEASE(m_pOne);
    AX_SAFE_RELEASE(m_pTwo);
}

void Spawn::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_pOne->startWithTarget(pTarget);
    m_pTwo->startWithTarget(pTarget);
}

void Spawn::stop(void)
{
    m_pOne->stop();
    m_pTwo->stop();
    ActionInterval::stop();
}

void Spawn::update(float time)
{
    if (m_pOne)
    {
        m_pOne->update(time);
    }
    if (m_pTwo)
    {
        m_pTwo->update(time);
    }
}

ActionInterval* Spawn::reverse(void)
{
    return Spawn::createWithTwoActions(m_pOne->reverse(), m_pTwo->reverse());
}

//
// RotateTo
//

RotateTo* RotateTo::create(float fDuration, float fDeltaAngle)
{
    RotateTo* pRotateTo = new RotateTo();
    pRotateTo->initWithDuration(fDuration, fDeltaAngle);
    pRotateTo->autorelease();

    return pRotateTo;
}

bool RotateTo::initWithDuration(float fDuration, float fDeltaAngle)
{
    if (ActionInterval::initWithDuration(fDuration))
    {
        m_fDstAngleX = m_fDstAngleY = fDeltaAngle;
        return true;
    }

    return false;
}

RotateTo* RotateTo::create(float fDuration, float fDeltaAngleX, float fDeltaAngleY)
{
    RotateTo* pRotateTo = new RotateTo();
    pRotateTo->initWithDuration(fDuration, fDeltaAngleX, fDeltaAngleY);
    pRotateTo->autorelease();
    
    return pRotateTo;
}

bool RotateTo::initWithDuration(float fDuration, float fDeltaAngleX, float fDeltaAngleY)
{
    if (ActionInterval::initWithDuration(fDuration))
    {
        m_fDstAngleX = fDeltaAngleX;
        m_fDstAngleY = fDeltaAngleY;
        
        return true;
    }
    
    return false;
}

Object* RotateTo::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    RotateTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (RotateTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new RotateTo();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_fDstAngleX, m_fDstAngleY);

    //Action *copy = [[[self class] allocWithZone: zone] initWithDuration:[self duration] angle: angle];
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void RotateTo::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    
    // Calculate X
    m_fStartAngleX = pTarget->getRotationX();
    if (m_fStartAngleX > 0)
    {
        m_fStartAngleX = fmodf(m_fStartAngleX, 360.0f);
    }
    else
    {
        m_fStartAngleX = fmodf(m_fStartAngleX, -360.0f);
    }

    m_fDiffAngleX = m_fDstAngleX - m_fStartAngleX;
    if (m_fDiffAngleX > 180)
    {
        m_fDiffAngleX -= 360;
    }
    if (m_fDiffAngleX < -180)
    {
        m_fDiffAngleX += 360;
    }
    
    //Calculate Y: It's duplicated from calculating X since the rotation wrap should be the same
    m_fStartAngleY = m_pTarget->getRotationY();

    if (m_fStartAngleY > 0)
    {
        m_fStartAngleY = fmodf(m_fStartAngleY, 360.0f);
    }
    else
    {
        m_fStartAngleY = fmodf(m_fStartAngleY, -360.0f);
    }

    m_fDiffAngleY = m_fDstAngleY - m_fStartAngleY;
    if (m_fDiffAngleY > 180)
    {
        m_fDiffAngleY -= 360;
    }

    if (m_fDiffAngleY < -180)
    {
        m_fDiffAngleY += 360;
    }
}

void RotateTo::update(float time)
{
    if (m_pTarget)
    {
        m_pTarget->setRotationX(m_fStartAngleX + m_fDiffAngleX * time);
        m_pTarget->setRotationY(m_fStartAngleY + m_fDiffAngleY * time);
    }
}

//
// RotateBy
//

RotateBy* RotateBy::create(float fDuration, float fDeltaAngle)
{
    RotateBy *pRotateBy = new RotateBy();
    pRotateBy->initWithDuration(fDuration, fDeltaAngle);
    pRotateBy->autorelease();

    return pRotateBy;
}

bool RotateBy::initWithDuration(float fDuration, float fDeltaAngle)
{
    if (ActionInterval::initWithDuration(fDuration))
    {
        m_fAngleX = m_fAngleY = fDeltaAngle;
        return true;
    }

    return false;
}

RotateBy* RotateBy::create(float fDuration, float fDeltaAngleX, float fDeltaAngleY)
{
    RotateBy *pRotateBy = new RotateBy();
    pRotateBy->initWithDuration(fDuration, fDeltaAngleX, fDeltaAngleY);
    pRotateBy->autorelease();
    
    return pRotateBy;
}

bool RotateBy::initWithDuration(float fDuration, float fDeltaAngleX, float fDeltaAngleY)
{
    if (ActionInterval::initWithDuration(fDuration))
    {
        m_fAngleX = fDeltaAngleX;
        m_fAngleY = fDeltaAngleY;
        return true;
    }
    
    return false;
}

Object* RotateBy::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    RotateBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (RotateBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new RotateBy();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_fAngleX, m_fAngleY);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void RotateBy::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_fStartAngleX = pTarget->getRotationX();
    m_fStartAngleY = pTarget->getRotationY();
}

void RotateBy::update(float time)
{
    // XXX: shall I add % 360
    if (m_pTarget)
    {
        m_pTarget->setRotationX(m_fStartAngleX + m_fAngleX * time);
        m_pTarget->setRotationY(m_fStartAngleY + m_fAngleY * time);
    }
}

ActionInterval* RotateBy::reverse(void)
{
    return RotateBy::create(m_fDuration, -m_fAngleX, -m_fAngleY);
}

//
// MoveBy
//

MoveBy* MoveBy::create(float duration, const Vec2& deltaPosition)
{
    MoveBy *pRet = new MoveBy();
    pRet->initWithDuration(duration, deltaPosition);
    pRet->autorelease();

    return pRet;
}

bool MoveBy::initWithDuration(float duration, const Vec2& deltaPosition)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_positionDelta = deltaPosition;
        return true;
    }

    return false;
}

Object* MoveBy::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    MoveBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (MoveBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new MoveBy();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_positionDelta);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void MoveBy::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_previousPosition = m_startPosition = pTarget->getPosition();
}

ActionInterval* MoveBy::reverse(void)
{
    return MoveBy::create(m_fDuration, Vec2( -m_positionDelta.x, -m_positionDelta.y));
}


void MoveBy::update(float t)
{
    if (m_pTarget)
    {
#if AX_ENABLE_STACKABLE_ACTIONS
        Vec2 currentPos = m_pTarget->getPosition();
        Vec2 diff = PointSub(currentPos, m_previousPosition);
        m_startPosition = PointAdd( m_startPosition, diff);
        Vec2 newPos =  PointAdd( m_startPosition, PointMult(m_positionDelta, t) );
        m_pTarget->setPosition(newPos);
        m_previousPosition = newPos;
#else
        m_pTarget->setPosition(PointAdd( m_startPosition, PointMult(m_positionDelta, t)));
#endif // AX_ENABLE_STACKABLE_ACTIONS
    }
}

//
// MoveTo
//

MoveTo* MoveTo::create(float duration, const Vec2& position)
{
    MoveTo *pRet = new MoveTo();
    pRet->initWithDuration(duration, position);
    pRet->autorelease();

    return pRet;
}

bool MoveTo::initWithDuration(float duration, const Vec2& position)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_endPosition = position;
        return true;
    }

    return false;
}

Object* MoveTo::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    MoveTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (MoveTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new MoveTo();
        pZone = pNewZone = new Zone(pCopy);
    }

    MoveBy::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_endPosition);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void MoveTo::startWithTarget(Node *pTarget)
{
    MoveBy::startWithTarget(pTarget);
    m_positionDelta = PointSub( m_endPosition, pTarget->getPosition() );
}


//
// SkewTo
//
SkewTo* SkewTo::create(float t, float sx, float sy)
{
    SkewTo *pSkewTo = new SkewTo();
    if (pSkewTo)
    {
        if (pSkewTo->initWithDuration(t, sx, sy))
        {
            pSkewTo->autorelease();
        }
        else
        {
            AX_SAFE_DELETE(pSkewTo);
        }
    }

    return pSkewTo;
}

bool SkewTo::initWithDuration(float t, float sx, float sy)
{
    bool bRet = false;

    if (ActionInterval::initWithDuration(t))
    {
        m_fEndSkewX = sx;
        m_fEndSkewY = sy;

        bRet = true;
    }

    return bRet;
}

Object* SkewTo::copyWithZone(Zone* pZone)
{
    Zone* pNewZone = NULL;
    SkewTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (SkewTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new SkewTo();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_fEndSkewX, m_fEndSkewY);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void SkewTo::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);

    m_fStartSkewX = pTarget->getSkewX();

    if (m_fStartSkewX > 0)
    {
        m_fStartSkewX = fmodf(m_fStartSkewX, 180.f);
    }
    else
    {
        m_fStartSkewX = fmodf(m_fStartSkewX, -180.f);
    }

    m_fDeltaX = m_fEndSkewX - m_fStartSkewX;

    if (m_fDeltaX > 180)
    {
        m_fDeltaX -= 360;
    }
    if (m_fDeltaX < -180)
    {
        m_fDeltaX += 360;
    }

    m_fStartSkewY = pTarget->getSkewY();

    if (m_fStartSkewY > 0)
    {
        m_fStartSkewY = fmodf(m_fStartSkewY, 360.f);
    }
    else
    {
        m_fStartSkewY = fmodf(m_fStartSkewY, -360.f);
    }

    m_fDeltaY = m_fEndSkewY - m_fStartSkewY;

    if (m_fDeltaY > 180)
    {
        m_fDeltaY -= 360;
    }
    if (m_fDeltaY < -180)
    {
        m_fDeltaY += 360;
    }
}

void SkewTo::update(float t)
{
    m_pTarget->setSkewX(m_fStartSkewX + m_fDeltaX * t);
    m_pTarget->setSkewY(m_fStartSkewY + m_fDeltaY * t);
}

SkewTo::SkewTo()
: m_fSkewX(0.0)
, m_fSkewY(0.0)
, m_fStartSkewX(0.0)
, m_fStartSkewY(0.0)
, m_fEndSkewX(0.0)
, m_fEndSkewY(0.0)
, m_fDeltaX(0.0)
, m_fDeltaY(0.0)
{
}

//
// SkewBy
//
SkewBy* SkewBy::create(float t, float sx, float sy)
{
    SkewBy *pSkewBy = new SkewBy();
    if (pSkewBy)
    {
        if (pSkewBy->initWithDuration(t, sx, sy))
        {
            pSkewBy->autorelease();
        }
        else
        {
            AX_SAFE_DELETE(pSkewBy);
        }
    }

    return pSkewBy;
}

bool SkewBy::initWithDuration(float t, float deltaSkewX, float deltaSkewY)
{
    bool bRet = false;

    if (SkewTo::initWithDuration(t, deltaSkewX, deltaSkewY))
    {
        m_fSkewX = deltaSkewX;
        m_fSkewY = deltaSkewY;

        bRet = true;
    }

    return bRet;
}

void SkewBy::startWithTarget(Node *pTarget)
{
    SkewTo::startWithTarget(pTarget);
    m_fDeltaX = m_fSkewX;
    m_fDeltaY = m_fSkewY;
    m_fEndSkewX = m_fStartSkewX + m_fDeltaX;
    m_fEndSkewY = m_fStartSkewY + m_fDeltaY;
}

ActionInterval* SkewBy::reverse()
{
    return create(m_fDuration, -m_fSkewX, -m_fSkewY);
}

//
// JumpBy
//

JumpBy* JumpBy::create(float duration, const Vec2& position, float height, unsigned int jumps)
{
    JumpBy *pJumpBy = new JumpBy();
    pJumpBy->initWithDuration(duration, position, height, jumps);
    pJumpBy->autorelease();

    return pJumpBy;
}

bool JumpBy::initWithDuration(float duration, const Vec2& position, float height, unsigned int jumps)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_delta = position;
        m_height = height;
        m_nJumps = jumps;

        return true;
    }

    return false;
}

Object* JumpBy::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    JumpBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (JumpBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new JumpBy();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_delta, m_height, m_nJumps);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void JumpBy::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_previousPos = m_startPosition = pTarget->getPosition();
}

void JumpBy::update(float t)
{
    // parabolic jump (since v0.8.2)
    if (m_pTarget)
    {
        float frac = fmodf( t * m_nJumps, 1.0f );
        float y = m_height * 4 * frac * (1 - frac);
        y += m_delta.y * t;

        float x = m_delta.x * t;
#if AX_ENABLE_STACKABLE_ACTIONS
        Vec2 currentPos = m_pTarget->getPosition();

        Vec2 diff = PointSub( currentPos, m_previousPos );
        m_startPosition = PointAdd( diff, m_startPosition);

        Vec2 newPos = PointAdd( m_startPosition, Vec2(x,y));
        m_pTarget->setPosition(newPos);

        m_previousPos = newPos;
#else
        m_pTarget->setPosition(PointAdd( m_startPosition, Vec2(x,y)));
#endif // !AX_ENABLE_STACKABLE_ACTIONS
    }
}

ActionInterval* JumpBy::reverse(void)
{
    return JumpBy::create(m_fDuration, Vec2(-m_delta.x, -m_delta.y),
        m_height, m_nJumps);
}

//
// JumpTo
//

JumpTo* JumpTo::create(float duration, const Vec2& position, float height, int jumps)
{
    JumpTo *pJumpTo = new JumpTo();
    pJumpTo->initWithDuration(duration, position, height, jumps);
    pJumpTo->autorelease();

    return pJumpTo;
}

Object* JumpTo::copyWithZone(Zone* pZone)
{
    Zone* pNewZone = NULL;
    JumpTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (JumpTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new JumpTo();
        pZone = pNewZone = new Zone(pCopy);
    }

    JumpBy::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_delta, m_height, m_nJumps);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void JumpTo::startWithTarget(Node *pTarget)
{
    JumpBy::startWithTarget(pTarget);
    m_delta = Vec2(m_delta.x - m_startPosition.x, m_delta.y - m_startPosition.y);
}

// Bezier cubic formula:
//    ((1 - t) + t)3 = 1 
// Expands to 
//   (1 - t)3 + 3t(1-t)2 + 3t2(1 - t) + t3 = 1 
static inline float bezierat( float a, float b, float c, float d, float t )
{
    return (powf(1-t,3) * a + 
            3*t*(powf(1-t,2))*b + 
            3*powf(t,2)*(1-t)*c +
            powf(t,3)*d );
}

//
// BezierBy
//

BezierBy* BezierBy::create(float t, const ccBezierConfig& c)
{
    BezierBy *pBezierBy = new BezierBy();
    pBezierBy->initWithDuration(t, c);
    pBezierBy->autorelease();

    return pBezierBy;
}

bool BezierBy::initWithDuration(float t, const ccBezierConfig& c)
{
    if (ActionInterval::initWithDuration(t))
    {
        m_sConfig = c;
        return true;
    }

    return false;
}

void BezierBy::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_previousPosition = m_startPosition = pTarget->getPosition();
}

Object* BezierBy::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    BezierBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (BezierBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new BezierBy();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sConfig);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void BezierBy::update(float time)
{
    if (m_pTarget)
    {
        float xa = 0;
        float xb = m_sConfig.controlPoint_1.x;
        float xc = m_sConfig.controlPoint_2.x;
        float xd = m_sConfig.endPosition.x;

        float ya = 0;
        float yb = m_sConfig.controlPoint_1.y;
        float yc = m_sConfig.controlPoint_2.y;
        float yd = m_sConfig.endPosition.y;

        float x = bezierat(xa, xb, xc, xd, time);
        float y = bezierat(ya, yb, yc, yd, time);

#if AX_ENABLE_STACKABLE_ACTIONS
        Vec2 currentPos = m_pTarget->getPosition();
        Vec2 diff = PointSub(currentPos, m_previousPosition);
        m_startPosition = PointAdd( m_startPosition, diff);

        Vec2 newPos = PointAdd( m_startPosition, Vec2(x,y));
        m_pTarget->setPosition(newPos);

        m_previousPosition = newPos;
#else
        m_pTarget->setPosition(PointAdd( m_startPosition, Vec2(x,y)));
#endif // !AX_ENABLE_STACKABLE_ACTIONS
    }
}

ActionInterval* BezierBy::reverse(void)
{
    ccBezierConfig r;

    r.endPosition = PointNeg(m_sConfig.endPosition);
    r.controlPoint_1 = PointAdd(m_sConfig.controlPoint_2, PointNeg(m_sConfig.endPosition));
    r.controlPoint_2 = PointAdd(m_sConfig.controlPoint_1, PointNeg(m_sConfig.endPosition));

    BezierBy *pAction = BezierBy::create(m_fDuration, r);
    return pAction;
}

//
// BezierTo
//

BezierTo* BezierTo::create(float t, const ccBezierConfig& c)
{
    BezierTo *pBezierTo = new BezierTo();
    pBezierTo->initWithDuration(t, c);
    pBezierTo->autorelease();

    return pBezierTo;
}

bool BezierTo::initWithDuration(float t, const ccBezierConfig &c)
{
    bool bRet = false;
    
    if (ActionInterval::initWithDuration(t))
    {
        m_sToConfig = c;
    }
    
    return bRet;
}

Object* BezierTo::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    BezierBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (BezierTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new BezierTo();
        pZone = pNewZone = new Zone(pCopy);
    }

    BezierBy::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sConfig);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void BezierTo::startWithTarget(Node *pTarget)
{
    BezierBy::startWithTarget(pTarget);
    m_sConfig.controlPoint_1 = PointSub(m_sToConfig.controlPoint_1, m_startPosition);
    m_sConfig.controlPoint_2 = PointSub(m_sToConfig.controlPoint_2, m_startPosition);
    m_sConfig.endPosition = PointSub(m_sToConfig.endPosition, m_startPosition);
}

//
// ScaleTo
//
ScaleTo* ScaleTo::create(float duration, float s)
{
    ScaleTo *pScaleTo = new ScaleTo();
    pScaleTo->initWithDuration(duration, s);
    pScaleTo->autorelease();

    return pScaleTo;
}

bool ScaleTo::initWithDuration(float duration, float s)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_fEndScaleX = s;
        m_fEndScaleY = s;

        return true;
    }

    return false;
}

ScaleTo* ScaleTo::create(float duration, float sx, float sy)
{
    ScaleTo *pScaleTo = new ScaleTo();
    pScaleTo->initWithDuration(duration, sx, sy);
    pScaleTo->autorelease();

    return pScaleTo;
}

bool ScaleTo::initWithDuration(float duration, float sx, float sy)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_fEndScaleX = sx;
        m_fEndScaleY = sy;

        return true;
    }

    return false;
}

Object* ScaleTo::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    ScaleTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (ScaleTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new ScaleTo();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);


    pCopy->initWithDuration(m_fDuration, m_fEndScaleX, m_fEndScaleY);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void ScaleTo::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_fStartScaleX = pTarget->getScaleX();
    m_fStartScaleY = pTarget->getScaleY();
    m_fDeltaX = m_fEndScaleX - m_fStartScaleX;
    m_fDeltaY = m_fEndScaleY - m_fStartScaleY;
}

void ScaleTo::update(float time)
{
    if (m_pTarget)
    {
        m_pTarget->setScaleX(m_fStartScaleX + m_fDeltaX * time);
        m_pTarget->setScaleY(m_fStartScaleY + m_fDeltaY * time);
    }
}

//
// ScaleBy
//

ScaleBy* ScaleBy::create(float duration, float s)
{
    ScaleBy *pScaleBy = new ScaleBy();
    pScaleBy->initWithDuration(duration, s);
    pScaleBy->autorelease();

    return pScaleBy;
}

ScaleBy* ScaleBy::create(float duration, float sx, float sy)
{
    ScaleBy *pScaleBy = new ScaleBy();
    pScaleBy->initWithDuration(duration, sx, sy);
    pScaleBy->autorelease();

    return pScaleBy;
}

Object* ScaleBy::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    ScaleTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (ScaleBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new ScaleBy();
        pZone = pNewZone = new Zone(pCopy);
    }

    ScaleTo::copyWithZone(pZone);


    pCopy->initWithDuration(m_fDuration, m_fEndScaleX, m_fEndScaleY);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void ScaleBy::startWithTarget(Node *pTarget)
{
    ScaleTo::startWithTarget(pTarget);
    m_fDeltaX = m_fStartScaleX * m_fEndScaleX - m_fStartScaleX;
    m_fDeltaY = m_fStartScaleY * m_fEndScaleY - m_fStartScaleY;
}

ActionInterval* ScaleBy::reverse(void)
{
    return ScaleBy::create(m_fDuration, 1 / m_fEndScaleX, 1 / m_fEndScaleY);
}

//
// Blink
//

Blink* Blink::create(float duration, unsigned int uBlinks)
{
    Blink *pBlink = new Blink();
    pBlink->initWithDuration(duration, uBlinks);
    pBlink->autorelease();

    return pBlink;
}

bool Blink::initWithDuration(float duration, unsigned int uBlinks)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_nTimes = uBlinks;
        return true;
    }

    return false;
}

void Blink::stop()
{
    m_pTarget->setVisible(m_bOriginalState);
    ActionInterval::stop();
}

void Blink::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_bOriginalState = pTarget->isVisible();
}

Object* Blink::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    Blink* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (Blink*)(pZone->m_pCopyObject);

    }
    else
    {
        pCopy = new Blink();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, (unsigned int)m_nTimes);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void Blink::update(float time)
{
    if (m_pTarget && ! isDone())
    {
        float slice = 1.0f / m_nTimes;
        float m = fmodf(time, slice);
        m_pTarget->setVisible(m > slice / 2 ? true : false);
    }
}

ActionInterval* Blink::reverse(void)
{
    // return 'self'
    return Blink::create(m_fDuration, m_nTimes);
}

//
// FadeIn
//

FadeIn* FadeIn::create(float d)
{
    FadeIn* pAction = new FadeIn();

    pAction->initWithDuration(d);
    pAction->autorelease();

    return pAction;
}

Object* FadeIn::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    FadeIn* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (FadeIn*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new FadeIn();
        pZone = pNewZone = new Zone(pCopy);
    }
    
    ActionInterval::copyWithZone(pZone);

    AX_SAFE_DELETE(pNewZone);

    return pCopy;
}

void FadeIn::update(float time)
{
    RGBAProtocol *pRGBAProtocol = dynamic_cast<RGBAProtocol*>(m_pTarget);
    if (pRGBAProtocol)
    {
        pRGBAProtocol->setOpacity((GLubyte)(255 * time));
    }
    /*m_pTarget->setOpacity((GLubyte)(255 * time));*/
}

ActionInterval* FadeIn::reverse(void)
{
    return FadeOut::create(m_fDuration);
}

//
// FadeOut
//

FadeOut* FadeOut::create(float d)
{
    FadeOut* pAction = new FadeOut();

    pAction->initWithDuration(d);
    pAction->autorelease();

    return pAction;
}

Object* FadeOut::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    FadeOut* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (FadeOut*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new FadeOut();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    AX_SAFE_DELETE(pNewZone);

    return pCopy;
}

void FadeOut::update(float time)
{
    RGBAProtocol *pRGBAProtocol = dynamic_cast<RGBAProtocol*>(m_pTarget);
    if (pRGBAProtocol)
    {
        pRGBAProtocol->setOpacity(GLubyte(255 * (1 - time)));
    }
    /*m_pTarget->setOpacity(GLubyte(255 * (1 - time)));*/    
}

ActionInterval* FadeOut::reverse(void)
{
    return FadeIn::create(m_fDuration);
}

//
// FadeTo
//

FadeTo* FadeTo::create(float duration, GLubyte opacity)
{
    FadeTo *pFadeTo = new FadeTo();
    pFadeTo->initWithDuration(duration, opacity);
    pFadeTo->autorelease();

    return pFadeTo;
}

bool FadeTo::initWithDuration(float duration, GLubyte opacity)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_toOpacity = opacity;
        return true;
    }

    return false;
}

Object* FadeTo::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    FadeTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (FadeTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new FadeTo();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_toOpacity);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void FadeTo::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);

    RGBAProtocol *pRGBAProtocol = dynamic_cast<RGBAProtocol*>(pTarget);
    if (pRGBAProtocol)
    {
        m_fromOpacity = pRGBAProtocol->getOpacity();
    }
    /*m_fromOpacity = pTarget->getOpacity();*/
}

void FadeTo::update(float time)
{
    RGBAProtocol *pRGBAProtocol = dynamic_cast<RGBAProtocol*>(m_pTarget);
    if (pRGBAProtocol)
    {
        pRGBAProtocol->setOpacity((GLubyte)(m_fromOpacity + (m_toOpacity - m_fromOpacity) * time));
    }
    /*m_pTarget->setOpacity((GLubyte)(m_fromOpacity + (m_toOpacity - m_fromOpacity) * time));*/
}

//
// TintTo
//
TintTo* TintTo::create(float duration, GLubyte red, GLubyte green, GLubyte blue)
{
    TintTo *pTintTo = new TintTo();
    pTintTo->initWithDuration(duration, red, green, blue);
    pTintTo->autorelease();

    return pTintTo;
}

bool TintTo::initWithDuration(float duration, GLubyte red, GLubyte green, GLubyte blue)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_to = ccc3(red, green, blue);
        return true;
    }

    return false;
}

Object* TintTo::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    TintTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (TintTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new TintTo();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_to.r, m_to.g, m_to.b);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void TintTo::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    RGBAProtocol *pRGBAProtocol = dynamic_cast<RGBAProtocol*>(m_pTarget);
    if (pRGBAProtocol)
    {
        m_from = pRGBAProtocol->getColor();
    }
    /*m_from = pTarget->getColor();*/
}

void TintTo::update(float time)
{
    RGBAProtocol *pRGBAProtocol = dynamic_cast<RGBAProtocol*>(m_pTarget);
    if (pRGBAProtocol)
    {
        pRGBAProtocol->setColor(ccc3(GLubyte(m_from.r + (m_to.r - m_from.r) * time), 
            (GLbyte)(m_from.g + (m_to.g - m_from.g) * time),
            (GLbyte)(m_from.b + (m_to.b - m_from.b) * time)));
    }    
}

//
// TintBy
//

TintBy* TintBy::create(float duration, GLshort deltaRed, GLshort deltaGreen, GLshort deltaBlue)
{
    TintBy *pTintBy = new TintBy();
    pTintBy->initWithDuration(duration, deltaRed, deltaGreen, deltaBlue);
    pTintBy->autorelease();

    return pTintBy;
}

bool TintBy::initWithDuration(float duration, GLshort deltaRed, GLshort deltaGreen, GLshort deltaBlue)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_deltaR = deltaRed;
        m_deltaG = deltaGreen;
        m_deltaB = deltaBlue;

        return true;
    }

    return false;
}

Object* TintBy::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    TintBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (TintBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new TintBy();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, (GLubyte)m_deltaR, (GLubyte)m_deltaG, (GLubyte)m_deltaB);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void TintBy::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);

    RGBAProtocol *pRGBAProtocol = dynamic_cast<RGBAProtocol*>(pTarget);
    if (pRGBAProtocol)
    {
        ccColor3B color = pRGBAProtocol->getColor();
        m_fromR = color.r;
        m_fromG = color.g;
        m_fromB = color.b;
    }    
}

void TintBy::update(float time)
{
    RGBAProtocol *pRGBAProtocol = dynamic_cast<RGBAProtocol*>(m_pTarget);
    if (pRGBAProtocol)
    {
        pRGBAProtocol->setColor(ccc3((GLubyte)(m_fromR + m_deltaR * time),
            (GLubyte)(m_fromG + m_deltaG * time),
            (GLubyte)(m_fromB + m_deltaB * time)));
    }    
}

ActionInterval* TintBy::reverse(void)
{
    return TintBy::create(m_fDuration, -m_deltaR, -m_deltaG, -m_deltaB);
}

//
// DelayTime
//
DelayTime* DelayTime::create(float d)
{
    DelayTime* pAction = new DelayTime();

    pAction->initWithDuration(d);
    pAction->autorelease();

    return pAction;
}

Object* DelayTime::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    DelayTime* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (DelayTime*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new DelayTime();
        pZone = pNewZone = new Zone(pCopy);
    }

    
    ActionInterval::copyWithZone(pZone);

    AX_SAFE_DELETE(pNewZone);

    return pCopy;
}

void DelayTime::update(float time)
{
    AX_UNUSED_PARAM(time);
    return;
}

ActionInterval* DelayTime::reverse(void)
{
    return DelayTime::create(m_fDuration);
}

//
// ReverseTime
//

ReverseTime* ReverseTime::create(FiniteTimeAction *pAction)
{
    // casting to prevent warnings
    ReverseTime *pReverseTime = new ReverseTime();
    pReverseTime->initWithAction(pAction);
    pReverseTime->autorelease();

    return pReverseTime;
}

bool ReverseTime::initWithAction(FiniteTimeAction *pAction)
{
    AXAssert(pAction != NULL, "");
    AXAssert(pAction != m_pOther, "");

    if (ActionInterval::initWithDuration(pAction->getDuration()))
    {
        // Don't leak if action is reused
        AX_SAFE_RELEASE(m_pOther);

        m_pOther = pAction;
        pAction->retain();

        return true;
    }

    return false;
}

Object* ReverseTime::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    ReverseTime* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (ReverseTime*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new ReverseTime();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithAction((FiniteTimeAction*)(m_pOther->copy()->autorelease()));

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

ReverseTime::ReverseTime() : m_pOther(NULL) 
{

}

ReverseTime::~ReverseTime(void)
{
    AX_SAFE_RELEASE(m_pOther);
}

void ReverseTime::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_pOther->startWithTarget(pTarget);
}

void ReverseTime::stop(void)
{
    m_pOther->stop();
    ActionInterval::stop();
}

void ReverseTime::update(float time)
{
    if (m_pOther)
    {
        m_pOther->update(1 - time);
    }
}

ActionInterval* ReverseTime::reverse(void)
{
    return (ActionInterval*)(m_pOther->copy()->autorelease());
}

//
// Animate
//
Animate* Animate::create(Animation *pAnimation)
{
    Animate *pAnimate = new Animate();
    pAnimate->initWithAnimation(pAnimation);
    pAnimate->autorelease();

    return pAnimate;
}

bool Animate::initWithAnimation(Animation *pAnimation)
{
    AXAssert( pAnimation!=NULL, "Animate: argument Animation must be non-NULL");

    float singleDuration = pAnimation->getDuration();

    if ( ActionInterval::initWithDuration(singleDuration * pAnimation->getLoops() ) ) 
    {
        m_nNextFrame = 0;
        setAnimation(pAnimation);
        m_pOrigFrame = NULL;
        m_uExecutedLoops = 0;

        m_pSplitTimes->reserve(pAnimation->getFrames()->count());

        float accumUnitsOfTime = 0;
        float newUnitOfTimeValue = singleDuration / pAnimation->getTotalDelayUnits();

        Array* pFrames = pAnimation->getFrames();
        AXARRAY_VERIFY_TYPE(pFrames, AnimationFrame*);

        Object* pObj = NULL;
        AXARRAY_FOREACH(pFrames, pObj)
        {
            AnimationFrame* frame = (AnimationFrame*)pObj;
            float value = (accumUnitsOfTime * newUnitOfTimeValue) / singleDuration;
            accumUnitsOfTime += frame->getDelayUnits();
            m_pSplitTimes->push_back(value);
        }    
        return true;
    }
    return false;
}

Object* Animate::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    Animate* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (Animate*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new Animate();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithAnimation((Animation*)m_pAnimation->copy()->autorelease());

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

Animate::Animate()
: m_pAnimation(NULL)
, m_pSplitTimes(new std::vector<float>)
, m_nNextFrame(0)
, m_pOrigFrame(NULL)
, m_uExecutedLoops(0)
{

}

Animate::~Animate()
{
    AX_SAFE_RELEASE(m_pAnimation);
    AX_SAFE_RELEASE(m_pOrigFrame);
    AX_SAFE_DELETE(m_pSplitTimes);
}

void Animate::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    Sprite *pSprite = (Sprite*)(pTarget);

    AX_SAFE_RELEASE(m_pOrigFrame);

    if (m_pAnimation->getRestoreOriginalFrame())
    {
        m_pOrigFrame = pSprite->displayFrame();
        m_pOrigFrame->retain();
    }
    m_nNextFrame = 0;
    m_uExecutedLoops = 0;
}

void Animate::stop(void)
{
    if (m_pAnimation->getRestoreOriginalFrame() && m_pTarget)
    {
        ((Sprite*)(m_pTarget))->setDisplayFrame(m_pOrigFrame);
    }

    ActionInterval::stop();
}

void Animate::update(float t)
{
    // if t==1, ignore. Animation should finish with t==1
    if( t < 1.0f ) {
        t *= m_pAnimation->getLoops();

        // new loop?  If so, reset frame counter
        unsigned int loopNumber = (unsigned int)t;
        if( loopNumber > m_uExecutedLoops ) {
            m_nNextFrame = 0;
            m_uExecutedLoops++;
        }

        // new t for animations
        t = fmodf(t, 1.0f);
    }

    Array* frames = m_pAnimation->getFrames();
    unsigned int numberOfFrames = frames->count();
    SpriteFrame *frameToDisplay = NULL;

    for( unsigned int i=m_nNextFrame; i < numberOfFrames; i++ ) {
        float splitTime = m_pSplitTimes->at(i);

        if( splitTime <= t ) {
            AnimationFrame* frame = (AnimationFrame*)frames->objectAtIndex(i);
            frameToDisplay = frame->getSpriteFrame();
            ((Sprite*)m_pTarget)->setDisplayFrame(frameToDisplay);

            Dictionary* dict = frame->getUserInfo();
            if( dict )
            {
                //TODO: [[NSNotificationCenter defaultCenter] postNotificationName:CCAnimationFrameDisplayedNotification object:target_ userInfo:dict];
            }
            m_nNextFrame = i+1;
        }
        // Issue 1438. Could be more than one frame per tick, due to low frame rate or frame delta < 1/FPS
        else {
            break;
        }
    }
}

ActionInterval* Animate::reverse(void)
{
    Array* pOldArray = m_pAnimation->getFrames();
    Array* pNewArray = Array::createWithCapacity(pOldArray->count());
   
    AXARRAY_VERIFY_TYPE(pOldArray, AnimationFrame*);

    if (pOldArray->count() > 0)
    {
        Object* pObj = NULL;
        AXARRAY_FOREACH_REVERSE(pOldArray, pObj)
        {
            AnimationFrame* pElement = (AnimationFrame*)pObj;
            if (! pElement)
            {
                break;
            }

            pNewArray->addObject((AnimationFrame*)(pElement->copy()->autorelease()));
        }
    }

    Animation *newAnim = Animation::create(pNewArray, m_pAnimation->getDelayPerUnit(), m_pAnimation->getLoops());
    newAnim->setRestoreOriginalFrame(m_pAnimation->getRestoreOriginalFrame());
    return create(newAnim);
}

// TargetedAction

TargetedAction::TargetedAction()
: m_pForcedTarget(NULL)
, m_pAction(NULL)
{

}

TargetedAction::~TargetedAction()
{
    AX_SAFE_RELEASE(m_pForcedTarget);
    AX_SAFE_RELEASE(m_pAction);
}

TargetedAction* TargetedAction::create(Node* pTarget, FiniteTimeAction* pAction)
{
    TargetedAction* p = new TargetedAction();
    p->initWithTarget(pTarget, pAction);
    p->autorelease();
    return p;
}


bool TargetedAction::initWithTarget(Node* pTarget, FiniteTimeAction* pAction)
{
    if(ActionInterval::initWithDuration(pAction->getDuration()))
    {
        AX_SAFE_RETAIN(pTarget);
        m_pForcedTarget = pTarget;
        AX_SAFE_RETAIN(pAction);
        m_pAction = pAction;
        return true;
    }
    return false;
}

Object* TargetedAction::copyWithZone(Zone* pZone)
{
    Zone* pNewZone = NULL;
    TargetedAction* pRet = NULL;
    if(pZone && pZone->m_pCopyObject) //in case of being called at sub class
    {
        pRet = (TargetedAction*)(pZone->m_pCopyObject);
    }
    else
    {
        pRet = new TargetedAction();
        pZone = pNewZone = new Zone(pRet);
    }
    ActionInterval::copyWithZone(pZone);
    // win32 : use the m_pOther's copy object.
    pRet->initWithTarget(m_pForcedTarget, (FiniteTimeAction*)m_pAction->copy()->autorelease()); 
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

void TargetedAction::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_pAction->startWithTarget(m_pForcedTarget);
}

void TargetedAction::stop(void)
{
    m_pAction->stop();
}

void TargetedAction::update(float time)
{
    m_pAction->update(time);
}

NS_AX_END
