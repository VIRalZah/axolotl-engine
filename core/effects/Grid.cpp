/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2009      On-Core

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
#include "ccMacros.h"
#include "effects/Grid.h"
#include "base/Director.h"
#include "effects/Grabber.h"
#include "support/ccUtils.h"
#include "shaders/GLProgram.h"
#include "shaders/ShaderCache.h"
#include "shaders/ccGLStateCache.h"
#include "GL.h"
#include "support/PointExtension.h"
#include "support/TransformUtils.h"
#include "kazmath/kazmath.h"
#include "kazmath/GL/matrix.h"

NS_AX_BEGIN
// implementation of GridBase

GridBase* GridBase::create(const Size& gridSize)
{
    GridBase *pGridBase = new GridBase();

    if (pGridBase)
    {
        if (pGridBase->initWithSize(gridSize))
        {
            pGridBase->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pGridBase);
        }
    }

    return pGridBase;
}

GridBase* GridBase::create(const Size& gridSize, Texture2D *texture, bool flipped)
{
    GridBase *pGridBase = new GridBase();

    if (pGridBase)
    {
        if (pGridBase->initWithSize(gridSize, texture, flipped))
        {
            pGridBase->autorelease();
        }
        else
        {
            AX_SAFE_RELEASE_NULL(pGridBase);
        }
    }

    return pGridBase;
}

bool GridBase::initWithSize(const Size& gridSize, Texture2D *pTexture, bool bFlipped)
{
    bool bRet = true;

    m_bActive = false;
    m_nReuseGrid = 0;
    m_sGridSize = gridSize;

    m_pTexture = pTexture;
    AX_SAFE_RETAIN(m_pTexture);
    m_bIsTextureFlipped = bFlipped;

    Size texSize = m_pTexture->getContentSize();
    m_obStep.x = texSize.width / m_sGridSize.width;
    m_obStep.y = texSize.height / m_sGridSize.height;

    m_pGrabber = new CCGrabber();
    if (m_pGrabber)
    {
        m_pGrabber->grab(m_pTexture);
    }
    else
    {
        bRet = false;
    }
    
    m_pShaderProgram = ShaderCache::sharedShaderCache()->programForKey(kAXShader_PositionTexture);
    calculateVertexPoints();

    return bRet;
}

bool GridBase::initWithSize(const Size& gridSize)
{
    Director *pDirector = Director::sharedDirector();
    Size s = pDirector->getWinSizeInPixels();
    
    unsigned long POTWide = ccNextPOT((unsigned int)s.width);
    unsigned long POTHigh = ccNextPOT((unsigned int)s.height);

    // we only use rgba8888
    CCTexture2DPixelFormat format = kCCTexture2DPixelFormat_RGBA8888;

    void *data = calloc((int)(POTWide * POTHigh * 4), 1);
    if (! data)
    {
        AXLOG("cocos2d: CCGrid: not enough memory.");
        this->release();
        return false;
    }

    Texture2D *pTexture = new Texture2D();
    pTexture->initWithData(data, format, POTWide, POTHigh, s);

    free(data);

    if (! pTexture)
    {
        AXLOG("cocos2d: CCGrid: error creating texture");
        return false;
    }

    initWithSize(gridSize, pTexture, false);

    pTexture->release();

    return true;
}

GridBase::~GridBase(void)
{
    AXLOGINFO("cocos2d: deallocing %p", this);

//TODO: ? why 2.0 comments this line        setActive(false);
    AX_SAFE_RELEASE(m_pTexture);
    AX_SAFE_RELEASE(m_pGrabber);
}

// properties
void GridBase::setActive(bool bActive)
{
    m_bActive = bActive;
    if (! bActive)
    {
        Director *pDirector = Director::sharedDirector();
        Projection proj = pDirector->getProjection();
        pDirector->setProjection(proj);
    }
}

void GridBase::setTextureFlipped(bool bFlipped)
{
    if (m_bIsTextureFlipped != bFlipped)
    {
        m_bIsTextureFlipped = bFlipped;
        calculateVertexPoints();
    }
}

void GridBase::set2DProjection()
{
    Director *director = Director::sharedDirector();

    Size    size = director->getWinSizeInPixels();

    glViewport(0, 0, (GLsizei)(size.width), (GLsizei)(size.height) );
    kmGLMatrixMode(KM_GL_PROJECTION);
    kmGLLoadIdentity();

    kmMat4 orthoMatrix;
    kmMat4OrthographicProjection(&orthoMatrix, 0, size.width, 0, size.height, -1, 1);
    kmGLMultMatrix( &orthoMatrix );

    kmGLMatrixMode(KM_GL_MODELVIEW);
    kmGLLoadIdentity();


    ccSetProjectionMatrixDirty();
}

void GridBase::beforeDraw(void)
{
    // save projection
    Director *director = Director::sharedDirector();
    m_directorProjection = director->getProjection();

    // 2d projection
    //    [director setProjection:ORTHOGRAPHIC];
    set2DProjection();
    m_pGrabber->beforeRender(m_pTexture);
}

void GridBase::afterDraw(axolotl::Node *pTarget)
{
    m_pGrabber->afterRender(m_pTexture);

    // restore projection
    Director *director = Director::sharedDirector();
    director->setProjection(m_directorProjection);

    if (pTarget->getCamera()->isDirty())
    {
        Vec2 offset = pTarget->getAnchorPointInPoints();

        //
        // XXX: Camera should be applied in the AnchorPoint
        //
        kmGLTranslatef(offset.x, offset.y, 0);
        pTarget->getCamera()->locate();
        kmGLTranslatef(-offset.x, -offset.y, 0);
    }

    ccGLBindTexture2D(m_pTexture->getName());

    // restore projection for default FBO .fixed bug #543 #544
//TODO:         Director::sharedDirector()->setProjection(Director::sharedDirector()->getProjection());
//TODO:         Director::sharedDirector()->applyOrientation();
    blit();
}

void GridBase::blit(void)
{
    AXAssert(0, "");
}

void GridBase::reuse(void)
{
    AXAssert(0, "");
}

void GridBase::calculateVertexPoints(void)
{
    AXAssert(0, "");
}

// implementation of Grid3D

Grid3D* Grid3D::create(const Size& gridSize, Texture2D *pTexture, bool bFlipped)
{
    Grid3D *pRet= new Grid3D();

    if (pRet)
    {
        if (pRet->initWithSize(gridSize, pTexture, bFlipped))
        {
            pRet->autorelease();
        }
        else
        {
            delete pRet;
            pRet = NULL;
        }
    }

    return pRet;
}

Grid3D* Grid3D::create(const Size& gridSize)
{
    Grid3D *pRet= new Grid3D();

    if (pRet)
    {
        if (pRet->initWithSize(gridSize))
        {
            pRet->autorelease();
        }
        else
        {
            delete pRet;
            pRet = NULL;
        }
    }

    return pRet;
}


Grid3D::Grid3D()
    : m_pTexCoordinates(NULL)
    , m_pVertices(NULL)
    , m_pOriginalVertices(NULL)
    , m_pIndices(NULL)
{

}

Grid3D::~Grid3D(void)
{
    AX_SAFE_FREE(m_pTexCoordinates);
    AX_SAFE_FREE(m_pVertices);
    AX_SAFE_FREE(m_pIndices);
    AX_SAFE_FREE(m_pOriginalVertices);
}

void Grid3D::blit(void)
{
    int n = m_sGridSize.width * m_sGridSize.height;

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords );
    m_pShaderProgram->use();
    m_pShaderProgram->setUniformsForBuiltins();;

    //
    // Attributes
    //
#ifdef EMSCRIPTEN
    // Size calculations from calculateVertexPoints().
    unsigned int numOfPoints = (m_sGridSize.width+1) * (m_sGridSize.height+1);

    // position
    setGLBufferData(m_pVertices, numOfPoints * sizeof(ccVertex3F), 0);
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // texCoords
    setGLBufferData(m_pTexCoordinates, numOfPoints * sizeof(ccVertex2F), 1);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);

    setGLIndexData(m_pIndices, n * 12, 0);
    glDrawElements(GL_TRIANGLES, (GLsizei) n*6, GL_UNSIGNED_SHORT, 0);
#else
    // position
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, 0, m_pVertices);

    // texCoords
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, m_pTexCoordinates);

    glDrawElements(GL_TRIANGLES, (GLsizei) n*6, GL_UNSIGNED_SHORT, m_pIndices);
#endif // EMSCRIPTEN
    AX_INCREMENT_GL_DRAWS(1);
}

void Grid3D::calculateVertexPoints(void)
{
    float width = (float)m_pTexture->getPixelsWide();
    float height = (float)m_pTexture->getPixelsHigh();
    float imageH = m_pTexture->getContentSizeInPixels().height;

    int x, y, i;
    AX_SAFE_FREE(m_pVertices);
    AX_SAFE_FREE(m_pOriginalVertices);
    AX_SAFE_FREE(m_pTexCoordinates);
    AX_SAFE_FREE(m_pIndices);

    unsigned int numOfPoints = (m_sGridSize.width+1) * (m_sGridSize.height+1);

    m_pVertices = malloc(numOfPoints * sizeof(ccVertex3F));
    m_pOriginalVertices = malloc(numOfPoints * sizeof(ccVertex3F));
    m_pTexCoordinates = malloc(numOfPoints * sizeof(ccVertex2F));
    m_pIndices = (GLushort*)malloc(m_sGridSize.width * m_sGridSize.height * sizeof(GLushort) * 6);

    GLfloat *vertArray = (GLfloat*)m_pVertices;
    GLfloat *texArray = (GLfloat*)m_pTexCoordinates;
    GLushort *idxArray = m_pIndices;

    for (x = 0; x < m_sGridSize.width; ++x)
    {
        for (y = 0; y < m_sGridSize.height; ++y)
        {
            int idx = (y * m_sGridSize.width) + x;

            GLfloat x1 = x * m_obStep.x;
            GLfloat x2 = x1 + m_obStep.x;
            GLfloat y1 = y * m_obStep.y;
            GLfloat y2= y1 + m_obStep.y;

            GLushort a = (GLushort)(x * (m_sGridSize.height + 1) + y);
            GLushort b = (GLushort)((x + 1) * (m_sGridSize.height + 1) + y);
            GLushort c = (GLushort)((x + 1) * (m_sGridSize.height + 1) + (y + 1));
            GLushort d = (GLushort)(x * (m_sGridSize.height + 1) + (y + 1));

            GLushort tempidx[6] = {a, b, d, b, c, d};

            memcpy(&idxArray[6*idx], tempidx, 6*sizeof(GLushort));

            int l1[4] = {a*3, b*3, c*3, d*3};
            ccVertex3F e = {x1, y1, 0};
            ccVertex3F f = {x2, y1, 0};
            ccVertex3F g = {x2, y2, 0};
            ccVertex3F h = {x1, y2, 0};

            ccVertex3F l2[4] = {e, f, g, h};

            int tex1[4] = {a*2, b*2, c*2, d*2};
            Vec2 tex2[4] = {Vec2(x1, y1), Vec2(x2, y1), Vec2(x2, y2), Vec2(x1, y2)};

            for (i = 0; i < 4; ++i)
            {
                vertArray[l1[i]] = l2[i].x;
                vertArray[l1[i] + 1] = l2[i].y;
                vertArray[l1[i] + 2] = l2[i].z;

                texArray[tex1[i]] = tex2[i].x / width;
                if (m_bIsTextureFlipped)
                {
                    texArray[tex1[i] + 1] = (imageH - tex2[i].y) / height;
                }
                else
                {
                    texArray[tex1[i] + 1] = tex2[i].y / height;
                }
            }
        }
    }

    memcpy(m_pOriginalVertices, m_pVertices, (m_sGridSize.width+1) * (m_sGridSize.height+1) * sizeof(ccVertex3F));
}

ccVertex3F Grid3D::vertex(const Vec2& pos)
{
    AXAssert( pos.x == (unsigned int)pos.x && pos.y == (unsigned int) pos.y , "Numbers must be integers");
    
    int index = (pos.x * (m_sGridSize.height+1) + pos.y) * 3;
    float *vertArray = (float*)m_pVertices;

    ccVertex3F vert = {vertArray[index], vertArray[index+1], vertArray[index+2]};

    return vert;
}

ccVertex3F Grid3D::originalVertex(const Vec2& pos)
{
    AXAssert( pos.x == (unsigned int)pos.x && pos.y == (unsigned int) pos.y , "Numbers must be integers");
    
    int index = (pos.x * (m_sGridSize.height+1) + pos.y) * 3;
    float *vertArray = (float*)m_pOriginalVertices;

    ccVertex3F vert = {vertArray[index], vertArray[index+1], vertArray[index+2]};

    return vert;
}

void Grid3D::setVertex(const Vec2& pos, const ccVertex3F& vertex)
{
    AXAssert( pos.x == (unsigned int)pos.x && pos.y == (unsigned int) pos.y , "Numbers must be integers");
    int index = (pos.x * (m_sGridSize.height + 1) + pos.y) * 3;
    float *vertArray = (float*)m_pVertices;
    vertArray[index] = vertex.x;
    vertArray[index+1] = vertex.y;
    vertArray[index+2] = vertex.z;
}

void Grid3D::reuse(void)
{
    if (m_nReuseGrid > 0)
    {
        memcpy(m_pOriginalVertices, m_pVertices, (m_sGridSize.width+1) * (m_sGridSize.height+1) * sizeof(ccVertex3F));
        --m_nReuseGrid;
    }
}

// implementation of TiledGrid3D

TiledGrid3D::TiledGrid3D()
    : m_pTexCoordinates(NULL)
    , m_pVertices(NULL)
    , m_pOriginalVertices(NULL)
    , m_pIndices(NULL)
{

}

TiledGrid3D::~TiledGrid3D(void)
{
    AX_SAFE_FREE(m_pTexCoordinates);
    AX_SAFE_FREE(m_pVertices);
    AX_SAFE_FREE(m_pOriginalVertices);
    AX_SAFE_FREE(m_pIndices);
}

TiledGrid3D* TiledGrid3D::create(const Size& gridSize, Texture2D *pTexture, bool bFlipped)
{
    TiledGrid3D *pRet= new TiledGrid3D();

    if (pRet)
    {
        if (pRet->initWithSize(gridSize, pTexture, bFlipped))
        {
            pRet->autorelease();
        }
        else
        {
            delete pRet;
            pRet = NULL;
        }
    }

    return pRet;
}

TiledGrid3D* TiledGrid3D::create(const Size& gridSize)
{
    TiledGrid3D *pRet= new TiledGrid3D();

    if (pRet)
    {
        if (pRet->initWithSize(gridSize))
        {
            pRet->autorelease();
        }
        else
        {
            delete pRet;
            pRet = NULL;
        }
    }

    return pRet;
}

void TiledGrid3D::blit(void)
{
    int n = m_sGridSize.width * m_sGridSize.height;

    
    m_pShaderProgram->use();
    m_pShaderProgram->setUniformsForBuiltins();

    //
    // Attributes
    //
    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords );
#ifdef EMSCRIPTEN
    int numQuads = m_sGridSize.width * m_sGridSize.height;

    // position
    setGLBufferData(m_pVertices, (numQuads*4*sizeof(ccVertex3F)), 0);
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // texCoords
    setGLBufferData(m_pTexCoordinates, (numQuads*4*sizeof(ccVertex2F)), 1);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);

    setGLIndexData(m_pIndices, n * 12, 0);
    glDrawElements(GL_TRIANGLES, (GLsizei) n*6, GL_UNSIGNED_SHORT, 0);
#else
    // position
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, 0, m_pVertices);

    // texCoords
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, m_pTexCoordinates);

    glDrawElements(GL_TRIANGLES, (GLsizei)n*6, GL_UNSIGNED_SHORT, m_pIndices);
#endif // EMSCRIPTEN


    AX_INCREMENT_GL_DRAWS(1);
}

void TiledGrid3D::calculateVertexPoints(void)
{
    float width = (float)m_pTexture->getPixelsWide();
    float height = (float)m_pTexture->getPixelsHigh();
    float imageH = m_pTexture->getContentSizeInPixels().height;
    
    int numQuads = m_sGridSize.width * m_sGridSize.height;
    AX_SAFE_FREE(m_pVertices);
    AX_SAFE_FREE(m_pOriginalVertices);
    AX_SAFE_FREE(m_pTexCoordinates);
    AX_SAFE_FREE(m_pIndices);

    m_pVertices = malloc(numQuads*4*sizeof(ccVertex3F));
    m_pOriginalVertices = malloc(numQuads*4*sizeof(ccVertex3F));
    m_pTexCoordinates = malloc(numQuads*4*sizeof(ccVertex2F));
    m_pIndices = (GLushort*)malloc(numQuads*6*sizeof(GLushort));

    GLfloat *vertArray = (GLfloat*)m_pVertices;
    GLfloat *texArray = (GLfloat*)m_pTexCoordinates;
    GLushort *idxArray = m_pIndices;
    
    int x, y;
    
    for( x = 0; x < m_sGridSize.width; x++ )
    {
        for( y = 0; y < m_sGridSize.height; y++ )
        {
            float x1 = x * m_obStep.x;
            float x2 = x1 + m_obStep.x;
            float y1 = y * m_obStep.y;
            float y2 = y1 + m_obStep.y;
            
            *vertArray++ = x1;
            *vertArray++ = y1;
            *vertArray++ = 0;
            *vertArray++ = x2;
            *vertArray++ = y1;
            *vertArray++ = 0;
            *vertArray++ = x1;
            *vertArray++ = y2;
            *vertArray++ = 0;
            *vertArray++ = x2;
            *vertArray++ = y2;
            *vertArray++ = 0;
            
            float newY1 = y1;
            float newY2 = y2;
            
            if (m_bIsTextureFlipped) 
            {
                newY1 = imageH - y1;
                newY2 = imageH - y2;
            }

            *texArray++ = x1 / width;
            *texArray++ = newY1 / height;
            *texArray++ = x2 / width;
            *texArray++ = newY1 / height;
            *texArray++ = x1 / width;
            *texArray++ = newY2 / height;
            *texArray++ = x2 / width;
            *texArray++ = newY2 / height;
        }
    }
    
    for (x = 0; x < numQuads; x++)
    {
        idxArray[x*6+0] = (GLushort)(x * 4 + 0);
        idxArray[x*6+1] = (GLushort)(x * 4 + 1);
        idxArray[x*6+2] = (GLushort)(x * 4 + 2);
        
        idxArray[x*6+3] = (GLushort)(x * 4 + 1);
        idxArray[x*6+4] = (GLushort)(x * 4 + 2);
        idxArray[x*6+5] = (GLushort)(x * 4 + 3);
    }
    
    memcpy(m_pOriginalVertices, m_pVertices, numQuads * 12 * sizeof(GLfloat));
}

void TiledGrid3D::setTile(const Vec2& pos, const ccQuad3& coords)
{
    AXAssert( pos.x == (unsigned int)pos.x && pos.y == (unsigned int) pos.y , "Numbers must be integers");
    int idx = (m_sGridSize.height * pos.x + pos.y) * 4 * 3;
    float *vertArray = (float*)m_pVertices;
    memcpy(&vertArray[idx], &coords, sizeof(ccQuad3));
}

ccQuad3 TiledGrid3D::originalTile(const Vec2& pos)
{
    AXAssert( pos.x == (unsigned int)pos.x && pos.y == (unsigned int) pos.y , "Numbers must be integers");
    int idx = (m_sGridSize.height * pos.x + pos.y) * 4 * 3;
    float *vertArray = (float*)m_pOriginalVertices;

    ccQuad3 ret;
    memcpy(&ret, &vertArray[idx], sizeof(ccQuad3));

    return ret;
}

ccQuad3 TiledGrid3D::tile(const Vec2& pos)
{
    AXAssert( pos.x == (unsigned int)pos.x && pos.y == (unsigned int) pos.y , "Numbers must be integers");
    int idx = (m_sGridSize.height * pos.x + pos.y) * 4 * 3;
    float *vertArray = (float*)m_pVertices;

    ccQuad3 ret;
    memcpy(&ret, &vertArray[idx], sizeof(ccQuad3));

    return ret;
}

void TiledGrid3D::reuse(void)
{
    if (m_nReuseGrid > 0)
    {
        int numQuads = m_sGridSize.width * m_sGridSize.height;

        memcpy(m_pOriginalVertices, m_pVertices, numQuads * 12 * sizeof(GLfloat));
        --m_nReuseGrid;
    }
}

NS_AX_END
