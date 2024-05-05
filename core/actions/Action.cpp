/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.
 
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

#include "Action.h"
#include "ActionInterval.h"
#include "base/Node.h"
#include "support/PointExtension.h"
#include "base/Director.h"
#include "base/Zone.h"

NS_AX_BEGIN
//
// Action Base Class
//

Action::Action()
:m_pOriginalTarget(NULL)
,m_pTarget(NULL)
,m_nTag(kCCActionTagInvalid)
{
}

Action::~Action()
{
    AXLOGINFO("cocos2d: deallocing");
}

Action* Action::create()
{
    Action * pRet = new Action();
    pRet->autorelease();
    return pRet;
}

const char* Action::description()
{
    return String::createWithFormat("<Action | Tag = %d>", m_nTag)->getCString();
}

Object* Action::copyWithZone(Zone *pZone)
{
    Zone *pNewZone = NULL;
    Action *pRet = NULL;
    if (pZone && pZone->m_pCopyObject)
    {
        pRet = (Action*)(pZone->m_pCopyObject);
    }
    else
    {
        pRet = new Action();
        pNewZone = new Zone(pRet);
    }
    //copy member data
    pRet->m_nTag = m_nTag;
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

void Action::startWithTarget(Node *aTarget)
{
    m_pOriginalTarget = m_pTarget = aTarget;
}

void Action::stop()
{
    m_pTarget = NULL;
}

bool Action::isDone()
{
    return true;
}

void Action::step(float dt)
{
    AX_UNUSED_PARAM(dt);
    AXLOG("[Action step]. override me");
}

void Action::update(float time)
{
    AX_UNUSED_PARAM(time);
    AXLOG("[Action update]. override me");
}

//
// FiniteTimeAction
//

FiniteTimeAction *FiniteTimeAction::reverse()
{
    AXLOG("cocos2d: FiniteTimeAction#reverse: Implement me");
    return NULL;
}

//
// Speed
//
Speed::~Speed()
{
    AX_SAFE_RELEASE(m_pInnerAction);
}

Speed* Speed::create(ActionInterval* pAction, float fSpeed)
{
    Speed *pRet = new Speed();
    if (pRet && pRet->initWithAction(pAction, fSpeed))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool Speed::initWithAction(ActionInterval *pAction, float fSpeed)
{
    AXAssert(pAction != NULL, "");
    pAction->retain();
    m_pInnerAction = pAction;
    m_fSpeed = fSpeed;    
    return true;
}

Object *Speed::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    Speed* pRet = NULL;
    if(pZone && pZone->m_pCopyObject) //in case of being called at sub class
    {
        pRet = (Speed*)(pZone->m_pCopyObject);
    }
    else
    {
        pRet = new Speed();
        pZone = pNewZone = new Zone(pRet);
    }
    Action::copyWithZone(pZone);

    pRet->initWithAction( (ActionInterval*)(m_pInnerAction->copy()->autorelease()) , m_fSpeed );
    
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

void Speed::startWithTarget(Node* pTarget)
{
    Action::startWithTarget(pTarget);
    m_pInnerAction->startWithTarget(pTarget);
}

void Speed::stop()
{
    m_pInnerAction->stop();
    Action::stop();
}

void Speed::step(float dt)
{
    m_pInnerAction->step(dt * m_fSpeed);
}

bool Speed::isDone()
{
    return m_pInnerAction->isDone();
}

ActionInterval *Speed::reverse()
{
     return (ActionInterval*)(Speed::create(m_pInnerAction->reverse(), m_fSpeed));
}

void Speed::setInnerAction(ActionInterval *pAction)
{
    if (m_pInnerAction != pAction)
    {
        AX_SAFE_RELEASE(m_pInnerAction);
        m_pInnerAction = pAction;
        AX_SAFE_RETAIN(m_pInnerAction);
    }
}

//
// Follow
//
Follow::~Follow()
{
    AX_SAFE_RELEASE(m_pobFollowedNode);
}

Follow* Follow::create(Node *pFollowedNode, const Rect& rect/* = Rect::ZERO*/)
{
    Follow *pRet = new Follow();
    if (pRet && pRet->initWithTarget(pFollowedNode, rect))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool Follow::initWithTarget(Node *pFollowedNode, const Rect& rect/* = Rect::ZERO*/)
{
    AXAssert(pFollowedNode != NULL, "");
 
    pFollowedNode->retain();
    m_pobFollowedNode = pFollowedNode;
    if (rect.equals(Rect::ZERO))
    {
        m_bBoundarySet = false;
    }
    else
    {
        m_bBoundarySet = true;
    }
    
    m_bBoundaryFullyCovered = false;

    Size winSize = Director::sharedDirector()->getWinSize();
    m_obFullScreenSize = Vec2(winSize.width, winSize.height);
    m_obHalfScreenSize = PointMult(m_obFullScreenSize, 0.5f);

    if (m_bBoundarySet)
    {
        m_fLeftBoundary = -((rect.origin.x+rect.size.width) - m_obFullScreenSize.x);
        m_fRightBoundary = -rect.origin.x ;
        m_fTopBoundary = -rect.origin.y;
        m_fBottomBoundary = -((rect.origin.y+rect.size.height) - m_obFullScreenSize.y);

        if(m_fRightBoundary < m_fLeftBoundary)
        {
            // screen width is larger than world's boundary width
            //set both in the middle of the world
            m_fRightBoundary = m_fLeftBoundary = (m_fLeftBoundary + m_fRightBoundary) / 2;
        }
        if(m_fTopBoundary < m_fBottomBoundary)
        {
            // screen width is larger than world's boundary width
            //set both in the middle of the world
            m_fTopBoundary = m_fBottomBoundary = (m_fTopBoundary + m_fBottomBoundary) / 2;
        }

        if( (m_fTopBoundary == m_fBottomBoundary) && (m_fLeftBoundary == m_fRightBoundary) )
        {
            m_bBoundaryFullyCovered = true;
        }
    }
    
    return true;
}

Object *Follow::copyWithZone(Zone *pZone)
{
    Zone *pNewZone = NULL;
    Follow *pRet = NULL;
    if(pZone && pZone->m_pCopyObject) //in case of being called at sub class
    {
        pRet = (Follow*)(pZone->m_pCopyObject);
    }
    else
    {
        pRet = new Follow();
        pZone = pNewZone = new Zone(pRet);
    }
    Action::copyWithZone(pZone);
    // copy member data
    pRet->m_nTag = m_nTag;
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

void Follow::step(float dt)
{
    AX_UNUSED_PARAM(dt);

    if(m_bBoundarySet)
    {
        // whole map fits inside a single screen, no need to modify the position - unless map boundaries are increased
        if(m_bBoundaryFullyCovered)
            return;

        Vec2 tempPos = PointSub( m_obHalfScreenSize, m_pobFollowedNode->getPosition());

        m_pTarget->setPosition(Vec2(clampf(tempPos.x, m_fLeftBoundary, m_fRightBoundary), 
                                   clampf(tempPos.y, m_fBottomBoundary, m_fTopBoundary)));
    }
    else
    {
        m_pTarget->setPosition(PointSub(m_obHalfScreenSize, m_pobFollowedNode->getPosition()));
    }
}

bool Follow::isDone()
{
    return ( !m_pobFollowedNode->isRunning() );
}

void Follow::stop()
{
    m_pTarget = NULL;
    Action::stop();
}

NS_AX_END


