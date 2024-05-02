/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2009 Jason Booth

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

#ifndef __ACTION_AXEASE_ACTION_H__
#define __ACTION_AXEASE_ACTION_H__

#include "ActionInterval.h"

NS_AX_BEGIN

class Object;
class Zone;

/**
 * @addtogroup actions
 * @{
 */

/** 
 @brief Base class for Easing actions
 @ingroup Actions
 */
class AX_DLL ActionEase : public ActionInterval
{
public:
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~ActionEase(void);

    /** initializes the action */
    bool initWithAction(ActionInterval *pAction);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void stop(void);
    virtual void update(float time);
    virtual ActionInterval* reverse(void);
    virtual ActionInterval* getInnerAction();

public:

    /** creates the action */
    static ActionEase* create(ActionInterval *pAction);

protected:
    /** The inner action */
    ActionInterval *m_pInner;
};

/** 
 @brief Base class for Easing actions with rate parameters
 @ingroup Actions
 */
class AX_DLL EaseRateAction : public ActionEase
{
public:
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~EaseRateAction(void);

    /** set rate value for the actions */
    inline void setRate(float rate) { m_fRate = rate; }
    /** get rate value for the actions */
    inline float getRate(void) { return m_fRate; }

    /** Initializes the action with the inner action and the rate parameter */
    bool initWithAction(ActionInterval *pAction, float fRate);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual ActionInterval* reverse(void);

public:

    /** Creates the action with the inner action and the rate parameter */
    static EaseRateAction* create(ActionInterval* pAction, float fRate);

protected:
    float m_fRate;
};

/** 
 @brief EaseIn action with a rate
 @ingroup Actions
 */
class AX_DLL EaseIn : public EaseRateAction
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

    /** Creates the action with the inner action and the rate parameter */
    static EaseIn* create(ActionInterval* pAction, float fRate);
};

/** 
 @brief EaseOut action with a rate
 @ingroup Actions
 */
class AX_DLL EaseOut : public EaseRateAction
{
public:
    virtual void update(float time);
    virtual ActionInterval* reverse();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);

public:

    /** Creates the action with the inner action and the rate parameter */
    static EaseOut* create(ActionInterval* pAction, float fRate);
};

/** 
 @brief EaseInOut action with a rate
 @ingroup Actions
 */
class AX_DLL EaseInOut : public EaseRateAction
{
public:
    virtual void update(float time);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual ActionInterval* reverse(void);

public:

    /** Creates the action with the inner action and the rate parameter */
    static EaseInOut* create(ActionInterval* pAction, float fRate);
};

/** 
 @brief CCEase Exponential In
 @ingroup Actions
 */
class AX_DLL EaseExponentialIn : public ActionEase
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
    static EaseExponentialIn* create(ActionInterval* pAction);
};

/** 
 @brief Ease Exponential Out
 @ingroup Actions
 */
class AX_DLL EaseExponentialOut : public ActionEase
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
    static EaseExponentialOut* create(ActionInterval* pAction);
};

/** 
 @brief Ease Exponential InOut
 @ingroup Actions
 */
class AX_DLL EaseExponentialInOut : public ActionEase
{
public:
    virtual void update(float time);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual ActionInterval* reverse();

public:

    /** creates the action */
    static EaseExponentialInOut* create(ActionInterval* pAction);
};

/** 
 @brief Ease Sine In
 @ingroup Actions
 */
class AX_DLL EaseSineIn : public ActionEase
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
    static EaseSineIn* create(ActionInterval* pAction);
};

/** 
 @brief Ease Sine Out
 @ingroup Actions
 */
class AX_DLL EaseSineOut : public ActionEase
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
    static EaseSineOut* create(ActionInterval* pAction);
};

/** 
 @brief Ease Sine InOut
 @ingroup Actions
 */
class AX_DLL EaseSineInOut : public ActionEase
{
public:
    virtual void update(float time);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual ActionInterval* reverse();

public:

    /** creates the action */
    static EaseSineInOut* create(ActionInterval* pAction);
};

/** 
 @brief Ease Elastic abstract class
 @since v0.8.2
 @ingroup Actions
 */
class AX_DLL EaseElastic : public ActionEase
{
public:
    /** get period of the wave in radians. default is 0.3 */
    inline float getPeriod(void) { return m_fPeriod; }
    /** set period of the wave in radians. */
    inline void setPeriod(float fPeriod) { m_fPeriod = fPeriod; }

    /** Initializes the action with the inner action and the period in radians (default is 0.3) */
    bool initWithAction(ActionInterval *pAction, float fPeriod = 0.3f);

    virtual ActionInterval* reverse(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);

public:

    /** Creates the action with the inner action and the period in radians (default is 0.3) */
    static EaseElastic* create(ActionInterval *pAction, float fPeriod);
    static EaseElastic* create(ActionInterval *pAction);
protected:
    float m_fPeriod;
};

/** 
 @brief Ease Elastic In action.
 @warning This action doesn't use a bijective function. Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class AX_DLL EaseElasticIn : public EaseElastic
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

    /** Creates the action with the inner action and the period in radians (default is 0.3) */
    static EaseElasticIn* create(ActionInterval *pAction, float fPeriod);
    static EaseElasticIn* create(ActionInterval *pAction);
};

/** 
 @brief Ease Elastic Out action.
 @warning This action doesn't use a bijective function. Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class AX_DLL EaseElasticOut : public EaseElastic
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

    /** Creates the action with the inner action and the period in radians (default is 0.3) */
    static EaseElasticOut* create(ActionInterval *pAction, float fPeriod);
    static EaseElasticOut* create(ActionInterval *pAction);
};

/** 
 @brief Ease Elastic InOut action.
 @warning This action doesn't use a bijective function. Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class AX_DLL EaseElasticInOut : public EaseElastic
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

    /** Creates the action with the inner action and the period in radians (default is 0.3) */
    static EaseElasticInOut* create(ActionInterval *pAction, float fPeriod);
    static EaseElasticInOut* create(ActionInterval *pAction);
};

/** 
 @brief EaseBounce abstract class.
 @since v0.8.2
 @ingroup Actions
*/
class AX_DLL EaseBounce : public ActionEase
{
public:
    float bounceTime(float time);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual ActionInterval* reverse();

public:

    /** creates the action */
    static EaseBounce* create(ActionInterval* pAction);
};

/** 
 @brief EaseBounceIn action.
 @warning This action doesn't use a bijective function. Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
*/
class AX_DLL EaseBounceIn : public EaseBounce
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
    static EaseBounceIn* create(ActionInterval* pAction);
};

/** 
 @brief EaseBounceOut action.
 @warning This action doesn't use a bijective function. Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class AX_DLL EaseBounceOut : public EaseBounce
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
    static EaseBounceOut* create(ActionInterval* pAction);
};

/** 
 @brief EaseBounceInOut action.
 @warning This action doesn't use a bijective function. Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class AX_DLL EaseBounceInOut : public EaseBounce
{
public:
    virtual void update(float time);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual ActionInterval* reverse();

public:

    /** creates the action */
    static EaseBounceInOut* create(ActionInterval* pAction);
};

/** 
 @brief EaseBackIn action.
 @warning This action doesn't use a bijective function. Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class AX_DLL EaseBackIn : public ActionEase
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
    static EaseBackIn* create(ActionInterval* pAction);
};

/** 
 @brief EaseBackOut action.
 @warning This action doesn't use a bijective function. Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class AX_DLL EaseBackOut : public ActionEase
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
    static EaseBackOut* create(ActionInterval* pAction);
};

/** 
 @brief EaseBackInOut action.
 @warning This action doesn't use a bijective function. Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class AX_DLL EaseBackInOut : public ActionEase
{
public:
    virtual void update(float time);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    virtual ActionInterval* reverse();

public:

    /** creates the action */
    static EaseBackInOut* create(ActionInterval* pAction);
};

// end of actions group
/// @}

NS_AX_END

#endif // __ACTION_AXEASE_ACTION_H__
