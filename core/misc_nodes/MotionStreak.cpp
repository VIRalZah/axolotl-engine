/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011 ForzeField Studios S.L.

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
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN false EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "MotionStreak.h"
#include "textures/TextureCache.h"
#include "shaders/ccGLStateCache.h"
#include "shaders/GLProgram.h"
#include "shaders/ShaderCache.h"
#include "ccMacros.h"

#include "support/Vertex.h"
#include "support/PointExtension.h"

NS_AX_BEGIN

MotionStreak::MotionStreak()
: m_bFastMode(false)
, m_bStartingPositionInitialized(false)
, m_pTexture(NULL)
, m_tPositionR(Vec2::ZERO)
, m_fStroke(0.0f)
, m_fFadeDelta(0.0f)
, m_fMinSeg(0.0f)
, m_uMaxPoints(0)
, m_uNuPoints(0)
, m_uPreviousNuPoints(0)
, m_pPointVertexes(NULL)
, m_pPointState(NULL)
, m_pVertices(NULL)
, m_pColorPointer(NULL)
, m_pTexCoords(NULL)
{
    m_tBlendFunc.src = GL_SRC_ALPHA;
    m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
}

MotionStreak::~MotionStreak()
{
    AX_SAFE_RELEASE(m_pTexture);
    AX_SAFE_FREE(m_pPointState);
    AX_SAFE_FREE(m_pPointVertexes);
    AX_SAFE_FREE(m_pVertices);
    AX_SAFE_FREE(m_pColorPointer);
    AX_SAFE_FREE(m_pTexCoords);
}

MotionStreak* MotionStreak::create(float fade, float minSeg, float stroke, const ccColor3B& color, const char* path)
{
    MotionStreak *pRet = new MotionStreak();
    if (pRet && pRet->initWithFade(fade, minSeg, stroke, color, path))
    {
        pRet->autorelease();
        return pRet;
    }

    AX_SAFE_DELETE(pRet);
    return NULL;
}

MotionStreak* MotionStreak::create(float fade, float minSeg, float stroke, const ccColor3B& color, Texture2D* texture)
{
    MotionStreak *pRet = new MotionStreak();
    if (pRet && pRet->initWithFade(fade, minSeg, stroke, color, texture))
    {
        pRet->autorelease();
        return pRet;
    }

    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool MotionStreak::initWithFade(float fade, float minSeg, float stroke, const ccColor3B& color, const char* path)
{
    AXAssert(path != NULL, "Invalid filename");

    Texture2D *texture = TextureCache::sharedTextureCache()->addImage(path);
    return initWithFade(fade, minSeg, stroke, color, texture);
}

bool MotionStreak::initWithFade(float fade, float minSeg, float stroke, const ccColor3B& color, Texture2D* texture)
{
    Node::setPosition(Vec2::ZERO);
    setAnchorPoint(Vec2::ZERO);
    ignoreAnchorPointForPosition(true);
    m_bStartingPositionInitialized = false;

    m_tPositionR = Vec2::ZERO;
    m_bFastMode = true;
    m_fMinSeg = (minSeg == -1.0f) ? stroke/5.0f : minSeg;
    m_fMinSeg *= m_fMinSeg;

    m_fStroke = stroke;
    m_fFadeDelta = 1.0f/fade;

    m_uMaxPoints = (int)(fade*60.0f)+2;
    m_uNuPoints = 0;
    m_pPointState = (float *)malloc(sizeof(float) * m_uMaxPoints);
    m_pPointVertexes = (Vec2*)malloc(sizeof(Vec2) * m_uMaxPoints);

    m_pVertices = (ccVertex2F*)malloc(sizeof(ccVertex2F) * m_uMaxPoints * 2);
    m_pTexCoords = (ccTex2F*)malloc(sizeof(ccTex2F) * m_uMaxPoints * 2);
    m_pColorPointer =  (GLubyte*)malloc(sizeof(GLubyte) * m_uMaxPoints * 2 * 4);

    // Set blend mode
    m_tBlendFunc.src = GL_SRC_ALPHA;
    m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

    // shader program
    setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(kAXShader_PositionTextureColor));

    setTexture(texture);
    setColor(color);
    scheduleUpdate();

    return true;
}

void MotionStreak::setPosition(const Vec2& position)
{
    m_bStartingPositionInitialized = true;
    m_tPositionR = position;
}

void MotionStreak::tintWithColor(ccColor3B colors)
{
    setColor(colors);

    // Fast assignation
    for(unsigned int i = 0; i<m_uNuPoints*2; i++) 
    {
        *((ccColor3B*) (m_pColorPointer+i*4)) = colors;
    }
}

Texture2D* MotionStreak::getTexture(void)
{
    return m_pTexture;
}

void MotionStreak::setTexture(Texture2D *texture)
{
    if (m_pTexture != texture)
    {
        AX_SAFE_RETAIN(texture);
        AX_SAFE_RELEASE(m_pTexture);
        m_pTexture = texture;
    }
}

void MotionStreak::setBlendFunc(ccBlendFunc blendFunc)
{
    m_tBlendFunc = blendFunc;
}

ccBlendFunc MotionStreak::getBlendFunc(void)
{
    return m_tBlendFunc;
}

void MotionStreak::setOpacity(GLubyte opacity)
{
    AXAssert(false, "Set opacity no supported");
}

GLubyte MotionStreak::getOpacity(void)
{
    AXAssert(false, "Opacity no supported");
    return 0;
}

void MotionStreak::setOpacityModifyRGB(bool bValue)
{
    AX_UNUSED_PARAM(bValue);
}

bool MotionStreak::isOpacityModifyRGB(void)
{
    return false;
}

void MotionStreak::update(float delta)
{
    if (!m_bStartingPositionInitialized)
    {
        return;
    }
    
    delta *= m_fFadeDelta;

    unsigned int newIdx, newIdx2, i, i2;
    unsigned int mov = 0;

    // Update current points
    for(i = 0; i<m_uNuPoints; i++)
    {
        m_pPointState[i]-=delta;

        if(m_pPointState[i] <= 0)
            mov++;
        else
        {
            newIdx = i-mov;

            if(mov>0)
            {
                // Move data
                m_pPointState[newIdx] = m_pPointState[i];

                // Move point
                m_pPointVertexes[newIdx] = m_pPointVertexes[i];

                // Move vertices
                i2 = i*2;
                newIdx2 = newIdx*2;
                m_pVertices[newIdx2] = m_pVertices[i2];
                m_pVertices[newIdx2+1] = m_pVertices[i2+1];

                // Move color
                i2 *= 4;
                newIdx2 *= 4;
                m_pColorPointer[newIdx2+0] = m_pColorPointer[i2+0];
                m_pColorPointer[newIdx2+1] = m_pColorPointer[i2+1];
                m_pColorPointer[newIdx2+2] = m_pColorPointer[i2+2];
                m_pColorPointer[newIdx2+4] = m_pColorPointer[i2+4];
                m_pColorPointer[newIdx2+5] = m_pColorPointer[i2+5];
                m_pColorPointer[newIdx2+6] = m_pColorPointer[i2+6];
            }else
                newIdx2 = newIdx*8;

            const GLubyte op = (GLubyte)(m_pPointState[newIdx] * 255.0f);
            m_pColorPointer[newIdx2+3] = op;
            m_pColorPointer[newIdx2+7] = op;
        }
    }
    m_uNuPoints-=mov;

    // Append new point
    bool appendNewPoint = true;
    if(m_uNuPoints >= m_uMaxPoints)
    {
        appendNewPoint = false;
    }

    else if(m_uNuPoints>0)
    {
        bool a1 = PointDistanceSQ(m_pPointVertexes[m_uNuPoints-1], m_tPositionR) < m_fMinSeg;
        bool a2 = (m_uNuPoints == 1) ? false : (PointDistanceSQ(m_pPointVertexes[m_uNuPoints-2], m_tPositionR) < (m_fMinSeg * 2.0f));
        if(a1 || a2)
        {
            appendNewPoint = false;
        }
    }

    if(appendNewPoint)
    {
        m_pPointVertexes[m_uNuPoints] = m_tPositionR;
        m_pPointState[m_uNuPoints] = 1.0f;

        // Color assignment
        const unsigned int offset = m_uNuPoints*8;
        *((ccColor3B*)(m_pColorPointer + offset)) = _displayedColor;
        *((ccColor3B*)(m_pColorPointer + offset+4)) = _displayedColor;

        // Opacity
        m_pColorPointer[offset+3] = 255;
        m_pColorPointer[offset+7] = 255;

        // Generate polygon
        if(m_uNuPoints > 0 && m_bFastMode )
        {
            if(m_uNuPoints > 1)
            {
                ccVertexLineToPolygon(m_pPointVertexes, m_fStroke, m_pVertices, m_uNuPoints, 1);
            }
            else
            {
                ccVertexLineToPolygon(m_pPointVertexes, m_fStroke, m_pVertices, 0, 2);
            }
        }

        m_uNuPoints ++;
    }

    if( ! m_bFastMode )
    {
        ccVertexLineToPolygon(m_pPointVertexes, m_fStroke, m_pVertices, 0, m_uNuPoints);
    }

    // Updated Tex Coords only if they are different than previous step
    if( m_uNuPoints  && m_uPreviousNuPoints != m_uNuPoints ) {
        float texDelta = 1.0f / m_uNuPoints;
        for( i=0; i < m_uNuPoints; i++ ) {
            m_pTexCoords[i*2] = tex2(0, texDelta*i);
            m_pTexCoords[i*2+1] = tex2(1, texDelta*i);
        }

        m_uPreviousNuPoints = m_uNuPoints;
    }
}

void MotionStreak::reset()
{
    m_uNuPoints = 0;
}

void MotionStreak::draw()
{
    if(m_uNuPoints <= 1)
        return;

    AX_NODE_DRAW_SETUP();

    ccGLEnableVertexAttribs(kCCVertexAttribFlag_PosColorTex );
    ccGLBlendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );

    ccGLBindTexture2D( m_pTexture->getName() );

#ifdef EMSCRIPTEN
    // Size calculations from ::initWithFade
    setGLBufferData(m_pVertices, (sizeof(ccVertex2F) * m_uMaxPoints * 2), 0);
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);

    setGLBufferData(m_pTexCoords, (sizeof(ccTex2F) * m_uMaxPoints * 2), 1);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);

    setGLBufferData(m_pColorPointer, (sizeof(GLubyte) * m_uMaxPoints * 2 * 4), 2);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
#else
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, m_pVertices);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, m_pTexCoords);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, m_pColorPointer);
#endif // EMSCRIPTEN

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)m_uNuPoints*2);

    AX_INCREMENT_GL_DRAWS(1);
}

NS_AX_END

