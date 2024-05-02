/*
 * Copyright (c) 2012 cocos2d-x.org
 * http://www.cocos2d-x.org
 *
 * Copyright 2012 Yannick Loriot. All rights reserved.
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
 * IMPLIED, INCLUDING BUT falseT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND falseNINFRINGEMENT. IN false EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "ControlStepper.h"

NS_AX_EXT_BEGIN

#define AXControlStepperLabelColorEnabled   ccc3(55, 55, 55)
#define AXControlStepperLabelColorDisabled  ccc3(147, 147, 147)

#define AXControlStepperLabelFont           "CourierNewPSMT"


#define kAutorepeatDeltaTime                0.15f
#define kAutorepeatIncreaseTimeIncrement    12

CCControlStepper::CCControlStepper()
: m_pMinusSprite(NULL)
, m_pPlusSprite(NULL)
, m_pMinusLabel(NULL)
, m_pPlusLabel(NULL)
, m_dValue(0.0)
, m_bContinuous(false)
, m_bAutorepeat(false)
, m_bWraps(false)
, m_dMinimumValue(0.0)
, m_dMaximumValue(0.0)
, m_dStepValue(0.0)
, m_bTouchInsideFlag(false)
, m_eTouchedPart(kCCControlStepperPartNone)
, m_nAutorepeatCount(0)
{

}

CCControlStepper::~CCControlStepper()
{
    unscheduleAllSelectors();
    
    AX_SAFE_RELEASE(m_pMinusSprite);
    AX_SAFE_RELEASE(m_pPlusSprite);
    AX_SAFE_RELEASE(m_pMinusLabel);
    AX_SAFE_RELEASE(m_pPlusLabel);
}

bool CCControlStepper::initWithMinusSpriteAndPlusSprite(Sprite *minusSprite, Sprite *plusSprite)
{
    if (CCControl::init())
    {
        AXAssert(minusSprite,   "Minus sprite must be not nil");
        AXAssert(plusSprite,    "Plus sprite must be not nil");
        
        setTouchEnabled(true);

        // Set the default values
        m_bAutorepeat                         = true;
        m_bContinuous                         = true;
        m_dMinimumValue                       = 0;
        m_dMaximumValue                       = 100;
        m_dValue                              = 0;
        m_dStepValue                          = 1;
        m_bWraps                              = false;
        this->ignoreAnchorPointForPosition( false );
    
        // Add the minus components
        this->setMinusSprite(minusSprite);
		m_pMinusSprite->setPosition( Point(minusSprite->getContentSize().width / 2, minusSprite->getContentSize().height / 2) );
		this->addChild(m_pMinusSprite);
        
        this->setMinusLabel( LabelTTF::create("-", AXControlStepperLabelFont, 40));
        m_pMinusLabel->setColor(AXControlStepperLabelColorDisabled);
        m_pMinusLabel->setPosition(Point(m_pMinusSprite->getContentSize().width / 2, m_pMinusSprite->getContentSize().height / 2) );
        m_pMinusSprite->addChild(m_pMinusLabel);
        
        // Add the plus components 
        this->setPlusSprite( plusSprite );
		m_pPlusSprite->setPosition( Point(minusSprite->getContentSize().width + plusSprite->getContentSize().width / 2, 
                                                  minusSprite->getContentSize().height / 2) );
		this->addChild(m_pPlusSprite);
        
        this->setPlusLabel( LabelTTF::create("+", AXControlStepperLabelFont, 40 ));
        m_pPlusLabel->setColor( AXControlStepperLabelColorEnabled );
        m_pPlusLabel->setPosition( Point(m_pPlusSprite->getContentSize().width / 2, m_pPlusSprite->getContentSize().height / 2) );
        m_pPlusSprite->addChild(m_pPlusLabel);
        
        // Defines the content size
        Rect maxRect = CCControlUtils::CCRectUnion(m_pMinusSprite->boundingBox(), m_pPlusSprite->boundingBox());
        this->setContentSize( CCSizeMake(m_pMinusSprite->getContentSize().width + m_pPlusSprite->getContentSize().height, maxRect.size.height) );
        return true;
    }
    return false;
}

CCControlStepper* CCControlStepper::create(Sprite *minusSprite, Sprite *plusSprite)
{
    CCControlStepper* pRet = new CCControlStepper();
    if (pRet != NULL && pRet->initWithMinusSpriteAndPlusSprite(minusSprite, plusSprite))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }
    return pRet;
}

//#pragma mark Properties

void CCControlStepper::setWraps(bool wraps)
{
    m_bWraps = wraps;
    
    if (m_bWraps)
    {
        m_pMinusLabel->setColor( AXControlStepperLabelColorEnabled );
        m_pPlusLabel->setColor(AXControlStepperLabelColorEnabled );
    }
    
    this->setValue( m_dValue );
}

void CCControlStepper::setMinimumValue(double minimumValue)
{
    if (minimumValue >= m_dMaximumValue)
    {
        AXAssert(0, "Must be numerically less than maximumValue.");
    }
    
    m_dMinimumValue   = minimumValue;
    this->setValue( m_dValue );
}

void CCControlStepper::setMaximumValue(double maximumValue)
{
    if (maximumValue <= m_dMinimumValue)
    {
        AXAssert(0, "Must be numerically greater than minimumValue.");
    }
    
    m_dMaximumValue   = maximumValue;
    this->setValue(m_dValue);
}

void CCControlStepper::setValue(double value)
{
    this->setValueWithSendingEvent(value, true);
}

double CCControlStepper::getValue()
{
    return m_dValue;
}

void CCControlStepper::setStepValue(double stepValue)
{
    if (stepValue <= 0)
    {
        AXAssert(0,"Must be numerically greater than 0.");
    }

    m_dStepValue  = stepValue;
}

bool CCControlStepper::isContinuous()
{
    return m_bContinuous;
}

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
//#pragma mark -
//#pragma mark CCControlStepper Public Methods
#endif

void CCControlStepper::setValueWithSendingEvent(double value, bool send)
{
    if (value < m_dMinimumValue)
    {
        value = m_bWraps ? m_dMaximumValue : m_dMinimumValue;
    } else if (value > m_dMaximumValue)
    {
        value = m_bWraps ? m_dMinimumValue : m_dMaximumValue;
    }
    
    m_dValue = value;
    
    if (!m_bWraps)
    {
        m_pMinusLabel->setColor((value == m_dMinimumValue) ? AXControlStepperLabelColorDisabled : AXControlStepperLabelColorEnabled);
        m_pPlusLabel->setColor((value == m_dMaximumValue) ? AXControlStepperLabelColorDisabled : AXControlStepperLabelColorEnabled);
    }
    
    if (send)
    {
        this->sendActionsForControlEvents(CCControlEventValueChanged);
    }
}

void CCControlStepper::startAutorepeat()
{
    m_nAutorepeatCount    = -1;
    
    this->schedule(schedule_selector(CCControlStepper::update), kAutorepeatDeltaTime, kAXRepeatForever, kAutorepeatDeltaTime * 3);
}

/** Stop the autorepeat. */
void CCControlStepper::stopAutorepeat()
{
    this->unschedule(schedule_selector(CCControlStepper::update));
}

void CCControlStepper::update(float dt)
{
    m_nAutorepeatCount++;
    
    if ((m_nAutorepeatCount < kAutorepeatIncreaseTimeIncrement) && (m_nAutorepeatCount % 3) != 0)
        return;
    
    if (m_eTouchedPart == kCCControlStepperPartMinus)
    {
        this->setValueWithSendingEvent(m_dValue - m_dStepValue, m_bContinuous);
    } else if (m_eTouchedPart == kCCControlStepperPartPlus)
    {
        this->setValueWithSendingEvent(m_dValue + m_dStepValue, m_bContinuous);
    }
}

//#pragma mark CCControlStepper Private Methods

void CCControlStepper::updateLayoutUsingTouchLocation(Point location)
{
    if (location.x < m_pMinusSprite->getContentSize().width
        && m_dValue > m_dMinimumValue)
    {
        m_eTouchedPart        = kCCControlStepperPartMinus;
        
        m_pMinusSprite->setColor(ccGRAY);
        m_pPlusSprite->setColor(ccWHITE);
    } else if (location.x >= m_pMinusSprite->getContentSize().width
               && m_dValue < m_dMaximumValue)
    {
        m_eTouchedPart        = kCCControlStepperPartPlus;
        
        m_pMinusSprite->setColor(ccWHITE);
        m_pPlusSprite->setColor(ccGRAY);
    } else
    {
        m_eTouchedPart        = kCCControlStepperPartNone;
        
        m_pMinusSprite->setColor(ccWHITE);
        m_pPlusSprite->setColor(ccWHITE);
    }
}


bool CCControlStepper::ccTouchBegan(Touch *pTouch)
{
    if (!isTouchInside(pTouch) || !isEnabled() || !isVisible())
    {
        return false;
    }
    
    Point location    = this->getTouchLocation(pTouch);
    this->updateLayoutUsingTouchLocation(location);
    
    m_bTouchInsideFlag = true;
    
    if (m_bAutorepeat)
    {
        this->startAutorepeat();
    }
    
    return true;
}

void CCControlStepper::ccTouchMoved(Touch *pTouch)
{
    if (this->isTouchInside(pTouch))
    {
        Point location    = this->getTouchLocation(pTouch);
        this->updateLayoutUsingTouchLocation(location);
        
        if (!m_bTouchInsideFlag)
        {
            m_bTouchInsideFlag    = true;
            
            if (m_bAutorepeat)
            {
                this->startAutorepeat();
            }
        }
    } else
    {
        m_bTouchInsideFlag    = false;
        
        m_eTouchedPart        = kCCControlStepperPartNone;
        
        m_pMinusSprite->setColor(ccWHITE);
        m_pPlusSprite->setColor(ccWHITE);
        
        if (m_bAutorepeat)
        {
            this->stopAutorepeat();
        }
    }
}

void CCControlStepper::ccTouchEnded(Touch *pTouch)
{
    m_pMinusSprite->setColor(ccWHITE);
    m_pPlusSprite->setColor(ccWHITE);
    
    if (m_bAutorepeat)
    {
        this->stopAutorepeat();
    }
    
    if (this->isTouchInside(pTouch))
    {
        Point location    = this->getTouchLocation(pTouch);
        
        this->setValue(m_dValue + ((location.x < m_pMinusSprite->getContentSize().width) ? (0.0-m_dStepValue) : m_dStepValue));
    }
}

NS_AX_EXT_END
