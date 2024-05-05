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
#include "LabelAtlas.h"
#include "textures/TextureAtlas.h"
#include "textures/TextureCache.h"
#include "support/PointExtension.h"
#include "draw_nodes/DrawingPrimitives.h"
#include "ccConfig.h"
#include "shaders/ShaderCache.h"
#include "shaders/GLProgram.h"
#include "shaders/ccGLStateCache.h"
#include "base/Director.h"
#include "support/TransformUtils.h"
#include "base/Integer.h"
#include "platform/FileUtils.h"
// external
#include "kazmath/GL/matrix.h"

NS_AX_BEGIN

//LabelAtlas - Creation & Init

LabelAtlas* LabelAtlas::create(const char *string, const char *charMapFile, unsigned int itemWidth, int unsigned itemHeight, unsigned int startCharMap)
{
    LabelAtlas *pRet = new LabelAtlas();
    if(pRet && pRet->initWithString(string, charMapFile, itemWidth, itemHeight, startCharMap))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool LabelAtlas::initWithString(const char *string, const char *charMapFile, unsigned int itemWidth, unsigned int itemHeight, unsigned int startCharMap)
{
    Texture2D *texture = TextureCache::sharedTextureCache()->addImage(charMapFile);
	return initWithString(string, texture, itemWidth, itemHeight, startCharMap);
}

bool LabelAtlas::initWithString(const char *string, Texture2D* texture, unsigned int itemWidth, unsigned int itemHeight, unsigned int startCharMap)
{
    AXAssert(string != NULL, "");
    if (AtlasNode::initWithTexture(texture, itemWidth, itemHeight, strlen(string)))
    {
        m_uMapStartChar = startCharMap;
        this->setString(string);
        return true;
    }
    return false;
}

LabelAtlas* LabelAtlas::create(const char *string, const char *fntFile)
{    
    LabelAtlas *ret = new LabelAtlas();
    if (ret)
    {
        if (ret->initWithString(string, fntFile))
        {
            ret->autorelease();
        }
        else 
        {
            AX_SAFE_RELEASE_NULL(ret);
        }
    }
    
    return ret;
}

bool LabelAtlas::initWithString(const char *theString, const char *fntFile)
{
  std::string pathStr = FileUtils::sharedFileUtils()->fullPathForFilename(fntFile);
  std::string relPathStr = pathStr.substr(0, pathStr.find_last_of("/"))+"/";
  Dictionary *dict = Dictionary::createWithContentsOfFile(pathStr.c_str());
  
  AXAssert(((String*)dict->objectForKey("version"))->intValue() == 1, "Unsupported version. Upgrade cocos2d version");
    
  std::string texturePathStr = relPathStr + ((String*)dict->objectForKey("textureFilename"))->getCString();
  String *textureFilename = String::create(texturePathStr);
  unsigned int width = ((String*)dict->objectForKey("itemWidth"))->intValue() / AX_CONTENT_SCALE_FACTOR();
  unsigned int height = ((String*)dict->objectForKey("itemHeight"))->intValue() / AX_CONTENT_SCALE_FACTOR();
  unsigned int startChar = ((String*)dict->objectForKey("firstChar"))->intValue();
  

  this->initWithString(theString, textureFilename->getCString(), width, height, startChar);
    
  return true;
}

//LabelAtlas - Atlas generation
void LabelAtlas::updateAtlasValues()
{
    unsigned int n = m_sString.length();

    const unsigned char *s = (unsigned char*)m_sString.c_str();

    Texture2D *texture = m_pTextureAtlas->getTexture();
    float textureWide = (float) texture->getPixelsWide();
    float textureHigh = (float) texture->getPixelsHigh();
    float itemWidthInPixels = m_uItemWidth * AX_CONTENT_SCALE_FACTOR();
    float itemHeightInPixels = m_uItemHeight * AX_CONTENT_SCALE_FACTOR();
    if (m_bIgnoreContentScaleFactor)
    {
        itemWidthInPixels = m_uItemWidth;
        itemHeightInPixels = m_uItemHeight;
    }

    AXAssert( n <= m_pTextureAtlas->getCapacity(), "updateAtlasValues: Invalid String length");
    ccV3F_C4B_T2F_Quad* quads = m_pTextureAtlas->getQuads();
    for(unsigned int i = 0; i < n; i++) {

        unsigned char a = s[i] - m_uMapStartChar;
        float row = (float) (a % m_uItemsPerRow);
        float col = (float) (a / m_uItemsPerRow);

#if AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL
        // Issue #938. Don't use texStepX & texStepY
        float left        = (2 * row * itemWidthInPixels + 1) / (2 * textureWide);
        float right        = left + (itemWidthInPixels * 2 - 2) / (2 * textureWide);
        float top        = (2 * col * itemHeightInPixels + 1) / (2 * textureHigh);
        float bottom    = top + (itemHeightInPixels * 2 - 2) / (2 * textureHigh);
#else
        float left        = row * itemWidthInPixels / textureWide;
        float right        = left + itemWidthInPixels / textureWide;
        float top        = col * itemHeightInPixels / textureHigh;
        float bottom    = top + itemHeightInPixels / textureHigh;
#endif // ! AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL

        quads[i].tl.texCoords.u = left;
        quads[i].tl.texCoords.v = top;
        quads[i].tr.texCoords.u = right;
        quads[i].tr.texCoords.v = top;
        quads[i].bl.texCoords.u = left;
        quads[i].bl.texCoords.v = bottom;
        quads[i].br.texCoords.u = right;
        quads[i].br.texCoords.v = bottom;

        quads[i].bl.vertices.x = (float) (i * m_uItemWidth);
        quads[i].bl.vertices.y = 0;
        quads[i].bl.vertices.z = 0.0f;
        quads[i].br.vertices.x = (float)(i * m_uItemWidth + m_uItemWidth);
        quads[i].br.vertices.y = 0;
        quads[i].br.vertices.z = 0.0f;
        quads[i].tl.vertices.x = (float)(i * m_uItemWidth);
        quads[i].tl.vertices.y = (float)(m_uItemHeight);
        quads[i].tl.vertices.z = 0.0f;
        quads[i].tr.vertices.x = (float)(i * m_uItemWidth + m_uItemWidth);
        quads[i].tr.vertices.y = (float)(m_uItemHeight);
        quads[i].tr.vertices.z = 0.0f;
        ccColor4B c = { _displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity };
        quads[i].tl.colors = c;
        quads[i].tr.colors = c;
        quads[i].bl.colors = c;
        quads[i].br.colors = c;
    }
    if (n > 0 ){
        m_pTextureAtlas->setDirty(true);
        unsigned int totalQuads = m_pTextureAtlas->getTotalQuads();
        if (n > totalQuads) {
            m_pTextureAtlas->increaseTotalQuadsWith(n - totalQuads);
        }
    }
}

//LabelAtlas - LabelProtocol
void LabelAtlas::setString(const std::string& label)
{
    unsigned int len = label.length();
    if (len > m_pTextureAtlas->getTotalQuads())
    {
        m_pTextureAtlas->resizeCapacity(len);
    }
    m_sString.clear();
    m_sString = label;
    this->updateAtlasValues();

    Size s = Size(len * m_uItemWidth, m_uItemHeight);

    this->setContentSize(s);

    m_uQuadsToDraw = len;
}

const std::string& LabelAtlas::getString(void)
{
    return m_sString;
}

//LabelAtlas - draw

#if AX_LABELATLAS_DEBUG_DRAW    
void LabelAtlas::draw()
{
    AtlasNode::draw();

    const Size& s = this->getContentSize();
    Vec2 vertices[4]={
        Vec2(0,0),Vec2(s.width,0),
        Vec2(s.width,s.height),Vec2(0,s.height),
    };
    ccDrawPoly(vertices, 4, true);
}
#endif

NS_AX_END
