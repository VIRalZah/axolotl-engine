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
#ifndef __ACTION_AXTILEDGRID_ACTION_H__
#define __ACTION_AXTILEDGRID_ACTION_H__

#include "ActionGrid.h"

NS_AX_BEGIN

/**
 * @addtogroup actions
 * @{
 */

/** @brief ShakyTiles3D action */
class AX_DLL ShakyTiles3D : public TiledGrid3DAction
{
public:
    /** initializes the action with a range, whether or not to shake Z vertices, a grid size, and duration */
    virtual bool initWithDuration(float duration, const Size& gridSize, int nRange, bool bShakeZ);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);

public:

    /** creates the action with a range, whether or not to shake Z vertices, a grid size, and duration */
    static ShakyTiles3D* create(float duration, const Size& gridSize, int nRange, bool bShakeZ);

protected:
    int m_nRandrange;
    bool m_bShakeZ;
};

/** @brief ShatteredTiles3D action */
class AX_DLL ShatteredTiles3D : public TiledGrid3DAction
{
public:
    /** initializes the action with a range, whether or not to shatter Z vertices, a grid size and duration */
    virtual bool initWithDuration(float duration, const Size& gridSize, int nRange, bool bShatterZ);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);

public:

    /** creates the action with a range, whether of not to shatter Z vertices, a grid size and duration */
    static ShatteredTiles3D* create(float duration, const Size& gridSize, int nRange, bool bShatterZ);
protected:
    int m_nRandrange;
    bool m_bOnce;
    bool m_bShatterZ;
};

struct Tile;
/** @brief ShuffleTiles action
 Shuffle the tiles in random order
 */
class AX_DLL ShuffleTiles : public TiledGrid3DAction
{
public:
    /**
     *  @js NA
     *  @lua NA
     */
    ~ShuffleTiles(void);
    /** initializes the action with a random seed, the grid size and the duration */
    virtual bool initWithDuration(float duration, const Size& gridSize, unsigned int seed);
    void shuffle(unsigned int *pArray, unsigned int nLen);
    Size getDelta(const Size& pos);
    void placeTile(const Vec2& pos, Tile *t);

    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);

public:
    /** creates the action with a random seed, the grid size and the duration */
    static ShuffleTiles* create(float duration, const Size& gridSize, unsigned int seed);
protected:
    unsigned int m_nSeed;
    unsigned int m_nTilesCount;
    unsigned int* m_pTilesOrder;
    Tile* m_pTiles;
};

/** @brief FadeOutTRTiles action
 Fades out the tiles in a Top-Right direction
 */
class AX_DLL FadeOutTRTiles : public TiledGrid3DAction
{
public:
    virtual float testFunc(const Size& pos, float time);
    void turnOnTile(const Vec2& pos);
    void turnOffTile(const Vec2& pos);
    virtual void transformTile(const Vec2& pos, float distance);
    virtual void update(float time);

public:

    /** creates the action with the grid size and the duration */
    static FadeOutTRTiles* create(float duration, const Size& gridSize);
};

/** @brief FadeOutBLTiles action.
 Fades out the tiles in a Bottom-Left direction
 */
class AX_DLL FadeOutBLTiles : public FadeOutTRTiles
{
public:
    virtual float testFunc(const Size& pos, float time);

public:

    /** creates the action with the grid size and the duration */
    static FadeOutBLTiles* create(float duration, const Size& gridSize);
};

/** @brief FadeOutUpTiles action.
 Fades out the tiles in upwards direction
 */
class AX_DLL FadeOutUpTiles : public FadeOutTRTiles
{
public:
    virtual float testFunc(const Size& pos, float time);
    virtual void transformTile(const Vec2& pos, float distance);

public:
    /** creates the action with the grid size and the duration */
    static FadeOutUpTiles* create(float duration, const Size& gridSize);

};

/** @brief FadeOutDownTiles action.
 Fades out the tiles in downwards direction
 */
class AX_DLL FadeOutDownTiles : public FadeOutUpTiles
{
public:
    virtual float testFunc(const Size& pos, float time);

public:

    /** creates the action with the grid size and the duration */
    static FadeOutDownTiles* create(float duration, const Size& gridSize);
};

/** @brief TurnOffTiles action.
 Turn off the files in random order
 */
class AX_DLL TurnOffTiles : public TiledGrid3DAction
{
public:
    /**
     *  @js NA
     *  @lua NA
     */
    ~TurnOffTiles(void);
    /** initializes the action with a random seed, the grid size and the duration */
    virtual bool initWithDuration(float duration, const Size& gridSize, unsigned int seed);
    void shuffle(unsigned int *pArray, unsigned int nLen);
    void turnOnTile(const Vec2& pos);
    void turnOffTile(const Vec2& pos);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);

public:

    /** creates the action with the grid size and the duration */
    static TurnOffTiles* create(float duration, const Size& gridSize);
    /** creates the action with a random seed, the grid size and the duration */
    static TurnOffTiles* create(float duration, const Size& gridSize, unsigned int seed);

protected:
    unsigned int    m_nSeed;
    unsigned int    m_nTilesCount;
    unsigned int*   m_pTilesOrder;
};

/** @brief WavesTiles3D action. */
class AX_DLL WavesTiles3D : public TiledGrid3DAction
{
public:
    /** waves amplitude */
    inline float getAmplitude(void) { return m_fAmplitude; }
    inline void setAmplitude(float fAmplitude) { m_fAmplitude = fAmplitude; }

    /** waves amplitude rate */
    inline float getAmplitudeRate(void) { return m_fAmplitudeRate; }
    inline void setAmplitudeRate(float fAmplitudeRate) { m_fAmplitudeRate = fAmplitudeRate; }

    /** initializes the action with a number of waves, the waves amplitude, the grid size and the duration */
    virtual bool initWithDuration(float duration, const Size& gridSize, unsigned int waves, float amplitude);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);

public:
    /** creates the action with a number of waves, the waves amplitude, the grid size and the duration */
    static WavesTiles3D* create(float duration, const Size& gridSize, unsigned int waves, float amplitude);
protected:
    unsigned int m_nWaves;
    float m_fAmplitude;
    float m_fAmplitudeRate;
};

/** @brief JumpTiles3D action.
 A sin function is executed to move the tiles across the Z axis
 */
class AX_DLL JumpTiles3D : public TiledGrid3DAction
{
public:
    /** amplitude of the sin*/
    inline float getAmplitude(void) { return m_fAmplitude; }
    inline void setAmplitude(float fAmplitude) { m_fAmplitude = fAmplitude; }

    /** amplitude rate */
    inline float getAmplitudeRate(void) { return m_fAmplitudeRate; }
    inline void setAmplitudeRate(float fAmplitudeRate) { m_fAmplitudeRate = fAmplitudeRate; }

    /** initializes the action with the number of jumps, the sin amplitude, the grid size and the duration */
    bool initWithDuration(float duration, const Size& gridSize, unsigned int numberOfJumps, float amplitude);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);

public:

    /** creates the action with the number of jumps, the sin amplitude, the grid size and the duration */
    static JumpTiles3D* create(float duration, const Size& gridSize, unsigned int numberOfJumps, float amplitude);
protected:
    unsigned int m_nJumps;
    float m_fAmplitude;
    float m_fAmplitudeRate;
};

/** @brief SplitRows action */
class AX_DLL SplitRows : public TiledGrid3DAction
{
public :
    /** initializes the action with the number of rows to split and the duration */
    virtual bool initWithDuration(float duration, unsigned int nRows);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);
    virtual void startWithTarget(Node *pTarget);

public:

    /** creates the action with the number of rows to split and the duration */
    static SplitRows* create(float duration, unsigned int nRows);
protected:
    unsigned int m_nRows;
    Size m_winSize;
};

/** @brief SplitCols action */
class AX_DLL SplitCols : public TiledGrid3DAction
{
public:
    /** initializes the action with the number of columns to split and the duration */
    virtual bool initWithDuration(float duration, unsigned int nCols);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);
    virtual void startWithTarget(Node *pTarget);

public:
    /** creates the action with the number of columns to split and the duration */
    static SplitCols* create(float duration, unsigned int nCols);
protected:
    unsigned int m_nCols;
    Size m_winSize;
};

// end of actions group
/// @}

NS_AX_END

#endif // __ACTION_AXTILEDGRID_ACTION_H__
