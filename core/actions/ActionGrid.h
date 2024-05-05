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
#ifndef __ACTION_AXGRID_ACTION_H__
#define __ACTION_AXGRID_ACTION_H__

#include "ActionInterval.h"
#include "ActionInstant.h"

NS_AX_BEGIN

class GridBase;

/**
 * @addtogroup actions
 * @{
 */

/** @brief Base class for Grid actions */
class AX_DLL GridAction : public ActionInterval
{
public:
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual ActionInterval* reverse(void);

    /** initializes the action with size and duration */
    virtual bool initWithDuration(float duration, const Size& gridSize);

    /** returns the grid */
    virtual GridBase* getGrid(void);

public:
    /** creates the action with size and duration */
    // We can't make this create function compatible with previous version, bindings-generator will be confused since they
    // have the same function name and the same number of arguments. So sorry about that.
    //AX_DEPRECATED_ATTRIBUTE static GridAction* create(const Size& gridSize, float duration);

    /** creates the action with size and duration */
    static GridAction* create(float duration, const Size& gridSize);
protected:
    Size m_sGridSize;
};

/** 
 @brief Base class for Grid3D actions.
 Grid3D actions can modify a non-tiled grid.
 */
class AX_DLL Grid3DAction : public GridAction
{
public:
    /** returns the grid */
    virtual GridBase* getGrid(void);
    /** returns the vertex than belongs to certain position in the grid */
    ccVertex3F vertex(const Vec2& position);
    /** returns the non-transformed vertex than belongs to certain position in the grid */
    ccVertex3F originalVertex(const Vec2& position);
    /** sets a new vertex to a certain position of the grid */
    void setVertex(const Vec2& position, const ccVertex3F& vertex);

public:
    /** creates the action with size and duration 
     *  @js NA
     *  @lua NA
     */
    static Grid3DAction* create(float duration, const Size& gridSize);
};

/** @brief Base class for TiledGrid3D actions */
class AX_DLL TiledGrid3DAction : public GridAction
{
public:
    /** returns the tile that belongs to a certain position of the grid */
    ccQuad3 tile(const Vec2& position);
    /** returns the non-transformed tile that belongs to a certain position of the grid */
    ccQuad3 originalTile(const Vec2& position);
    /** sets a new tile to a certain position of the grid */
    void setTile(const Vec2& position, const ccQuad3& coords);

    /** returns the grid */
    virtual GridBase* getGrid(void);

public:
    /** creates the action with size and duration 
     *  @js NA
     *  @lua NA
     */
    static TiledGrid3DAction* create(float duration, const Size& gridSize);
};

/** @brief AccelDeccelAmplitude action */
class AX_DLL AccelDeccelAmplitude : public ActionInterval
{
public:
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~AccelDeccelAmplitude(void);
    /** initializes the action with an inner action that has the amplitude property, and a duration time */
    bool initWithAction(Action *pAction, float duration);

    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);
    virtual ActionInterval* reverse(void);

    /** get amplitude rate */
    inline float getRate(void) { return m_fRate; }
    /** set amplitude rate */
    inline void setRate(float fRate) { m_fRate = fRate; }

public:
    /** creates the action with an inner action that has the amplitude property, and a duration time */
    static AccelDeccelAmplitude* create(Action *pAction, float duration);

protected:
    float m_fRate;
    ActionInterval *m_pOther;
};

/** @brief AccelAmplitude action */
class AX_DLL AccelAmplitude : public ActionInterval
{
public:
    /**
     *  @js NA
     *  @lua NA
     */
    ~AccelAmplitude(void);
    /** initializes the action with an inner action that has the amplitude property, and a duration time */
    bool initWithAction(Action *pAction, float duration);

    /** get amplitude rate */
    inline float getRate(void) { return m_fRate; }
    /** set amplitude rate */
    inline void setRate(float fRate) { m_fRate = fRate; }

    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);
    virtual ActionInterval* reverse(void);

public:
    /** creates the action with an inner action that has the amplitude property, and a duration time */
    static AccelAmplitude* create(Action *pAction, float duration);
protected:
    float m_fRate;
    ActionInterval *m_pOther;
};

/** @brief DeccelAmplitude action */
class AX_DLL DeccelAmplitude : public ActionInterval
{
public:
    /**
     *  @js NA
     *  @lua NA
     */
    ~DeccelAmplitude(void);
    /** initializes the action with an inner action that has the amplitude property, and a duration time */
    bool initWithAction(Action *pAction, float duration);

    /** get amplitude rate */
    inline float getRate(void) { return m_fRate; }
    /** set amplitude rate */
    inline void setRate(float fRate) { m_fRate = fRate; }

    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);
    virtual ActionInterval* reverse(void);

public:
    /** creates the action with an inner action that has the amplitude property, and a duration time */
    static DeccelAmplitude* create(Action *pAction, float duration);

protected:
    float m_fRate;
    ActionInterval *m_pOther;
};

/** @brief StopGrid action.
 @warning Don't call this action if another grid action is active.
 Call if you want to remove the the grid effect. Example:
 Sequence::actions(Lens::action(...), StopGrid::action(...), NULL);
 */
class AX_DLL StopGrid : public ActionInstant
{
public:
    virtual void startWithTarget(Node *pTarget);

public:
    /** Allocates and initializes the action */
    static StopGrid* create(void);
};

/** @brief ReuseGrid action */
class AX_DLL ReuseGrid : public ActionInstant
{
public:
    /** initializes an action with the number of times that the current grid will be reused */
    bool initWithTimes(int times);

    virtual void startWithTarget(Node *pTarget);

public:
    /** creates an action with the number of times that the current grid will be reused */
    static ReuseGrid* create(int times);
protected:
    int m_nTimes;
};

// end of actions group
/// @}

NS_AX_END

#endif // __ACTION_AXGRID_ACTION_H__
