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
#include "ActionGrid3D.h"
#include "support/PointExtension.h"
#include "base/Director.h"
#include "base/Zone.h"
#include <stdlib.h>

NS_AX_BEGIN
// implementation of Waves3D

Waves3D* Waves3D::create(float duration, const Size& gridSize, unsigned int waves, float amplitude)
{
    Waves3D *pAction = new Waves3D();

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

bool Waves3D::initWithDuration(float duration, const Size& gridSize, unsigned int waves, float amplitude)
{
    if (Grid3DAction::initWithDuration(duration, gridSize))
    {
        m_nWaves = waves;
        m_fAmplitude = amplitude;
        m_fAmplitudeRate = 1.0f;

        return true;
    }

    return false;
}

Object* Waves3D::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    Waves3D* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (Waves3D*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new Waves3D();
        pZone = pNewZone = new Zone(pCopy);
    }

    Grid3DAction::copyWithZone(pZone);


    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_nWaves, m_fAmplitude);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void Waves3D::update(float time)
{
    int i, j;
    for (i = 0; i < m_sGridSize.width + 1; ++i)
    {
        for (j = 0; j < m_sGridSize.height + 1; ++j)
        {
            ccVertex3F v = originalVertex(Vec2(i ,j));
            v.z += (sinf((float)M_PI * time * m_nWaves * 2 + (v.y+v.x) * 0.01f) * m_fAmplitude * m_fAmplitudeRate);
            //AXLOG("v.z offset is %f\n", (sinf((float)M_PI * time * m_nWaves * 2 + (v.y+v.x) * .01f) * m_fAmplitude * m_fAmplitudeRate));
            setVertex(Vec2(i, j), v);
        }
    }
}

// implementation of FlipX3D

FlipX3D* FlipX3D::create(float duration)
{
    FlipX3D *pAction = new FlipX3D();

    if (pAction)
    {
        if (pAction->initWithDuration(duration))
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

bool FlipX3D::initWithDuration(float duration)
{
    return Grid3DAction::initWithDuration(duration, Size(1, 1));
}

bool FlipX3D::initWithSize(const Size& gridSize, float duration)
{
    if (gridSize.width != 1 || gridSize.height != 1)
    {
        // Grid size must be (1,1)
        AXAssert(0, "Grid size must be (1,1)");

        return false;
    }

    return Grid3DAction::initWithDuration(duration, gridSize);
}

Object* FlipX3D::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    FlipX3D* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (FlipX3D*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new FlipX3D();
        pZone = pNewZone = new Zone(pCopy);
    }

    Grid3DAction::copyWithZone(pZone);

    pCopy->initWithSize(m_sGridSize, m_fDuration);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void FlipX3D::update(float time)
{
    float angle = (float)M_PI * time; // 180 degrees
    float mz = sinf(angle);
    angle = angle / 2.0f; // x calculates degrees from 0 to 90
    float mx = cosf(angle);

    ccVertex3F v0, v1, v, diff;

    v0 = originalVertex(Vec2(1, 1));
    v1 = originalVertex(Vec2(0, 0));

    float    x0 = v0.x;
    float    x1 = v1.x;
    float x;
    Vec2    a, b, c, d;

    if ( x0 > x1 )
    {
        // Normal Grid
        a = Vec2(0,0);
        b = Vec2(0,1);
        c = Vec2(1,0);
        d = Vec2(1,1);
        x = x0;
    }
    else
    {
        // Reversed Grid
        c = Vec2(0,0);
        d = Vec2(0,1);
        a = Vec2(1,0);
        b = Vec2(1,1);
        x = x1;
    }
    
    diff.x = ( x - x * mx );
    diff.z = fabsf( floorf( (x * mz) / 4.0f ) );

    // bottom-left
    v = originalVertex(a);
    v.x = diff.x;
    v.z += diff.z;
    setVertex(a, v);
    
    // upper-left
    v = originalVertex(b);
    v.x = diff.x;
    v.z += diff.z;
    setVertex(b, v);
    
    // bottom-right
    v = originalVertex(c);
    v.x -= diff.x;
    v.z -= diff.z;
    setVertex(c, v);
    
    // upper-right
    v = originalVertex(d);
    v.x -= diff.x;
    v.z -= diff.z;
    setVertex(d, v);
}

// implementation of FlipY3D

FlipY3D* FlipY3D::create(float duration)
{
    FlipY3D *pAction = new FlipY3D();

    if (pAction)
    {
        if (pAction->initWithDuration(duration))
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

Object* FlipY3D::copyWithZone(Zone* pZone)
{
    Zone* pNewZone = NULL;
    FlipY3D* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (FlipY3D*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new FlipY3D();
        pZone = pNewZone = new Zone(pCopy);
    }

    FlipX3D::copyWithZone(pZone);

    pCopy->initWithSize(m_sGridSize, m_fDuration);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void FlipY3D::update(float time)
{
    float angle = (float)M_PI * time; // 180 degrees
    float mz = sinf( angle );
    angle = angle / 2.0f;     // x calculates degrees from 0 to 90
    float my = cosf(angle);
    
    ccVertex3F    v0, v1, v, diff;
    
    v0 = originalVertex(Vec2(1, 1));
    v1 = originalVertex(Vec2(0, 0));
    
    float    y0 = v0.y;
    float    y1 = v1.y;
    float y;
    Vec2    a, b, c, d;
    
    if (y0 > y1)
    {
        // Normal Grid
        a = Vec2(0,0);
        b = Vec2(0,1);
        c = Vec2(1,0);
        d = Vec2(1,1);
        y = y0;
    }
    else
    {
        // Reversed Grid
        b = Vec2(0,0);
        a = Vec2(0,1);
        d = Vec2(1,0);
        c = Vec2(1,1);
        y = y1;
    }
    
    diff.y = y - y * my;
    diff.z = fabsf(floorf((y * mz) / 4.0f));
    
    // bottom-left
    v = originalVertex(a);
    v.y = diff.y;
    v.z += diff.z;
    setVertex(a, v);
    
    // upper-left
    v = originalVertex(b);
    v.y -= diff.y;
    v.z -= diff.z;
    setVertex(b, v);
    
    // bottom-right
    v = originalVertex(c);
    v.y = diff.y;
    v.z += diff.z;
    setVertex(c, v);
    
    // upper-right
    v = originalVertex(d);
    v.y -= diff.y;
    v.z -= diff.z;
    setVertex(d, v);
}


// implementation of Lens3D

Lens3D* Lens3D::create(float duration, const Size& gridSize, const Vec2& position, float radius)
{
    Lens3D *pAction = new Lens3D();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize, position, radius))
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

bool Lens3D::initWithDuration(float duration, const Size& gridSize, const Vec2& position, float radius)
{
    if (Grid3DAction::initWithDuration(duration, gridSize))
    {
        m_position = Vec2(-1, -1);
        setPosition(position);
        m_fRadius = radius;
        m_fLensEffect = 0.7f;
        m_bConcave = false;
        m_bDirty = true;

        return true;
    }

    return false;
}

Object* Lens3D::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    Lens3D* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (Lens3D*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new Lens3D();
        pZone = pNewZone = new Zone(pCopy);
    }

    Grid3DAction::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_position, m_fRadius);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void Lens3D::setPosition(const Vec2& pos)
{
    if( !pos.equals(m_position))
    {
        m_position = pos;
        m_bDirty = true;
    }
}

void Lens3D::update(float time)
{
    AX_UNUSED_PARAM(time);
    if (m_bDirty)
    {
        int i, j;
        
        for (i = 0; i < m_sGridSize.width + 1; ++i)
        {
            for (j = 0; j < m_sGridSize.height + 1; ++j)
            {
                ccVertex3F v = originalVertex(Vec2(i, j));
                Vec2 vect = PointSub(m_position, Vec2(v.x, v.y));
                float r = PointLength(vect);
                
                if (r < m_fRadius)
                {
                    r = m_fRadius - r;
                    float pre_log = r / m_fRadius;
                    if ( pre_log == 0 ) 
                    {
                        pre_log = 0.001f;
                    }

                    float l = logf(pre_log) * m_fLensEffect;
                    float new_r = expf( l ) * m_fRadius;
                    
                    if (PointLength(vect) > 0)
                    {
                        vect = PointNormalize(vect);
                        Vec2 new_vect = PointMult(vect, new_r);
                        v.z += (m_bConcave ? -1.0f : 1.0f) * PointLength(new_vect) * m_fLensEffect;
                    }
                }
                
                setVertex(Vec2(i, j), v);
            }
        }
        
        m_bDirty = false;
    }
}

// implementation of Ripple3D

Ripple3D* Ripple3D::create(float duration, const Size& gridSize, const Vec2& position, float radius, unsigned int waves, float amplitude)
{
    Ripple3D *pAction = new Ripple3D();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize, position, radius, waves, amplitude))
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

bool Ripple3D::initWithDuration(float duration, const Size& gridSize, const Vec2& position, float radius, unsigned int waves, float amplitude)
{
    if (Grid3DAction::initWithDuration(duration, gridSize))
    {
        setPosition(position);
        m_fRadius = radius;
        m_nWaves = waves;
        m_fAmplitude = amplitude;
        m_fAmplitudeRate = 1.0f;

        return true;
    }

    return false;
}

void Ripple3D::setPosition(const Vec2& position)
{
    m_position = position;
}

Object* Ripple3D::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    Ripple3D* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (Ripple3D*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new Ripple3D();
        pZone = pNewZone = new Zone(pCopy);
    }

    Grid3DAction::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_position, m_fRadius, m_nWaves, m_fAmplitude);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void Ripple3D::update(float time)
{
    int i, j;

    for (i = 0; i < (m_sGridSize.width+1); ++i)
    {
        for (j = 0; j < (m_sGridSize.height+1); ++j)
        {
            ccVertex3F v = originalVertex(Vec2(i, j));
            Vec2 vect = PointSub(m_position, Vec2(v.x,v.y));
            float r = PointLength(vect);
            
            if (r < m_fRadius)
            {
                r = m_fRadius - r;
                float rate = powf(r / m_fRadius, 2);
                v.z += (sinf( time*(float)M_PI * m_nWaves * 2 + r * 0.1f) * m_fAmplitude * m_fAmplitudeRate * rate);
            }
            
            setVertex(Vec2(i, j), v);
        }
    }
}

// implementation of Shaky3D

Shaky3D* Shaky3D::create(float duration, const Size& gridSize, int range, bool shakeZ)
{
    Shaky3D *pAction = new Shaky3D();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize, range, shakeZ))
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

bool Shaky3D::initWithDuration(float duration, const Size& gridSize, int range, bool shakeZ)
{
    if (Grid3DAction::initWithDuration(duration, gridSize))
    {
        m_nRandrange = range;
        m_bShakeZ = shakeZ;

        return true;
    }

    return false;
}

Object* Shaky3D::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    Shaky3D* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (Shaky3D*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new Shaky3D();
        pZone = pNewZone = new Zone(pCopy);
    }

    Grid3DAction::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_nRandrange, m_bShakeZ);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void Shaky3D::update(float time)
{
    AX_UNUSED_PARAM(time);
    int i, j;

    for (i = 0; i < (m_sGridSize.width+1); ++i)
    {
        for (j = 0; j < (m_sGridSize.height+1); ++j)
        {
            ccVertex3F v = originalVertex(Vec2(i ,j));
            v.x += (rand() % (m_nRandrange*2)) - m_nRandrange;
            v.y += (rand() % (m_nRandrange*2)) - m_nRandrange;
            if (m_bShakeZ)
            {
                v.z += (rand() % (m_nRandrange*2)) - m_nRandrange;
            }
            
            setVertex(Vec2(i, j), v);
        }
    }
}

// implementation of Liquid

Liquid* Liquid::create(float duration, const Size& gridSize, unsigned int waves, float amplitude)
{
    Liquid *pAction = new Liquid();

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

bool Liquid::initWithDuration(float duration, const Size& gridSize, unsigned int waves, float amplitude)
{
    if (Grid3DAction::initWithDuration(duration, gridSize))
    {
        m_nWaves = waves;
        m_fAmplitude = amplitude;
        m_fAmplitudeRate = 1.0f;

        return true;
    }

    return false;
}

Object* Liquid::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    Liquid* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (Liquid*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new Liquid();
        pZone = pNewZone = new Zone(pCopy);
    }

    Grid3DAction::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_nWaves, m_fAmplitude);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void Liquid::update(float time)
{
    int i, j;

    for (i = 1; i < m_sGridSize.width; ++i)
    {
        for (j = 1; j < m_sGridSize.height; ++j)
        {
            ccVertex3F v = originalVertex(Vec2(i, j));
            v.x = (v.x + (sinf(time * (float)M_PI * m_nWaves * 2 + v.x * .01f) * m_fAmplitude * m_fAmplitudeRate));
            v.y = (v.y + (sinf(time * (float)M_PI * m_nWaves * 2 + v.y * .01f) * m_fAmplitude * m_fAmplitudeRate));
            setVertex(Vec2(i, j), v);
        }
    }
}

// implementation of Waves

Waves* Waves::create(float duration, const Size& gridSize, unsigned int waves, float amplitude, bool horizontal, bool vertical)
{
    Waves *pAction = new Waves();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize, waves, amplitude, horizontal, vertical))
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

bool Waves::initWithDuration(float duration, const Size& gridSize, unsigned int waves, float amplitude, bool horizontal, bool vertical)
{
    if (Grid3DAction::initWithDuration(duration, gridSize))
    {
        m_nWaves = waves;
        m_fAmplitude = amplitude;
        m_fAmplitudeRate = 1.0f;
        m_bHorizontal = horizontal;
        m_bVertical = vertical;

        return true;
    }

    return false;
}

Object* Waves::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    Waves* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (Waves*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new Waves();
        pZone = pNewZone = new Zone(pCopy);
    }

    Grid3DAction::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_nWaves, m_fAmplitude, m_bHorizontal, m_bVertical);
    
    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void Waves::update(float time)
{
    int i, j;

    for (i = 0; i < m_sGridSize.width + 1; ++i)
    {
        for (j = 0; j < m_sGridSize.height + 1; ++j)
        {
            ccVertex3F v = originalVertex(Vec2(i, j));

            if (m_bVertical)
            {
                v.x = (v.x + (sinf(time * (float)M_PI * m_nWaves * 2 + v.y * .01f) * m_fAmplitude * m_fAmplitudeRate));
            }

            if (m_bHorizontal)
            {
                v.y = (v.y + (sinf(time * (float)M_PI * m_nWaves * 2 + v.x * .01f) * m_fAmplitude * m_fAmplitudeRate));
            }

            setVertex(Vec2(i, j), v);
        }
    }
}

// implementation of Twirl

Twirl* Twirl::create(float duration, const Size& gridSize, Vec2 position, unsigned int twirls, float amplitude)
{
    Twirl *pAction = new Twirl();

    if (pAction)
    {
        if (pAction->initWithDuration(duration, gridSize, position, twirls, amplitude))
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

bool Twirl::initWithDuration(float duration, const Size& gridSize, Vec2 position, unsigned int twirls, float amplitude)
{
    if (Grid3DAction::initWithDuration(duration, gridSize))
    {
        setPosition(position);
        m_nTwirls = twirls;
        m_fAmplitude = amplitude;
        m_fAmplitudeRate = 1.0f;

        return true;
    }

    return false;
}

void Twirl::setPosition(const Vec2& position)
{
    m_position = position;
}

Object* Twirl::copyWithZone(Zone *pZone)
{
    Zone* pNewZone = NULL;
    Twirl* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (Twirl*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new Twirl();
        pZone = pNewZone = new Zone(pCopy);
    }

    Grid3DAction::copyWithZone(pZone);


    pCopy->initWithDuration(m_fDuration, m_sGridSize, m_position, m_nTwirls, m_fAmplitude);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

void Twirl::update(float time)
{
    int i, j;
    Vec2    c = m_position;
    
    for (i = 0; i < (m_sGridSize.width+1); ++i)
    {
        for (j = 0; j < (m_sGridSize.height+1); ++j)
        {
            ccVertex3F v = originalVertex(Vec2(i ,j));
            
            Vec2    avg = Vec2(i-(m_sGridSize.width/2.0f), j-(m_sGridSize.height/2.0f));
            float r = PointLength(avg);
            
            float amp = 0.1f * m_fAmplitude * m_fAmplitudeRate;
            float a = r * cosf( (float)M_PI/2.0f + time * (float)M_PI * m_nTwirls * 2 ) * amp;
            
            Vec2 d = Vec2(
                sinf(a) * (v.y-c.y) + cosf(a) * (v.x-c.x),
                cosf(a) * (v.y-c.y) - sinf(a) * (v.x-c.x));
            
            v.x = c.x + d.x;
            v.y = c.y + d.y;

            setVertex(Vec2(i ,j), v);
        }
    }
}

NS_AX_END

