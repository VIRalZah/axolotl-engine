/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2009      Jason Booth

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

#include "base/Configuration.h"
#include "misc_nodes/RenderTexture.h"
#include "base/Director.h"
#include "platform/platform.h"
#include "platform/Image.h"
#include "shaders/GLProgram.h"
#include "shaders/ccGLStateCache.h"
#include "base/Configuration.h"
#include "support/ccUtils.h"
#include "textures/TextureCache.h"
#include "platform/FileUtils.h"
#include "GL.h"
#include "support/NotificationCenter.h"
#include "EventType.h"
#include "effects/Grid.h"
// extern
#include "kazmath/GL/matrix.h"
#include "GLViewImpl.h"

NS_AX_BEGIN

// implementation RenderTexture
RenderTexture::RenderTexture()
: m_pSprite(NULL)
, m_uFBO(0)
, m_uDepthRenderBufffer(0)
, m_nOldFBO(0)
, m_pTexture(0)
, m_pTextureCopy(0)
, m_pUITextureImage(NULL)
, m_ePixelFormat(kCCTexture2DPixelFormat_RGBA8888)
, m_uClearFlags(0)
, m_sClearColor(ccc4f(0,0,0,0))
, m_fClearDepth(0.0f)
, m_nClearStencil(0)
, m_bAutoDraw(false)
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    // Listen this event to save render texture before come to background.
    // Then it can be restored after coming to foreground on Android.
    NotificationCenter::sharedNotificationCenter()->addObserver(this,
                                                                  callfuncO_selector(RenderTexture::listenToBackground),
                                                                  EVENT_COME_TO_BACKGROUND,
                                                                  NULL);
    
    NotificationCenter::sharedNotificationCenter()->addObserver(this,
                                                                  callfuncO_selector(RenderTexture::listenToForeground),
                                                                  EVENT_COME_TO_FOREGROUND, // this is misspelt
                                                                  NULL);
#endif
}

RenderTexture::~RenderTexture()
{
    AX_SAFE_RELEASE(m_pSprite);
    AX_SAFE_RELEASE(m_pTextureCopy);
    
    glDeleteFramebuffers(1, &m_uFBO);
    if (m_uDepthRenderBufffer)
    {
        glDeleteRenderbuffers(1, &m_uDepthRenderBufffer);
    }
    AX_SAFE_DELETE(m_pUITextureImage);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    NotificationCenter::sharedNotificationCenter()->removeObserver(this, EVENT_COME_TO_BACKGROUND);
    NotificationCenter::sharedNotificationCenter()->removeObserver(this, EVENT_COME_TO_FOREGROUND);
#endif
}

void RenderTexture::listenToBackground(axolotl::Object *obj)
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    AX_SAFE_DELETE(m_pUITextureImage);
    
    // to get the rendered texture data
    m_pUITextureImage = newCCImage(false);

    if (m_pUITextureImage)
    {
        const Size& s = m_pTexture->getContentSizeInPixels();
        VolatileTexture::addDataTexture(m_pTexture, m_pUITextureImage->getData(), kTexture2DPixelFormat_RGBA8888, s);
        
        if ( m_pTextureCopy )
        {
            VolatileTexture::addDataTexture(m_pTextureCopy, m_pUITextureImage->getData(), kTexture2DPixelFormat_RGBA8888, s);
        }
    }
    else
    {
        AXLOG("Cache rendertexture failed!");
    }
    
    glDeleteFramebuffers(1, &m_uFBO);
    m_uFBO = 0;
#endif
}

void RenderTexture::listenToForeground(axolotl::Object *obj)
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    // -- regenerate frame buffer object and attach the texture
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_nOldFBO);
    
    glGenFramebuffers(1, &m_uFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_uFBO);
    
    m_pTexture->setAliasTexParameters();
    
    if ( m_pTextureCopy )
    {
        m_pTextureCopy->setAliasTexParameters();
    }
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTexture->getName(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nOldFBO);
#endif
}

Sprite * RenderTexture::getSprite()
{
    return m_pSprite;
}

void RenderTexture::setSprite(Sprite* var)
{
    AX_SAFE_RELEASE(m_pSprite);
    m_pSprite = var;
    AX_SAFE_RETAIN(m_pSprite);
}

unsigned int RenderTexture::getClearFlags() const
{
    return m_uClearFlags;
}

void RenderTexture::setClearFlags(unsigned int uClearFlags)
{
    m_uClearFlags = uClearFlags;
}

const ccColor4F& RenderTexture::getClearColor() const
{
    return m_sClearColor;
}

void RenderTexture::setClearColor(const ccColor4F &clearColor)
{
    m_sClearColor = clearColor;
}

float RenderTexture::getClearDepth() const
{
    return m_fClearDepth;
}

void RenderTexture::setClearDepth(float fClearDepth)
{
    m_fClearDepth = fClearDepth;
}

int RenderTexture::getClearStencil() const
{
    return m_nClearStencil;
}

void RenderTexture::setClearStencil(float fClearStencil)
{
    m_nClearStencil = fClearStencil;
}

bool RenderTexture::isAutoDraw() const
{
    return m_bAutoDraw;
}

void RenderTexture::setAutoDraw(bool bAutoDraw)
{
    m_bAutoDraw = bAutoDraw;
}

RenderTexture * RenderTexture::create(int w, int h, CCTexture2DPixelFormat eFormat)
{
    RenderTexture *pRet = new RenderTexture();

    if(pRet && pRet->initWithWidthAndHeight(w, h, eFormat))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

RenderTexture * RenderTexture::create(int w ,int h, CCTexture2DPixelFormat eFormat, GLuint uDepthStencilFormat)
{
    RenderTexture *pRet = new RenderTexture();

    if(pRet && pRet->initWithWidthAndHeight(w, h, eFormat, uDepthStencilFormat))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

RenderTexture * RenderTexture::create(int w, int h)
{
    RenderTexture *pRet = new RenderTexture();

    if(pRet && pRet->initWithWidthAndHeight(w, h, kCCTexture2DPixelFormat_RGBA8888, 0))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool RenderTexture::initWithWidthAndHeight(int w, int h, CCTexture2DPixelFormat eFormat)
{
    return initWithWidthAndHeight(w, h, eFormat, 0);
}

bool RenderTexture::initWithWidthAndHeight(int w, int h, CCTexture2DPixelFormat eFormat, GLuint uDepthStencilFormat)
{
    AXAssert(eFormat != kCCTexture2DPixelFormat_A8, "only RGB and RGBA formats are valid for a render texture");

    bool bRet = false;
    void *data = NULL;
    do 
    {
        w = (int)(w * AX_CONTENT_SCALE_FACTOR());
        h = (int)(h * AX_CONTENT_SCALE_FACTOR());

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_nOldFBO);

        // textures must be power of two squared
        unsigned int powW = 0;
        unsigned int powH = 0;

        if (Configuration::sharedConfiguration()->supportsNPOT())
        {
            powW = w;
            powH = h;
        }
        else
        {
            powW = ccNextPOT(w);
            powH = ccNextPOT(h);
        }

        data = malloc((int)(powW * powH * 4));
        AX_BREAK_IF(! data);

        memset(data, 0, (int)(powW * powH * 4));
        m_ePixelFormat = eFormat;

        m_pTexture = new Texture2D();
        if (m_pTexture)
        {
            m_pTexture->initWithData(data, (CCTexture2DPixelFormat)m_ePixelFormat, powW, powH, Size((float)w, (float)h));
        }
        else
        {
            break;
        }
        GLint oldRBO;
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRBO);
        
        if (Configuration::sharedConfiguration()->checkForGLExtension("GL_QCOM"))
        {
            m_pTextureCopy = new Texture2D();
            if (m_pTextureCopy)
            {
                m_pTextureCopy->initWithData(data, (CCTexture2DPixelFormat)m_ePixelFormat, powW, powH, Size((float)w, (float)h));
            }
            else
            {
                break;
            }
        }

        // generate FBO
        glGenFramebuffers(1, &m_uFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_uFBO);

        // associate texture with FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTexture->getName(), 0);

        if (uDepthStencilFormat != 0)
        {
            //create and attach depth buffer
            glGenRenderbuffers(1, &m_uDepthRenderBufffer);
            glBindRenderbuffer(GL_RENDERBUFFER, m_uDepthRenderBufffer);
            glRenderbufferStorage(GL_RENDERBUFFER, uDepthStencilFormat, (GLsizei)powW, (GLsizei)powH);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_uDepthRenderBufffer);

            // if depth format is the one with stencil part, bind same render buffer as stencil attachment
            if (uDepthStencilFormat == GL_DEPTH24_STENCIL8)
            {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_uDepthRenderBufffer);
            }
        }

        // check if it worked (probably worth doing :) )
        AXAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Could not attach texture to framebuffer");

        m_pTexture->setAliasTexParameters();

        // retained
        setSprite(Sprite::createWithTexture(m_pTexture));

        m_pTexture->release();
        m_pSprite->setScaleY(-1);

        ccBlendFunc tBlendFunc = {GL_ONE, GL_ONE_MINUS_SRC_ALPHA };
        m_pSprite->setBlendFunc(tBlendFunc);

        glBindRenderbuffer(GL_RENDERBUFFER, oldRBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_nOldFBO);
        
        // Diabled by default.
        m_bAutoDraw = false;
        
        // add sprite for backward compatibility
        addChild(m_pSprite);
        
        bRet = true;
    } while (0);
    
    AX_SAFE_FREE(data);
    
    return bRet;
}

void RenderTexture::begin()
{
    kmGLMatrixMode(KM_GL_PROJECTION);
	kmGLPushMatrix();
	kmGLMatrixMode(KM_GL_MODELVIEW);
    kmGLPushMatrix();
    
    Director *director = Director::getInstance();
    director->setProjection(director->getProjection());

#if AX_TARGET_PLATFORM == AX_PLATFORM_WP8
    kmMat4 modifiedProjection;
    kmGLGetMatrix(KM_GL_PROJECTION, &modifiedProjection);
    kmMat4Multiply(&modifiedProjection, EGLView::sharedEGLView()->getReverseOrientationMatrix(), &modifiedProjection);
    kmGLMatrixMode(KM_GL_PROJECTION);
    kmGLLoadMatrix(&modifiedProjection);
    kmGLMatrixMode(KM_GL_MODELVIEW);
#endif

    const Size& texSize = m_pTexture->getContentSizeInPixels();

    // Calculate the adjustment ratios based on the old and new projections
    Size size = director->getDesignSizeInPixels();
    float widthRatio = size.width / texSize.width;
    float heightRatio = size.height / texSize.height;

    // Adjust the orthographic projection and viewport
    glViewport(0, 0, (GLsizei)texSize.width, (GLsizei)texSize.height);


    kmMat4 orthoMatrix;
    kmMat4OrthographicProjection(&orthoMatrix, (float)-1.0 / widthRatio,  (float)1.0 / widthRatio,
        (float)-1.0 / heightRatio, (float)1.0 / heightRatio, -1,1 );
    kmGLMultMatrix(&orthoMatrix);

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_nOldFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_uFBO);
    
    /*  Certain Qualcomm Andreno gpu's will retain data in memory after a frame buffer switch which corrupts the render to the texture. The solution is to clear the frame buffer before rendering to the texture. However, calling glClear has the unintended result of clearing the current texture. Create a temporary texture to overcome this. At the end of RenderTexture::begin(), switch the attached texture to the second one, call glClear, and then switch back to the original texture. This solution is unnecessary for other devices as they don't have the same issue with switching frame buffers.
     */
    if (Configuration::sharedConfiguration()->checkForGLExtension("GL_QCOM"))
    {
        // -- bind a temporary texture so we can clear the render buffer without losing our texture
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTextureCopy->getName(), 0);
        CHECK_GL_ERROR_DEBUG();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTexture->getName(), 0);
    }
}

void RenderTexture::beginWithClear(float r, float g, float b, float a)
{
    beginWithClear(r, g, b, a, 0, 0, GL_COLOR_BUFFER_BIT);
}

void RenderTexture::beginWithClear(float r, float g, float b, float a, float depthValue)
{
    beginWithClear(r, g, b, a, depthValue, 0, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void RenderTexture::beginWithClear(float r, float g, float b, float a, float depthValue, int stencilValue)
{
    beginWithClear(r, g, b, a, depthValue, stencilValue, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}

void RenderTexture::beginWithClear(float r, float g, float b, float a, float depthValue, int stencilValue, GLbitfield flags)
{
    this->begin();

    // save clear color
    GLfloat	clearColor[4] = {0.0f};
    GLfloat depthClearValue = 0.0f;
    int stencilClearValue = 0;
    
    if (flags & GL_COLOR_BUFFER_BIT)
    {
        glGetFloatv(GL_COLOR_CLEAR_VALUE,clearColor);
        glClearColor(r, g, b, a);
    }
    
    if (flags & GL_DEPTH_BUFFER_BIT)
    {
        glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depthClearValue);
        glClearDepth(depthValue);
    }

    if (flags & GL_STENCIL_BUFFER_BIT)
    {
        glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &stencilClearValue);
        glClearStencil(stencilValue);
    }
    
    glClear(flags);

    // restore
    if (flags & GL_COLOR_BUFFER_BIT)
    {
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    }
    if (flags & GL_DEPTH_BUFFER_BIT)
    {
        glClearDepth(depthClearValue);
    }
    if (flags & GL_STENCIL_BUFFER_BIT)
    {
        glClearStencil(stencilClearValue);
    }
}

void RenderTexture::end()
{
    Director *director = Director::getInstance();
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_nOldFBO);

    // restore viewport
    director->setViewport();

    kmGLMatrixMode(KM_GL_PROJECTION);
	kmGLPopMatrix();
	kmGLMatrixMode(KM_GL_MODELVIEW);
	kmGLPopMatrix();
}

void RenderTexture::clear(float r, float g, float b, float a)
{
    this->beginWithClear(r, g, b, a);
    this->end();
}

void RenderTexture::clearDepth(float depthValue)
{
    this->begin();
    //! save old depth value
    GLfloat depthClearValue;
    glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depthClearValue);

    glClearDepth(depthValue);
    glClear(GL_DEPTH_BUFFER_BIT);

    // restore clear color
    glClearDepth(depthClearValue);
    this->end();
}

void RenderTexture::clearStencil(int stencilValue)
{
    // save old stencil value
    int stencilClearValue;
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &stencilClearValue);

    glClearStencil(stencilValue);
    glClear(GL_STENCIL_BUFFER_BIT);

    // restore clear color
    glClearStencil(stencilClearValue);
}

void RenderTexture::visit()
{
    // override visit.
	// Don't call visit on its children
    if (!m_bVisible)
    {
        return;
    }
	
	kmGLPushMatrix();
	
    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->beforeDraw();
        transformAncestors();
    }
    
    transform();
    m_pSprite->visit();
    draw();
	
    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->afterDraw(this);
    }
	
	kmGLPopMatrix();

    m_uOrderOfArrival = 0;
}

void RenderTexture::draw()
{
    if( m_bAutoDraw)
    {
        begin();
		
        if (m_uClearFlags)
        {
            GLfloat oldClearColor[4] = {0.0f};
			GLfloat oldDepthClearValue = 0.0f;
			GLint oldStencilClearValue = 0;
			
			// backup and set
			if (m_uClearFlags & GL_COLOR_BUFFER_BIT)
            {
				glGetFloatv(GL_COLOR_CLEAR_VALUE, oldClearColor);
				glClearColor(m_sClearColor.r, m_sClearColor.g, m_sClearColor.b, m_sClearColor.a);
			}
			
			if (m_uClearFlags & GL_DEPTH_BUFFER_BIT)
            {
				glGetFloatv(GL_DEPTH_CLEAR_VALUE, &oldDepthClearValue);
				glClearDepth(m_fClearDepth);
			}
			
			if (m_uClearFlags & GL_STENCIL_BUFFER_BIT)
            {
				glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &oldStencilClearValue);
				glClearStencil(m_nClearStencil);
			}
			
			// clear
			glClear(m_uClearFlags);
			
			// restore
			if (m_uClearFlags & GL_COLOR_BUFFER_BIT)
            {
				glClearColor(oldClearColor[0], oldClearColor[1], oldClearColor[2], oldClearColor[3]);
            }
			if (m_uClearFlags & GL_DEPTH_BUFFER_BIT)
            {
				glClearDepth(oldDepthClearValue);
            }
			if (m_uClearFlags & GL_STENCIL_BUFFER_BIT)
            {
				glClearStencil(oldStencilClearValue);
            }
		}
		
		//! make sure all children are drawn
        sortAllChildren();
		
		Object *pElement;
		AXARRAY_FOREACH(m_pChildren, pElement)
        {
            Node *pChild = (Node*)pElement;

            if (pChild != m_pSprite)
            {
                pChild->visit();
            }
		}
        
        end();
	}
}

bool RenderTexture::saveToFile(const char *szFilePath)
{
    bool bRet = false;

    Image *pImage = newCCImage(true);
    if (pImage)
    {
        bRet = pImage->saveToFile(szFilePath, kCCImageFormatJPEG);
    }

    AX_SAFE_DELETE(pImage);
    return bRet;
}
bool RenderTexture::saveToFile(const char *fileName, tCCImageFormat format)
{
    bool bRet = false;
    AXAssert(format == kCCImageFormatJPEG || format == kCCImageFormatPNG,
             "the image can only be saved as JPG or PNG format");

    Image *pImage = newCCImage(true);
    if (pImage)
    {
        std::string fullpath = FileUtils::sharedFileUtils()->getWritablePath() + fileName;
        
        bRet = pImage->saveToFile(fullpath.c_str(), true);
    }

    AX_SAFE_DELETE(pImage);

    return bRet;
}

/* get buffer as Image */
Image* RenderTexture::newCCImage(bool flipImage)
{
    AXAssert(m_ePixelFormat == kCCTexture2DPixelFormat_RGBA8888, "only RGBA8888 can be saved as image");

    if (NULL == m_pTexture)
    {
        return NULL;
    }

    const Size& s = m_pTexture->getContentSizeInPixels();

    // to get the image size to save
    //        if the saving image domain exceeds the buffer texture domain,
    //        it should be cut
    int nSavedBufferWidth = (int)s.width;
    int nSavedBufferHeight = (int)s.height;

    GLubyte *pBuffer = NULL;
    GLubyte *pTempData = NULL;
    Image *pImage = new Image();

    do
    {
        AX_BREAK_IF(! (pBuffer = new GLubyte[nSavedBufferWidth * nSavedBufferHeight * 4]));

        if(! (pTempData = new GLubyte[nSavedBufferWidth * nSavedBufferHeight * 4]))
        {
            delete[] pBuffer;
            pBuffer = NULL;
            break;
        }

        this->begin();
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0,0,nSavedBufferWidth, nSavedBufferHeight,GL_RGBA,GL_UNSIGNED_BYTE, pTempData);
        this->end();

        if ( flipImage ) // -- flip is only required when saving image to file
        {
            // to get the actual texture data
            // #640 the image read from rendertexture is dirty
            for (int i = 0; i < nSavedBufferHeight; ++i)
            {
                memcpy(&pBuffer[i * nSavedBufferWidth * 4], 
                       &pTempData[(nSavedBufferHeight - i - 1) * nSavedBufferWidth * 4], 
                       nSavedBufferWidth * 4);
            }

            pImage->initWithImageData(pBuffer, nSavedBufferWidth * nSavedBufferHeight * 4, Image::kFmtRawData, nSavedBufferWidth, nSavedBufferHeight, 8);
        }
        else
        {
            pImage->initWithImageData(pTempData, nSavedBufferWidth * nSavedBufferHeight * 4, Image::kFmtRawData, nSavedBufferWidth, nSavedBufferHeight, 8);
        }
        
    } while (0);

    AX_SAFE_DELETE_ARRAY(pBuffer);
    AX_SAFE_DELETE_ARRAY(pTempData);

    return pImage;
}

NS_AX_END
