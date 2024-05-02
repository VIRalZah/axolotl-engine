/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2009      Leonardo Kasperavičius
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

#include "GL.h"
#include "ParticleSystemQuad.h"
#include "sprite_nodes/SpriteFrame.h"
#include "base/Director.h"
#include "ParticleBatchNode.h"
#include "textures/TextureAtlas.h"
#include "shaders/ShaderCache.h"
#include "shaders/ccGLStateCache.h"
#include "shaders/GLProgram.h"
#include "support/TransformUtils.h"
#include "support/NotificationCenter.h"
#include "EventType.h"

// extern
#include "kazmath/GL/matrix.h"

NS_AX_BEGIN

//implementation ParticleSystemQuad
// overriding the init method
bool ParticleSystemQuad::initWithTotalParticles(unsigned int numberOfParticles)
{
    // base initialization
    if( ParticleSystem::initWithTotalParticles(numberOfParticles) ) 
    {
        // allocating data space
        if( ! this->allocMemory() ) {
            this->release();
            return false;
        }

        initIndices();
#if AX_TEXTURE_ATLAS_USE_VAO
        setupVBOandVAO();
#else
        setupVBO();
#endif

        setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(kAXShader_PositionTextureColor));
        

#if AX_ENABLE_CACHE_TEXTURE_DATA
        // Need to listen the event only when not use batchnode, because it will use VBO
        NotificationCenter::sharedNotificationCenter()->addObserver(this,
                                                                      callfuncO_selector(ParticleSystemQuad::listenBackToForeground),
                                                                      EVENT_COME_TO_FOREGROUND,
                                                                      NULL);
#endif

        return true;
    }
    return false;
}

ParticleSystemQuad::ParticleSystemQuad()
:m_pQuads(NULL)
,m_pIndices(NULL)
#if AX_TEXTURE_ATLAS_USE_VAO
,m_uVAOname(0)
#endif
{
    memset(m_pBuffersVBO, 0, sizeof(m_pBuffersVBO));
}

ParticleSystemQuad::~ParticleSystemQuad()
{
    if (NULL == m_pBatchNode)
    {
        AX_SAFE_FREE(m_pQuads);
        AX_SAFE_FREE(m_pIndices);
        glDeleteBuffers(2, &m_pBuffersVBO[0]);
#if AX_TEXTURE_ATLAS_USE_VAO
        glDeleteVertexArrays(1, &m_uVAOname);
        ccGLBindVAO(0);
#endif
    }

#if AX_ENABLE_CACHE_TEXTURE_DATA
    NotificationCenter::sharedNotificationCenter()->removeObserver(this, EVENT_COME_TO_FOREGROUND);
#endif
}

// implementation ParticleSystemQuad

ParticleSystemQuad * ParticleSystemQuad::create(const char *plistFile)
{
    ParticleSystemQuad *pRet = new ParticleSystemQuad();
    if (pRet && pRet->initWithFile(plistFile))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return pRet;
}

ParticleSystemQuad * ParticleSystemQuad::createWithTotalParticles(unsigned int numberOfParticles) {
    ParticleSystemQuad *pRet = new ParticleSystemQuad();
    if (pRet && pRet->initWithTotalParticles(numberOfParticles))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return pRet;
}


// pointRect should be in Texture coordinates, not pixel coordinates
void ParticleSystemQuad::initTexCoordsWithRect(const Rect& pointRect)
{
    // convert to Tex coords

    Rect rect = CCRectMake(
        pointRect.origin.x * AX_CONTENT_SCALE_FACTOR(),
        pointRect.origin.y * AX_CONTENT_SCALE_FACTOR(),
        pointRect.size.width * AX_CONTENT_SCALE_FACTOR(),
        pointRect.size.height * AX_CONTENT_SCALE_FACTOR());

    GLfloat wide = (GLfloat) pointRect.size.width;
    GLfloat high = (GLfloat) pointRect.size.height;

    if (m_pTexture)
    {
        wide = (GLfloat)m_pTexture->getPixelsWide();
        high = (GLfloat)m_pTexture->getPixelsHigh();
    }

#if AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL
    GLfloat left = (rect.origin.x*2+1) / (wide*2);
    GLfloat bottom = (rect.origin.y*2+1) / (high*2);
    GLfloat right = left + (rect.size.width*2-2) / (wide*2);
    GLfloat top = bottom + (rect.size.height*2-2) / (high*2);
#else
    GLfloat left = rect.origin.x / wide;
    GLfloat bottom = rect.origin.y / high;
    GLfloat right = left + rect.size.width / wide;
    GLfloat top = bottom + rect.size.height / high;
#endif // ! AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL

    // Important. Texture in cocos2d are inverted, so the Y component should be inverted
    AX_SWAP( top, bottom, float);

    ccV3F_C4B_T2F_Quad *quads = NULL;
    unsigned int start = 0, end = 0;
    if (m_pBatchNode)
    {
        quads = m_pBatchNode->getTextureAtlas()->getQuads();
        start = m_uAtlasIndex;
        end = m_uAtlasIndex + m_uTotalParticles;
    }
    else
    {
        quads = m_pQuads;
        start = 0;
        end = m_uTotalParticles;
    }

    for(unsigned int i=start; i<end; i++) 
    {
        // bottom-left vertex:
        quads[i].bl.texCoords.u = left;
        quads[i].bl.texCoords.v = bottom;
        // bottom-right vertex:
        quads[i].br.texCoords.u = right;
        quads[i].br.texCoords.v = bottom;
        // top-left vertex:
        quads[i].tl.texCoords.u = left;
        quads[i].tl.texCoords.v = top;
        // top-right vertex:
        quads[i].tr.texCoords.u = right;
        quads[i].tr.texCoords.v = top;
    }
}
void ParticleSystemQuad::setTextureWithRect(Texture2D *texture, const Rect& rect)
{
    // Only update the texture if is different from the current one
    if( !m_pTexture || texture->getName() != m_pTexture->getName() )
    {
        ParticleSystem::setTexture(texture);
    }

    this->initTexCoordsWithRect(rect);
}
void ParticleSystemQuad::setTexture(Texture2D* texture)
{
    const Size& s = texture->getContentSize();
    this->setTextureWithRect(texture, CCRectMake(0, 0, s.width, s.height));
}
void ParticleSystemQuad::setDisplayFrame(SpriteFrame *spriteFrame)
{
    AXAssert(spriteFrame->getOffsetInPixels().equals(Point::ZERO), 
             "QuadParticle only supports SpriteFrames with no offsets");

    // update texture before updating texture rect
    if ( !m_pTexture || spriteFrame->getTexture()->getName() != m_pTexture->getName())
    {
        this->setTexture(spriteFrame->getTexture());
    }
}

void ParticleSystemQuad::initIndices()
{
    for(unsigned int i = 0; i < m_uTotalParticles; ++i)
    {
        const unsigned int i6 = i*6;
        const unsigned int i4 = i*4;
        m_pIndices[i6+0] = (GLushort) i4+0;
        m_pIndices[i6+1] = (GLushort) i4+1;
        m_pIndices[i6+2] = (GLushort) i4+2;

        m_pIndices[i6+5] = (GLushort) i4+1;
        m_pIndices[i6+4] = (GLushort) i4+2;
        m_pIndices[i6+3] = (GLushort) i4+3;
    }
}

void ParticleSystemQuad::updateQuadWithParticle(tCCParticle* particle, const Point& newPosition)
{
    ccV3F_C4B_T2F_Quad *quad;

    if (m_pBatchNode)
    {
        ccV3F_C4B_T2F_Quad *batchQuads = m_pBatchNode->getTextureAtlas()->getQuads();
        quad = &(batchQuads[m_uAtlasIndex+particle->atlasIndex]);
    }
    else
    {
        quad = &(m_pQuads[m_uParticleIdx]);
    }
    ccColor4B color = (m_bOpacityModifyRGB)
        ? ccc4( particle->color.r*particle->color.a*255, particle->color.g*particle->color.a*255, particle->color.b*particle->color.a*255, particle->color.a*255)
        : ccc4( particle->color.r*255, particle->color.g*255, particle->color.b*255, particle->color.a*255);

    quad->bl.colors = color;
    quad->br.colors = color;
    quad->tl.colors = color;
    quad->tr.colors = color;

    // vertices
    GLfloat size_2 = particle->size/2;
    if (particle->rotation) 
    {
        GLfloat x1 = -size_2;
        GLfloat y1 = -size_2;

        GLfloat x2 = size_2;
        GLfloat y2 = size_2;
        GLfloat x = newPosition.x;
        GLfloat y = newPosition.y;

        GLfloat r = (GLfloat)-AX_DEGREES_TO_RADIANS(particle->rotation);
        GLfloat cr = cosf(r);
        GLfloat sr = sinf(r);
        GLfloat ax = x1 * cr - y1 * sr + x;
        GLfloat ay = x1 * sr + y1 * cr + y;
        GLfloat bx = x2 * cr - y1 * sr + x;
        GLfloat by = x2 * sr + y1 * cr + y;
        GLfloat cx = x2 * cr - y2 * sr + x;
        GLfloat cy = x2 * sr + y2 * cr + y;
        GLfloat dx = x1 * cr - y2 * sr + x;
        GLfloat dy = x1 * sr + y2 * cr + y;

        // bottom-left
        quad->bl.vertices.x = ax;
        quad->bl.vertices.y = ay;

        // bottom-right vertex:
        quad->br.vertices.x = bx;
        quad->br.vertices.y = by;

        // top-left vertex:
        quad->tl.vertices.x = dx;
        quad->tl.vertices.y = dy;

        // top-right vertex:
        quad->tr.vertices.x = cx;
        quad->tr.vertices.y = cy;
    } 
    else 
    {
        // bottom-left vertex:
        quad->bl.vertices.x = newPosition.x - size_2;
        quad->bl.vertices.y = newPosition.y - size_2;

        // bottom-right vertex:
        quad->br.vertices.x = newPosition.x + size_2;
        quad->br.vertices.y = newPosition.y - size_2;

        // top-left vertex:
        quad->tl.vertices.x = newPosition.x - size_2;
        quad->tl.vertices.y = newPosition.y + size_2;

        // top-right vertex:
        quad->tr.vertices.x = newPosition.x + size_2;
        quad->tr.vertices.y = newPosition.y + size_2;                
    }
}
void ParticleSystemQuad::postStep()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
	
	// Option 1: Sub Data
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_pQuads[0])*m_uTotalParticles, m_pQuads);
	
	// Option 2: Data
    //	glBufferData(GL_ARRAY_BUFFER, sizeof(quads_[0]) * particleCount, quads_, GL_DYNAMIC_DRAW);
	
	// Option 3: Orphaning + glMapBuffer
	// glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0])*m_uTotalParticles, NULL, GL_STREAM_DRAW);
	// void *buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	// memcpy(buf, m_pQuads, sizeof(m_pQuads[0])*m_uTotalParticles);
	// glUnmapBuffer(GL_ARRAY_BUFFER);
    
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    
	CHECK_GL_ERROR_DEBUG();
}

// overriding draw method
void ParticleSystemQuad::draw()
{    
	if (NULL == m_pTexture)
	{
		return;
	}
	if(m_uParticleCount == 0)
	{
		return;
	}
    AXAssert(!m_pBatchNode, "draw should not be called when added to a particleBatchNode");
	AXAssert( m_uParticleIdx == m_uParticleCount, "Abnormal error in particle quad");

    AX_NODE_DRAW_SETUP();

    ccGLBindTexture2D( m_pTexture->getName() );
    ccGLBlendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );

#if AX_TEXTURE_ATLAS_USE_VAO
    //
    // Using VBO and VAO
    //
    ccGLBindVAO(m_uVAOname);

#if AX_REBIND_INDICES_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
#endif

    glDrawElements(GL_TRIANGLES, (GLsizei) m_uParticleIdx*6, GL_UNSIGNED_SHORT, 0);

#if AX_REBIND_INDICES_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif

#else
    //
    // Using VBO without VAO
    //

    #define kQuadSize sizeof(m_pQuads[0].bl)

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );

    glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
    // vertices
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( ccV3F_C4B_T2F, vertices));
    // colors
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( ccV3F_C4B_T2F, colors));
    // tex coords
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( ccV3F_C4B_T2F, texCoords));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);

    glDrawElements(GL_TRIANGLES, (GLsizei) m_uParticleIdx*6, GL_UNSIGNED_SHORT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#endif

    AX_INCREMENT_GL_DRAWS(1);
    CHECK_GL_ERROR_DEBUG();
}

void ParticleSystemQuad::setTotalParticles(unsigned int tp)
{
    // If we are setting the total number of particles to a number higher
    // than what is allocated, we need to allocate new arrays
    if( tp > m_uAllocatedParticles )
    {
        // Allocate new memory
        size_t particlesSize = tp * sizeof(tCCParticle);
        size_t quadsSize = sizeof(m_pQuads[0]) * tp * 1;
        size_t indicesSize = sizeof(m_pIndices[0]) * tp * 6 * 1;

        tCCParticle* particlesNew = (tCCParticle*)realloc(m_pParticles, particlesSize);
        ccV3F_C4B_T2F_Quad* quadsNew = (ccV3F_C4B_T2F_Quad*)realloc(m_pQuads, quadsSize);
        GLushort* indicesNew = (GLushort*)realloc(m_pIndices, indicesSize);

        if (particlesNew && quadsNew && indicesNew)
        {
            // Assign pointers
            m_pParticles = particlesNew;
            m_pQuads = quadsNew;
            m_pIndices = indicesNew;

            // Clear the memory
            // XXX: Bug? If the quads are cleared, then drawing doesn't work... WHY??? XXX
            memset(m_pParticles, 0, particlesSize);
            memset(m_pQuads, 0, quadsSize);
            memset(m_pIndices, 0, indicesSize);

            m_uAllocatedParticles = tp;
        }
        else
        {
            // Out of memory, failed to resize some array
            if (particlesNew) m_pParticles = particlesNew;
            if (quadsNew) m_pQuads = quadsNew;
            if (indicesNew) m_pIndices = indicesNew;

            AXLOG("Particle system: out of memory");
            return;
        }

        m_uTotalParticles = tp;

        // Init particles
        if (m_pBatchNode)
        {
            for (unsigned int i = 0; i < m_uTotalParticles; i++)
            {
                m_pParticles[i].atlasIndex=i;
            }
        }

        initIndices();
#if AX_TEXTURE_ATLAS_USE_VAO
        setupVBOandVAO();
#else
        setupVBO();
#endif
    }
    else
    {
        m_uTotalParticles = tp;
    }
    
    resetSystem();
}

#if AX_TEXTURE_ATLAS_USE_VAO
void ParticleSystemQuad::setupVBOandVAO()
{
    // clean VAO
    glDeleteBuffers(2, &m_pBuffersVBO[0]);
    glDeleteVertexArrays(1, &m_uVAOname);
    ccGLBindVAO(0);

    glGenVertexArrays(1, &m_uVAOname);
    ccGLBindVAO(m_uVAOname);

#define kQuadSize sizeof(m_pQuads[0].bl)

    glGenBuffers(2, &m_pBuffersVBO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0]) * m_uTotalParticles, m_pQuads, GL_DYNAMIC_DRAW);

    // vertices
    glEnableVertexAttribArray(kCCVertexAttrib_Position);
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( ccV3F_C4B_T2F, vertices));

    // colors
    glEnableVertexAttribArray(kCCVertexAttrib_Color);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( ccV3F_C4B_T2F, colors));

    // tex coords
    glEnableVertexAttribArray(kCCVertexAttrib_TexCoords);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( ccV3F_C4B_T2F, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_pIndices[0]) * m_uTotalParticles * 6, m_pIndices, GL_STATIC_DRAW);

    // Must unbind the VAO before changing the element buffer.
    ccGLBindVAO(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERROR_DEBUG();
}
#else

void ParticleSystemQuad::setupVBO()
{
    glDeleteBuffers(2, &m_pBuffersVBO[0]);
    
    glGenBuffers(2, &m_pBuffersVBO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0]) * m_uTotalParticles, m_pQuads, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_pIndices[0]) * m_uTotalParticles * 6, m_pIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERROR_DEBUG();
}

#endif

void ParticleSystemQuad::listenBackToForeground(Object *obj)
{
#if AX_TEXTURE_ATLAS_USE_VAO
        setupVBOandVAO();
#else
        setupVBO();
#endif
}

bool ParticleSystemQuad::allocMemory()
{
    AXAssert( ( !m_pQuads && !m_pIndices), "Memory already alloced");
    AXAssert( !m_pBatchNode, "Memory should not be alloced when not using batchNode");

    AX_SAFE_FREE(m_pQuads);
    AX_SAFE_FREE(m_pIndices);

    m_pQuads = (ccV3F_C4B_T2F_Quad*)malloc(m_uTotalParticles * sizeof(ccV3F_C4B_T2F_Quad));
    m_pIndices = (GLushort*)malloc(m_uTotalParticles * 6 * sizeof(GLushort));
    
    if( !m_pQuads || !m_pIndices) 
    {
        AXLOG("cocos2d: Particle system: not enough memory");
        AX_SAFE_FREE(m_pQuads);
        AX_SAFE_FREE(m_pIndices);

        return false;
    }

    memset(m_pQuads, 0, m_uTotalParticles * sizeof(ccV3F_C4B_T2F_Quad));
    memset(m_pIndices, 0, m_uTotalParticles * 6 * sizeof(GLushort));

    return true;
}

void ParticleSystemQuad::setBatchNode(ParticleBatchNode * batchNode)
{
    if( m_pBatchNode != batchNode ) 
    {
        ParticleBatchNode* oldBatch = m_pBatchNode;

        ParticleSystem::setBatchNode(batchNode);

        // NEW: is self render ?
        if( ! batchNode ) 
        {
            allocMemory();
            initIndices();
            setTexture(oldBatch->getTexture());
#if AX_TEXTURE_ATLAS_USE_VAO
            setupVBOandVAO();
#else
            setupVBO();
#endif
        }
        // OLD: was it self render ? cleanup
        else if( !oldBatch )
        {
            // copy current state to batch
            ccV3F_C4B_T2F_Quad *batchQuads = m_pBatchNode->getTextureAtlas()->getQuads();
            ccV3F_C4B_T2F_Quad *quad = &(batchQuads[m_uAtlasIndex] );
            memcpy( quad, m_pQuads, m_uTotalParticles * sizeof(m_pQuads[0]) );

            AX_SAFE_FREE(m_pQuads);
            AX_SAFE_FREE(m_pIndices);

            glDeleteBuffers(2, &m_pBuffersVBO[0]);
            memset(m_pBuffersVBO, 0, sizeof(m_pBuffersVBO));
#if AX_TEXTURE_ATLAS_USE_VAO
            glDeleteVertexArrays(1, &m_uVAOname);
            ccGLBindVAO(0);
            m_uVAOname = 0;
#endif
        }
    }
}

ParticleSystemQuad * ParticleSystemQuad::create() {
    ParticleSystemQuad *pParticleSystemQuad = new ParticleSystemQuad();
    if (pParticleSystemQuad && pParticleSystemQuad->init())
    {
        pParticleSystemQuad->autorelease();
        return pParticleSystemQuad;
    }
    AX_SAFE_DELETE(pParticleSystemQuad);
    return NULL;
}

NS_AX_END
