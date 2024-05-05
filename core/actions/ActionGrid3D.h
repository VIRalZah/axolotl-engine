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
#ifndef __ACTION_AXGRID3D_ACTION_H__
#define __ACTION_AXGRID3D_ACTION_H__

#include "ActionGrid.h"

NS_AX_BEGIN

/**
 * @addtogroup actions
 * @{
 */

/** 
@brief Waves3D action 
*/
class AX_DLL Waves3D : public Grid3DAction
{
public:
    inline float getAmplitude(void) { return m_fAmplitude; }
    inline void setAmplitude(float fAmplitude) { m_fAmplitude = fAmplitude; }

    inline float getAmplitudeRate(void) { return m_fAmplitudeRate; }
    inline void setAmplitudeRate(float fAmplitudeRate) { m_fAmplitudeRate = fAmplitudeRate; }

    /** initializes an action with duration, grid size, waves and amplitude */
    bool initWithDuration(float duration, const Size& gridSize, unsigned int waves, float amplitude);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);

public:
    /** creates an action with duration, grid size, waves and amplitude */
    static Waves3D* create(float duration, const Size& gridSize, unsigned int waves, float amplitude);
protected:
    unsigned int m_nWaves;
    float m_fAmplitude;
    float m_fAmplitudeRate;
};

/** @brief FlipX3D action */
class AX_DLL FlipX3D : public Grid3DAction
{
public:
    /** initializes the action with duration */
    virtual bool initWithDuration(float duration);
    virtual bool initWithSize(const Size& gridSize, float duration);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);

public:
    /** creates the action with duration */
    static FlipX3D* create(float duration);
};

/** @brief FlipY3D action */
class AX_DLL FlipY3D : public FlipX3D
{
public:
    virtual void update(float time);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);

public:
    /** creates the action with duration */
    static FlipY3D* create(float duration);
};

/** @brief Lens3D action */
class AX_DLL Lens3D : public Grid3DAction
{
public:
    /** Get lens center position */
    inline float getLensEffect(void) { return m_fLensEffect; }
    /** Set lens center position */
    inline void setLensEffect(float fLensEffect) { m_fLensEffect = fLensEffect; }
    /** Set whether lens is concave */
    inline void setConcave(bool bConcave) { m_bConcave = bConcave; }
  
    inline const Vec2& getPosition(void) { return m_position; }
    void setPosition(const Vec2& position);

    /** initializes the action with center position, radius, a grid size and duration */
    bool initWithDuration(float duration, const Size& gridSize, const Vec2& position, float radius);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);

public:
    /** creates the action with center position, radius, a grid size and duration */
    static Lens3D* create(float duration, const Size& gridSize, const Vec2& position, float radius);
protected:
    /* lens center position */
    Vec2 m_position;
    float m_fRadius;
    /** lens effect. Defaults to 0.7 - 0 means no effect, 1 is very strong effect */
    float m_fLensEffect;
    /** lens is concave. (true = concave, false = convex) default is convex i.e. false */
    bool m_bConcave;

    bool    m_bDirty;
};

/** @brief Ripple3D action */
class AX_DLL Ripple3D : public Grid3DAction
{
public:
    /** get center position */
    inline const Vec2& getPosition(void) { return m_position; }
    /** set center position */
    void setPosition(const Vec2& position);

    inline float getAmplitude(void) { return m_fAmplitude; }
    inline void setAmplitude(float fAmplitude) { m_fAmplitude = fAmplitude; }

    inline float getAmplitudeRate(void) { return m_fAmplitudeRate; }
    inline void setAmplitudeRate(float fAmplitudeRate) { m_fAmplitudeRate = fAmplitudeRate; }

    /** initializes the action with radius, number of waves, amplitude, a grid size and duration */
    bool initWithDuration(float duration, const Size& gridSize, const Vec2& position, float radius, unsigned int waves, float amplitude);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);

public:
    /** creates the action with radius, number of waves, amplitude, a grid size and duration */
    static Ripple3D* create(float duration, const Size& gridSize, const Vec2& position, float radius, unsigned int waves, float amplitude);
protected:
    /* center position */
    Vec2 m_position;
    float m_fRadius;
    unsigned int m_nWaves;
    float m_fAmplitude;
    float m_fAmplitudeRate;
};

/** @brief Shaky3D action */
class AX_DLL Shaky3D : public Grid3DAction
{
public:
    /** initializes the action with a range, shake Z vertices, a grid and duration */
    bool initWithDuration(float duration, const Size& gridSize, int range, bool shakeZ);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);

public:
    /** creates the action with a range, shake Z vertices, a grid and duration */
    static Shaky3D* create(float duration, const Size& gridSize, int range, bool shakeZ);
protected:
    int m_nRandrange;
    bool m_bShakeZ;
};

/** @brief Liquid action */
class AX_DLL Liquid : public Grid3DAction
{
public:
    inline float getAmplitude(void) { return m_fAmplitude; }
    inline void setAmplitude(float fAmplitude) { m_fAmplitude = fAmplitude; }

    inline float getAmplitudeRate(void) { return m_fAmplitudeRate; }
    inline void setAmplitudeRate(float fAmplitudeRate) { m_fAmplitudeRate = fAmplitudeRate; }

    /** initializes the action with amplitude, a grid and duration */
    bool initWithDuration(float duration, const Size& gridSize, unsigned int waves, float amplitude);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);

public:
    /** creates the action with amplitude, a grid and duration */
    static Liquid* create(float duration, const Size& gridSize, unsigned int waves, float amplitude);
protected:
    unsigned int m_nWaves;
    float m_fAmplitude;
    float m_fAmplitudeRate;
};

/** @brief Waves action */
class AX_DLL Waves : public Grid3DAction
{
public:
    inline float getAmplitude(void) { return m_fAmplitude; }
    inline void setAmplitude(float fAmplitude) { m_fAmplitude = fAmplitude; }

    inline float getAmplitudeRate(void) { return m_fAmplitudeRate; }
    inline void setAmplitudeRate(float fAmplitudeRate) { m_fAmplitudeRate = fAmplitudeRate; }

    /** initializes the action with amplitude, horizontal sin, vertical sin, a grid and duration */
    bool initWithDuration(float duration, const Size& gridSize, unsigned int waves, float amplitude, bool horizontal, bool vertical);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);

public:

    /** initializes the action with amplitude, horizontal sin, vertical sin, a grid and duration */
    static Waves* create(float duration, const Size& gridSize, unsigned int waves, float amplitude, bool horizontal, bool vertical);
protected:
    unsigned int m_nWaves;
    float m_fAmplitude;
    float m_fAmplitudeRate;
    bool m_bVertical;
    bool m_bHorizontal;
};

/** @brief Twirl action */
class AX_DLL Twirl : public Grid3DAction
{
public:
    /** get twirl center */
    inline const Vec2& getPosition(void) { return m_position; }
    /** set twirl center */
    void setPosition(const Vec2& position);

    inline float getAmplitude(void) { return m_fAmplitude; }
    inline void setAmplitude(float fAmplitude) { m_fAmplitude = fAmplitude; }

    inline float getAmplitudeRate(void) { return m_fAmplitudeRate; }
    inline void setAmplitudeRate(float fAmplitudeRate) { m_fAmplitudeRate = fAmplitudeRate; }

    /** initializes the action with center position, number of twirls, amplitude, a grid size and duration */
    bool initWithDuration(float duration, const Size& gridSize, Vec2 position, unsigned int twirls, float amplitude);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);

public:
    /** creates the action with center position, number of twirls, amplitude, a grid size and duration */
    static Twirl* create(float duration, const Size& gridSize, Vec2 position, unsigned int twirls, float amplitude);
protected:
    /* twirl center */
    Vec2 m_position;
    unsigned int m_nTwirls;
    float m_fAmplitude;
    float m_fAmplitudeRate;
};

// end of actions group
/// @}

NS_AX_END

#endif // __ACTION_AXGRID3D_ACTION_H__
