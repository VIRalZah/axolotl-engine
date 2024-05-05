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

#include "ActionInstant.h"
#include "base/Node.h"
#include "sprite_nodes/Sprite.h"
#include "base/Zone.h"

NS_AX_BEGIN
//
// InstantAction
//
ActionInstant::ActionInstant() {
}

Object * ActionInstant::copyWithZone(Zone *pZone) {
    Zone *pNewZone = NULL;
    ActionInstant *pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        pRet = (ActionInstant*) (pZone->m_pCopyObject);
    } else {
        pRet = new ActionInstant();
        pZone = pNewZone = new Zone(pRet);
    }

    FiniteTimeAction::copyWithZone(pZone);
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

bool ActionInstant::isDone() {
    return true;
}

void ActionInstant::step(float dt) {
    AX_UNUSED_PARAM(dt);
    update(1);
}

void ActionInstant::update(float time) {
    AX_UNUSED_PARAM(time);
    // nothing
}

FiniteTimeAction * ActionInstant::reverse() {
    return (FiniteTimeAction*) (copy()->autorelease());
}

//
// Show
//

Show* Show::create() 
{
    Show* pRet = new Show();

    if (pRet) {
        pRet->autorelease();
    }

    return pRet;
}

void Show::update(float time) {
    AX_UNUSED_PARAM(time);
    m_pTarget->setVisible(true);
}

FiniteTimeAction* Show::reverse() {
    return (FiniteTimeAction*) (Hide::create());
}

Object* Show::copyWithZone(Zone *pZone) {

    Zone *pNewZone = NULL;
    Show *pRet = NULL;
    if (pZone && pZone->m_pCopyObject) {
        pRet = (Show*) (pZone->m_pCopyObject);
    } else {
        pRet = new Show();
        pZone = pNewZone = new Zone(pRet);
    }

    ActionInstant::copyWithZone(pZone);
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

//
// Hide
//
Hide * Hide::create() 
{
    Hide *pRet = new Hide();

    if (pRet) {
        pRet->autorelease();
    }

    return pRet;
}

void Hide::update(float time) {
    AX_UNUSED_PARAM(time);
    m_pTarget->setVisible(false);
}

FiniteTimeAction *Hide::reverse() {
    return (FiniteTimeAction*) (Show::create());
}

Object* Hide::copyWithZone(Zone *pZone) {
    Zone *pNewZone = NULL;
    Hide *pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        pRet = (Hide*) (pZone->m_pCopyObject);
    } else {
        pRet = new Hide();
        pZone = pNewZone = new Zone(pRet);
    }

    ActionInstant::copyWithZone(pZone);
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

//
// ToggleVisibility
//
ToggleVisibility * ToggleVisibility::create()
{
    ToggleVisibility *pRet = new ToggleVisibility();

    if (pRet)
    {
        pRet->autorelease();
    }

    return pRet;
}

void ToggleVisibility::update(float time) 
{
    AX_UNUSED_PARAM(time);
    m_pTarget->setVisible(!m_pTarget->isVisible());
}

Object* ToggleVisibility::copyWithZone(Zone *pZone)
{
    Zone *pNewZone = NULL;
    ToggleVisibility *pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        pRet = (ToggleVisibility*) (pZone->m_pCopyObject);
    } else {
        pRet = new ToggleVisibility();
        pZone = pNewZone = new Zone(pRet);
    }

    ActionInstant::copyWithZone(pZone);
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

//
// Remove Self
//
RemoveSelf * RemoveSelf::create(bool isNeedCleanUp /*= true*/) 
{
	RemoveSelf *pRet = new RemoveSelf();

	if (pRet && pRet->init(isNeedCleanUp)) {
		pRet->autorelease();
	}

	return pRet;
}

bool RemoveSelf::init(bool isNeedCleanUp) {
	m_bIsNeedCleanUp = isNeedCleanUp;
	return true;
}

void RemoveSelf::update(float time) {
	AX_UNUSED_PARAM(time);
	m_pTarget->removeFromParentAndCleanup(m_bIsNeedCleanUp);
}

FiniteTimeAction *RemoveSelf::reverse() {
	return (FiniteTimeAction*) (RemoveSelf::create(m_bIsNeedCleanUp));
}

Object* RemoveSelf::copyWithZone(Zone *pZone) {
	Zone *pNewZone = NULL;
	RemoveSelf *pRet = NULL;

	if (pZone && pZone->m_pCopyObject) {
		pRet = (RemoveSelf*) (pZone->m_pCopyObject);
	} else {
		pRet = new RemoveSelf();
		pZone = pNewZone = new Zone(pRet);
	}

	ActionInstant::copyWithZone(pZone);
	pRet->init(m_bIsNeedCleanUp);
	AX_SAFE_DELETE(pNewZone);
	return pRet;
}

//
// FlipX
//

FlipX *FlipX::create(bool x)
{
    FlipX *pRet = new FlipX();

    if (pRet && pRet->initWithFlipX(x)) {
        pRet->autorelease();
        return pRet;
    }

    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool FlipX::initWithFlipX(bool x) {
    m_bFlipX = x;
    return true;
}

void FlipX::update(float time) {
    AX_UNUSED_PARAM(time);
    ((Sprite*) (m_pTarget))->setFlipX(m_bFlipX);
}

FiniteTimeAction* FlipX::reverse() {
    return FlipX::create(!m_bFlipX);
}

Object * FlipX::copyWithZone(Zone *pZone) {
    Zone *pNewZone = NULL;
    FlipX *pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        pRet = (FlipX*) (pZone->m_pCopyObject);
    } else {
        pRet = new FlipX();
        pZone = pNewZone = new Zone(pRet);
    }

    ActionInstant::copyWithZone(pZone);
    pRet->initWithFlipX(m_bFlipX);
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

//
// FlipY
//

FlipY * FlipY::create(bool y)
{
    FlipY *pRet = new FlipY();

    if (pRet && pRet->initWithFlipY(y)) {
        pRet->autorelease();
        return pRet;
    }

    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool FlipY::initWithFlipY(bool y) {
    m_bFlipY = y;
    return true;
}

void FlipY::update(float time) {
    AX_UNUSED_PARAM(time);
    ((Sprite*) (m_pTarget))->setFlipY(m_bFlipY);
}

FiniteTimeAction* FlipY::reverse() {
    return FlipY::create(!m_bFlipY);
}

Object* FlipY::copyWithZone(Zone *pZone) {
    Zone *pNewZone = NULL;
    FlipY *pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        pRet = (FlipY*) (pZone->m_pCopyObject);
    } else {
        pRet = new FlipY();
        pZone = pNewZone = new Zone(pRet);
    }

    ActionInstant::copyWithZone(pZone);
    pRet->initWithFlipY(m_bFlipY);
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

//
// Place
//

Place* Place::create(const Vec2& pos)
{
    Place *pRet = new Place();

    if (pRet && pRet->initWithPosition(pos)) {
        pRet->autorelease();
        return pRet;
    }

    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool Place::initWithPosition(const Vec2& pos) {
    m_tPosition = pos;
    return true;
}

Object * Place::copyWithZone(Zone *pZone) {
    Zone *pNewZone = NULL;
    Place *pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        pRet = (Place*) (pZone->m_pCopyObject);
    } else {
        pRet = new Place();
        pZone = pNewZone = new Zone(pRet);
    }

    ActionInstant::copyWithZone(pZone);
    pRet->initWithPosition(m_tPosition);
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

void Place::update(float time) {
    AX_UNUSED_PARAM(time);
    m_pTarget->setPosition(m_tPosition);
}

//
// CallFunc
//
CallFunc * CallFunc::create(Object* pSelectorTarget, SEL_CallFunc selector) 
{
    CallFunc *pRet = new CallFunc();

    if (pRet && pRet->initWithTarget(pSelectorTarget)) {
        pRet->m_pCallFunc = selector;
        pRet->autorelease();
        return pRet;
    }

    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool CallFunc::initWithTarget(Object* pSelectorTarget) {
    if (pSelectorTarget) 
    {
        pSelectorTarget->retain();
    }

    if (m_pSelectorTarget) 
    {
        m_pSelectorTarget->release();
    }

    m_pSelectorTarget = pSelectorTarget;
    return true;
}

CallFunc::~CallFunc(void)
{
    AX_SAFE_RELEASE(m_pSelectorTarget);
}

Object * CallFunc::copyWithZone(Zone *pZone) {
    Zone* pNewZone = NULL;
    CallFunc* pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        //in case of being called at sub class
        pRet = (CallFunc*) (pZone->m_pCopyObject);
    } else {
        pRet = new CallFunc();
        pZone = pNewZone = new Zone(pRet);
    }

    ActionInstant::copyWithZone(pZone);
    pRet->initWithTarget(m_pSelectorTarget);
    pRet->m_pCallFunc = m_pCallFunc;
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

void CallFunc::update(float time) {
    AX_UNUSED_PARAM(time);
    this->execute();
}

void CallFunc::execute() {
    if (m_pCallFunc) {
        (m_pSelectorTarget->*m_pCallFunc)();
    }
}

//
// CallFuncN
//
void CallFuncN::execute() {
    if (m_pCallFuncN) {
        (m_pSelectorTarget->*m_pCallFuncN)(m_pTarget);
    }
}

CallFuncN * CallFuncN::create(Object* pSelectorTarget, SEL_CallFuncN selector)
{
    CallFuncN *pRet = new CallFuncN();

    if (pRet && pRet->initWithTarget(pSelectorTarget, selector))
    {
        pRet->autorelease();
        return pRet;
    }

    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool CallFuncN::initWithTarget(Object* pSelectorTarget,
        SEL_CallFuncN selector) {
    if (CallFunc::initWithTarget(pSelectorTarget)) {
        m_pCallFuncN = selector;
        return true;
    }

    return false;
}

Object * CallFuncN::copyWithZone(Zone* zone) {
    Zone* pNewZone = NULL;
    CallFuncN* pRet = NULL;

    if (zone && zone->m_pCopyObject) {
        //in case of being called at sub class
        pRet = (CallFuncN*) (zone->m_pCopyObject);
    } else {
        pRet = new CallFuncN();
        zone = pNewZone = new Zone(pRet);
    }

    CallFunc::copyWithZone(zone);
    pRet->initWithTarget(m_pSelectorTarget, m_pCallFuncN);
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

//
// CallFuncND
//

CallFuncND * CallFuncND::create(Object* pSelectorTarget, SEL_CallFuncND selector, void* d)
{
    CallFuncND* pRet = new CallFuncND();

    if (pRet && pRet->initWithTarget(pSelectorTarget, selector, d)) {
        pRet->autorelease();
        return pRet;
    }

    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool CallFuncND::initWithTarget(Object* pSelectorTarget,
        SEL_CallFuncND selector, void* d) {
    if (CallFunc::initWithTarget(pSelectorTarget)) {
        m_pData = d;
        m_pCallFuncND = selector;
        return true;
    }

    return false;
}

Object * CallFuncND::copyWithZone(Zone* zone) {
    Zone* pNewZone = NULL;
    CallFuncND* pRet = NULL;

    if (zone && zone->m_pCopyObject) {
        //in case of being called at sub class
        pRet = (CallFuncND*) (zone->m_pCopyObject);
    } else {
        pRet = new CallFuncND();
        zone = pNewZone = new Zone(pRet);
    }

    CallFunc::copyWithZone(zone);
    pRet->initWithTarget(m_pSelectorTarget, m_pCallFuncND, m_pData);
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

void CallFuncND::execute() {
    if (m_pCallFuncND) {
        (m_pSelectorTarget->*m_pCallFuncND)(m_pTarget, m_pData);
    }
}

//
// CallFuncO
//
CallFuncO::CallFuncO() :
        m_pObject(NULL) {
}

CallFuncO::~CallFuncO() {
    AX_SAFE_RELEASE(m_pObject);
}

void CallFuncO::execute() {
    if (m_pCallFuncO) {
        (m_pSelectorTarget->*m_pCallFuncO)(m_pObject);
    }
}

CallFuncO * CallFuncO::create(Object* pSelectorTarget, SEL_CallFuncO selector, Object* pObject)
{
    CallFuncO *pRet = new CallFuncO();

    if (pRet && pRet->initWithTarget(pSelectorTarget, selector, pObject)) {
        pRet->autorelease();
        return pRet;
    }

    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool CallFuncO::initWithTarget(Object* pSelectorTarget,
        SEL_CallFuncO selector, Object* pObject) {
    if (CallFunc::initWithTarget(pSelectorTarget)) {
        m_pObject = pObject;
        AX_SAFE_RETAIN(m_pObject);

        m_pCallFuncO = selector;
        return true;
    }

    return false;
}

Object * CallFuncO::copyWithZone(Zone* zone) {
    Zone* pNewZone = NULL;
    CallFuncO* pRet = NULL;

    if (zone && zone->m_pCopyObject) {
        //in case of being called at sub class
        pRet = (CallFuncO*) (zone->m_pCopyObject);
    } else {
        pRet = new CallFuncO();
        zone = pNewZone = new Zone(pRet);
    }

    CallFunc::copyWithZone(zone);
    pRet->initWithTarget(m_pSelectorTarget, m_pCallFuncO, m_pObject);
    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

NS_AX_END
