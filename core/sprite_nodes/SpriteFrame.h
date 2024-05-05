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

#ifndef __SPRITE_AXSPRITE_FRAME_H__
#define __SPRITE_AXSPRITE_FRAME_H__

#include "base/Node.h"
#include "Protocols.h"
#include "base/Object.h"


NS_AX_BEGIN

class Texture2D;
class Zone;

/**
 * @addtogroup sprite_nodes
 * @{
 */

/** @brief A SpriteFrame has:
    - texture: A Texture2D that will be used by the Sprite
    - rectangle: A rectangle of the texture


 You can modify the frame of a Sprite by doing:
 
    SpriteFrame *frame = SpriteFrame::frameWithTexture(texture, rect, offset);
    sprite->setDisplayFrame(frame);
 */
class AX_DLL SpriteFrame : public Object
{
public:
    // attributes

    inline const Rect& getRectInPixels(void) { return m_obRectInPixels; }
    void setRectInPixels(const Rect& rectInPixels);

    inline bool isRotated(void) { return m_bRotated; }
    inline void setRotated(bool bRotated) { m_bRotated = bRotated; }

    /** get rect of the frame */
    inline const Rect& getRect(void) { return m_obRect; }
    /** set rect of the frame */
    void setRect(const Rect& rect);

    /** get offset of the frame */
    const Vec2& getOffsetInPixels(void);
    /** set offset of the frame */
    void setOffsetInPixels(const Vec2& offsetInPixels);

    /** get original size of the trimmed image */
    inline const Size& getOriginalSizeInPixels(void) { return m_obOriginalSizeInPixels; }
    /** set original size of the trimmed image */
    inline void setOriginalSizeInPixels(const Size& sizeInPixels) { m_obOriginalSizeInPixels = sizeInPixels; }

    /** get original size of the trimmed image */
    inline const Size& getOriginalSize(void) { return m_obOriginalSize; }
    /** set original size of the trimmed image */
    inline void setOriginalSize(const Size& sizeInPixels) { m_obOriginalSize = sizeInPixels; }

    /** get texture of the frame */
    Texture2D* getTexture(void);
    /** set texture of the frame, the texture is retained */
    void setTexture(Texture2D* pobTexture);

    const Vec2& getOffset(void);
    void setOffset(const Vec2& offsets);

public:
    /**
     *  @js NA
     *  @lua NA
     */
    ~SpriteFrame(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);

    /** Create a SpriteFrame with a texture filename, rect in points.
     It is assumed that the frame was not trimmed.
     */
    static SpriteFrame* create(const char* filename, const Rect& rect);
    
    /** Create a SpriteFrame with a texture filename, rect, rotated, offset and originalSize in pixels.
     The originalSize is the size in pixels of the frame before being trimmed.
     */
    static SpriteFrame* create(const char* filename, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize);
    
    /** Create a SpriteFrame with a texture, rect in points.
     It is assumed that the frame was not trimmed.
     */
    static SpriteFrame* createWithTexture(Texture2D* pobTexture, const Rect& rect);

    /** Create a SpriteFrame with a texture, rect, rotated, offset and originalSize in pixels.
     The originalSize is the size in points of the frame before being trimmed.
     */
    static SpriteFrame* createWithTexture(Texture2D* pobTexture, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize);

public:
    /** Initializes a SpriteFrame with a texture, rect in points.
     It is assumed that the frame was not trimmed.
     */
    bool initWithTexture(Texture2D* pobTexture, const Rect& rect);

    /** Initializes a SpriteFrame with a texture filename, rect in points;
     It is assumed that the frame was not trimmed.
     */
    bool initWithTextureFilename(const char* filename, const Rect& rect);

    /** Initializes a SpriteFrame with a texture, rect, rotated, offset and originalSize in pixels.
    The originalSize is the size in points of the frame before being trimmed.
    */
    bool initWithTexture(Texture2D* pobTexture, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize);

    /** Initializes a SpriteFrame with a texture, rect, rotated, offset and originalSize in pixels.
     The originalSize is the size in pixels of the frame before being trimmed.

     @since v1.1
     */
    bool initWithTextureFilename(const char* filename, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize);


protected:
    Vec2 m_obOffset;
    Size m_obOriginalSize;
    Rect m_obRectInPixels;
    bool   m_bRotated;
    Rect m_obRect;
    Vec2 m_obOffsetInPixels;
    Size m_obOriginalSizeInPixels;
    Texture2D *m_pobTexture;
    std::string  m_strTextureFilename;
};

// end of sprite_nodes group
/// @}

NS_AX_END

#endif //__SPRITE_AXSPRITE_FRAME_H__
