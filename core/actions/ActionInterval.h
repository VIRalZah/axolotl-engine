/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2011 Ricardo Quesada
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

#ifndef __ACTION_AXINTERVAL_ACTION_H__
#define __ACTION_AXINTERVAL_ACTION_H__

#include "base/Node.h"
#include "Action.h"
#include "Protocols.h"
#include "sprite_nodes/SpriteFrame.h"
#include "sprite_nodes/Animation.h"
#include <vector>

NS_AX_BEGIN

/**
 * @addtogroup actions
 * @{
 */

/** 
@brief An interval action is an action that takes place within a certain period of time.
It has an start time, and a finish time. The finish time is the parameter
duration plus the start time.

These ActionInterval actions have some interesting properties, like:
- They can run normally (default)
- They can run reversed with the reverse method
- They can run with the time altered with the Accelerate, AccelDeccel and Speed actions.

For example, you can simulate a Ping Pong effect running the action normally and
then running it again in Reverse mode.

Example:

Action *pingPongAction = Sequence::actions(action, action->reverse(), NULL);
*/
class AX_DLL ActionInterval : public FiniteTimeAction
{
public:
    /** how many seconds had elapsed since the actions started to run. */
    inline float getElapsed(void) { return m_elapsed; }

    /** initializes the action */
    bool initWithDuration(float d);

    /** returns true if the action has finished */
    virtual bool isDone(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void step(float dt);
    virtual void startWithTarget(Node *pTarget);
    /** returns a reversed action */
    virtual ActionInterval* reverse(void);

public:

    /** creates the action */
    static ActionInterval* create(float d);

public:
    //extension in GridAction 
    void setAmplitudeRate(float amp);
    float getAmplitudeRate(void);

protected:
    float m_elapsed;
    bool   m_bFirstTick;
};

/** @brief Runs actions sequentially, one after another
 */
class AX_DLL Sequence : public ActionInterval
{
public:
    /**
     * @js NA
     * @lua NA
     */
    ~Sequence(void);

    /** initializes the action 
     * @lua NA
     */
    bool initWithTwoActions(FiniteTimeAction *pActionOne, FiniteTimeAction *pActionTwo);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    /**
     * @lua NA
     */
    virtual void startWithTarget(Node *pTarget);
    /**
     * @lua NA
     */
    virtual void stop(void);
    /**
     * @lua NA
     */
    virtual void update(float t);
    virtual ActionInterval* reverse(void);

public:

    /** helper constructor to create an array of sequenceable actions 
     * @lua NA
     */
    static Sequence* create(FiniteTimeAction *pAction1, ...);
    /** helper constructor to create an array of sequenceable actions given an array 
     * @js NA
     */
    static Sequence* create(Array *arrayOfActions);
    /** helper constructor to create an array of sequence-able actions 
     * @js NA
     * @lua NA
     */
    static Sequence* createWithVariableList(FiniteTimeAction *pAction1, va_list args);
    /** creates the action 
     * @js NA
     */
    static Sequence* createWithTwoActions(FiniteTimeAction *pActionOne, FiniteTimeAction *pActionTwo);

protected:
    FiniteTimeAction *m_pActions[2];
    float m_split;
    int m_last;
};

/** @brief Repeats an action a number of times.
 * To repeat an action forever use the RepeatForever action.
 */
class AX_DLL Repeat : public ActionInterval
{
public:
    /**
     *  @js NA
     *  @lua NA
     */
    ~Repeat(void);

    /** initializes a Repeat action. Times is an unsigned integer between 1 and pow(2,30) */
    bool initWithAction(FiniteTimeAction *pAction, unsigned int times);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void stop(void);
    virtual void update(float dt);
    virtual bool isDone(void);
    virtual ActionInterval* reverse(void);

    inline void setInnerAction(FiniteTimeAction *pAction)
    {
        if (m_pInnerAction != pAction)
        {
            AX_SAFE_RETAIN(pAction);
            AX_SAFE_RELEASE(m_pInnerAction);
            m_pInnerAction = pAction;
        }
    }

    inline FiniteTimeAction* getInnerAction()
    {
        return m_pInnerAction;
    }

public:

    /** creates a Repeat action. Times is an unsigned integer between 1 and pow(2,30) */
    static Repeat* create(FiniteTimeAction *pAction, unsigned int times);
protected:
    unsigned int m_uTimes;
    unsigned int m_uTotal;
    float m_fNextDt;
    bool m_bActionInstant;
    /** Inner action */
    FiniteTimeAction *m_pInnerAction;
};

/** @brief Repeats an action for ever.
To repeat the an action for a limited number of times use the Repeat action.
@warning This action can't be Sequenceable because it is not an IntervalAction
*/
class AX_DLL RepeatForever : public ActionInterval
{
public:
    /**
     *  @js ctor
     */
    RepeatForever()
        : m_pInnerAction(NULL)
    {}
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~RepeatForever();

    /** initializes the action */
    bool initWithAction(ActionInterval *pAction);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);
    virtual void startWithTarget(Node* pTarget);
    virtual void step(float dt);
    virtual bool isDone(void);
    virtual ActionInterval* reverse(void);

    inline void setInnerAction(ActionInterval *pAction)
    {
        if (m_pInnerAction != pAction)
        {
            AX_SAFE_RELEASE(m_pInnerAction);
            m_pInnerAction = pAction;
            AX_SAFE_RETAIN(m_pInnerAction);
        }
    }

    inline ActionInterval* getInnerAction()
    {
        return m_pInnerAction;
    }

public:

    /** creates the action */
    static RepeatForever* create(ActionInterval *pAction);
protected:
    /** Inner action */
    ActionInterval *m_pInnerAction;
};

/** @brief Spawn a new action immediately
 */
class AX_DLL Spawn : public ActionInterval
{
public:
    /**
     * @js NA
     * @lua NA
     */
    ~Spawn(void);

    /** initializes the Spawn action with the 2 actions to spawn 
     * @lua NA
     */
    bool initWithTwoActions(FiniteTimeAction *pAction1, FiniteTimeAction *pAction2);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    /**
     * @lua NA
     */
    virtual void startWithTarget(Node *pTarget);
    /**
     * @lua NA
     */
    virtual void stop(void);
    /**
     * @lua NA
     */
    virtual void update(float time);
    virtual ActionInterval* reverse(void);

public:

    /** helper constructor to create an array of spawned actions 
     * @lua NA
     */
    static Spawn* create(FiniteTimeAction *pAction1, ...);
    
    /** helper constructor to create an array of spawned actions 
     * @js  NA
     * @lua NA
     */
    static Spawn* createWithVariableList(FiniteTimeAction *pAction1, va_list args);

    /** helper constructor to create an array of spawned actions given an array 
     * @js NA
     */
    static Spawn* create(Array *arrayOfActions);

    /** creates the Spawn action 
     * @js NA
     */
    static Spawn* createWithTwoActions(FiniteTimeAction *pAction1, FiniteTimeAction *pAction2);

protected:
    FiniteTimeAction *m_pOne;
    FiniteTimeAction *m_pTwo;
};

/** @brief Rotates a Node object to a certain angle by modifying it's
 rotation attribute.
 The direction will be decided by the shortest angle.
*/ 
class AX_DLL RotateTo : public ActionInterval
{
public:
    /** creates the action */
    static RotateTo* create(float fDuration, float fDeltaAngle);
    /** initializes the action */
    bool initWithDuration(float fDuration, float fDeltaAngle);
    
    /** creates the action with separate rotation angles */
    static RotateTo* create(float fDuration, float fDeltaAngleX, float fDeltaAngleY);
    virtual bool initWithDuration(float fDuration, float fDeltaAngleX, float fDeltaAngleY);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);
    
protected:
    float m_fDstAngleX;
    float m_fStartAngleX;
    float m_fDiffAngleX;
    
    float m_fDstAngleY;
    float m_fStartAngleY;
    float m_fDiffAngleY;
};

/** @brief Rotates a Node object clockwise a number of degrees by modifying it's rotation attribute.
*/
class AX_DLL RotateBy : public ActionInterval
{
public:
    /** creates the action */
    static RotateBy* create(float fDuration, float fDeltaAngle);
    /** initializes the action */
    bool initWithDuration(float fDuration, float fDeltaAngle);
    
    static RotateBy* create(float fDuration, float fDeltaAngleX, float fDeltaAngleY);
    bool initWithDuration(float fDuration, float fDeltaAngleX, float fDeltaAngleY);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);
    virtual ActionInterval* reverse(void);
    
protected:
    float m_fAngleX;
    float m_fStartAngleX;
    float m_fAngleY;
    float m_fStartAngleY;
};

/**  Moves a Node object x,y pixels by modifying it's position attribute.
 x and y are relative to the position of the object.
 Several MoveBy actions can be concurrently called, and the resulting
 movement will be the sum of individual movements.
 @since v2.1beta2-custom
 */
class AX_DLL MoveBy : public ActionInterval
{
public:
    /** initializes the action */
    bool initWithDuration(float duration, const Vec2& deltaPosition);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual ActionInterval* reverse(void);
    virtual void update(float time);

public:
    /** creates the action */
    static MoveBy* create(float duration, const Vec2& deltaPosition);
protected:
    Vec2 m_positionDelta;
    Vec2 m_startPosition;
    Vec2 m_previousPosition;
};

/** Moves a Node object to the position x,y. x and y are absolute coordinates by modifying it's position attribute.
 Several MoveTo actions can be concurrently called, and the resulting
 movement will be the sum of individual movements.
 @since v2.1beta2-custom
 */
class AX_DLL MoveTo : public MoveBy
{
public:
    /** initializes the action */
    bool initWithDuration(float duration, const Vec2& position);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);

public:
    /** creates the action */
    static MoveTo* create(float duration, const Vec2& position);
protected:
    Vec2 m_endPosition;
};

/** Skews a Node object to given angles by modifying it's skewX and skewY attributes
@since v1.0
*/
class AX_DLL SkewTo : public ActionInterval
{
public:
    /**
     *  @js ctor
     */
    SkewTo();
    virtual bool initWithDuration(float t, float sx, float sy);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);

public:

    /** creates the action */
    static SkewTo* create(float t, float sx, float sy);
protected:
    float m_fSkewX;
    float m_fSkewY;
    float m_fStartSkewX;
    float m_fStartSkewY;
    float m_fEndSkewX;
    float m_fEndSkewY;
    float m_fDeltaX;
    float m_fDeltaY;
};

/** Skews a Node object by skewX and skewY degrees
@since v1.0
*/
class AX_DLL SkewBy : public SkewTo
{
public:
    virtual bool initWithDuration(float t, float sx, float sy);
    virtual void startWithTarget(Node *pTarget);
    virtual ActionInterval* reverse(void);

public:

    /** creates the action */
    static SkewBy* create(float t, float deltaSkewX, float deltaSkewY);
};

/** @brief Moves a Node object simulating a parabolic jump movement by modifying it's position attribute.
*/
class AX_DLL JumpBy : public ActionInterval
{
public:
    /** initializes the action */
    bool initWithDuration(float duration, const Vec2& position, float height, unsigned int jumps);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);
    virtual ActionInterval* reverse(void);

public:
    /** creates the action */
    static JumpBy* create(float duration, const Vec2& position, float height, unsigned int jumps);
protected:
    Vec2         m_startPosition;
    Vec2         m_delta;
    float           m_height;
    unsigned int    m_nJumps;
    Vec2         m_previousPos;
};

/** @brief Moves a Node object to a parabolic position simulating a jump movement by modifying it's position attribute.
*/ 
class AX_DLL JumpTo : public JumpBy
{
public:
    virtual void startWithTarget(Node *pTarget);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);

public:
    /** creates the action */
    static JumpTo* create(float duration, const Vec2& position, float height, int jumps);
};

/** @typedef bezier configuration structure
 */
typedef struct _ccBezierConfig {
    //! end position of the bezier
    Vec2 endPosition;
    //! Bezier control point 1
    Vec2 controlPoint_1;
    //! Bezier control point 2
    Vec2 controlPoint_2;
} ccBezierConfig;

/** @brief An action that moves the target with a cubic Bezier curve by a certain distance.
 */
class AX_DLL BezierBy : public ActionInterval
{
public:
    /** initializes the action with a duration and a bezier configuration 
     *  @lua NA
     */
    bool initWithDuration(float t, const ccBezierConfig& c);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    /**
     *  @lua NA
     */
    virtual void startWithTarget(Node *pTarget);
    /**
     *  @lua NA
     */
    virtual void update(float time);
    virtual ActionInterval* reverse(void);

public:
    /** creates the action with a duration and a bezier configuration 
     *  @code
     *  when this function bound to js,the input params are changed
     *  js: var create(var t, var pointTable)
     *  @endcode
     */
    static BezierBy* create(float t, const ccBezierConfig& c);
protected:
    ccBezierConfig m_sConfig;
    Vec2 m_startPosition;
    Vec2 m_previousPosition;
};

/** @brief An action that moves the target with a cubic Bezier curve to a destination point.
 @since v0.8.2
 */
class AX_DLL BezierTo : public BezierBy
{
public:
    /**
     *  @lua NA
     */
    virtual void startWithTarget(Node *pTarget);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);

public:

    /** creates the action with a duration and a bezier configuration
     *  @code
     *  when this function bound to js,the input params are changed
     *  js: var create(var t, var pointTable)
     *  @endcode
     */
    static BezierTo* create(float t, const ccBezierConfig& c);
    /**
     *  @lua NA
     */
    bool initWithDuration(float t, const ccBezierConfig &c);
    
protected:
    ccBezierConfig m_sToConfig;
};

/** @brief Scales a Node object to a zoom factor by modifying it's scale attribute.
 @warning This action doesn't support "reverse"
 */
class AX_DLL ScaleTo : public ActionInterval
{
public:
    /** initializes the action with the same scale factor for X and Y */
    bool initWithDuration(float duration, float s);

    /** initializes the action with and X factor and a Y factor */
    bool initWithDuration(float duration, float sx, float sy);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);

public:

    /** creates the action with the same scale factor for X and Y */
    static ScaleTo* create(float duration, float s);

    /** creates the action with and X factor and a Y factor */
    static ScaleTo* create(float duration, float sx, float sy);
protected:
    float _scaleX;
    float _scaleY;
    float m_fStartScaleX;
    float m_fStartScaleY;
    float m_fEndScaleX;
    float m_fEndScaleY;
    float m_fDeltaX;
    float m_fDeltaY;
};

/** @brief Scales a Node object a zoom factor by modifying it's scale attribute.
*/
class AX_DLL ScaleBy : public ScaleTo
{
public:
    virtual void startWithTarget(Node *pTarget);
    virtual ActionInterval* reverse(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);

public:

    /** creates the action with the same scale factor for X and Y */
    static ScaleBy* create(float duration, float s);

    /** creates the action with and X factor and a Y factor */
    static ScaleBy* create(float duration, float sx, float sy);
};

/** @brief Blinks a Node object by modifying it's visible attribute
*/
class AX_DLL Blink : public ActionInterval
{
public:
    /** initializes the action */
    bool initWithDuration(float duration, unsigned int uBlinks);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void update(float time);
    virtual ActionInterval* reverse(void);

public:

    /** creates the action */
    static Blink* create(float duration, unsigned int uBlinks);
    
    virtual void startWithTarget(Node *pTarget);
    virtual void stop();
    
protected:
    unsigned int m_nTimes;
    bool m_bOriginalState;
};

/** @brief Fades In an object that implements the RGBAProtocol protocol. It modifies the opacity from 0 to 255.
 The "reverse" of this action is FadeOut
 */
class AX_DLL FadeIn : public ActionInterval
{
public:
    virtual void update(float time);
    virtual ActionInterval* reverse(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);

public:
    /** creates the action */
    static FadeIn* create(float d);
};

/** @brief Fades Out an object that implements the RGBAProtocol protocol. It modifies the opacity from 255 to 0.
 The "reverse" of this action is FadeIn
*/
class AX_DLL FadeOut : public ActionInterval
{
public:
    virtual void update(float time);
    virtual ActionInterval* reverse(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);

public:

    /** creates the action */
    static FadeOut* create(float d);
};

/** @brief Fades an object that implements the RGBAProtocol protocol. It modifies the opacity from the current value to a custom one.
 @warning This action doesn't support "reverse"
 */
class AX_DLL FadeTo : public ActionInterval
{
public:
    /** initializes the action with duration and opacity */
    bool initWithDuration(float duration, GLubyte opacity);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);

public:
    /** creates an action with duration and opacity */
    static FadeTo* create(float duration, GLubyte opacity);
protected:
    GLubyte m_toOpacity;
    GLubyte m_fromOpacity;
};

/** @brief Tints a Node that implements the CCNodeRGB protocol from current tint to a custom one.
 @warning This action doesn't support "reverse"
 @since v0.7.2
*/
class AX_DLL TintTo : public ActionInterval
{
public:
    /** initializes the action with duration and color */
    bool initWithDuration(float duration, GLubyte red, GLubyte green, GLubyte blue);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);

public:
    /** creates an action with duration and color */
    static TintTo* create(float duration, GLubyte red, GLubyte green, GLubyte blue);
protected:
    ccColor3B m_to;
    ccColor3B m_from;
};

/** @brief Tints a Node that implements the CCNodeRGB protocol from current tint to a custom one.
 @since v0.7.2
 */
class AX_DLL TintBy : public ActionInterval
{
public:
    /** initializes the action with duration and color */
    bool initWithDuration(float duration, GLshort deltaRed, GLshort deltaGreen, GLshort deltaBlue);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);
    virtual ActionInterval* reverse(void);

public:
    /** creates an action with duration and color */
    static TintBy* create(float duration, GLshort deltaRed, GLshort deltaGreen, GLshort deltaBlue);
protected:
    GLshort m_deltaR;
    GLshort m_deltaG;
    GLshort m_deltaB;

    GLshort m_fromR;
    GLshort m_fromG;
    GLshort m_fromB;
};

/** @brief Delays the action a certain amount of seconds
*/
class AX_DLL DelayTime : public ActionInterval
{
public:
    virtual void update(float time);
    virtual ActionInterval* reverse(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);

public:

    /** creates the action */
    static DelayTime* create(float d);
};

/** @brief Executes an action in reverse order, from time=duration to time=0
 
 @warning Use this action carefully. This action is not
 sequenceable. Use it as the default "reversed" method
 of your own actions, but using it outside the "reversed"
 scope is not recommended.
*/
class AX_DLL ReverseTime : public ActionInterval
{
public:
    /**
     *  @js ctor
     */
    ~ReverseTime(void);
    /**
     *  @js NA
     *  @lua NA
     */
    ReverseTime();

    /** initializes the action */
    bool initWithAction(FiniteTimeAction *pAction);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void stop(void);
    virtual void update(float time);
    virtual ActionInterval* reverse(void);

public:
    /** creates the action */
    static ReverseTime* create(FiniteTimeAction *pAction);
protected:
    FiniteTimeAction *m_pOther;
};

class Texture2D;
/** @brief Animates a sprite given the name of an Animation */
class AX_DLL Animate : public ActionInterval
{
public:
    /**
     *  @js ctor
     */
    Animate();
    /**
     *  @js NA
     *  @lua NA
     */
    ~Animate();

    /** initializes the action with an Animation and will restore the original frame when the animation is over */
    bool initWithAnimation(Animation *pAnimation);

    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void stop(void);
    virtual void update(float t);
    virtual ActionInterval* reverse(void);

public:
    /** creates the action with an Animation and will restore the original frame when the animation is over */
    static Animate* create(Animation *pAnimation);
    AX_SYNTHESIZE_RETAIN(Animation*, m_pAnimation, Animation)
protected:
    std::vector<float>* m_pSplitTimes;
    int                m_nNextFrame;
    SpriteFrame*  m_pOrigFrame;
       unsigned int    m_uExecutedLoops;
};

/** Overrides the target of an action so that it always runs on the target
 * specified at action creation rather than the one specified by runAction.
 */
class AX_DLL TargetedAction : public ActionInterval
{
public:
    /**
     *  @js ctor
     */
    TargetedAction();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TargetedAction();

    /** Create an action with the specified action and forced target */
    static TargetedAction* create(Node* pTarget, FiniteTimeAction* pAction);

    /** Init an action with the specified action and forced target */
    bool initWithTarget(Node* pTarget, FiniteTimeAction* pAction);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void stop(void);
    virtual void update(float time);

    /** This is the target that the action will be forced to run with */
    AX_SYNTHESIZE_RETAIN(Node*, m_pForcedTarget, ForcedTarget);
private:
    FiniteTimeAction* m_pAction;
};

// end of actions group
/// @}

NS_AX_END

#endif //__ACTION_AXINTERVAL_ACTION_H__
