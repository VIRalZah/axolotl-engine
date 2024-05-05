/*
 * Copyright (c) 2012 cocos2d-x.org
 * http://www.cocos2d-x.org
 *
 * Copyright 2012 Stewart Hamilton-Arrandale.
 * http://creativewax.co.uk
 *
 * Modified by Yannick Loriot.
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


#ifndef __AXCONTROL_SATURATION_PICKER_H__
#define __AXCONTROL_SATURATION_PICKER_H__

#include "Control.h"
#include "Invocation.h"

NS_AX_EXT_BEGIN

/**
 * @addtogroup GUI
 * @{
 * @addtogroup control_extension
 * @{
 */

class AX_EX_DLL CCControlSaturationBrightnessPicker : public CCControl
{
    /** Contains the receivers current saturation value. */
    AX_SYNTHESIZE_READONLY(float, m_saturation, Saturation);
    /** Contains the receivers current brightness value. */
    AX_SYNTHESIZE_READONLY(float, m_brightness, Brightness);

    //not sure if these need to be there actually. I suppose someone might want to access the sprite?
    AX_SYNTHESIZE_READONLY(Sprite*, m_background, Background);
    AX_SYNTHESIZE_READONLY(Sprite*, m_overlay, Overlay);
    AX_SYNTHESIZE_READONLY(Sprite*, m_shadow, Shadow);
    AX_SYNTHESIZE_READONLY(Sprite*, m_slider, Slider);
    AX_SYNTHESIZE_READONLY(Vec2, m_startPos, StartPos);

protected:
    int         boxPos;
    int         boxSize;
    
public:
    CCControlSaturationBrightnessPicker();
    virtual ~CCControlSaturationBrightnessPicker();
    virtual bool initWithTargetAndPos(Node* target, Vec2 pos);

    static CCControlSaturationBrightnessPicker* create(Node* target, Vec2 pos);

    virtual void setEnabled(bool enabled);
    virtual void updateWithHSV(HSV hsv);
    virtual void updateDraggerWithHSV(HSV hsv);

protected:    
    void updateSliderPosition(Vec2 location);
    bool checkSliderPosition(Vec2 location);

    virtual bool ccTouchBegan(Touch* touch);
    virtual void ccTouchMoved(Touch *pTouch);
};

// end of GUI group
/// @}
/// @}

NS_AX_EXT_END

#endif