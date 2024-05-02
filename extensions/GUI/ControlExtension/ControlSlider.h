/*
 * Copyright (c) 2012 cocos2d-x.org
 * http://www.cocos2d-x.org
 *
 * Copyright 2011 Yannick Loriot. All rights reserved.
 * http://yannickloriot.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Converted to c++ / cocos2d-x by Angus C
 */


#ifndef __AXCONTROL_SLIDER_H__
#define __AXCONTROL_SLIDER_H__

#include "Control.h"
#include "Invocation.h"

NS_AX_EXT_BEGIN

/**
 * @addtogroup GUI
 * @{
 * @addtogroup control_extension
 * @{
 */

class AX_EX_DLL CCControlSlider: public CCControl
{
    //maunally put in the setters
    /** Contains the receivers current value. */
    AX_SYNTHESIZE_READONLY(float, m_value, Value);
    virtual void setValue(float val);
    /** Contains the minimum value of the receiver. 
    * The default value of this property is 0.0. */
    AX_SYNTHESIZE_READONLY(float, m_minimumValue, MinimumValue);
    virtual void setMinimumValue(float val);
    /** Contains the maximum value of the receiver. 
    * The default value of this property is 1.0. */
    AX_SYNTHESIZE_READONLY(float, m_maximumValue, MaximumValue);
    virtual void setMaximumValue(float val);
    virtual void setEnabled(bool enabled);
    virtual bool isTouchInside(Touch * touch);
    Point locationFromTouch(Touch* touch);

    AX_SYNTHESIZE(float, m_minimumAllowedValue, MinimumAllowedValue);
    AX_SYNTHESIZE(float, m_maximumAllowedValue, MaximumAllowedValue);

    // maybe this should be read-only
    AX_SYNTHESIZE_RETAIN(Sprite*, m_thumbSprite, ThumbSprite);
    AX_SYNTHESIZE_RETAIN(Sprite*, m_progressSprite, ProgressSprite);
    AX_SYNTHESIZE_RETAIN(Sprite*, m_backgroundSprite, BackgroundSprite);

public:
    CCControlSlider();
    virtual ~CCControlSlider();

    /** 
    * Initializes a slider with a background sprite, a progress bar and a thumb
    * item.
    *
    * @param backgroundSprite  Sprite, that is used as a background.
    * @param progressSprite    Sprite, that is used as a progress bar.
    * @param thumbItem         Sprite, that is used as a thumb.
    */
    virtual bool initWithSprites(Sprite * backgroundSprite, Sprite* progressSprite, Sprite* thumbSprite);

    /** 
    * Creates slider with a background filename, a progress filename and a 
    * thumb image filename.
    */
    static CCControlSlider* create(const char* bgFile, const char* progressFile, const char* thumbFile);

    /** 
    * Creates a slider with a given background sprite and a progress bar and a
    * thumb item.
    *
    * @see initWithBackgroundSprite:progressSprite:thumbMenuItem:
    */
    static CCControlSlider* create(Sprite * backgroundSprite, Sprite* pogressSprite, Sprite* thumbSprite);

    virtual void needsLayout();
protected:
    void sliderBegan(Point location);
    void sliderMoved(Point location);
    void sliderEnded(Point location);

    virtual bool ccTouchBegan(Touch* touch);
    virtual void ccTouchMoved(Touch *pTouch);
    virtual void ccTouchEnded(Touch *pTouch);

/** Returns the value for the given location. */
    float valueForLocation(Point location);
};

// end of GUI group
/// @}
/// @}

NS_AX_EXT_END

#endif