/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

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
#ifndef __AX_ANIMATION_H__
#define __AX_ANIMATION_H__

#include "platform/PlatformConfig.h"
#include "base/Object.h"
#include "base/Array.h"
#include "base/Dictionary.h"

#include "SpriteFrame.h"
#include <string>

NS_AX_BEGIN

class Texture2D;
class SpriteFrame;

/**
 * @addtogroup sprite_nodes
 * @{
 */

/** AnimationFrame
 A frame of the animation. It contains information like:
    - sprite frame name
    - # of delay units.
    - offset
 
 @since v2.0
 */
class AX_DLL AnimationFrame : public Object
{
public:
    /**
     * @js ctor
     */
    AnimationFrame();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~AnimationFrame();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);
    /** initializes the animation frame with a spriteframe, number of delay units and a notification user info */
    bool initWithSpriteFrame(SpriteFrame* spriteFrame, float delayUnits, Dictionary* userInfo);
    
    /** CCSpriteFrameName to be used */
    AX_SYNTHESIZE_RETAIN(SpriteFrame*, m_pSpriteFrame, SpriteFrame)

    /**  how many units of time the frame takes */
    AX_SYNTHESIZE(float, m_fDelayUnits, DelayUnits)

    /**  A CCAnimationFrameDisplayedNotification notification will be broadcast when the frame is displayed with this dictionary as UserInfo. If UserInfo is nil, then no notification will be broadcast. */
    AX_SYNTHESIZE_RETAIN(Dictionary*, m_pUserInfo, UserInfo)
};




/** A Animation object is used to perform animations on the Sprite objects.

The Animation object contains AnimationFrame objects, and a possible delay between the frames.
You can animate a Animation object by using the Animate action. Example:

[sprite runAction:[Animate actionWithAnimation:animation]];

*/
class AX_DLL Animation : public Object
{
public:
    /**
     * @js ctor
     */
    Animation();
    /**
     * @js NA
     * @lua NA
     */
    ~Animation(void);
public:
    /** Creates an animation
    @since v0.99.5
    */
    static Animation* create(void);

    /* Creates an animation with an array of SpriteFrame and a delay between frames in seconds.
     The frames will be added with one "delay unit".
     @since v0.99.5
     @js create
    */
    static Animation* createWithSpriteFrames(Array* arrayOfSpriteFrameNames, float delay = 0.0f);

    /* Creates an animation with an array of AnimationFrame, the delay per units in seconds and and how many times it should be executed.
     @since v2.0
     */
    static Animation* create(Array *arrayOfAnimationFrameNames, float delayPerUnit, unsigned int loops);
    static Animation* create(Array *arrayOfAnimationFrameNames, float delayPerUnit) {
        return Animation::create(arrayOfAnimationFrameNames, delayPerUnit, 1);
    }

    /** Adds a SpriteFrame to a Animation.
     The frame will be added with one "delay unit".
    */
    void addSpriteFrame(SpriteFrame *pFrame);

    /** Adds a frame with an image filename. Internally it will create a SpriteFrame and it will add it.
     The frame will be added with one "delay unit".
     Added to facilitate the migration from v0.8 to v0.9.
     * @js addSpriteFrameWithFile
     */  
    void addSpriteFrameWithFileName(const char *pszFileName);

    /** Adds a frame with a texture and a rect. Internally it will create a SpriteFrame and it will add it.
     The frame will be added with one "delay unit".
     Added to facilitate the migration from v0.8 to v0.9.
     */
    void addSpriteFrameWithTexture(Texture2D* pobTexture, const Rect& rect);
    /**
     * @lua NA
     */
    bool init();

    /** Initializes a Animation with frames and a delay between frames
     @since v0.99.5
     @lua NA
    */
    bool initWithSpriteFrames(Array *pFrames, float delay = 0.0f);

    /** Initializes a Animation with AnimationFrame
     @since v2.0
     @lua NA
    */
    bool initWithAnimationFrames(Array* arrayOfAnimationFrames, float delayPerUnit, unsigned int loops);
    /**
     * @js NA
     * @lua NA
     */
    virtual Object* copyWithZone(Zone* pZone);

    /** total Delay units of the Animation. */
    AX_SYNTHESIZE_READONLY(float, m_fTotalDelayUnits, TotalDelayUnits)

    /** Delay in seconds of the "delay unit" */
    AX_SYNTHESIZE(float, m_fDelayPerUnit, DelayPerUnit)

    /** duration in seconds of the whole animation. It is the result of totalDelayUnits * delayPerUnit */
    AX_PROPERTY_READONLY(float, m_fDuration, Duration)

    /** array of CCAnimationFrames */
    AX_SYNTHESIZE_RETAIN(Array*, m_pFrames, Frames)

    /** whether or not it shall restore the original frame when the animation finishes */
    AX_SYNTHESIZE(bool, m_bRestoreOriginalFrame, RestoreOriginalFrame)

    /** how many times the animation is going to loop. 0 means animation is not animated. 1, animation is executed one time, ... */
    AX_SYNTHESIZE(unsigned int, m_uLoops, Loops)
};

// end of sprite_nodes group
/// @}

NS_AX_END

#endif // __AX_ANIMATION_H__
