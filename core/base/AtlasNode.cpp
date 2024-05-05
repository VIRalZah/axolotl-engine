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

#include "AtlasNode.h"
#include "textures/TextureAtlas.h"
#include "textures/TextureCache.h"
#include "base/Director.h"
#include "shaders/GLProgram.h"
#include "shaders/ShaderCache.h"
#include "shaders/ccGLStateCache.h"
#include "base/Director.h"
#include "support/TransformUtils.h"

// external
#include "kazmath/GL/matrix.h"

NS_AX_BEGIN

// implementation AtlasNode

// AtlasNode - Creation & Init

AtlasNode::AtlasNode()
: m_uItemsPerRow(0)
, m_uItemsPerColumn(0)
, m_uItemWidth(0)
, m_uItemHeight(0)
, m_pTextureAtlas(NULL)
, m_bIsOpacityModifyRGB(false)
, m_uQuadsToDraw(0)
, m_nUniformColor(0)
, m_bIgnoreContentScaleFactor(false)
{
}

AtlasNode::~AtlasNode()
{
    AX_SAFE_RELEASE(m_pTextureAtlas);
}

AtlasNode * AtlasNode::create(const char *tile, unsigned int tileWidth, unsigned int tileHeight, 
											 unsigned int itemsToRender)
{
	AtlasNode * pRet = new AtlasNode();
	if (pRet->initWithTileFile(tile, tileWidth, tileHeight, itemsToRender))
	{
		pRet->autorelease();
		return pRet;
	}
	AX_SAFE_DELETE(pRet);
	return NULL;
}

bool AtlasNode::initWithTileFile(const char *tile, unsigned int tileWidth, unsigned int tileHeight, unsigned int itemsToRender)
{
    AXAssert(tile != NULL, "title should not be null");
    Texture2D *texture = TextureCache::sharedTextureCache()->addImage(tile);
	return initWithTexture(texture, tileWidth, tileHeight, itemsToRender);
}

bool AtlasNode::initWithTexture(Texture2D* texture, unsigned int tileWidth, unsigned int tileHeight, 
                                   unsigned int itemsToRender)
{
    m_uItemWidth  = tileWidth;
    m_uItemHeight = tileHeight;

    m_tColorUnmodified = ccWHITE;
    m_bIsOpacityModifyRGB = true;

    m_tBlendFunc.src = AX_BLEND_SRC;
    m_tBlendFunc.dst = AX_BLEND_DST;

    m_pTextureAtlas = new TextureAtlas();
    m_pTextureAtlas->initWithTexture(texture, itemsToRender);

    if (! m_pTextureAtlas)
    {
        AXLOG("cocos2d: Could not initialize AtlasNode. Invalid Texture.");
        return false;
    }

    this->updateBlendFunc();
    this->updateOpacityModifyRGB();

    this->calculateMaxItems();

    m_uQuadsToDraw = itemsToRender;

    // shader stuff
    setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(kAXShader_PositionTexture_uColor));
    m_nUniformColor = glGetUniformLocation( getShaderProgram()->getProgram(), "u_color");

    return true;
}


// AtlasNode - Atlas generation

void AtlasNode::calculateMaxItems()
{
    Size s = m_pTextureAtlas->getTexture()->getContentSize();
    
    if (m_bIgnoreContentScaleFactor)
    {
        s = m_pTextureAtlas->getTexture()->getContentSizeInPixels();
    }
    
    m_uItemsPerColumn = (int)(s.height / m_uItemHeight);
    m_uItemsPerRow = (int)(s.width / m_uItemWidth);
}

void AtlasNode::updateAtlasValues()
{
    AXAssert(false, "AtlasNode:Abstract updateAtlasValue not overridden");
}

// AtlasNode - draw
void AtlasNode::draw(void)
{
    AX_NODE_DRAW_SETUP();

    ccGLBlendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );

    GLfloat colors[4] = {_displayedColor.r / 255.0f, _displayedColor.g / 255.0f, _displayedColor.b / 255.0f, _displayedOpacity / 255.0f};
    getShaderProgram()->setUniformLocationWith4fv(m_nUniformColor, colors, 1);

    m_pTextureAtlas->drawNumberOfQuads(m_uQuadsToDraw, 0);
}

// AtlasNode - RGBA protocol

const ccColor3B& AtlasNode::getColor()
{
    if(m_bIsOpacityModifyRGB)
    {
        return m_tColorUnmodified;
    }
    return NodeRGBA::getColor();
}

void AtlasNode::setColor(const ccColor3B& color3)
{
    ccColor3B tmp = color3;
    m_tColorUnmodified = color3;

    if( m_bIsOpacityModifyRGB )
    {
        tmp.r = tmp.r * _displayedOpacity/255;
        tmp.g = tmp.g * _displayedOpacity/255;
        tmp.b = tmp.b * _displayedOpacity/255;
    }
    NodeRGBA::setColor(tmp);
}

void AtlasNode::setOpacity(GLubyte opacity)
{
    NodeRGBA::setOpacity(opacity);

    // special opacity for premultiplied textures
    if( m_bIsOpacityModifyRGB )
        this->setColor(m_tColorUnmodified);
}

void AtlasNode::setOpacityModifyRGB(bool bValue)
{
    ccColor3B oldColor = this->getColor();
    m_bIsOpacityModifyRGB = bValue;
    this->setColor(oldColor);
}

bool AtlasNode::isOpacityModifyRGB()
{
    return m_bIsOpacityModifyRGB;
}

void AtlasNode::updateOpacityModifyRGB()
{
    m_bIsOpacityModifyRGB = m_pTextureAtlas->getTexture()->hasPremultipliedAlpha();
}

void AtlasNode::setIgnoreContentScaleFactor(bool bIgnoreContentScaleFactor)
{
    m_bIgnoreContentScaleFactor = bIgnoreContentScaleFactor;
}

// AtlasNode - CocosNodeTexture protocol

ccBlendFunc AtlasNode::getBlendFunc()
{
    return m_tBlendFunc;
}

void AtlasNode::setBlendFunc(ccBlendFunc blendFunc)
{
    m_tBlendFunc = blendFunc;
}

void AtlasNode::updateBlendFunc()
{
    if( ! m_pTextureAtlas->getTexture()->hasPremultipliedAlpha() ) {
        m_tBlendFunc.src = GL_SRC_ALPHA;
        m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
    }
}

void AtlasNode::setTexture(Texture2D *texture)
{
    m_pTextureAtlas->setTexture(texture);
    this->updateBlendFunc();
    this->updateOpacityModifyRGB();
}

Texture2D * AtlasNode::getTexture()
{
    return m_pTextureAtlas->getTexture();
}

void AtlasNode::setTextureAtlas(TextureAtlas* var)
{
    AX_SAFE_RETAIN(var);
    AX_SAFE_RELEASE(m_pTextureAtlas);
    m_pTextureAtlas = var;
}

TextureAtlas * AtlasNode::getTextureAtlas()
{
    return m_pTextureAtlas;
}

unsigned int AtlasNode::getQuadsToDraw()
{
    return m_uQuadsToDraw;
}

void AtlasNode::setQuadsToDraw(unsigned int uQuadsToDraw)
{
    m_uQuadsToDraw = uQuadsToDraw;
}

NS_AX_END
