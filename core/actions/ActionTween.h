/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright 2009 lhunath (Maarten Billemont)

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
#ifndef __AXACTIONTWEEN_H__
#define __AXACTIONTWEEN_H__

#include "ActionInterval.h"

NS_AX_BEGIN

/**
 * @addtogroup actions
 * @{
 */

class AX_DLL ActionTweenDelegate
{
public:
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~ActionTweenDelegate() {}
    virtual void updateTweenAction(float value, const char* key) = 0;
};

/** ActionTween

 ActionTween is an action that lets you update any property of an object.
 For example, if you want to modify the "width" property of a target from 200 to 300 in 2 seconds, then:

    id modifyWidth = [ActionTween actionWithDuration:2 key:@"width" from:200 to:300];
    [target runAction:modifyWidth];


 Another example: ScaleTo action could be rewritten using CCPropertyAction:

    // scaleA and scaleB are equivalents
    id scaleA = [ScaleTo actionWithDuration:2 scale:3];
    id scaleB = [ActionTween actionWithDuration:2 key:@"scale" from:1 to:3];


 @since v0.99.2
 */
class AX_DLL ActionTween : public ActionInterval
{
public:
    /** creates an initializes the action with the property name (key), and the from and to parameters. */
    static ActionTween* create(float aDuration, const char* key, float from, float to);
    /** initializes the action with the property name (key), and the from and to parameters. */
    bool initWithDuration(float aDuration, const char* key, float from, float to);

    void startWithTarget(Node *pTarget);
    void update(float dt);
    ActionInterval* reverse();

    std::string        m_strKey;
    float            m_fFrom, m_fTo;
    float            m_fDelta;
};

// end of actions group
/// @}

NS_AX_END

#endif /* __AXACTIONTWEEN_H__ */


