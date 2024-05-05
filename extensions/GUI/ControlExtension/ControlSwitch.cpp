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
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "ControlSwitch.h"
#include "axolotl.h"

NS_AX_EXT_BEGIN
// CCControlSwitchSprite

class CCControlSwitchSprite : public Sprite, public ActionTweenDelegate
{
public:
    CCControlSwitchSprite();
    virtual ~CCControlSwitchSprite();
    bool initWithMaskSprite(
        Sprite *maskSprite, 
        Sprite *onSprite, 
        Sprite *offSprite,
        Sprite *thumbSprite,
        LabelTTF* onLabel, 
        LabelTTF* offLabel);
    void draw();
    void needsLayout();
    void setSliderXPosition(float sliderXPosition);
    float getSliderXPosition() {return m_fSliderXPosition;}
    float onSideWidth();
    float offSideWidth();
    virtual void updateTweenAction(float value, const char* key);
/** Contains the position (in x-axis) of the slider inside the receiver. */
    float m_fSliderXPosition;
    AX_SYNTHESIZE(float, m_fOnPosition, OnPosition)
    AX_SYNTHESIZE(float, m_fOffPosition, OffPosition)
    
    AX_SYNTHESIZE_RETAIN(Texture2D*, m_pMaskTexture, MaskTexture)
    AX_SYNTHESIZE(GLuint, m_uTextureLocation, TextureLocation)
    AX_SYNTHESIZE(GLuint, m_uMaskLocation, MaskLocation)
    
    AX_SYNTHESIZE_RETAIN(Sprite*, m_pOnSprite, OnSprite)
    AX_SYNTHESIZE_RETAIN(Sprite*, m_pOffSprite, OffSprite)
    AX_SYNTHESIZE_RETAIN(Sprite*, m_ThumbSprite, ThumbSprite)
    AX_SYNTHESIZE_RETAIN(LabelTTF*, m_pOnLabel, OnLabel)
    AX_SYNTHESIZE_RETAIN(LabelTTF*, m_pOffLabel, OffLabel)
};

CCControlSwitchSprite::CCControlSwitchSprite()
: m_fSliderXPosition(0.0f)
, m_fOnPosition(0.0f)
, m_fOffPosition(0.0f)
, m_pMaskTexture(NULL)
, m_uTextureLocation(0)
, m_uMaskLocation(0)
, m_pOnSprite(NULL)
, m_pOffSprite(NULL)
, m_ThumbSprite(NULL)
, m_pOnLabel(NULL)
, m_pOffLabel(NULL)
{

}

CCControlSwitchSprite::~CCControlSwitchSprite()
{
    AX_SAFE_RELEASE(m_pOnSprite);
    AX_SAFE_RELEASE(m_pOffSprite);
    AX_SAFE_RELEASE(m_ThumbSprite);
    AX_SAFE_RELEASE(m_pOnLabel);
    AX_SAFE_RELEASE(m_pOffLabel);
    AX_SAFE_RELEASE(m_pMaskTexture);
}

bool CCControlSwitchSprite::initWithMaskSprite(
    Sprite *maskSprite, 
    Sprite *onSprite, 
    Sprite *offSprite,
    Sprite *thumbSprite,
    LabelTTF* onLabel, 
    LabelTTF* offLabel)
{
    if (Sprite::initWithTexture(maskSprite->getTexture()))
    {
        // Sets the default values
        m_fOnPosition             = 0;
        m_fOffPosition            = -onSprite->getContentSize().width + thumbSprite->getContentSize().width / 2;
        m_fSliderXPosition        = m_fOnPosition; 

        setOnSprite(onSprite);
        setOffSprite(offSprite);
        setThumbSprite(thumbSprite);
        setOnLabel(onLabel);
        setOffLabel(offLabel);

        addChild(m_ThumbSprite);

        // Set up the mask with the Mask shader
        setMaskTexture(maskSprite->getTexture());
        GLProgram* pProgram = new GLProgram();
        pProgram->initWithVertexShaderByteArray(ccPositionTextureColor_vert, ccExSwitchMask_frag);
        setShaderProgram(pProgram);
        pProgram->release();

        CHECK_GL_ERROR_DEBUG();

        getShaderProgram()->addAttribute(kAXAttributeNamePosition, kCCVertexAttrib_Position);
        getShaderProgram()->addAttribute(kAXAttributeNameColor, kCCVertexAttrib_Color);
        getShaderProgram()->addAttribute(kAXAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        CHECK_GL_ERROR_DEBUG();

        getShaderProgram()->link();
        CHECK_GL_ERROR_DEBUG();

        getShaderProgram()->updateUniforms();
        CHECK_GL_ERROR_DEBUG();                

        m_uTextureLocation    = glGetUniformLocation( getShaderProgram()->getProgram(), "u_texture");
        m_uMaskLocation       = glGetUniformLocation( getShaderProgram()->getProgram(), "u_mask");
        CHECK_GL_ERROR_DEBUG();

        setContentSize(m_pMaskTexture->getContentSize());

        needsLayout();
        return true;
    }
    return false;
}

void CCControlSwitchSprite::updateTweenAction(float value, const char* key)
{
    AXLOG("key = %s, value = %f", key, value);
    setSliderXPosition(value);
}

void CCControlSwitchSprite::draw()
{
    AX_NODE_DRAW_SETUP();

    ccGLEnableVertexAttribs(kCCVertexAttribFlag_PosColorTex);
    ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    getShaderProgram()->setUniformsForBuiltins();

    glActiveTexture(GL_TEXTURE0);
    ccGLBindTexture2DN(0,getTexture()->getName());
    glUniform1i(m_uTextureLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    ccGLBindTexture2DN(1,m_pMaskTexture->getName());
    glUniform1i(m_uMaskLocation, 1);

#define kQuadSize sizeof(m_sQuad.bl)
#ifdef EMSCRIPTEN
    long offset = 0;
    setGLBufferData(&m_sQuad, 4 * kQuadSize, 0);
#else
    long offset = (long)&m_sQuad;
#endif // EMSCRIPTEN

    // vertex
    int diff = offsetof( ccV3F_C4B_T2F, vertices);
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));

    // texCoods
    diff = offsetof( ccV3F_C4B_T2F, texCoords);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

    // color
    diff = offsetof( ccV3F_C4B_T2F, colors);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);    
    glActiveTexture(GL_TEXTURE0);
}

void CCControlSwitchSprite::needsLayout()
{
    m_pOnSprite->setPosition(Vec2(m_pOnSprite->getContentSize().width / 2 + m_fSliderXPosition,
        m_pOnSprite->getContentSize().height / 2));
    m_pOffSprite->setPosition(Vec2(m_pOnSprite->getContentSize().width + m_pOffSprite->getContentSize().width / 2 + m_fSliderXPosition, 
        m_pOffSprite->getContentSize().height / 2));
    m_ThumbSprite->setPosition(Vec2(m_pOnSprite->getContentSize().width + m_fSliderXPosition,
        m_pMaskTexture->getContentSize().height / 2));

    if (m_pOnLabel)
    {
        m_pOnLabel->setPosition(Vec2(m_pOnSprite->getPosition().x - m_ThumbSprite->getContentSize().width / 6,
            m_pOnSprite->getContentSize().height / 2));
    }
    if (m_pOffLabel)
    {
        m_pOffLabel->setPosition(Vec2(m_pOffSprite->getPosition().x + m_ThumbSprite->getContentSize().width / 6,
            m_pOffSprite->getContentSize().height / 2));
    }

    RenderTexture *rt = RenderTexture::create((int)m_pMaskTexture->getContentSize().width, (int)m_pMaskTexture->getContentSize().height);

    rt->begin();
    m_pOnSprite->visit();
    m_pOffSprite->visit();

    if (m_pOnLabel)
    {
        m_pOnLabel->visit();
    }
    if (m_pOffLabel)
    {
        m_pOffLabel->visit();
    }

    rt->end();

    setTexture(rt->getSprite()->getTexture());
    setFlipY(true);
}

void CCControlSwitchSprite::setSliderXPosition(float sliderXPosition)
{
    if (sliderXPosition <= m_fOffPosition)
    {
        // Off
        sliderXPosition = m_fOffPosition;
    } else if (sliderXPosition >= m_fOnPosition)
    {
        // On
        sliderXPosition = m_fOnPosition;
    }

    m_fSliderXPosition    = sliderXPosition;

    needsLayout();
}


float CCControlSwitchSprite::onSideWidth()
{
    return m_pOnSprite->getContentSize().width;
}

float CCControlSwitchSprite::offSideWidth()
{
    return m_pOffSprite->getContentSize().height;
}


// CCControlSwitch

CCControlSwitch::CCControlSwitch()
: m_pSwitchSprite(NULL)
, m_fInitialTouchXPosition(0.0f)
, m_bMoved(false)
, m_bOn(false)
{

}

CCControlSwitch::~CCControlSwitch()
{
    AX_SAFE_RELEASE(m_pSwitchSprite);
}

bool CCControlSwitch::initWithMaskSprite(Sprite *maskSprite, Sprite * onSprite, Sprite * offSprite, Sprite * thumbSprite)
{
    return initWithMaskSprite(maskSprite, onSprite, offSprite, thumbSprite, NULL, NULL);
}

CCControlSwitch* CCControlSwitch::create(Sprite *maskSprite, Sprite * onSprite, Sprite * offSprite, Sprite * thumbSprite)
{
    CCControlSwitch* pRet = new CCControlSwitch();
    if (pRet && pRet->initWithMaskSprite(maskSprite, onSprite, offSprite, thumbSprite, NULL, NULL))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }
    return pRet;
}

bool CCControlSwitch::initWithMaskSprite(Sprite *maskSprite, Sprite * onSprite, Sprite * offSprite, Sprite * thumbSprite, LabelTTF* onLabel, LabelTTF* offLabel)
{
    if (CCControl::init())
    {
        AXAssert(maskSprite,    "Mask must not be nil.");
        AXAssert(onSprite,      "onSprite must not be nil.");
        AXAssert(offSprite,     "offSprite must not be nil.");
        AXAssert(thumbSprite,   "thumbSprite must not be nil.");
        
        setTouchEnabled(true);
        m_bOn = true;

        m_pSwitchSprite = new CCControlSwitchSprite();
        m_pSwitchSprite->initWithMaskSprite(maskSprite,
                                            onSprite,
                                           offSprite,
                                           thumbSprite,
                                           onLabel,
                                           offLabel);
        m_pSwitchSprite->setPosition(Vec2 (m_pSwitchSprite->getContentSize().width / 2, m_pSwitchSprite->getContentSize().height / 2));
        addChild(m_pSwitchSprite);
        
        ignoreAnchorPointForPosition(false);
        setAnchorPoint(Vec2 (0.5f, 0.5f));
        setContentSize(m_pSwitchSprite->getContentSize());
        return true;
    }
    return false;
}

CCControlSwitch* CCControlSwitch::create(Sprite *maskSprite, Sprite * onSprite, Sprite * offSprite, Sprite * thumbSprite, LabelTTF* onLabel, LabelTTF* offLabel)
{
    CCControlSwitch* pRet = new CCControlSwitch();
    if (pRet && pRet->initWithMaskSprite(maskSprite, onSprite, offSprite, thumbSprite, onLabel, offLabel))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }
    return pRet;
}

void CCControlSwitch::setOn(bool isOn)
{
    setOn(isOn, false);
}

void CCControlSwitch::setOn(bool isOn, bool animated)
{
    m_bOn     = isOn;
    
    if (animated) {
        m_pSwitchSprite->runAction
        (
            ActionTween::create
                (
                    0.2f,
                    "sliderXPosition",
                    m_pSwitchSprite->getSliderXPosition(),
                    (m_bOn) ? m_pSwitchSprite->getOnPosition() : m_pSwitchSprite->getOffPosition()
                )
         );
    }
    else {
        m_pSwitchSprite->setSliderXPosition((m_bOn) ? m_pSwitchSprite->getOnPosition() : m_pSwitchSprite->getOffPosition());
    }
    
    sendActionsForControlEvents(CCControlEventValueChanged);
}

void CCControlSwitch::setEnabled(bool enabled)
{
    m_bEnabled = enabled;
    if (m_pSwitchSprite != NULL)
    {
        m_pSwitchSprite->setOpacity((enabled) ? 255 : 128);
    } 
}

Vec2 CCControlSwitch::locationFromTouch(Touch* pTouch)
{
    Vec2 touchLocation   = pTouch->getLocation();                      // Get the touch position
    touchLocation           = this->convertToNodeSpace(touchLocation);                  // Convert to the node space of this class
    
    return touchLocation;
}

bool CCControlSwitch::ccTouchBegan(Touch *pTouch)
{
    if (!isTouchInside(pTouch) || !isEnabled() || !isVisible())
    {
        return false;
    }
    
    m_bMoved = false;
    
    Vec2 location = this->locationFromTouch(pTouch);
    
    m_fInitialTouchXPosition = location.x - m_pSwitchSprite->getSliderXPosition();
    
    m_pSwitchSprite->getThumbSprite()->setColor(ccGRAY);
    m_pSwitchSprite->needsLayout();
    
    return true;
}

void CCControlSwitch::ccTouchMoved(Touch *pTouch)
{
    Vec2 location    = this->locationFromTouch(pTouch);
    location            = Vec2 (location.x - m_fInitialTouchXPosition, 0);
    
    m_bMoved              = true;
    
    m_pSwitchSprite->setSliderXPosition(location.x);
}

void CCControlSwitch::ccTouchEnded(Touch *pTouch)
{
    Vec2 location   = this->locationFromTouch(pTouch);
    
    m_pSwitchSprite->getThumbSprite()->setColor(ccWHITE);
    
    if (hasMoved())
    {
        setOn(!(location.x < m_pSwitchSprite->getContentSize().width / 2), true);
    } 
    else
    {
        setOn(!m_bOn, true);
    }
}

void CCControlSwitch::ccTouchCancelled(Touch *pTouch)
{
    Vec2 location   = this->locationFromTouch(pTouch);
    
    m_pSwitchSprite->getThumbSprite()->setColor(ccWHITE);
    
    if (hasMoved())
    {
        setOn(!(location.x < m_pSwitchSprite->getContentSize().width / 2), true);
    } else
    {
        setOn(!m_bOn, true);
    }
}

NS_AX_EXT_END
