/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2009      On-Core 

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
#include "ActionGrid.h"
#include "base/Director.h"
#include "effects/Grid.h"
#include "cocoa/Zone.h"

NS_AX_BEGIN
// implementation of GridAction

GridAction* GridAction::create(float duration, const Size& gridSize)
{
    GridAction *pAction = new GridAction();
    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_DELETE(pAction);
        }
    }

    return pAction;
}

bool GridAction::initWithDuration(float duration, const Size& gridSize)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_sGridSize = gridSize;

        return true;
    }

    return false;
}

void GridAction::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);

    GridBase *newgrid = this->getGrid();

    Node *t = m_pTarget;
    GridBase *targetGrid = t->getGrid();

    if (targetGrid && targetGrid->getReuseGrid() > 0)
    {
        if (targetGrid->isActive() && targetGrid->getGridSize().width == m_sGridSize.width
            && targetGrid->getGridSize().height == m_sGridSize.height /*&& dynamic_cast<GridBase*>(targetGrid) != NULL*/)
        {
            targetGrid->reuse();
        }
        else
        {
            AXAssert(0, "");
        }
    }
    else
    {
        if (targetGrid && targetGrid->isActive())
        {
            targetGrid->setActive(false);
        }

        t->setGrid(newgrid);
        t->getGrid()->setActive(true);
    }
}

GridBase* GridAction::getGrid(void)
{
    // Abstract class needs implementation
    AXAssert(0, "");

    return NULL;
}

ActionInterval* GridAction::reverse(void)
{
    return ReverseTime::create(this);
}

Object* GridAction::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    GridAction* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (GridAction*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new GridAction();
        pZone = pNewZone = new Zone(pCopy);
    }

    ActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sGridSize);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

// implementation of Grid3DAction

GridBase* Grid3DAction::getGrid(void)
{
    return Grid3D::create(m_sGridSize);
}

ccVertex3F Grid3DAction::vertex(const Point& position)
{
    Grid3D *g = (Grid3D*)m_pTarget->getGrid();
    return g->vertex(position);
}

ccVertex3F Grid3DAction::originalVertex(const Point& position)
{
    Grid3D *g = (Grid3D*)m_pTarget->getGrid();
    return g->originalVertex(position);
}

void Grid3DAction::setVertex(const Point& position, const ccVertex3F& vertex)
{
    Grid3D *g = (Grid3D*)m_pTarget->getGrid();
    g->setVertex(position, vertex);
}

// implementation of TiledGrid3DAction

GridBase* TiledGrid3DAction::getGrid(void)
{
    return TiledGrid3D::create(m_sGridSize);
}

ccQuad3 TiledGrid3DAction::tile(const Point& pos)
{
    TiledGrid3D *g = (TiledGrid3D*)m_pTarget->getGrid();
    return g->tile(pos);
}

ccQuad3 TiledGrid3DAction::originalTile(const Point& pos)
{
    TiledGrid3D *g = (TiledGrid3D*)m_pTarget->getGrid();
    return g->originalTile(pos);
}

void TiledGrid3DAction::setTile(const Point& pos, const ccQuad3& coords)
{
    TiledGrid3D *g = (TiledGrid3D*)m_pTarget->getGrid();
    return g->setTile(pos, coords);
}

TiledGrid3DAction* TiledGrid3DAction::create(float duration, const Size& gridSize)
{
    TiledGrid3DAction* pRet = new TiledGrid3DAction();
    if (pRet && pRet->initWithDuration(duration, gridSize))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }
    return pRet;
}

// implementation AccelDeccelAmplitude

AccelDeccelAmplitude* AccelDeccelAmplitude::create(Action *pAction, float duration)
{
    AccelDeccelAmplitude *pRet = new AccelDeccelAmplitude();
    if (pRet)
    {
        if (pRet->initWithAction(pAction, duration))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_DELETE(pRet);
        }
    }

    return pRet;
}

bool AccelDeccelAmplitude::initWithAction(Action *pAction, float duration)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_fRate = 1.0f;
        m_pOther = (ActionInterval*)(pAction);
        pAction->retain();

        return true;
    }

    return false;
}

AccelDeccelAmplitude::~AccelDeccelAmplitude(void)
{
    AX_SAFE_RELEASE(m_pOther);
}

void AccelDeccelAmplitude::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_pOther->startWithTarget(pTarget);
}

void AccelDeccelAmplitude::update(float time)
{
    float f = time * 2;

    if (f > 1)
    {
        f -= 1;
        f = 1 - f;
    }

    ((AccelDeccelAmplitude*)(m_pOther))->setAmplitudeRate(powf(f, m_fRate));
}

ActionInterval* AccelDeccelAmplitude::reverse(void)
{
    return AccelDeccelAmplitude::create(m_pOther->reverse(), m_fDuration);
}

// implementation of AccelAmplitude

AccelAmplitude* AccelAmplitude::create(Action *pAction, float duration)
{
    AccelAmplitude *pRet = new AccelAmplitude();
    if (pRet)
    {
        if (pRet->initWithAction(pAction, duration))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_DELETE(pRet);
        }
    }

    return pRet;
}

bool AccelAmplitude::initWithAction(Action *pAction, float duration)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_fRate = 1.0f;
        m_pOther = (ActionInterval*)(pAction);
        pAction->retain();

        return true;
    }

    return false;
}

AccelAmplitude::~AccelAmplitude(void)
{
    AX_SAFE_DELETE(m_pOther);
}

void AccelAmplitude::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_pOther->startWithTarget(pTarget);
}

void AccelAmplitude::update(float time)
{
    ((AccelAmplitude*)(m_pOther))->setAmplitudeRate(powf(time, m_fRate));
    m_pOther->update(time);
}

ActionInterval* AccelAmplitude::reverse(void)
{
    return AccelAmplitude::create(m_pOther->reverse(), m_fDuration);
}

// DeccelAmplitude

DeccelAmplitude* DeccelAmplitude::create(Action *pAction, float duration)
{
    DeccelAmplitude *pRet = new DeccelAmplitude();
    if (pRet)
    {
        if (pRet->initWithAction(pAction, duration))
        {
            pRet->autorelease();
        }
        else
        {
            AX_SAFE_DELETE(pRet);
        }
    }

    return pRet;
}


bool DeccelAmplitude::initWithAction(Action *pAction, float duration)
{
    if (ActionInterval::initWithDuration(duration))
    {
        m_fRate = 1.0f;
        m_pOther = (ActionInterval*)(pAction);
        pAction->retain();

        return true;
    }

    return false;
}

DeccelAmplitude::~DeccelAmplitude(void)
{
    AX_SAFE_RELEASE(m_pOther);
}

void DeccelAmplitude::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget(pTarget);
    m_pOther->startWithTarget(pTarget);
}

void DeccelAmplitude::update(float time)
{
    ((DeccelAmplitude*)(m_pOther))->setAmplitudeRate(powf((1 - time), m_fRate));
    m_pOther->update(time);
}

ActionInterval* DeccelAmplitude::reverse(void)
{
    return DeccelAmplitude::create(m_pOther->reverse(), m_fDuration);
}

// implementation of StopGrid

void StopGrid::startWithTarget(Node *pTarget)
{
    ActionInstant::startWithTarget(pTarget);

    GridBase *pGrid = m_pTarget->getGrid();
    if (pGrid && pGrid->isActive())
    {
        pGrid->setActive(false);
    }
}

StopGrid* StopGrid::create(void)
{
    StopGrid* pAction = new StopGrid();
    pAction->autorelease();

    return pAction;
}
// implementation of ReuseGrid

ReuseGrid* ReuseGrid::create(int times)
{
    ReuseGrid *pAction = new ReuseGrid();
    if (pAction)
    {
        if (pAction->initWithTimes(times))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_DELETE(pAction);
        }
    }

    return pAction;
}

bool ReuseGrid::initWithTimes(int times)
{
    m_nTimes = times;

    return true;
}

void ReuseGrid::startWithTarget(Node *pTarget)
{
    ActionInstant::startWithTarget(pTarget);

    if (m_pTarget->getGrid() && m_pTarget->getGrid()->isActive())
    {
        m_pTarget->getGrid()->setReuseGrid(m_pTarget->getGrid()->getReuseGrid() + m_nTimes);
    }
}

NS_AX_END
