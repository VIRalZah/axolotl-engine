/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada

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
#include "LabelTTF.h"
#include "base/Director.h"
#include "shaders/GLProgram.h"
#include "shaders/ShaderCache.h"
#include "Application.h"
#include "support/ccUTF8.h"

NS_AX_BEGIN

#if AX_USE_LA88_LABELS
#define SHADER_PROGRAM kAXShader_PositionTextureColor
#else
#define SHADER_PROGRAM kAXShader_PositionTextureA8Color
#endif

//
//LabelTTF
//
LabelTTF::LabelTTF()
: m_hAlignment(kCCTextAlignmentCenter)
, m_vAlignment(kCCVerticalTextAlignmentTop)
, m_pFontName(NULL)
, m_fFontSize(0.0)
, m_string("")
, m_shadowEnabled(false)
, m_strokeEnabled(false)
, m_textFillColor(ccWHITE)
{
}

LabelTTF::~LabelTTF()
{
    AX_SAFE_DELETE(m_pFontName);
}

LabelTTF * LabelTTF::create()
{
    LabelTTF * pRet = new LabelTTF();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }
    return pRet;
}

LabelTTF * LabelTTF::create(const std::string& string, const char *fontName, float fontSize)
{
    return LabelTTF::create(string, fontName, fontSize,
                              Size::ZERO, kCCTextAlignmentCenter, kCCVerticalTextAlignmentTop);
}

LabelTTF * LabelTTF::create(const std::string& string, const char *fontName, float fontSize,
                                const Size& dimensions, CCTextAlignment hAlignment)
{
    return LabelTTF::create(string, fontName, fontSize, dimensions, hAlignment, kCCVerticalTextAlignmentTop);
}

LabelTTF* LabelTTF::create(const std::string& string, const char *fontName, float fontSize,
                               const Size &dimensions, CCTextAlignment hAlignment, 
                               CCVerticalTextAlignment vAlignment)
{
    LabelTTF *pRet = new LabelTTF();
    if(pRet && pRet->initWithString(string, fontName, fontSize, dimensions, hAlignment, vAlignment))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

LabelTTF * LabelTTF::createWithFontDefinition(const std::string& string, axFontDefinition &textDefinition)
{
    LabelTTF *pRet = new LabelTTF();
    if(pRet && pRet->initWithStringAndTextDefinition(string, textDefinition))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool LabelTTF::init()
{
    return this->initWithString("", "Helvetica", 12);
}

bool LabelTTF::initWithString(const std::string& string, const char *fontName, float fontSize,
                                const Size& dimensions, CCTextAlignment alignment)
{
    return this->initWithString(string, fontName, fontSize, dimensions, alignment, kCCVerticalTextAlignmentTop);
}

bool LabelTTF::initWithString(const std::string& string, const char *fontName, float fontSize)
{
    return this->initWithString(string, fontName, fontSize, 
                                Size::ZERO, kCCTextAlignmentLeft, kCCVerticalTextAlignmentTop);
}

bool LabelTTF::initWithString(const std::string& string, const char *fontName, float fontSize,
                                const axolotl::Size &dimensions, CCTextAlignment hAlignment,
                                CCVerticalTextAlignment vAlignment)
{
    if (Sprite::init())
    {
        // shader program
        this->setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(SHADER_PROGRAM));
        
        m_tDimensions = Size(dimensions.width, dimensions.height);
        m_hAlignment  = hAlignment;
        m_vAlignment  = vAlignment;
        m_pFontName   = new std::string(fontName);
        m_fFontSize   = fontSize;
        
        this->setString(string);
        
        return true;
    }
    
    return false;
}

bool LabelTTF::initWithStringAndTextDefinition(const std::string& string, axFontDefinition &textDefinition)
{
    if (Sprite::init())
    {
        // shader program
        this->setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(SHADER_PROGRAM));
        
        // prepare everythin needed to render the label
        _updateWithTextDefinition(textDefinition, false);
        
        // set the string
        this->setString(string);
        
        //
        return true;
    }
    else
    {
        return false;
    }
}

void LabelTTF::setString(const std::string& string)
{
    if (m_string.compare(string))
    {
        m_string.assign(string);

        this->updateTexture();
    }
}

const std::string& LabelTTF::getString(void)
{
    return m_string;
}

const char* LabelTTF::description()
{
    return String::createWithFormat("<LabelTTF | FontName = %s, FontSize = %.1f>", m_pFontName->c_str(), m_fFontSize)->getCString();
}

CCTextAlignment LabelTTF::getHorizontalAlignment()
{
    return m_hAlignment;
}

void LabelTTF::setHorizontalAlignment(CCTextAlignment alignment)
{
    if (alignment != m_hAlignment)
    {
        m_hAlignment = alignment;
        
        // Force update
        if (m_string.size() > 0)
        {
            this->updateTexture();
        }
    }
}

CCVerticalTextAlignment LabelTTF::getVerticalAlignment()
{
    return m_vAlignment;
}

void LabelTTF::setVerticalAlignment(CCVerticalTextAlignment verticalAlignment)
{
    if (verticalAlignment != m_vAlignment)
    {
        m_vAlignment = verticalAlignment;
        
        // Force update
        if (m_string.size() > 0)
        {
            this->updateTexture();
        }
    }
}

Size LabelTTF::getDimensions()
{
    return m_tDimensions;
}

void LabelTTF::setDimensions(const Size &dim)
{
    if (dim.width != m_tDimensions.width || dim.height != m_tDimensions.height)
    {
        m_tDimensions = dim;
        
        // Force update
        if (m_string.size() > 0)
        {
            this->updateTexture();
        }
    }
}

float LabelTTF::getFontSize()
{
    return m_fFontSize;
}

void LabelTTF::setFontSize(float fontSize)
{
    if (m_fFontSize != fontSize)
    {
        m_fFontSize = fontSize;
        
        // Force update
        if (m_string.size() > 0)
        {
            this->updateTexture();
        }
    }
}

const char* LabelTTF::getFontName()
{
    return m_pFontName->c_str();
}

void LabelTTF::setFontName(const char *fontName)
{
    if (m_pFontName->compare(fontName))
    {
        delete m_pFontName;
        m_pFontName = new std::string(fontName);
        
        // Force update
        if (m_string.size() > 0)
        {
            this->updateTexture();
        }
    }
}

// Helper
bool LabelTTF::updateTexture()
{
    Texture2D *tex;
    tex = new Texture2D();
    
    if (!tex)
        return false;
    
    #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
    
        axFontDefinition texDef = _prepareTextDefinition(true);
        tex->initWithString( m_string.c_str(), &texDef );
    
    #else
    
        tex->initWithString( m_string.c_str(),
                            m_pFontName->c_str(),
                            m_fFontSize * AX_CONTENT_SCALE_FACTOR(),
                            AX_SIZE_POINTS_TO_PIXELS(m_tDimensions),
                            m_hAlignment,
                            m_vAlignment);
    
    #endif
    
    // set the texture
    this->setTexture(tex);
    // release it
    tex->release();
    
    // set the size in the sprite
    Rect rect =Rect::ZERO;
    rect.size   = m_pobTexture->getContentSize();
    this->setTextureRect(rect);
    
    //ok
    return true;
}

void LabelTTF::enableShadow(const Size &shadowOffset, float shadowOpacity, float shadowBlur, bool updateTexture)
{
    #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
    
        bool valueChanged = false;
        
        if (false == m_shadowEnabled)
        {
            m_shadowEnabled = true;
            valueChanged    = true;
        }
        
        if ( (m_shadowOffset.width != shadowOffset.width) || (m_shadowOffset.height!=shadowOffset.height) )
        {
            m_shadowOffset.width  = shadowOffset.width;
            m_shadowOffset.height = shadowOffset.height;
            
            valueChanged = true;
        }
        
        if (m_shadowOpacity != shadowOpacity )
        {
            m_shadowOpacity = shadowOpacity;
            valueChanged = true;
        }

        if (m_shadowBlur    != shadowBlur)
        {
            m_shadowBlur = shadowBlur;
            valueChanged = true;
        }
        
        
        if ( valueChanged && updateTexture )
        {
            this->updateTexture();
        }
    
    #else
        AXLOGERROR("Currently only supported on iOS and Android!");
    #endif
    
}

void LabelTTF::disableShadow(bool updateTexture)
{
    #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
    
        if (m_shadowEnabled)
        {
            m_shadowEnabled = false;
    
            if (updateTexture)
                this->updateTexture();
            
        }
    
    #else
        AXLOGERROR("Currently only supported on iOS and Android!");
    #endif
}

void LabelTTF::enableStroke(const ccColor3B &strokeColor, float strokeSize, bool updateTexture)
{
    #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
    
        bool valueChanged = false;
        
        if(m_strokeEnabled == false)
        {
            m_strokeEnabled = true;
            valueChanged = true;
        }
        
        if ( (m_strokeColor.r != strokeColor.r) || (m_strokeColor.g != strokeColor.g) || (m_strokeColor.b != strokeColor.b) )
        {
            m_strokeColor = strokeColor;
            valueChanged = true;
        }
        
        if (m_strokeSize!=strokeSize)
        {
            m_strokeSize = strokeSize;
            valueChanged = true;
        }
        
        if ( valueChanged && updateTexture )
        {
            this->updateTexture();
        }
    
    #else
        AXLOGERROR("Currently only supported on iOS and Android!");
    #endif
    
}

void LabelTTF::disableStroke(bool updateTexture)
{
    #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
    
        if (m_strokeEnabled)
        {
            m_strokeEnabled = false;
            
            if (updateTexture)
                this->updateTexture();
        }
    
    #else
        AXLOGERROR("Currently only supported on iOS and Android!");
    #endif
    
}

void LabelTTF::setFontFillColor(const ccColor3B &tintColor, bool updateTexture)
{
    #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
        if (m_textFillColor.r != tintColor.r || m_textFillColor.g != tintColor.g || m_textFillColor.b != tintColor.b)
        {
            m_textFillColor = tintColor;
            
            if (updateTexture)
                this->updateTexture();
        }
    #else
        AXLOGERROR("Currently only supported on iOS and Android!");
    #endif
}

void LabelTTF::setTextDefinition(axFontDefinition *theDefinition)
{
    if (theDefinition)
    {
        _updateWithTextDefinition(*theDefinition, true);
    }
}

axFontDefinition *LabelTTF::getTextDefinition()
{
    axFontDefinition *tempDefinition = new axFontDefinition;
    *tempDefinition = _prepareTextDefinition(false);
    return tempDefinition;
}

void LabelTTF::_updateWithTextDefinition(axFontDefinition & textDefinition, bool mustUpdateTexture)
{
    m_tDimensions = Size(textDefinition.m_dimensions.width, textDefinition.m_dimensions.height);
    m_hAlignment  = textDefinition.m_alignment;
    m_vAlignment  = textDefinition.m_vertAlignment;
    
    m_pFontName   = new std::string(textDefinition.m_fontName);
    m_fFontSize   = textDefinition.m_fontSize;
    
    
    // shadow
    if ( textDefinition.m_shadow.m_shadowEnabled )
    {
        enableShadow(textDefinition.m_shadow.m_shadowOffset, textDefinition.m_shadow.m_shadowOpacity, textDefinition.m_shadow.m_shadowBlur, false);
    }
    
    // stroke
    if ( textDefinition.m_stroke.m_strokeEnabled )
    {
        enableStroke(textDefinition.m_stroke.m_strokeColor, textDefinition.m_stroke.m_strokeSize, false);
    }
    
    // fill color
    setFontFillColor(textDefinition.m_fontFillColor, false);
    
    if (mustUpdateTexture)
        updateTexture();
}

axFontDefinition LabelTTF::_prepareTextDefinition(bool adjustForResolution)
{
    axFontDefinition texDef;
    
    if (adjustForResolution)
        texDef.m_fontSize       =  m_fFontSize * AX_CONTENT_SCALE_FACTOR();
    else
        texDef.m_fontSize       =  m_fFontSize;
    
    texDef.m_fontName       = *m_pFontName;
    texDef.m_alignment      =  m_hAlignment;
    texDef.m_vertAlignment  =  m_vAlignment;
    
    
    if (adjustForResolution)
        texDef.m_dimensions     =  AX_SIZE_POINTS_TO_PIXELS(m_tDimensions);
    else
        texDef.m_dimensions     =  m_tDimensions;
    
    
    // stroke
    if ( m_strokeEnabled )
    {
        texDef.m_stroke.m_strokeEnabled = true;
        texDef.m_stroke.m_strokeColor   = m_strokeColor;
        
        if (adjustForResolution)
            texDef.m_stroke.m_strokeSize = m_strokeSize * AX_CONTENT_SCALE_FACTOR();
        else
            texDef.m_stroke.m_strokeSize = m_strokeSize;
        
        
    }
    else
    {
        texDef.m_stroke.m_strokeEnabled = false;
    }
    
    
    // shadow
    if ( m_shadowEnabled )
    {
        texDef.m_shadow.m_shadowEnabled         = true;
        texDef.m_shadow.m_shadowBlur            = m_shadowBlur;
        texDef.m_shadow.m_shadowOpacity         = m_shadowOpacity;
        
        if (adjustForResolution)
            texDef.m_shadow.m_shadowOffset = AX_SIZE_POINTS_TO_PIXELS(m_shadowOffset);
        else
            texDef.m_shadow.m_shadowOffset = m_shadowOffset;
    }
    else
    {
        texDef.m_shadow.m_shadowEnabled = false;
    }
    
    // text tint
    texDef.m_fontFillColor = m_textFillColor;
    
    return texDef;
}

NS_AX_END