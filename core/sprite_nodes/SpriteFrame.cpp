/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2011 Ricardo Quesada
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
#include "textures/TextureCache.h"
#include "SpriteFrame.h"
#include "base/Director.h"

NS_AX_BEGIN

// implementation of SpriteFrame

SpriteFrame* SpriteFrame::create(const char* filename, const Rect& rect)
{
    SpriteFrame *pSpriteFrame = new SpriteFrame();;
    pSpriteFrame->initWithTextureFilename(filename, rect);
    pSpriteFrame->autorelease();

    return pSpriteFrame;
}

SpriteFrame* SpriteFrame::createWithTexture(Texture2D *pobTexture, const Rect& rect)
{
    SpriteFrame *pSpriteFrame = new SpriteFrame();;
    pSpriteFrame->initWithTexture(pobTexture, rect);
    pSpriteFrame->autorelease();
    
    return pSpriteFrame;
}

SpriteFrame* SpriteFrame::createWithTexture(Texture2D* pobTexture, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize)
{
    SpriteFrame *pSpriteFrame = new SpriteFrame();;
    pSpriteFrame->initWithTexture(pobTexture, rect, rotated, offset, originalSize);
    pSpriteFrame->autorelease();

    return pSpriteFrame;
}

SpriteFrame* SpriteFrame::create(const char* filename, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize)
{
    SpriteFrame *pSpriteFrame = new SpriteFrame();;
    pSpriteFrame->initWithTextureFilename(filename, rect, rotated, offset, originalSize);
    pSpriteFrame->autorelease();

    return pSpriteFrame;
}

bool SpriteFrame::initWithTexture(Texture2D* pobTexture, const Rect& rect)
{
    Rect rectInPixels = AX_RECT_POINTS_TO_PIXELS(rect);
    return initWithTexture(pobTexture, rectInPixels, false, Vec2::ZERO, rectInPixels.size);
}

bool SpriteFrame::initWithTextureFilename(const char* filename, const Rect& rect)
{
    Rect rectInPixels = AX_RECT_POINTS_TO_PIXELS( rect );
    return initWithTextureFilename(filename, rectInPixels, false, Vec2::ZERO, rectInPixels.size);
}

bool SpriteFrame::initWithTexture(Texture2D* pobTexture, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize)
{
    m_pobTexture = pobTexture;

    if (pobTexture)
    {
        pobTexture->retain();
    }

    m_obRectInPixels = rect;
    m_obRect = AX_RECT_PIXELS_TO_POINTS(rect);
    m_obOffsetInPixels = offset;
    m_obOffset = AX_POINT_PIXELS_TO_POINTS( m_obOffsetInPixels );
    m_obOriginalSizeInPixels = originalSize;
    m_obOriginalSize = AX_SIZE_PIXELS_TO_POINTS( m_obOriginalSizeInPixels );
    m_bRotated = rotated;

    return true;
}

bool SpriteFrame::initWithTextureFilename(const char* filename, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize)
{
    m_pobTexture = NULL;
    m_strTextureFilename = filename;
    m_obRectInPixels = rect;
    m_obRect = AX_RECT_PIXELS_TO_POINTS( rect );
    m_obOffsetInPixels = offset;
    m_obOffset = AX_POINT_PIXELS_TO_POINTS( m_obOffsetInPixels );
    m_obOriginalSizeInPixels = originalSize;
    m_obOriginalSize = AX_SIZE_PIXELS_TO_POINTS( m_obOriginalSizeInPixels );
    m_bRotated = rotated;

    return true;
}

SpriteFrame::~SpriteFrame(void)
{
    AXLOGINFO("cocos2d: deallocing %p", this);
    AX_SAFE_RELEASE(m_pobTexture);
}

Object* SpriteFrame::copyWithZone(Zone *pZone)
{
    AX_UNUSED_PARAM(pZone);
    SpriteFrame *pCopy = new SpriteFrame();
    
    pCopy->initWithTextureFilename(m_strTextureFilename.c_str(), m_obRectInPixels, m_bRotated, m_obOffsetInPixels, m_obOriginalSizeInPixels);
    pCopy->setTexture(m_pobTexture);
    return pCopy;
}

void SpriteFrame::setRect(const Rect& rect)
{
    m_obRect = rect;
    m_obRectInPixels = AX_RECT_POINTS_TO_PIXELS(m_obRect);
}

void SpriteFrame::setRectInPixels(const Rect& rectInPixels)
{
    m_obRectInPixels = rectInPixels;
    m_obRect = AX_RECT_PIXELS_TO_POINTS(rectInPixels);
}

const Vec2& SpriteFrame::getOffset(void)
{
    return m_obOffset;
}

void SpriteFrame::setOffset(const Vec2& offsets)
{
    m_obOffset = offsets;
    m_obOffsetInPixels = AX_POINT_POINTS_TO_PIXELS( m_obOffset );
}

const Vec2& SpriteFrame::getOffsetInPixels(void)
{
    return m_obOffsetInPixels;
}

void SpriteFrame::setOffsetInPixels(const Vec2& offsetInPixels)
{
    m_obOffsetInPixels = offsetInPixels;
    m_obOffset = AX_POINT_PIXELS_TO_POINTS( m_obOffsetInPixels );
}

void SpriteFrame::setTexture(Texture2D * texture)
{
    if( m_pobTexture != texture ) {
        AX_SAFE_RELEASE(m_pobTexture);
        AX_SAFE_RETAIN(texture);
        m_pobTexture = texture;
    }
}

Texture2D* SpriteFrame::getTexture(void)
{
    if( m_pobTexture ) {
        return m_pobTexture;
    }

    if( m_strTextureFilename.length() > 0 ) {
        return TextureCache::sharedTextureCache()->addImage(m_strTextureFilename.c_str());
    }
    // no texture or texture filename
    return NULL;
}

NS_AX_END

