/****************************************************************************
Copyright (c) 2012 cocos2d-x.org

http://www.cocos2d-x.org

Created by Jung Sang-Taik on 12. 3. 16..
Copyright (c) 2012 Neofect. All rights reserved.

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

#include "Scale9Sprite.h"

NS_AX_EXT_BEGIN

enum positions
{
    pCentre = 0,
    pTop,
    pLeft,
    pRight,
    pBottom,
    pTopRight,
    pTopLeft,
    pBottomRight,
    pBottomLeft
};

CCScale9Sprite::CCScale9Sprite()
: m_insetLeft(0)
, m_insetTop(0)
, m_insetRight(0)
, m_insetBottom(0)
, m_bSpritesGenerated(false)
, m_bSpriteFrameRotated(false)
, m_positionsAreDirty(false)
, _scale9Image(NULL)
, _topLeft(NULL)
, _top(NULL)
, _topRight(NULL)
, _left(NULL)
, _centre(NULL)
, _right(NULL)
, _bottomLeft(NULL)
, _bottom(NULL)
, _bottomRight(NULL)
, _opacityModifyRGB(false)
, _opacity(255)
, _color(ccWHITE)
{

}

CCScale9Sprite::~CCScale9Sprite()
{
    AX_SAFE_RELEASE(_topLeft);
    AX_SAFE_RELEASE(_top);
    AX_SAFE_RELEASE(_topRight);
    AX_SAFE_RELEASE(_left);
    AX_SAFE_RELEASE(_centre);
    AX_SAFE_RELEASE(_right);
    AX_SAFE_RELEASE(_bottomLeft);
    AX_SAFE_RELEASE(_bottom);
    AX_SAFE_RELEASE(_bottomRight);
    AX_SAFE_RELEASE(_scale9Image);
}

bool CCScale9Sprite::init()
{
    return this->initWithBatchNode(NULL, CCRectZero, CCRectZero);
}

bool CCScale9Sprite::initWithBatchNode(SpriteBatchNode* batchnode, Rect rect, Rect capInsets)
{
    return this->initWithBatchNode(batchnode, rect, false, capInsets);
}

bool CCScale9Sprite::initWithBatchNode(SpriteBatchNode* batchnode, Rect rect, bool rotated, Rect capInsets)
{
    if(batchnode)
    {
        this->updateWithBatchNode(batchnode, rect, rotated, capInsets);
        this->setAnchorPoint(Point(0.5f, 0.5f));
    }
    this->m_positionsAreDirty = true;
    
    return true;
}

#define    TRANSLATE_X(x, y, xtranslate) \
    x+=xtranslate;                       \

#define    TRANSLATE_Y(x, y, ytranslate) \
    y+=ytranslate;                       \

bool CCScale9Sprite::updateWithBatchNode(SpriteBatchNode* batchnode, Rect rect, bool rotated, Rect capInsets)
{
    m_bSpriteFrameRotated = rotated;
    GLubyte opacity = getOpacity();
    ccColor3B color = getColor();

    // Release old sprites
    this->removeAllChildrenWithCleanup(true);

    AX_SAFE_RELEASE(this->_centre);
    AX_SAFE_RELEASE(this->_top);
    AX_SAFE_RELEASE(this->_topLeft);
    AX_SAFE_RELEASE(this->_topRight);
    AX_SAFE_RELEASE(this->_left);
    AX_SAFE_RELEASE(this->_right);
    AX_SAFE_RELEASE(this->_bottomLeft);
    AX_SAFE_RELEASE(this->_bottom);
    AX_SAFE_RELEASE(this->_bottomRight);

    
    if(this->_scale9Image != batchnode)
    {
        AX_SAFE_RELEASE(this->_scale9Image);
        _scale9Image = batchnode;
        AX_SAFE_RETAIN(_scale9Image);
    }

    _scale9Image->removeAllChildrenWithCleanup(true);

    m_capInsets = capInsets;
    
    // If there is no given rect
    if ( rect.equals(CCRectZero) )
    {
        // Get the texture size as original
        Size textureSize = _scale9Image->getTextureAtlas()->getTexture()->getContentSize();
    
        rect = CCRectMake(0, 0, textureSize.width, textureSize.height);
    }
    
    // Set the given rect's size as original size
    m_spriteRect = rect;
    m_originalSize = rect.size;
    m_preferredSize = m_originalSize;
    m_capInsetsInternal = capInsets;
    
    float w = rect.size.width;
    float h = rect.size.height;

    // If there is no specified center region
    if ( m_capInsetsInternal.equals(CCRectZero) )
    {
        // log("... cap insets not specified : using default cap insets ...");
        m_capInsetsInternal = CCRectMake(w/3, h/3, w/3, h/3);
    }

    float left_w = m_capInsetsInternal.origin.x;
    float center_w = m_capInsetsInternal.size.width;
    float right_w = rect.size.width - (left_w + center_w);

    float top_h = m_capInsetsInternal.origin.y;
    float center_h = m_capInsetsInternal.size.height;
    float bottom_h = rect.size.height - (top_h + center_h);

    // calculate rects

    // ... top row
    float x = 0.0;
    float y = 0.0;

    // top left
    Rect lefttopbounds = CCRectMake(x, y,
                                      left_w, top_h);

    // top center
    TRANSLATE_X(x, y, left_w);
    Rect centertopbounds = CCRectMake(x, y,
                                        center_w, top_h);
        
    // top right
    TRANSLATE_X(x, y, center_w);
    Rect righttopbounds = CCRectMake(x, y,
                                       right_w, top_h);

    // ... center row
    x = 0.0;
    y = 0.0;
    TRANSLATE_Y(x, y, top_h);

    // center left
    Rect leftcenterbounds = CCRectMake(x, y,
                                         left_w, center_h);

    // center center
    TRANSLATE_X(x, y, left_w);
    Rect centerbounds = CCRectMake(x, y,
                                     center_w, center_h);

    // center right
    TRANSLATE_X(x, y, center_w);
    Rect rightcenterbounds = CCRectMake(x, y,
                                          right_w, center_h);

    // ... bottom row
    x = 0.0;
    y = 0.0;
    TRANSLATE_Y(x, y, top_h);
    TRANSLATE_Y(x, y, center_h);

    // bottom left
    Rect leftbottombounds = CCRectMake(x, y,
                                         left_w, bottom_h);

    // bottom center
    TRANSLATE_X(x, y, left_w);
    Rect centerbottombounds = CCRectMake(x, y,
                                           center_w, bottom_h);

    // bottom right
    TRANSLATE_X(x, y, center_w);
    Rect rightbottombounds = CCRectMake(x, y,
                                          right_w, bottom_h);

    if (!rotated) {
        // log("!rotated");

        AffineTransform t = AffineTransformMakeIdentity();
        t = AffineTransformTranslate(t, rect.origin.x, rect.origin.y);

        centerbounds = CCRectApplyAffineTransform(centerbounds, t);
        rightbottombounds = CCRectApplyAffineTransform(rightbottombounds, t);
        leftbottombounds = CCRectApplyAffineTransform(leftbottombounds, t);
        righttopbounds = CCRectApplyAffineTransform(righttopbounds, t);
        lefttopbounds = CCRectApplyAffineTransform(lefttopbounds, t);
        rightcenterbounds = CCRectApplyAffineTransform(rightcenterbounds, t);
        leftcenterbounds = CCRectApplyAffineTransform(leftcenterbounds, t);
        centerbottombounds = CCRectApplyAffineTransform(centerbottombounds, t);
        centertopbounds = CCRectApplyAffineTransform(centertopbounds, t);

        // Centre
        _centre = new Sprite();
        _centre->initWithTexture(_scale9Image->getTexture(), centerbounds);
        _scale9Image->addChild(_centre, 0, pCentre);
        
        // Top
        _top = new Sprite();
        _top->initWithTexture(_scale9Image->getTexture(), centertopbounds);
        _scale9Image->addChild(_top, 1, pTop);
        
        // Bottom
        _bottom = new Sprite();
        _bottom->initWithTexture(_scale9Image->getTexture(), centerbottombounds);
        _scale9Image->addChild(_bottom, 1, pBottom);
        
        // Left
        _left = new Sprite();
        _left->initWithTexture(_scale9Image->getTexture(), leftcenterbounds);
        _scale9Image->addChild(_left, 1, pLeft);
        
        // Right
        _right = new Sprite();
        _right->initWithTexture(_scale9Image->getTexture(), rightcenterbounds);
        _scale9Image->addChild(_right, 1, pRight);
        
        // Top left
        _topLeft = new Sprite();
        _topLeft->initWithTexture(_scale9Image->getTexture(), lefttopbounds);
        _scale9Image->addChild(_topLeft, 2, pTopLeft);
        
        // Top right
        _topRight = new Sprite();
        _topRight->initWithTexture(_scale9Image->getTexture(), righttopbounds);
        _scale9Image->addChild(_topRight, 2, pTopRight);
        
        // Bottom left
        _bottomLeft = new Sprite();
        _bottomLeft->initWithTexture(_scale9Image->getTexture(), leftbottombounds);
        _scale9Image->addChild(_bottomLeft, 2, pBottomLeft);
        
        // Bottom right
        _bottomRight = new Sprite();
        _bottomRight->initWithTexture(_scale9Image->getTexture(), rightbottombounds);
        _scale9Image->addChild(_bottomRight, 2, pBottomRight);
    } else {
        // set up transformation of coordinates
        // to handle the case where the sprite is stored rotated
        // in the spritesheet
        // log("rotated");

        AffineTransform t = AffineTransformMakeIdentity();

        Rect rotatedcenterbounds = centerbounds;
        Rect rotatedrightbottombounds = rightbottombounds;
        Rect rotatedleftbottombounds = leftbottombounds;
        Rect rotatedrighttopbounds = righttopbounds;
        Rect rotatedlefttopbounds = lefttopbounds;
        Rect rotatedrightcenterbounds = rightcenterbounds;
        Rect rotatedleftcenterbounds = leftcenterbounds;
        Rect rotatedcenterbottombounds = centerbottombounds;
        Rect rotatedcentertopbounds = centertopbounds;
        
        t = AffineTransformTranslate(t, rect.size.height+rect.origin.x, rect.origin.y);
        t = AffineTransformRotate(t, 1.57079633f);
        
        centerbounds = CCRectApplyAffineTransform(centerbounds, t);
        rightbottombounds = CCRectApplyAffineTransform(rightbottombounds, t);
        leftbottombounds = CCRectApplyAffineTransform(leftbottombounds, t);
        righttopbounds = CCRectApplyAffineTransform(righttopbounds, t);
        lefttopbounds = CCRectApplyAffineTransform(lefttopbounds, t);
        rightcenterbounds = CCRectApplyAffineTransform(rightcenterbounds, t);
        leftcenterbounds = CCRectApplyAffineTransform(leftcenterbounds, t);
        centerbottombounds = CCRectApplyAffineTransform(centerbottombounds, t);
        centertopbounds = CCRectApplyAffineTransform(centertopbounds, t);

        rotatedcenterbounds.origin = centerbounds.origin;
        rotatedrightbottombounds.origin = rightbottombounds.origin;
        rotatedleftbottombounds.origin = leftbottombounds.origin;
        rotatedrighttopbounds.origin = righttopbounds.origin;
        rotatedlefttopbounds.origin = lefttopbounds.origin;
        rotatedrightcenterbounds.origin = rightcenterbounds.origin;
        rotatedleftcenterbounds.origin = leftcenterbounds.origin;
        rotatedcenterbottombounds.origin = centerbottombounds.origin;
        rotatedcentertopbounds.origin = centertopbounds.origin;

        // Centre
        _centre = new Sprite();
        _centre->initWithTexture(_scale9Image->getTexture(), rotatedcenterbounds, true);
        _scale9Image->addChild(_centre, 0, pCentre);
        
        // Top
        _top = new Sprite();
        _top->initWithTexture(_scale9Image->getTexture(), rotatedcentertopbounds, true);
        _scale9Image->addChild(_top, 1, pTop);
        
        // Bottom
        _bottom = new Sprite();
        _bottom->initWithTexture(_scale9Image->getTexture(), rotatedcenterbottombounds, true);
        _scale9Image->addChild(_bottom, 1, pBottom);
        
        // Left
        _left = new Sprite();
        _left->initWithTexture(_scale9Image->getTexture(), rotatedleftcenterbounds, true);
        _scale9Image->addChild(_left, 1, pLeft);
        
        // Right
        _right = new Sprite();
        _right->initWithTexture(_scale9Image->getTexture(), rotatedrightcenterbounds, true);
        _scale9Image->addChild(_right, 1, pRight);
        
        // Top left
        _topLeft = new Sprite();
        _topLeft->initWithTexture(_scale9Image->getTexture(), rotatedlefttopbounds, true);
        _scale9Image->addChild(_topLeft, 2, pTopLeft);
        
        // Top right
        _topRight = new Sprite();
        _topRight->initWithTexture(_scale9Image->getTexture(), rotatedrighttopbounds, true);
        _scale9Image->addChild(_topRight, 2, pTopRight);
        
        // Bottom left
        _bottomLeft = new Sprite();
        _bottomLeft->initWithTexture(_scale9Image->getTexture(), rotatedleftbottombounds, true);
        _scale9Image->addChild(_bottomLeft, 2, pBottomLeft);
        
        // Bottom right
        _bottomRight = new Sprite();
        _bottomRight->initWithTexture(_scale9Image->getTexture(), rotatedrightbottombounds, true);
        _scale9Image->addChild(_bottomRight, 2, pBottomRight);
    }

    this->setContentSize(rect.size);
    this->addChild(_scale9Image);
    
    if (m_bSpritesGenerated)
        {
            // Restore color and opacity
            this->setOpacity(opacity);
            this->setColor(color);
        }
    m_bSpritesGenerated = true;

    return true;
}

void CCScale9Sprite::setContentSize(const Size &size)
{
    Node::setContentSize(size);
    this->m_positionsAreDirty = true;
}

void CCScale9Sprite::updatePositions()
{
    // Check that instances are non-NULL
    if(!((_topLeft) &&
         (_topRight) &&
         (_bottomRight) &&
         (_bottomLeft) &&
         (_centre))) {
        // if any of the above sprites are NULL, return
        return;
    }

    Size size = this->m_obContentSize;

    float sizableWidth = size.width - _topLeft->getContentSize().width - _topRight->getContentSize().width;
    float sizableHeight = size.height - _topLeft->getContentSize().height - _bottomRight->getContentSize().height;
    
    float horizontalScale = sizableWidth/_centre->getContentSize().width;
    float verticalScale = sizableHeight/_centre->getContentSize().height;

    _centre->setScaleX(horizontalScale);
    _centre->setScaleY(verticalScale);

    float rescaledWidth = _centre->getContentSize().width * horizontalScale;
    float rescaledHeight = _centre->getContentSize().height * verticalScale;

    float leftWidth = _bottomLeft->getContentSize().width;
    float bottomHeight = _bottomLeft->getContentSize().height;

    _bottomLeft->setAnchorPoint(Point(0,0));
    _bottomRight->setAnchorPoint(Point(0,0));
    _topLeft->setAnchorPoint(Point(0,0));
    _topRight->setAnchorPoint(Point(0,0));
    _left->setAnchorPoint(Point(0,0));
    _right->setAnchorPoint(Point(0,0));
    _top->setAnchorPoint(Point(0,0));
    _bottom->setAnchorPoint(Point(0,0));
    _centre->setAnchorPoint(Point(0,0));

    // Position corners
    _bottomLeft->setPosition(Point(0,0));
    _bottomRight->setPosition(Point(leftWidth+rescaledWidth,0));
    _topLeft->setPosition(Point(0, bottomHeight+rescaledHeight));
    _topRight->setPosition(Point(leftWidth+rescaledWidth, bottomHeight+rescaledHeight));

    // Scale and position borders
    _left->setPosition(Point(0, bottomHeight));
    _left->setScaleY(verticalScale);
    _right->setPosition(Point(leftWidth+rescaledWidth,bottomHeight));
    _right->setScaleY(verticalScale);
    _bottom->setPosition(Point(leftWidth,0));
    _bottom->setScaleX(horizontalScale);
    _top->setPosition(Point(leftWidth,bottomHeight+rescaledHeight));
    _top->setScaleX(horizontalScale);

    // Position centre
    _centre->setPosition(Point(leftWidth, bottomHeight));
}

bool CCScale9Sprite::initWithFile(const char* file, Rect rect,  Rect capInsets)
{
    AXAssert(file != NULL, "Invalid file for sprite");
    
    SpriteBatchNode *batchnode = SpriteBatchNode::create(file, 9);
    bool pReturn = this->initWithBatchNode(batchnode, rect, capInsets);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::create(const char* file, Rect rect,  Rect capInsets)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithFile(file, rect, capInsets) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    AX_SAFE_DELETE(pReturn);
    return NULL;
}

bool CCScale9Sprite::initWithFile(const char* file, Rect rect)
{
    AXAssert(file != NULL, "Invalid file for sprite");
    bool pReturn = this->initWithFile(file, rect, CCRectZero);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::create(const char* file, Rect rect)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithFile(file, rect) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    AX_SAFE_DELETE(pReturn);
    return NULL;
}


bool CCScale9Sprite::initWithFile(Rect capInsets, const char* file)
{
    bool pReturn = this->initWithFile(file, CCRectZero, capInsets);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::create(Rect capInsets, const char* file)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithFile(capInsets, file) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    AX_SAFE_DELETE(pReturn);
    return NULL;
}

bool CCScale9Sprite::initWithFile(const char* file)
{
    bool pReturn = this->initWithFile(file, CCRectZero);
    return pReturn;
    
}

CCScale9Sprite* CCScale9Sprite::create(const char* file)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithFile(file) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    AX_SAFE_DELETE(pReturn);
    return NULL;
}

bool CCScale9Sprite::initWithSpriteFrame(SpriteFrame* spriteFrame, Rect capInsets)
{
    Texture2D* texture = spriteFrame->getTexture();
    AXAssert(texture != NULL, "CCTexture must be not nil");

    SpriteBatchNode *batchnode = SpriteBatchNode::createWithTexture(texture, 9);
    AXAssert(batchnode != NULL, "SpriteBatchNode must be not nil");

    bool pReturn = this->initWithBatchNode(batchnode, spriteFrame->getRect(), spriteFrame->isRotated(), capInsets);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::createWithSpriteFrame(SpriteFrame* spriteFrame, Rect capInsets)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithSpriteFrame(spriteFrame, capInsets) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    AX_SAFE_DELETE(pReturn);
    return NULL;
}
bool CCScale9Sprite::initWithSpriteFrame(SpriteFrame* spriteFrame)
{
    AXAssert(spriteFrame != NULL, "Invalid spriteFrame for sprite");
    bool pReturn = this->initWithSpriteFrame(spriteFrame, CCRectZero);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::createWithSpriteFrame(SpriteFrame* spriteFrame)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithSpriteFrame(spriteFrame) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    AX_SAFE_DELETE(pReturn);
    return NULL;
}

bool CCScale9Sprite::initWithSpriteFrameName(const char* spriteFrameName, Rect capInsets)
{
    AXAssert((SpriteFrameCache::sharedSpriteFrameCache()) != NULL, "sharedSpriteFrameCache must be non-NULL");

    SpriteFrame *frame = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(spriteFrameName);
    AXAssert(frame != NULL, "SpriteFrame must be non-NULL");

    if (NULL == frame) return false;

    bool pReturn = this->initWithSpriteFrame(frame, capInsets);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::createWithSpriteFrameName(const char* spriteFrameName, Rect capInsets)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithSpriteFrameName(spriteFrameName, capInsets) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    AX_SAFE_DELETE(pReturn);
    return NULL;
}

bool CCScale9Sprite::initWithSpriteFrameName(const char* spriteFrameName)
{
    bool pReturn = this->initWithSpriteFrameName(spriteFrameName, CCRectZero);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::createWithSpriteFrameName(const char* spriteFrameName)
{
    AXAssert(spriteFrameName != NULL, "spriteFrameName must be non-NULL");

    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithSpriteFrameName(spriteFrameName) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    AX_SAFE_DELETE(pReturn);

    log("Could not allocate CCScale9Sprite()");
    return NULL;
    
}

CCScale9Sprite* CCScale9Sprite::resizableSpriteWithCapInsets(Rect capInsets)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithBatchNode(_scale9Image, m_spriteRect, capInsets) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    AX_SAFE_DELETE(pReturn);
    return NULL;
}

CCScale9Sprite* CCScale9Sprite::create()
{ 
    CCScale9Sprite *pReturn = new CCScale9Sprite();
    if (pReturn && pReturn->init())
    { 
        pReturn->autorelease();   
        return pReturn;
    } 
    AX_SAFE_DELETE(pReturn);
    return NULL;
}

/** sets the opacity.
 @warning If the the texture has premultiplied alpha then, the R, G and B channels will be modifed.
 Values goes from 0 to 255, where 255 means fully opaque.
 */

void CCScale9Sprite::setPreferredSize(Size preferedSize)
{
    this->setContentSize(preferedSize);
    this->m_preferredSize = preferedSize;
}

Size CCScale9Sprite::getPreferredSize()
{
    return this->m_preferredSize;
}

void CCScale9Sprite::setCapInsets(Rect capInsets)
{
    if (!_scale9Image)
    {
        return;
    }
    Size contentSize = this->m_obContentSize;
    this->updateWithBatchNode(this->_scale9Image, this->m_spriteRect, m_bSpriteFrameRotated, capInsets);
    this->setContentSize(contentSize);
}

Rect CCScale9Sprite::getCapInsets()
{
    return m_capInsets;
}

void CCScale9Sprite::updateCapInset()
{
    Rect insets;
    if (this->m_insetLeft == 0 && this->m_insetTop == 0 && this->m_insetRight == 0 && this->m_insetBottom == 0)
    {
        insets = CCRectZero;
    }
    else
    {
        if (m_bSpriteFrameRotated)
        {
            insets = CCRectMake(m_insetBottom,
                m_insetLeft,
                m_spriteRect.size.width-m_insetRight-m_insetLeft,
                m_spriteRect.size.height-m_insetTop-m_insetBottom);
        }
        else
        {
            insets = CCRectMake(m_insetLeft,
                m_insetTop,
                m_spriteRect.size.width-m_insetLeft-m_insetRight,
                m_spriteRect.size.height-m_insetTop-m_insetBottom);
        }
    }
    this->setCapInsets(insets);
}

void CCScale9Sprite::setOpacityModifyRGB(bool var)
{
    if (!_scale9Image)
    {
        return;
    }
    _opacityModifyRGB = var;
    Object* child;
    Array* children = _scale9Image->getChildren();
    AXARRAY_FOREACH(children, child)
    {
        RGBAProtocol* pNode = dynamic_cast<RGBAProtocol*>(child);
        if (pNode)
        {
            pNode->setOpacityModifyRGB(_opacityModifyRGB);
        }
    }
}


bool CCScale9Sprite::isOpacityModifyRGB()
{
    return _opacityModifyRGB;
}

void CCScale9Sprite::updateDisplayedOpacity(GLubyte parentOpacity)
{
    NodeRGBA::updateDisplayedOpacity(parentOpacity);
    setOpacity(parentOpacity);
}

void CCScale9Sprite::updateDisplayedColor(const axolotl::ccColor3B &color)
{
    NodeRGBA::updateDisplayedColor(color);
    setColor(color);
}

void CCScale9Sprite::setSpriteFrame(SpriteFrame * spriteFrame)
{
    SpriteBatchNode * batchnode = SpriteBatchNode::createWithTexture(spriteFrame->getTexture(), 9);
    this->updateWithBatchNode(batchnode, spriteFrame->getRect(), spriteFrame->isRotated(), CCRectZero);

    // Reset insets
    this->m_insetLeft = 0;
    this->m_insetTop = 0;
    this->m_insetRight = 0;
    this->m_insetBottom = 0;
}

float CCScale9Sprite::getInsetLeft()
{
    return this->m_insetLeft;
}

float CCScale9Sprite::getInsetTop()
{
    return this->m_insetTop;
}

float CCScale9Sprite::getInsetRight()
{
    return this->m_insetRight;
}

float CCScale9Sprite::getInsetBottom()
{
    return this->m_insetBottom;
}

void CCScale9Sprite::setInsetLeft(float insetLeft)
{
    this->m_insetLeft = insetLeft;
    this->updateCapInset();
}

void CCScale9Sprite::setInsetTop(float insetTop)
{
    this->m_insetTop = insetTop;
    this->updateCapInset();
}

void CCScale9Sprite::setInsetRight(float insetRight)
{
    this->m_insetRight = insetRight;
    this->updateCapInset();
}

void CCScale9Sprite::setInsetBottom(float insetBottom)
{
    this->m_insetBottom = insetBottom;
    this->updateCapInset();
}

void CCScale9Sprite::visit()
{
    if(this->m_positionsAreDirty)
    {
        this->updatePositions();
        this->m_positionsAreDirty = false;
    }
    Node::visit();
}

void CCScale9Sprite::setColor(const ccColor3B& color)
{
    if (!_scale9Image)
    {
        return;
    }
    _color = color;

    Object* child;
    Array* children = _scale9Image->getChildren();
    AXARRAY_FOREACH(children, child)
    {
        RGBAProtocol* pNode = dynamic_cast<RGBAProtocol*>(child);
        if (pNode)
        {
            pNode->setColor(color);
        }
    }
}

const ccColor3B& CCScale9Sprite::getColor()
{
	return _color;
}

void CCScale9Sprite::setOpacity(GLubyte opacity)
{
    if (!_scale9Image)
    {
        return;
    }
    _opacity = opacity;

    Object* child;
    Array* children = _scale9Image->getChildren();
    AXARRAY_FOREACH(children, child)
    {
        RGBAProtocol* pNode = dynamic_cast<RGBAProtocol*>(child);
        if (pNode)
        {
            pNode->setOpacity(opacity);
        }
    }
}

GLubyte CCScale9Sprite::getOpacity()
{
	return _opacity;
}

NS_AX_EXT_END
