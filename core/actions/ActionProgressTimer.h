/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (C) 2010      Lam Pham

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
#ifndef __ACTION_AXPROGRESS_TIMER_H__
#define __ACTION_AXPROGRESS_TIMER_H__

#include "ActionInterval.h"

NS_AX_BEGIN

/**
 * @addtogroup actions
 * @{
 */

/**
@brief Progress to percentage
@since v0.99.1
*/
class AX_DLL ProgressTo : public ActionInterval
{
public:
    /** Initializes with a duration and a percent */
    bool initWithDuration(float duration, float fPercent);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);

public:
    /** Creates and initializes with a duration and a percent */
    static ProgressTo* create(float duration, float fPercent);
protected:
    float m_fTo;
    float m_fFrom;
};

/**
@brief Progress from a percentage to another percentage
@since v0.99.1
*/
class AX_DLL ProgressFromTo : public ActionInterval
{
public:
    /** Initializes the action with a duration, a "from" percentage and a "to" percentage */
    bool initWithDuration(float duration, float fFromPercentage, float fToPercentage);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);
    virtual ActionInterval* reverse(void);
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);

public:

    /** Creates and initializes the action with a duration, a "from" percentage and a "to" percentage */
    static ProgressFromTo* create(float duration, float fFromPercentage, float fToPercentage);
protected:
    float m_fTo;
    float m_fFrom;
};

// end of actions group
/// @}

NS_AX_END

#endif // __ACTION_AXPROGRESS_TIMER_H__
