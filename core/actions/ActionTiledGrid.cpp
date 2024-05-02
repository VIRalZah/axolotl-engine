/****************************************************************************
Copyright (c) 20102011 cocos2d-x.org
Copyright (c) 2009       On-Core
 
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
#include "ActionTiledGrid.h"
#include "base/Director.h"
#include "ccMacros.h"
#include "support/PointExtension.h"
#include "effects/Grid.h"
#include "cocoa/Zone.h"
#include <stdlib.h>

NS_AX_BEGIN

struct Tile
{
    Point    position;
    Point    startPosition;
    Size    delta;
};

// implementation of ShakyTiles3D

ShakyTiles3D* ShakyTiles3D::create(float duration, const Size& gridSize, int nRange, bool bShakeZ)
{
    ShakyTiles3D *pAction = new ShakyTiles3D();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize, nRange, bShakeZ))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pAction);
        }
    }

    return pAction;    
}

bool ShakyTiles3D::initWithDuration(float duration, const Size& gridSize, int nRange, bool bShakeZ)
{
    if (TiledGrid3DAction::initWithDuration(duration, gridSize))
    {
        m_nRandrange = nRange;
        m_bShakeZ = bShakeZ;

        return true;
    }

    return false;
}

Object* ShakyTiles3D::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    ShakyTiles3D* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (ShakyTiles3D*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new ShakyTiles3D();
        pZone = pNewZone = new Zone(pCopy);
    }
    
    TiledGrid3DAction::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_nRandrange, m_bShakeZ);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void ShakyTiles3D::update(float time)
{
    AX_UNUSED_PARAM(time);
    int i, j;

    for (i = 0; i < m_sGridSize.width; ++i)
    {
        for (j = 0; j < m_sGridSize.height; ++j)
        {
            ccQuad3 coords = originalTile(Point(i, j));

            // X
            coords.bl.x += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
            coords.br.x += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
            coords.tl.x += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
            coords.tr.x += ( rand() % (m_nRandrange*2) ) - m_nRandrange;

            // Y
            coords.bl.y += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
            coords.br.y += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
            coords.tl.y += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
            coords.tr.y += ( rand() % (m_nRandrange*2) ) - m_nRandrange;

            if (m_bShakeZ)
            {
                coords.bl.z += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                coords.br.z += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                coords.tl.z += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                coords.tr.z += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
            }
                        
            setTile(Point(i, j), coords);
        }
    }
}

// implementation of ShatteredTiles3D

ShatteredTiles3D* ShatteredTiles3D::create(float duration, const Size& gridSize, int nRange, bool bShatterZ)
{
    ShatteredTiles3D *pAction = new ShatteredTiles3D();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize, nRange, bShatterZ))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pAction);
        }
    }

    return pAction;    
}

bool ShatteredTiles3D::initWithDuration(float duration, const Size& gridSize, int nRange, bool bShatterZ)
{
    if (TiledGrid3DAction::initWithDuration(duration, gridSize))
    {
        m_bOnce = false;
        m_nRandrange = nRange;
        m_bShatterZ = bShatterZ;

        return true;
    }

    return false;
}

Object* ShatteredTiles3D::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    ShatteredTiles3D* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        pCopy = (ShatteredTiles3D*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new ShatteredTiles3D();
        pZone = pNewZone = new Zone(pCopy);
    }

    //copy super class's member
    TiledGrid3DAction::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_nRandrange, m_bShatterZ);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void ShatteredTiles3D::update(float time)
{
    AX_UNUSED_PARAM(time);
    int i, j;

    if (m_bOnce == false)
    {
        for (i = 0; i < m_sGridSize.width; ++i)
        {
            for (j = 0; j < m_sGridSize.height; ++j)
            {
                ccQuad3 coords = originalTile(Point(i ,j));
                
                // X
                coords.bl.x += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                coords.br.x += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                coords.tl.x += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                coords.tr.x += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                
                // Y
                coords.bl.y += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                coords.br.y += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                coords.tl.y += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                coords.tr.y += ( rand() % (m_nRandrange*2) ) - m_nRandrange;

                if (m_bShatterZ) 
                {
                    coords.bl.z += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                    coords.br.z += ( rand() % (m_nRandrange*2) ) - m_nRandrange;                
                    coords.tl.z += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                    coords.tr.z += ( rand() % (m_nRandrange*2) ) - m_nRandrange;
                }
                
                setTile(Point(i, j), coords);
            }
        }
        
        m_bOnce = true;
    }
}

// implementation of ShuffleTiles

ShuffleTiles* ShuffleTiles::create(float duration, const Size& gridSize, unsigned int seed)
{
    ShuffleTiles *pAction = new ShuffleTiles();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize, seed))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pAction);
        }
    }

    return pAction;    
}

bool ShuffleTiles::initWithDuration(float duration, const Size& gridSize, unsigned int seed)
{
    if (TiledGrid3DAction::initWithDuration(duration, gridSize))
    {
        m_nSeed = seed;
        m_pTilesOrder = NULL;
        m_pTiles = NULL;

        return true;
    }

    return false;
}

Object* ShuffleTiles::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    ShuffleTiles* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        pCopy = (ShuffleTiles*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new ShuffleTiles();
        pZone = pNewZone = new Zone(pCopy);
    }

    TiledGrid3DAction::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_nSeed);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

ShuffleTiles::~ShuffleTiles(void)
{
    AX_SAFE_DELETE_ARRAY(m_pTilesOrder);
    AX_SAFE_DELETE_ARRAY(m_pTiles);
}

void ShuffleTiles::shuffle(unsigned int *pArray, unsigned int nLen)
{
    int i;
    for( i = nLen - 1; i >= 0; i-- )
    {
        unsigned int j = rand() % (i+1);
        unsigned int v = pArray[i];
        pArray[i] = pArray[j];
        pArray[j] = v;
    }
}

Size ShuffleTiles::getDelta(const Size& pos)
{
    Point    pos2;

    unsigned int idx = pos.width * m_sGridSize.height + pos.height;

    pos2.x = (float)(m_pTilesOrder[idx] / (int)m_sGridSize.height);
    pos2.y = (float)(m_pTilesOrder[idx] % (int)m_sGridSize.height);

    return CCSizeMake((int)(pos2.x - pos.width), (int)(pos2.y - pos.height));
}

void ShuffleTiles::placeTile(const Point& pos, Tile *t)
{
    ccQuad3 coords = originalTile(pos);

    Point step = m_pTarget->getGrid()->getStep();
    coords.bl.x += (int)(t->position.x * step.x);
    coords.bl.y += (int)(t->position.y * step.y);

    coords.br.x += (int)(t->position.x * step.x);
    coords.br.y += (int)(t->position.y * step.y);

    coords.tl.x += (int)(t->position.x * step.x);
    coords.tl.y += (int)(t->position.y * step.y);

    coords.tr.x += (int)(t->position.x * step.x);
    coords.tr.y += (int)(t->position.y * step.y);

    setTile(pos, coords);
}

void ShuffleTiles::startWithTarget(Node *pTarget)
{
    TiledGrid3DAction::startWithTarget(pTarget);

    if (m_nSeed != (unsigned int)-1)
    {
        srand(m_nSeed);
    }

    m_nTilesCount = m_sGridSize.width * m_sGridSize.height;
    m_pTilesOrder = new unsigned int[m_nTilesCount];
    int i, j;
    unsigned int k;

    /**
     * Use k to loop. Because m_nTilesCount is unsigned int,
     * and i is used later for int.
     */
    for (k = 0; k < m_nTilesCount; ++k)
    {
        m_pTilesOrder[k] = k;
    }

    shuffle(m_pTilesOrder, m_nTilesCount);

    m_pTiles = (struct Tile *)new Tile[m_nTilesCount];
    Tile *tileArray = (Tile*) m_pTiles;

    for (i = 0; i < m_sGridSize.width; ++i)
    {
        for (j = 0; j < m_sGridSize.height; ++j)
        {
            tileArray->position = Point((float)i, (float)j);
            tileArray->startPosition = Point((float)i, (float)j);
            tileArray->delta = getDelta(CCSizeMake(i, j));
            ++tileArray;
        }
    }
}

void ShuffleTiles::update(float time)
{
    int i, j;

    Tile *tileArray = (Tile*)m_pTiles;

    for (i = 0; i < m_sGridSize.width; ++i)
    {
        for (j = 0; j < m_sGridSize.height; ++j)
        {
            tileArray->position = PointMult(Point((float)tileArray->delta.width, (float)tileArray->delta.height), time);
            placeTile(Point(i, j), tileArray);
            ++tileArray;
        }
    }
}

// implementation of FadeOutTRTiles

FadeOutTRTiles* FadeOutTRTiles::create(float duration, const Size& gridSize)
{
    FadeOutTRTiles *pAction = new FadeOutTRTiles();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pAction);
        }
    }

    return pAction;    
}

float FadeOutTRTiles::testFunc(const Size& pos, float time)
{
    Point n = PointMult(Point((float)m_sGridSize.width, (float)m_sGridSize.height), time);
    if ((n.x + n.y) == 0.0f)
    {
        return 1.0f;
    }

    return powf((pos.width + pos.height) / (n.x + n.y), 6);
}

void FadeOutTRTiles::turnOnTile(const Point& pos)
{
    setTile(pos, originalTile(pos));
}

void FadeOutTRTiles::turnOffTile(const Point& pos)
{
    ccQuad3 coords;
    memset(&coords, 0, sizeof(ccQuad3));
    setTile(pos, coords);
}

void FadeOutTRTiles::transformTile(const Point& pos, float distance)
{
    ccQuad3 coords = originalTile(pos);
    Point step = m_pTarget->getGrid()->getStep();

    coords.bl.x += (step.x / 2) * (1.0f - distance);
    coords.bl.y += (step.y / 2) * (1.0f - distance);

    coords.br.x -= (step.x / 2) * (1.0f - distance);
    coords.br.y += (step.y / 2) * (1.0f - distance);

    coords.tl.x += (step.x / 2) * (1.0f - distance);
    coords.tl.y -= (step.y / 2) * (1.0f - distance);

    coords.tr.x -= (step.x / 2) * (1.0f - distance);
    coords.tr.y -= (step.y / 2) * (1.0f - distance);

    setTile(pos, coords);
}

void FadeOutTRTiles::update(float time)
{
    int i, j;

    for (i = 0; i < m_sGridSize.width; ++i)
    {
        for (j = 0; j < m_sGridSize.height; ++j)
        {
            float distance = testFunc(CCSizeMake(i, j), time);
            if ( distance == 0 )
            {
                turnOffTile(Point(i, j));
            } else 
            if (distance < 1)
            {
                transformTile(Point(i, j), distance);
            }
            else
            {
                turnOnTile(Point(i, j));
            }
        }
    }
}

// implementation of FadeOutBLTiles

FadeOutBLTiles* FadeOutBLTiles::create(float duration, const Size& gridSize)
{
    FadeOutBLTiles *pAction = new FadeOutBLTiles();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pAction);
        }
    }

    return pAction;
}

float FadeOutBLTiles::testFunc(const Size& pos, float time)
{
    Point n = PointMult(Point((float)m_sGridSize.width, (float)m_sGridSize.height), (1.0f - time));
    if ((pos.width + pos.height) == 0)
    {
        return 1.0f;
    }

    return powf((n.x + n.y) / (pos.width + pos.height), 6);
}

// implementation of FadeOutUpTiles

FadeOutUpTiles* FadeOutUpTiles::create(float duration, const Size& gridSize)
{
    FadeOutUpTiles *pAction = new FadeOutUpTiles();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pAction);
        }
    }

    return pAction;
}

float FadeOutUpTiles::testFunc(const Size& pos, float time)
{
    Point n = PointMult(Point((float)m_sGridSize.width, (float)m_sGridSize.height), time);
    if (n.y == 0.0f)
    {
        return 1.0f;
    }

    return powf(pos.height / n.y, 6);
}

void FadeOutUpTiles::transformTile(const Point& pos, float distance)
{
    ccQuad3 coords = originalTile(pos);
    Point step = m_pTarget->getGrid()->getStep();

    coords.bl.y += (step.y / 2) * (1.0f - distance);
    coords.br.y += (step.y / 2) * (1.0f - distance);
    coords.tl.y -= (step.y / 2) * (1.0f - distance);
    coords.tr.y -= (step.y / 2) * (1.0f - distance);

    setTile(pos, coords);
}

// implementation of FadeOutDownTiles

FadeOutDownTiles* FadeOutDownTiles::create(float duration, const Size& gridSize)
{
    FadeOutDownTiles *pAction = new FadeOutDownTiles();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pAction);
        }
    }

    return pAction;
}

float FadeOutDownTiles::testFunc(const Size& pos, float time)
{
    Point n = PointMult(Point((float)m_sGridSize.width, (float)m_sGridSize.height), (1.0f - time));
    if (pos.height == 0)
    {
        return 1.0f;
    }

    return powf(n.y / pos.height, 6);
}

// implementation of TurnOffTiles

TurnOffTiles* TurnOffTiles::create(float duration, const Size& gridSize)
{
    TurnOffTiles* pAction = new TurnOffTiles();
    if (pAction->initWithDuration(duration, gridSize, 0))
    {
        pAction->autorelease();
    }
    else
    {
        AX_SAFE_RELEASE_NULL(pAction);
    }
    return pAction;
}

TurnOffTiles* TurnOffTiles::create(float duration, const Size& gridSize, unsigned int seed)
{
    TurnOffTiles *pAction = new TurnOffTiles();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize, seed))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pAction);
        }
    }

    return pAction;
}

bool TurnOffTiles::initWithDuration(float duration, const Size& gridSize, unsigned int seed)
{
    if (TiledGrid3DAction::initWithDuration(duration, gridSize))
    {
        m_nSeed = seed;
        m_pTilesOrder = NULL;

        return true;
    }

    return false;
}

Object* TurnOffTiles::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    TurnOffTiles* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        pCopy = (TurnOffTiles*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new TurnOffTiles();
        pZone = pNewZone = new Zone(pCopy);
    }

    TiledGrid3DAction::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_nSeed );

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

TurnOffTiles::~TurnOffTiles(void)
{
    AX_SAFE_DELETE_ARRAY(m_pTilesOrder);
}

void TurnOffTiles::shuffle(unsigned int *pArray, unsigned int nLen)
{
    int i;
    for (i = nLen - 1; i >= 0; i--)
    {
        unsigned int j = rand() % (i+1);
        unsigned int v = pArray[i];
        pArray[i] = pArray[j];
        pArray[j] = v;
    }
}

void TurnOffTiles::turnOnTile(const Point& pos)
{
    setTile(pos, originalTile(pos));
}

void TurnOffTiles::turnOffTile(const Point& pos)
{
    ccQuad3 coords;

    memset(&coords, 0, sizeof(ccQuad3));
    setTile(pos, coords);
}

void TurnOffTiles::startWithTarget(Node *pTarget)
{
    unsigned int i;

    TiledGrid3DAction::startWithTarget(pTarget);

    if (m_nSeed != (unsigned int)-1)
    {
        srand(m_nSeed);
    }

    m_nTilesCount = m_sGridSize.width * m_sGridSize.height;
    m_pTilesOrder = new unsigned int[m_nTilesCount];

    for (i = 0; i < m_nTilesCount; ++i)
    {
        m_pTilesOrder[i] = i;
    }

    shuffle(m_pTilesOrder, m_nTilesCount);
}

void TurnOffTiles::update(float time)
{
    unsigned int i, l, t;

    l = (unsigned int)(time * (float)m_nTilesCount);

    for( i = 0; i < m_nTilesCount; i++ )
    {
        t = m_pTilesOrder[i];
        Point tilePos = Point( (unsigned int)(t / m_sGridSize.height), t % (unsigned int)m_sGridSize.height );

        if ( i < l )
        {
            turnOffTile(tilePos);
        }
        else
        {
            turnOnTile(tilePos);
        }
    }
}

// implementation of WavesTiles3D

WavesTiles3D* WavesTiles3D::create(float duration, const Size& gridSize, unsigned int waves, float amplitude)
{
    WavesTiles3D *pAction = new WavesTiles3D();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize, waves, amplitude))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pAction);
        }
    }

    return pAction;
}

bool WavesTiles3D::initWithDuration(float duration, const Size& gridSize, unsigned int waves, float amplitude)
{
    if (TiledGrid3DAction::initWithDuration(duration, gridSize))
    {
        m_nWaves = waves;
        m_fAmplitude = amplitude;
        m_fAmplitudeRate = 1.0f;

        return true;
    }

    return false;
}

Object* WavesTiles3D::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    WavesTiles3D* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        pCopy = (WavesTiles3D*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new WavesTiles3D();
        pZone = pNewZone = new Zone(pCopy);
    }

    TiledGrid3DAction::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_nWaves, m_fAmplitude);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void WavesTiles3D::update(float time)
{
    int i, j;

    for( i = 0; i < m_sGridSize.width; i++ )
    {
        for( j = 0; j < m_sGridSize.height; j++ )
        {
            ccQuad3 coords = originalTile(Point(i, j));

            coords.bl.z = (sinf(time * (float)M_PI  *m_nWaves * 2 + 
                (coords.bl.y+coords.bl.x) * .01f) * m_fAmplitude * m_fAmplitudeRate );
            coords.br.z    = coords.bl.z;
            coords.tl.z = coords.bl.z;
            coords.tr.z = coords.bl.z;

            setTile(Point(i, j), coords);
        }
    }
}

// implementation of JumpTiles3D

JumpTiles3D* JumpTiles3D::create(float duration, const Size& gridSize, unsigned int numberOfJumps, float amplitude)
{
    JumpTiles3D *pAction = new JumpTiles3D();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize, numberOfJumps, amplitude))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pAction);
        }
    }

    return pAction;
}

bool JumpTiles3D::initWithDuration(float duration, const Size& gridSize, unsigned int numberOfJumps, float amplitude)
{
    if (TiledGrid3DAction::initWithDuration(duration, gridSize))
    {
        m_nJumps = numberOfJumps;
        m_fAmplitude = amplitude;
        m_fAmplitudeRate = 1.0f;

        return true;
    }

    return false;
}

Object* JumpTiles3D::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    JumpTiles3D* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        pCopy = (JumpTiles3D*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new JumpTiles3D();
        pZone = pNewZone = new Zone(pCopy);
    }

    TiledGrid3DAction::copyWithZone(pZone);
    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_nJumps, m_fAmplitude);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void JumpTiles3D::update(float time)
{
    int i, j;

    float sinz =  (sinf((float)M_PI * time * m_nJumps * 2) * m_fAmplitude * m_fAmplitudeRate );
    float sinz2 = (sinf((float)M_PI * (time * m_nJumps * 2 + 1)) * m_fAmplitude * m_fAmplitudeRate );

    for( i = 0; i < m_sGridSize.width; i++ )
    {
        for( j = 0; j < m_sGridSize.height; j++ )
        {
            ccQuad3 coords = originalTile(Point(i, j));

            if ( ((i+j) % 2) == 0 )
            {
                coords.bl.z += sinz;
                coords.br.z += sinz;
                coords.tl.z += sinz;
                coords.tr.z += sinz;
            }
            else
            {
                coords.bl.z += sinz2;
                coords.br.z += sinz2;
                coords.tl.z += sinz2;
                coords.tr.z += sinz2;
            }

            setTile(Point(i, j), coords);
        }
    }
}

// implementation of SplitRows

SplitRows* SplitRows::create(float duration, unsigned int nRows)
{
    SplitRows *pAction = new SplitRows();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, nRows))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pAction);
        }
    }

    return pAction;
}

bool SplitRows::initWithDuration(float duration, unsigned int nRows)
{
    m_nRows = nRows;

    return TiledGrid3DAction::initWithDuration(duration, CCSizeMake(1, nRows));
}

Object* SplitRows::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    SplitRows* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        pCopy = (SplitRows*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new SplitRows();
        pZone = pNewZone = new Zone(pCopy);
    }

    TiledGrid3DAction::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_nRows);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void SplitRows::startWithTarget(Node *pTarget)
{
    TiledGrid3DAction::startWithTarget(pTarget);
    m_winSize = Director::sharedDirector()->getWinSizeInPixels();
}

void SplitRows::update(float time)
{
    unsigned int j;

    for (j = 0; j < m_sGridSize.height; ++j)
    {
        ccQuad3 coords = originalTile(Point(0, j));
        float    direction = 1;

        if ( (j % 2 ) == 0 )
        {
            direction = -1;
        }

        coords.bl.x += direction * m_winSize.width * time;
        coords.br.x += direction * m_winSize.width * time;
        coords.tl.x += direction * m_winSize.width * time;
        coords.tr.x += direction * m_winSize.width * time;

        setTile(Point(0, j), coords);
    }
}

// implementation of SplitCols

SplitCols* SplitCols::create(float duration, unsigned int nCols)
{
    SplitCols *pAction = new SplitCols();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, nCols))
        {
            pAction->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pAction);
        }
    }

    return pAction;
}

bool SplitCols::initWithDuration(float duration, unsigned int nCols)
{
    m_nCols = nCols;
    return TiledGrid3DAction::initWithDuration(duration, CCSizeMake(nCols, 1));
}

Object* SplitCols::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    SplitCols* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        pCopy = (SplitCols*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new SplitCols();
        pZone = pNewZone = new Zone(pCopy);
    }

    TiledGrid3DAction::copyWithZone(pZone);
    pCopy->initWithDuration(m_fDuration, m_nCols);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void SplitCols::startWithTarget(Node *pTarget)
{
    TiledGrid3DAction::startWithTarget(pTarget);
    m_winSize = Director::sharedDirector()->getWinSizeInPixels();
}

void SplitCols::update(float time)
{
    unsigned int i;

    for (i = 0; i < m_sGridSize.width; ++i)
    {
        ccQuad3 coords = originalTile(Point(i, 0));
        float    direction = 1;

        if ( (i % 2 ) == 0 )
        {
            direction = -1;
        }

        coords.bl.y += direction * m_winSize.height * time;
        coords.br.y += direction * m_winSize.height * time;
        coords.tl.y += direction * m_winSize.height * time;
        coords.tr.y += direction * m_winSize.height * time;

        setTile(Point(i, 0), coords);
    }
}

NS_AX_END
