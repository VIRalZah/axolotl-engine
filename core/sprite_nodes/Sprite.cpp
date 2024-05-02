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

#include "SpriteBatchNode.h"
#include "Animation.h"
#include "AnimationCache.h"
#include "ccConfig.h"
#include "Sprite.h"
#include "SpriteFrame.h"
#include "SpriteFrameCache.h"
#include "textures/TextureCache.h"
#include "draw_nodes/DrawingPrimitives.h"
#include "shaders/ShaderCache.h"
#include "shaders/ccGLStateCache.h"
#include "shaders/GLProgram.h"
#include "base/Director.h"
#include "support/PointExtension.h"
#include "cocoa/Geometry.h"
#include "textures/Texture2D.h"
#include "cocoa/AffineTransform.h"
#include "support/TransformUtils.h"
#include "support/Profiling.h"
#include "platform/Image.h"
// external
#include "kazmath/GL/matrix.h"
#include <string.h>

using namespace std;

NS_AX_BEGIN

#if AX_SPRITEBATCHNODE_RENDER_SUBPIXEL
#define RENDER_IN_SUBPIXEL
#else
#define RENDER_IN_SUBPIXEL(__ARGS__) (ceil(__ARGS__))
#endif

Sprite* Sprite::createWithTexture(Texture2D *pTexture)
{
    Sprite *pobSprite = new Sprite();
    if (pobSprite && pobSprite->initWithTexture(pTexture))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    AX_SAFE_DELETE(pobSprite);
    return NULL;
}

Sprite* Sprite::createWithTexture(Texture2D *pTexture, const Rect& rect)
{
    Sprite *pobSprite = new Sprite();
    if (pobSprite && pobSprite->initWithTexture(pTexture, rect))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    AX_SAFE_DELETE(pobSprite);
    return NULL;
}

Sprite* Sprite::create(const char *pszFileName)
{
    Sprite *pobSprite = new Sprite();
    if (pobSprite && pobSprite->initWithFile(pszFileName))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    AX_SAFE_DELETE(pobSprite);
    return NULL;
}

Sprite* Sprite::create(const char *pszFileName, const Rect& rect)
{
    Sprite *pobSprite = new Sprite();
    if (pobSprite && pobSprite->initWithFile(pszFileName, rect))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    AX_SAFE_DELETE(pobSprite);
    return NULL;
}

Sprite* Sprite::createWithSpriteFrame(SpriteFrame *pSpriteFrame)
{
    Sprite *pobSprite = new Sprite();
    if (pSpriteFrame && pobSprite && pobSprite->initWithSpriteFrame(pSpriteFrame))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    AX_SAFE_DELETE(pobSprite);
    return NULL;
}

Sprite* Sprite::createWithSpriteFrameName(const char *pszSpriteFrameName)
{
    SpriteFrame *pFrame = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pszSpriteFrameName);
    
#if AXOLOTL_DEBUG > 0
    char msg[256] = {0};
    sprintf(msg, "Invalid spriteFrameName: %s", pszSpriteFrameName);
    AXAssert(pFrame != NULL, msg);
#endif
    
    return createWithSpriteFrame(pFrame);
}

Sprite* Sprite::create()
{
    Sprite *pSprite = new Sprite();
    if (pSprite && pSprite->init())
    {
        pSprite->autorelease();
        return pSprite;
    }
    AX_SAFE_DELETE(pSprite);
    return NULL;
}

bool Sprite::init(void)
{
    return initWithTexture(NULL, CCRectZero);
}

// designated initializer
bool Sprite::initWithTexture(Texture2D *pTexture, const Rect& rect, bool rotated)
{
    if (NodeRGBA::init())
    {
        m_pobBatchNode = NULL;
        
        m_bRecursiveDirty = false;
        setDirty(false);
        
        m_bOpacityModifyRGB = true;
        
        m_sBlendFunc.src = AX_BLEND_SRC;
        m_sBlendFunc.dst = AX_BLEND_DST;
        
        m_bFlipX = m_bFlipY = false;
        
        // default transform anchor: center
        setAnchorPoint(Point(0.5f, 0.5f));
        
        // zwoptex default values
        m_obOffsetPosition = Point::ZERO;
        
        m_bHasChildren = false;
        
        // clean the Quad
        memset(&m_sQuad, 0, sizeof(m_sQuad));
        
        // Atlas: Color
        ccColor4B tmpColor = { 255, 255, 255, 255 };
        m_sQuad.bl.colors = tmpColor;
        m_sQuad.br.colors = tmpColor;
        m_sQuad.tl.colors = tmpColor;
        m_sQuad.tr.colors = tmpColor;

        // shader program
        setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(kAXShader_PositionTextureColor));
        
        // update texture (calls updateBlendFunc)
        setTexture(pTexture);
        setTextureRect(rect, rotated, rect.size);
        
        // by default use "Self Render".
        // if the sprite is added to a batchnode, then it will automatically switch to "batchnode Render"
        setBatchNode(NULL);
        
        return true;
    }
    else
    {
        return false;
    }
}

bool Sprite::initWithTexture(Texture2D *pTexture, const Rect& rect)
{
    return initWithTexture(pTexture, rect, false);
}

bool Sprite::initWithTexture(Texture2D *pTexture)
{
    AXAssert(pTexture != NULL, "Invalid texture for sprite");

    Rect rect = CCRectZero;
    rect.size = pTexture->getContentSize();
    
    return initWithTexture(pTexture, rect);
}

bool Sprite::initWithFile(const char *pszFilename)
{
    AXAssert(pszFilename != NULL, "Invalid filename for sprite");

    Texture2D *pTexture = TextureCache::sharedTextureCache()->addImage(pszFilename);
    if (pTexture)
    {
        Rect rect = CCRectZero;
        rect.size = pTexture->getContentSize();
        return initWithTexture(pTexture, rect);
    }

    // don't release here.
    // when load texture failed, it's better to get a "transparent" sprite than a crashed program
    // this->release(); 
    return false;
}

bool Sprite::initWithFile(const char *pszFilename, const Rect& rect)
{
    AXAssert(pszFilename != NULL, "");

    Texture2D *pTexture = TextureCache::sharedTextureCache()->addImage(pszFilename);
    if (pTexture)
    {
        return initWithTexture(pTexture, rect);
    }

    // don't release here.
    // when load texture failed, it's better to get a "transparent" sprite than a crashed program
    // this->release(); 
    return false;
}

bool Sprite::initWithSpriteFrame(SpriteFrame *pSpriteFrame)
{
    AXAssert(pSpriteFrame != NULL, "");

    bool bRet = initWithTexture(pSpriteFrame->getTexture(), pSpriteFrame->getRect());
    setDisplayFrame(pSpriteFrame);

    return bRet;
}

bool Sprite::initWithSpriteFrameName(const char *pszSpriteFrameName)
{
    AXAssert(pszSpriteFrameName != NULL, "");

    SpriteFrame *pFrame = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pszSpriteFrameName);
    return initWithSpriteFrame(pFrame);
}

// XXX: deprecated
/*
Sprite* Sprite::initWithCGImage(CGImageRef pImage)
{
    // todo
    // because it is deprecated, so we do not implement it

    return NULL;
}
*/

/*
Sprite* Sprite::initWithCGImage(CGImageRef pImage, const char *pszKey)
{
    AXAssert(pImage != NULL);

    // XXX: possible bug. See issue #349. New API should be added
    Texture2D *pTexture = TextureCache::sharedTextureCache()->addCGImage(pImage, pszKey);

    const Size& size = pTexture->getContentSize();
    Rect rect = CCRectMake(0 ,0, size.width, size.height);

    return initWithTexture(texture, rect);
}
*/

Sprite::Sprite(void)
: m_bShouldBeHidden(false),
m_pobTexture(NULL)
{
}

Sprite::~Sprite(void)
{
    AX_SAFE_RELEASE(m_pobTexture);
}

void Sprite::setTextureRect(const Rect& rect)
{
    setTextureRect(rect, false, rect.size);
}


void Sprite::setTextureRect(const Rect& rect, bool rotated, const Size& untrimmedSize)
{
    m_bRectRotated = rotated;

    setContentSize(untrimmedSize);
    setVertexRect(rect);
    setTextureCoords(rect);

    Point relativeOffset = m_obUnflippedOffsetPositionFromCenter;

    // issue #732
    if (m_bFlipX)
    {
        relativeOffset.x = -relativeOffset.x;
    }
    if (m_bFlipY)
    {
        relativeOffset.y = -relativeOffset.y;
    }

    m_obOffsetPosition.x = relativeOffset.x + (m_obContentSize.width - m_obRect.size.width) / 2;
    m_obOffsetPosition.y = relativeOffset.y + (m_obContentSize.height - m_obRect.size.height) / 2;

    // rendering using batch node
    if (m_pobBatchNode)
    {
        // update dirty_, don't update recursiveDirty_
        setDirty(true);
    }
    else
    {
        // self rendering
        
        // Atlas: Vertex
        float x1 = 0 + m_obOffsetPosition.x;
        float y1 = 0 + m_obOffsetPosition.y;
        float x2 = x1 + m_obRect.size.width;
        float y2 = y1 + m_obRect.size.height;

        // Don't update Z.
        m_sQuad.bl.vertices = vertex3(x1, y1, 0);
        m_sQuad.br.vertices = vertex3(x2, y1, 0);
        m_sQuad.tl.vertices = vertex3(x1, y2, 0);
        m_sQuad.tr.vertices = vertex3(x2, y2, 0);
    }
}

// override this method to generate "double scale" sprites
void Sprite::setVertexRect(const Rect& rect)
{
    m_obRect = rect;
}

void Sprite::setTextureCoords(const Rect& _rect)
{
    Rect rect = AX_RECT_POINTS_TO_PIXELS(_rect);

    Texture2D *tex = m_pobBatchNode ? m_pobTextureAtlas->getTexture() : m_pobTexture;
    if (! tex)
    {
        return;
    }

    float atlasWidth = (float)tex->getPixelsWide();
    float atlasHeight = (float)tex->getPixelsHigh();

    float left, right, top, bottom;

    if (m_bRectRotated)
    {
#if AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL
        left    = (2*rect.origin.x+1)/(2*atlasWidth);
        right    = left+(rect.size.height*2-2)/(2*atlasWidth);
        top        = (2*rect.origin.y+1)/(2*atlasHeight);
        bottom    = top+(rect.size.width*2-2)/(2*atlasHeight);
#else
        left    = rect.origin.x/atlasWidth;
        right    = (rect.origin.x+rect.size.height) / atlasWidth;
        top        = rect.origin.y/atlasHeight;
        bottom    = (rect.origin.y+rect.size.width) / atlasHeight;
#endif // AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL

        if (m_bFlipX)
        {
            AX_SWAP(top, bottom, float);
        }

        if (m_bFlipY)
        {
            AX_SWAP(left, right, float);
        }

        m_sQuad.bl.texCoords.u = left;
        m_sQuad.bl.texCoords.v = top;
        m_sQuad.br.texCoords.u = left;
        m_sQuad.br.texCoords.v = bottom;
        m_sQuad.tl.texCoords.u = right;
        m_sQuad.tl.texCoords.v = top;
        m_sQuad.tr.texCoords.u = right;
        m_sQuad.tr.texCoords.v = bottom;
    }
    else
    {
#if AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL
        left    = (2*rect.origin.x+1)/(2*atlasWidth);
        right    = left + (rect.size.width*2-2)/(2*atlasWidth);
        top        = (2*rect.origin.y+1)/(2*atlasHeight);
        bottom    = top + (rect.size.height*2-2)/(2*atlasHeight);
#else
        left    = rect.origin.x/atlasWidth;
        right    = (rect.origin.x + rect.size.width) / atlasWidth;
        top        = rect.origin.y/atlasHeight;
        bottom    = (rect.origin.y + rect.size.height) / atlasHeight;
#endif // ! AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL

        if(m_bFlipX)
        {
            AX_SWAP(left,right,float);
        }

        if(m_bFlipY)
        {
            AX_SWAP(top,bottom,float);
        }

        m_sQuad.bl.texCoords.u = left;
        m_sQuad.bl.texCoords.v = bottom;
        m_sQuad.br.texCoords.u = right;
        m_sQuad.br.texCoords.v = bottom;
        m_sQuad.tl.texCoords.u = left;
        m_sQuad.tl.texCoords.v = top;
        m_sQuad.tr.texCoords.u = right;
        m_sQuad.tr.texCoords.v = top;
    }
}

void Sprite::updateTransform(void)
{
    AXAssert(m_pobBatchNode, "updateTransform is only valid when Sprite is being rendered using an SpriteBatchNode");

    // recalculate matrix only if it is dirty
    if( isDirty() ) {

        // If it is not visible, or one of its ancestors is not visible, then do nothing:
        if( !m_bVisible || ( m_pParent && m_pParent != m_pobBatchNode && ((Sprite*)m_pParent)->m_bShouldBeHidden) )
        {
            m_sQuad.br.vertices = m_sQuad.tl.vertices = m_sQuad.tr.vertices = m_sQuad.bl.vertices = vertex3(0,0,0);
            m_bShouldBeHidden = true;
        }
        else 
        {
            m_bShouldBeHidden = false;

            if( ! m_pParent || m_pParent == m_pobBatchNode )
            {
                m_transformToBatch = nodeToParentTransform();
            }
            else 
            {
                AXAssert( dynamic_cast<Sprite*>(m_pParent), "Logic error in Sprite. Parent must be a Sprite");
                m_transformToBatch = AffineTransformConcat( nodeToParentTransform() , ((Sprite*)m_pParent)->m_transformToBatch );
            }

            //
            // calculate the Quad based on the Affine Matrix
            //

            Size size = m_obRect.size;

            float x1 = m_obOffsetPosition.x;
            float y1 = m_obOffsetPosition.y;

            float x2 = x1 + size.width;
            float y2 = y1 + size.height;
            float x = m_transformToBatch.tx;
            float y = m_transformToBatch.ty;

            float cr = m_transformToBatch.a;
            float sr = m_transformToBatch.b;
            float cr2 = m_transformToBatch.d;
            float sr2 = -m_transformToBatch.c;
            float ax = x1 * cr - y1 * sr2 + x;
            float ay = x1 * sr + y1 * cr2 + y;

            float bx = x2 * cr - y1 * sr2 + x;
            float by = x2 * sr + y1 * cr2 + y;

            float cx = x2 * cr - y2 * sr2 + x;
            float cy = x2 * sr + y2 * cr2 + y;

            float dx = x1 * cr - y2 * sr2 + x;
            float dy = x1 * sr + y2 * cr2 + y;

            m_sQuad.bl.vertices = vertex3( RENDER_IN_SUBPIXEL(ax), RENDER_IN_SUBPIXEL(ay), m_fVertexZ );
            m_sQuad.br.vertices = vertex3( RENDER_IN_SUBPIXEL(bx), RENDER_IN_SUBPIXEL(by), m_fVertexZ );
            m_sQuad.tl.vertices = vertex3( RENDER_IN_SUBPIXEL(dx), RENDER_IN_SUBPIXEL(dy), m_fVertexZ );
            m_sQuad.tr.vertices = vertex3( RENDER_IN_SUBPIXEL(cx), RENDER_IN_SUBPIXEL(cy), m_fVertexZ );
        }

        // MARMALADE CHANGE: ADDED CHECK FOR NULL, TO PERMIT SPRITES WITH NO BATCH NODE / TEXTURE ATLAS
        if (m_pobTextureAtlas)
		{
            m_pobTextureAtlas->updateQuad(&m_sQuad, m_uAtlasIndex);
        }
		
        m_bRecursiveDirty = false;
        setDirty(false);
    }

    // MARMALADE CHANGED
    // recursively iterate over children
/*    if( m_bHasChildren ) 
    {
        // MARMALADE: CHANGED TO USE Node*
        // NOTE THAT WE HAVE ALSO DEFINED virtual Node::updateTransform()
        arrayMakeObjectsPerformSelector(m_pChildren, updateTransform, Sprite*);
    }*/
    Node::updateTransform();

#if AX_SPRITE_DEBUG_DRAW
    // draw bounding box
    Point vertices[4] = {
        Point( m_sQuad.bl.vertices.x, m_sQuad.bl.vertices.y ),
        Point( m_sQuad.br.vertices.x, m_sQuad.br.vertices.y ),
        Point( m_sQuad.tr.vertices.x, m_sQuad.tr.vertices.y ),
        Point( m_sQuad.tl.vertices.x, m_sQuad.tl.vertices.y ),
    };
    ccDrawPoly(vertices, 4, true);
#endif // AX_SPRITE_DEBUG_DRAW
}

// draw

void Sprite::draw(void)
{
    AX_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "Sprite - draw");

    AXAssert(!m_pobBatchNode, "If Sprite is being rendered by SpriteBatchNode, Sprite#draw SHOULD NOT be called");

    AX_NODE_DRAW_SETUP();

    ccGLBlendFunc( m_sBlendFunc.src, m_sBlendFunc.dst );

    ccGLBindTexture2D( m_pobTexture->getName() );
    ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );

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

    CHECK_GL_ERROR_DEBUG();


#if AX_SPRITE_DEBUG_DRAW == 1
    // draw bounding box
    Point vertices[4]={
        Point(m_sQuad.tl.vertices.x,m_sQuad.tl.vertices.y),
        Point(m_sQuad.bl.vertices.x,m_sQuad.bl.vertices.y),
        Point(m_sQuad.br.vertices.x,m_sQuad.br.vertices.y),
        Point(m_sQuad.tr.vertices.x,m_sQuad.tr.vertices.y),
    };
    ccDrawPoly(vertices, 4, true);
#elif AX_SPRITE_DEBUG_DRAW == 2
    // draw texture box
    Size s = this->getTextureRect().size;
    Point offsetPix = this->getOffsetPosition();
    Point vertices[4] = {
        Point(offsetPix.x,offsetPix.y), Point(offsetPix.x+s.width,offsetPix.y),
        Point(offsetPix.x+s.width,offsetPix.y+s.height), Point(offsetPix.x,offsetPix.y+s.height)
    };
    ccDrawPoly(vertices, 4, true);
#endif // AX_SPRITE_DEBUG_DRAW

    AX_INCREMENT_GL_DRAWS(1);

    AX_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, "Sprite - draw");
}

// Node overrides

void Sprite::addChild(Node* pChild)
{
    Node::addChild(pChild);
}

void Sprite::addChild(Node *pChild, int zOrder)
{
    Node::addChild(pChild, zOrder);
}

void Sprite::addChild(Node *pChild, int zOrder, int tag)
{
    AXAssert(pChild != NULL, "Argument must be non-NULL");

    if (m_pobBatchNode)
    {
        Sprite* pChildSprite = dynamic_cast<Sprite*>(pChild);
        AXAssert( pChildSprite, "Sprite only supports CCSprites as children when using SpriteBatchNode");
        AXAssert(pChildSprite->getTexture()->getName() == m_pobTextureAtlas->getTexture()->getName(), "");
        //put it in descendants array of batch node
        m_pobBatchNode->appendChild(pChildSprite);

        if (!m_bReorderChildDirty)
        {
            setReorderChildDirtyRecursively();
        }
    }
    //Node already sets isReorderChildDirty_ so this needs to be after batchNode check
    Node::addChild(pChild, zOrder, tag);
    m_bHasChildren = true;
}

void Sprite::reorderChild(Node *pChild, int zOrder)
{
    AXAssert(pChild != NULL, "");
    AXAssert(m_pChildren->containsObject(pChild), "");

    if (zOrder == pChild->getZOrder())
    {
        return;
    }

    if( m_pobBatchNode && ! m_bReorderChildDirty)
    {
        setReorderChildDirtyRecursively();
        m_pobBatchNode->reorderBatch(true);
    }

    Node::reorderChild(pChild, zOrder);
}

void Sprite::removeChild(Node *pChild, bool bCleanup)
{
    if (m_pobBatchNode)
    {
        m_pobBatchNode->removeSpriteFromAtlas((Sprite*)(pChild));
    }

    Node::removeChild(pChild, bCleanup);
    
}

void Sprite::removeAllChildrenWithCleanup(bool bCleanup)
{
    if (m_pobBatchNode)
    {
        Object* pObject = NULL;
        AXARRAY_FOREACH(m_pChildren, pObject)
        {
            Sprite* pChild = dynamic_cast<Sprite*>(pObject);
            if (pChild)
            {
                m_pobBatchNode->removeSpriteFromAtlas(pChild);
            }
        }
    }

    Node::removeAllChildrenWithCleanup(bCleanup);
    
    m_bHasChildren = false;
}

void Sprite::sortAllChildren()
{
    if (m_bReorderChildDirty)
    {
        int i = 0,j = 0,length = m_pChildren->data->num;
        Node** x = (Node**)m_pChildren->data->arr;
        Node *tempItem = NULL;

        // insertion sort
        for(i=1; i<length; i++)
        {
            tempItem = x[i];
            j = i-1;

            //continue moving element downwards while zOrder is smaller or when zOrder is the same but orderOfArrival is smaller
            while(j>=0 && ( tempItem->getZOrder() < x[j]->getZOrder() || ( tempItem->getZOrder() == x[j]->getZOrder() && tempItem->getOrderOfArrival() < x[j]->getOrderOfArrival() ) ) )
            {
                x[j+1] = x[j];
                j = j-1;
            }
            x[j+1] = tempItem;
        }

        if ( m_pobBatchNode)
        {
            arrayMakeObjectsPerformSelector(m_pChildren, sortAllChildren, Sprite*);
        }

        m_bReorderChildDirty = false;
    }
}

//
// Node property overloads
// used only when parent is SpriteBatchNode
//

void Sprite::setReorderChildDirtyRecursively(void)
{
    //only set parents flag the first time
    if ( ! m_bReorderChildDirty )
    {
        m_bReorderChildDirty = true;
        Node* pNode = (Node*)m_pParent;
        while (pNode && pNode != m_pobBatchNode)
        {
            ((Sprite*)pNode)->setReorderChildDirtyRecursively();
            pNode=pNode->getParent();
        }
    }
}


void Sprite::setDirtyRecursively(bool bValue)
{
    m_bRecursiveDirty = bValue;
    setDirty(bValue);
    // recursively set dirty
    if (m_bHasChildren)
    {
        Object* pObject = NULL;
        AXARRAY_FOREACH(m_pChildren, pObject)
        {
            Sprite* pChild = dynamic_cast<Sprite*>(pObject);
            if (pChild)
            {
                pChild->setDirtyRecursively(true);
            }
        }
    }
}

// XXX HACK: optimization
#define SET_DIRTY_RECURSIVELY() {                                    \
                    if (m_pobBatchNode && ! m_bRecursiveDirty) {    \
                        m_bRecursiveDirty = true;                    \
                        setDirty(true);                              \
                        if ( m_bHasChildren)                        \
                            setDirtyRecursively(true);                \
                        }                                            \
                    }

void Sprite::setPosition(const Point& pos)
{
    Node::setPosition(pos);
    SET_DIRTY_RECURSIVELY();
}

void Sprite::setRotation(float fRotation)
{
    Node::setRotation(fRotation);
    SET_DIRTY_RECURSIVELY();
}

void Sprite::setRotationX(float fRotationX)
{
    Node::setRotationX(fRotationX);
    SET_DIRTY_RECURSIVELY();
}

void Sprite::setRotationY(float fRotationY)
{
    Node::setRotationY(fRotationY);
    SET_DIRTY_RECURSIVELY();
}

void Sprite::setSkewX(float sx)
{
    Node::setSkewX(sx);
    SET_DIRTY_RECURSIVELY();
}

void Sprite::setSkewY(float sy)
{
    Node::setSkewY(sy);
    SET_DIRTY_RECURSIVELY();
}

void Sprite::setScaleX(float fScaleX)
{
    Node::setScaleX(fScaleX);
    SET_DIRTY_RECURSIVELY();
}

void Sprite::setScaleY(float fScaleY)
{
    Node::setScaleY(fScaleY);
    SET_DIRTY_RECURSIVELY();
}

void Sprite::setScale(float fScale)
{
    Node::setScale(fScale);
    SET_DIRTY_RECURSIVELY();
}

void Sprite::setVertexZ(float fVertexZ)
{
    Node::setVertexZ(fVertexZ);
    SET_DIRTY_RECURSIVELY();
}

void Sprite::setAnchorPoint(const Point& anchor)
{
    Node::setAnchorPoint(anchor);
    SET_DIRTY_RECURSIVELY();
}

void Sprite::ignoreAnchorPointForPosition(bool value)
{
    AXAssert(! m_pobBatchNode, "ignoreAnchorPointForPosition is invalid in Sprite");
    Node::ignoreAnchorPointForPosition(value);
}

void Sprite::setVisible(bool bVisible)
{
    Node::setVisible(bVisible);
    SET_DIRTY_RECURSIVELY();
}

void Sprite::setFlipX(bool bFlipX)
{
    if (m_bFlipX != bFlipX)
    {
        m_bFlipX = bFlipX;
        setTextureRect(m_obRect, m_bRectRotated, m_obContentSize);
    }
}

bool Sprite::isFlipX(void)
{
    return m_bFlipX;
}

void Sprite::setFlipY(bool bFlipY)
{
    if (m_bFlipY != bFlipY)
    {
        m_bFlipY = bFlipY;
        setTextureRect(m_obRect, m_bRectRotated, m_obContentSize);
    }
}

bool Sprite::isFlipY(void)
{
    return m_bFlipY;
}

//
// RGBA protocol
//

void Sprite::updateColor(void)
{
    ccColor4B color4 = { _displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity };
    
    // special opacity for premultiplied textures
	if (m_bOpacityModifyRGB)
    {
		color4.r *= _displayedOpacity/255.0f;
		color4.g *= _displayedOpacity/255.0f;
		color4.b *= _displayedOpacity/255.0f;
    }

    m_sQuad.bl.colors = color4;
    m_sQuad.br.colors = color4;
    m_sQuad.tl.colors = color4;
    m_sQuad.tr.colors = color4;

    // renders using batch node
    if (m_pobBatchNode)
    {
        if (m_uAtlasIndex != CCSpriteIndexNotInitialized)
        {
            m_pobTextureAtlas->updateQuad(&m_sQuad, m_uAtlasIndex);
        }
        else
        {
            // no need to set it recursively
            // update dirty_, don't update recursiveDirty_
            setDirty(true);
        }
    }

    // self render
    // do nothing
}

void Sprite::setOpacity(GLubyte opacity)
{
    NodeRGBA::setOpacity(opacity);

    updateColor();
}

void Sprite::setColor(const ccColor3B& color3)
{
    NodeRGBA::setColor(color3);

    updateColor();
}

void Sprite::setOpacityModifyRGB(bool modify)
{
    if (m_bOpacityModifyRGB != modify)
    {
        m_bOpacityModifyRGB = modify;
        updateColor();
    }
}

bool Sprite::isOpacityModifyRGB(void)
{
    return m_bOpacityModifyRGB;
}

void Sprite::updateDisplayedColor(const ccColor3B& parentColor)
{
    NodeRGBA::updateDisplayedColor(parentColor);
    
    updateColor();
}

void Sprite::updateDisplayedOpacity(GLubyte opacity)
{
    NodeRGBA::updateDisplayedOpacity(opacity);
    
    updateColor();
}

// Frames

void Sprite::setDisplayFrame(SpriteFrame *pNewFrame)
{
    m_obUnflippedOffsetPositionFromCenter = pNewFrame->getOffset();

    Texture2D *pNewTexture = pNewFrame->getTexture();
    // update texture before updating texture rect
    if (pNewTexture != m_pobTexture)
    {
        setTexture(pNewTexture);
    }

    // update rect
    m_bRectRotated = pNewFrame->isRotated();
    setTextureRect(pNewFrame->getRect(), m_bRectRotated, pNewFrame->getOriginalSize());
}

void Sprite::setDisplayFrameWithAnimationName(const char *animationName, int frameIndex)
{
    AXAssert(animationName, "Sprite#setDisplayFrameWithAnimationName. animationName must not be NULL");

    Animation *a = AnimationCache::sharedAnimationCache()->animationByName(animationName);

    AXAssert(a, "Sprite#setDisplayFrameWithAnimationName: Frame not found");

    AnimationFrame* frame = (AnimationFrame*)a->getFrames()->objectAtIndex(frameIndex);

    AXAssert(frame, "Sprite#setDisplayFrame. Invalid frame");

    setDisplayFrame(frame->getSpriteFrame());
}

bool Sprite::isFrameDisplayed(SpriteFrame *pFrame)
{
    Rect r = pFrame->getRect();

    return (r.equals(m_obRect) &&
            pFrame->getTexture()->getName() == m_pobTexture->getName() &&
            pFrame->getOffset().equals(m_obUnflippedOffsetPositionFromCenter));
}

SpriteFrame* Sprite::displayFrame(void)
{
    return SpriteFrame::createWithTexture(m_pobTexture,
                                           AX_RECT_POINTS_TO_PIXELS(m_obRect),
                                           m_bRectRotated,
                                           AX_POINT_POINTS_TO_PIXELS(m_obUnflippedOffsetPositionFromCenter),
                                           AX_SIZE_POINTS_TO_PIXELS(m_obContentSize));
}

SpriteBatchNode* Sprite::getBatchNode(void)
{
    return m_pobBatchNode;
}

void Sprite::setBatchNode(SpriteBatchNode *pobSpriteBatchNode)
{
    m_pobBatchNode = pobSpriteBatchNode; // weak reference

    // self render
    if( ! m_pobBatchNode ) {
        m_uAtlasIndex = CCSpriteIndexNotInitialized;
        setTextureAtlas(NULL);
        m_bRecursiveDirty = false;
        setDirty(false);

        float x1 = m_obOffsetPosition.x;
        float y1 = m_obOffsetPosition.y;
        float x2 = x1 + m_obRect.size.width;
        float y2 = y1 + m_obRect.size.height;
        m_sQuad.bl.vertices = vertex3( x1, y1, 0 );
        m_sQuad.br.vertices = vertex3( x2, y1, 0 );
        m_sQuad.tl.vertices = vertex3( x1, y2, 0 );
        m_sQuad.tr.vertices = vertex3( x2, y2, 0 );

    } else {

        // using batch
        m_transformToBatch = AffineTransformIdentity;
        setTextureAtlas(m_pobBatchNode->getTextureAtlas()); // weak ref
    }
}

// Texture protocol

void Sprite::updateBlendFunc(void)
{
    AXAssert (! m_pobBatchNode, "Sprite: updateBlendFunc doesn't work when the sprite is rendered using a SpriteBatchNode");

    // it is possible to have an untextured sprite
    if (! m_pobTexture || ! m_pobTexture->hasPremultipliedAlpha())
    {
        m_sBlendFunc.src = GL_SRC_ALPHA;
        m_sBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
        setOpacityModifyRGB(false);
    }
    else
    {
        m_sBlendFunc.src = AX_BLEND_SRC;
        m_sBlendFunc.dst = AX_BLEND_DST;
        setOpacityModifyRGB(true);
    }
}

/*
 * This array is the data of a white image with 2 by 2 dimension.
 * It's used for creating a default texture when sprite's texture is set to NULL.
 * Supposing codes as follows:
 *
 *   Sprite* sp = new Sprite();
 *   sp->init();  // Texture was set to NULL, in order to make opacity and color to work correctly, we need to create a 2x2 white texture.
 *
 * The test is in "TestCpp/SpriteTest/Sprite without texture".
 */
static unsigned char AX_2x2_white_image[] = {
    // RGBA8888
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
};

#define AX_2x2_WHITE_IMAGE_KEY  "AX_2x2_white_image"

void Sprite::setTexture(Texture2D *texture)
{
    // If batchnode, then texture id should be the same
    AXAssert(! m_pobBatchNode || texture->getName() == m_pobBatchNode->getTexture()->getName(), "Sprite: Batched sprites should use the same texture as the batchnode");
    // accept texture==nil as argument
    AXAssert( !texture || dynamic_cast<Texture2D*>(texture), "setTexture expects a Texture2D. Invalid argument");

    if (NULL == texture)
    {
        // Gets the texture by key firstly.
        texture = TextureCache::sharedTextureCache()->textureForKey(AX_2x2_WHITE_IMAGE_KEY);

        // If texture wasn't in cache, create it from RAW data.
        if (NULL == texture)
        {
            Image* image = new Image();
            bool isOK = image->initWithImageData(AX_2x2_white_image, sizeof(AX_2x2_white_image), Image::kFmtRawData, 2, 2, 8);
            AXAssert(isOK, "The 2x2 empty texture was created unsuccessfully.");

            texture = TextureCache::sharedTextureCache()->addUIImage(image, AX_2x2_WHITE_IMAGE_KEY);
            AX_SAFE_RELEASE(image);
        }
    }

    if (!m_pobBatchNode && m_pobTexture != texture)
    {
        AX_SAFE_RETAIN(texture);
        AX_SAFE_RELEASE(m_pobTexture);
        m_pobTexture = texture;
        updateBlendFunc();
    }
}

Texture2D* Sprite::getTexture(void)
{
    return m_pobTexture;
}

NS_AX_END
