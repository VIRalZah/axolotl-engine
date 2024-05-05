/*
 * Copyright (c) 2010-2012 cocos2d-x.org
 * cocos2d for iPhone: http://www.cocos2d-iphone.org
 *
 * Copyright (c) 2008 Radu Gruian
 *
 * Copyright (c) 2011 Vit Valentin
 *
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
 * Original code by Radu Gruian: http://www.codeproject.com/Articles/30838/Overhauser-Catmull-Rom-Splines-for-Camera-Animatio.So
 *
 * Adapted to cocos2d-x by Vit Valentin
 *
 * Adapted from cocos2d-x to cocos2d-iphone by Ricardo Quesada
 */
#include "ccMacros.h"
#include "support/PointExtension.h"
#include "ActionCatmullRom.h"
#include "base/Zone.h"

using namespace std;

NS_AX_BEGIN;

/*
 *  Implementation of PointArray
 */

PointArray* PointArray::create(unsigned int capacity)
{
    PointArray* ret = new PointArray();
    if (ret)
    {
        if (ret->initWithCapacity(capacity))
        {
            ret->autorelease();
        }
        else 
        {
            delete ret;
            ret = NULL;
        }
    }

    return ret;
}


bool PointArray::initWithCapacity(unsigned int capacity)
{
    m_pControlPoints = new vector<Vec2*>();
    
    return true;
}

Object* PointArray::copyWithZone(axolotl::Zone *zone)
{
    vector<Vec2*> *newArray = new vector<Vec2*>();
    vector<Vec2*>::iterator iter;
    for (iter = m_pControlPoints->begin(); iter != m_pControlPoints->end(); ++iter)
    {
        newArray->push_back(new Vec2((*iter)->x, (*iter)->y));
    }
    
    PointArray *points = new PointArray();
    points->initWithCapacity(10);
    points->setControlPoints(newArray);
    
    return points;
}

PointArray::~PointArray()
{
    vector<Vec2*>::iterator iter;
    for (iter = m_pControlPoints->begin(); iter != m_pControlPoints->end(); ++iter)
    {
        delete *iter;
    }
    delete m_pControlPoints;
}

PointArray::PointArray() :m_pControlPoints(NULL){}

const std::vector<Vec2*>* PointArray::getControlPoints()
{
    return m_pControlPoints;
}

void PointArray::setControlPoints(vector<Vec2*> *controlPoints)
{
    AXAssert(controlPoints != NULL, "control points should not be NULL");
    
    // delete old points
    vector<Vec2*>::iterator iter;
    for (iter = m_pControlPoints->begin(); iter != m_pControlPoints->end(); ++iter)
    {
        delete *iter;
    }
    delete m_pControlPoints;
    
    m_pControlPoints = controlPoints;
}

void PointArray::addControlPoint(Vec2 controlPoint)
{    
    m_pControlPoints->push_back(new Vec2(controlPoint.x, controlPoint.y));
}

void PointArray::insertControlPoint(Vec2 &controlPoint, unsigned int index)
{
    Vec2 *temp = new Vec2(controlPoint.x, controlPoint.y);
    m_pControlPoints->insert(m_pControlPoints->begin() + index, temp);
}

Vec2 PointArray::getControlPointAtIndex(unsigned int index)
{
    index = MIN(m_pControlPoints->size()-1, MAX(index, 0));
    return *(m_pControlPoints->at(index));
}

void PointArray::replaceControlPoint(axolotl::Vec2 &controlPoint, unsigned int index)
{

    Vec2 *temp = m_pControlPoints->at(index);
    temp->x = controlPoint.x;
    temp->y = controlPoint.y;
}

void PointArray::removeControlPointAtIndex(unsigned int index)
{
    vector<Vec2*>::iterator iter = m_pControlPoints->begin() + index;
    Vec2* pRemovedPoint = *iter;
    m_pControlPoints->erase(iter);
    delete pRemovedPoint;
}

unsigned int PointArray::count()
{
    return m_pControlPoints->size();
}

PointArray* PointArray::reverse()
{
    vector<Vec2*> *newArray = new vector<Vec2*>();
    vector<Vec2*>::reverse_iterator iter;
    Vec2 *point = NULL;
    for (iter = m_pControlPoints->rbegin(); iter != m_pControlPoints->rend(); ++iter)
    {
        point = *iter;
        newArray->push_back(new Vec2(point->x, point->y));
    }
    PointArray *config = PointArray::create(0);
    config->setControlPoints(newArray);
    
    return config;
}

void PointArray::reverseInline()
{
    unsigned int l = m_pControlPoints->size();
    Vec2 *p1 = NULL;
    Vec2 *p2 = NULL;
    int x, y;
    for (unsigned int i = 0; i < l/2; ++i)
    {
        p1 = m_pControlPoints->at(i);
        p2 = m_pControlPoints->at(l-i-1);
        
        x = p1->x;
        y = p1->y;
        
        p1->x = p2->x;
        p1->y = p2->y;
        
        p2->x = x;
        p2->y = y;
    }
}

// CatmullRom Spline formula:
Vec2 ccCardinalSplineAt(Vec2 &p0, Vec2 &p1, Vec2 &p2, Vec2 &p3, float tension, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;
    
	/*
	 * Formula: s(-ttt + 2tt - t)P1 + s(-ttt + tt)P2 + (2ttt - 3tt + 1)P2 + s(ttt - 2tt + t)P3 + (-2ttt + 3tt)P3 + s(ttt - tt)P4
	 */
    float s = (1 - tension) / 2;
	
    float b1 = s * ((-t3 + (2 * t2)) - t);                      // s(-t3 + 2 t2 - t)P1
    float b2 = s * (-t3 + t2) + (2 * t3 - 3 * t2 + 1);          // s(-t3 + t2)P2 + (2 t3 - 3 t2 + 1)P2
    float b3 = s * (t3 - 2 * t2 + t) + (-2 * t3 + 3 * t2);      // s(t3 - 2 t2 + t)P3 + (-2 t3 + 3 t2)P3
    float b4 = s * (t3 - t2);                                   // s(t3 - t2)P4
    
    float x = (p0.x*b1 + p1.x*b2 + p2.x*b3 + p3.x*b4);
    float y = (p0.y*b1 + p1.y*b2 + p2.y*b3 + p3.y*b4);
	
	return Vec2(x,y);
}

/* Implementation of CardinalSplineTo
 */

CardinalSplineTo* CardinalSplineTo::create(float duration, axolotl::PointArray *points, float tension)
{
    CardinalSplineTo *ret = new CardinalSplineTo();
    if (ret)
    {
        if (ret->initWithDuration(duration, points, tension))
        {
            ret->autorelease();
        }
        else 
        {
            AX_SAFE_RELEASE_NULL(ret);
        }
    }

    return ret;
}

bool CardinalSplineTo::initWithDuration(float duration, axolotl::PointArray *points, float tension)
{
    AXAssert(points->count() > 0, "Invalid configuration. It must at least have one control point");

    if (ActionInterval::initWithDuration(duration))
    {
        this->setPoints(points);
        this->m_fTension = tension;
        
        return true;
    }
    
    return false;
}

CardinalSplineTo::~CardinalSplineTo()
{
    AX_SAFE_RELEASE_NULL(m_pPoints);
}

CardinalSplineTo::CardinalSplineTo()
: m_pPoints(NULL)
, m_fDeltaT(0.f)
, m_fTension(0.f)
{
}

void CardinalSplineTo::startWithTarget(axolotl::Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
	
//    m_fDeltaT = (float) 1 / m_pPoints->count();
    
    // Issue #1441
    m_fDeltaT = (float) 1 / (m_pPoints->count() - 1);

    m_previousPosition = pTarget->getPosition();
    m_accumulatedDiff = Vec2::ZERO;
}

CardinalSplineTo* CardinalSplineTo::copyWithZone(axolotl::Zone *pZone)
{
    Zone* pNewZone = NULL;
    CardinalSplineTo* pRet = NULL;
    if(pZone && pZone->m_pCopyObject) //in case of being called at sub class
    {
        pRet = (CardinalSplineTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pRet = new CardinalSplineTo();
        pZone = pNewZone = new Zone(pRet);
    }

    ActionInterval::copyWithZone(pZone);

    pRet->initWithDuration(this->getDuration(), this->m_pPoints, this->m_fTension);

    AX_SAFE_DELETE(pNewZone);
    return pRet;
}

void CardinalSplineTo::update(float time)
{
    unsigned int p;
    float lt;
	
	// eg.
	// p..p..p..p..p..p..p
	// 1..2..3..4..5..6..7
	// want p to be 1, 2, 3, 4, 5, 6
    if (time == 1)
    {
        p = m_pPoints->count() - 1;
        lt = 1;
    }
    else 
    {
        p = time / m_fDeltaT;
        lt = (time - m_fDeltaT * (float)p) / m_fDeltaT;
    }
    
	// Interpolate    
    Vec2 pp0 = m_pPoints->getControlPointAtIndex(p-1);
    Vec2 pp1 = m_pPoints->getControlPointAtIndex(p+0);
    Vec2 pp2 = m_pPoints->getControlPointAtIndex(p+1);
    Vec2 pp3 = m_pPoints->getControlPointAtIndex(p+2);
	
    Vec2 newPos = ccCardinalSplineAt(pp0, pp1, pp2, pp3, m_fTension, lt);
	
#if AX_ENABLE_STACKABLE_ACTIONS
    // Support for stacked actions
    Node *node = m_pTarget;
    Vec2 diff = PointSub( node->getPosition(), m_previousPosition);
    if( diff.x !=0 || diff.y != 0 ) {
        m_accumulatedDiff = PointAdd( m_accumulatedDiff, diff);
        newPos = PointAdd( newPos, m_accumulatedDiff);
    }
#endif
    
    this->updatePosition(newPos);
}

void CardinalSplineTo::updatePosition(axolotl::Vec2 &newPos)
{
    m_pTarget->setPosition(newPos);
    m_previousPosition = newPos;
}

ActionInterval* CardinalSplineTo::reverse()
{
    PointArray *pReverse = m_pPoints->reverse();
    
    return CardinalSplineTo::create(m_fDuration, pReverse, m_fTension);
}

/* CardinalSplineBy
 */

CardinalSplineBy* CardinalSplineBy::create(float duration, axolotl::PointArray *points, float tension)
{
    CardinalSplineBy *ret = new CardinalSplineBy();
    if (ret)
    {
        if (ret->initWithDuration(duration, points, tension))
        {
            ret->autorelease();
        }
        else 
        {
            AX_SAFE_RELEASE_NULL(ret);
        }
    }

    return ret;
}

CardinalSplineBy::CardinalSplineBy() : m_startPosition(0,0)
{
}

void CardinalSplineBy::updatePosition(axolotl::Vec2 &newPos)
{
    Vec2 p = PointAdd(newPos, m_startPosition);
    m_pTarget->setPosition(p);
    m_previousPosition = p;
}

ActionInterval* CardinalSplineBy::reverse()
{
    PointArray *copyConfig = (PointArray*)m_pPoints->copy();
	
	//
	// convert "absolutes" to "diffs"
	//
    Vec2 p = copyConfig->getControlPointAtIndex(0);
    for (unsigned int i = 1; i < copyConfig->count(); ++i)
    {
        Vec2 current = copyConfig->getControlPointAtIndex(i);
        Vec2 diff = PointSub(current, p);
        copyConfig->replaceControlPoint(diff, i);
        
        p = current;
    }
	
	
	// convert to "diffs" to "reverse absolute"
	
    PointArray *pReverse = copyConfig->reverse();
    copyConfig->release();
	
	// 1st element (which should be 0,0) should be here too
    
    p = pReverse->getControlPointAtIndex(pReverse->count()-1);
    pReverse->removeControlPointAtIndex(pReverse->count()-1);
    
    p = PointNeg(p);
    pReverse->insertControlPoint(p, 0);
    
    for (unsigned int i = 1; i < pReverse->count(); ++i)
    {
        Vec2 current = pReverse->getControlPointAtIndex(i);
        current = PointNeg(current);
        Vec2 abs = PointAdd(current, p);
        pReverse->replaceControlPoint(abs, i);
        
        p = abs;
    }
	
    return CardinalSplineBy::create(m_fDuration, pReverse, m_fTension);
}

void CardinalSplineBy::startWithTarget(axolotl::Node *pTarget)
{    
    CardinalSplineTo::startWithTarget(pTarget);
    m_startPosition = pTarget->getPosition();
}

/* CatmullRomTo
 */

CatmullRomTo* CatmullRomTo::create(float dt, axolotl::PointArray *points)
{
    CatmullRomTo *ret = new CatmullRomTo();
    if (ret)
    {
        if (ret->initWithDuration(dt, points))
        {
            ret->autorelease();
        }
        else 
        {
            AX_SAFE_RELEASE_NULL(ret);
        }
    }

    return ret;
}

bool CatmullRomTo::initWithDuration(float dt, axolotl::PointArray *points)
{
    if (CardinalSplineTo::initWithDuration(dt, points, 0.5f))
    {
        return true;
    }
    
    return false;
}

/* CatmullRomBy
 */

CatmullRomBy* CatmullRomBy::create(float dt, axolotl::PointArray *points)
{
    CatmullRomBy *ret = new CatmullRomBy();
    if (ret)
    {
        if (ret->initWithDuration(dt, points))
        {
            ret->autorelease();
        }
        else 
        {
            AX_SAFE_RELEASE_NULL(ret);
        }
    }

    return ret;
}

bool CatmullRomBy::initWithDuration(float dt, axolotl::PointArray *points)
{
    if (CardinalSplineTo::initWithDuration(dt, points, 0.5f))
    {
        return true;
    }
    
    return false;
}

NS_AX_END;

