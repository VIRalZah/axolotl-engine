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

// standard includes
#include <string>

// axolotl includes
#include "Director.h"
#include "axFPSImages.h"
#include "draw_nodes/DrawingPrimitives.h"
#include "Configuration.h"
#include "NS.h"
#include "layers_scenes_transitions_nodes/Scene.h"
#include "Array.h"
#include "Scheduler.h"
#include "ccMacros.h"
#include "support/PointExtension.h"
#include "support/NotificationCenter.h"
#include "layers_scenes_transitions_nodes/Transition.h"
#include "textures/TextureCache.h"
#include "sprite_nodes/SpriteFrameCache.h"
#include "base/AutoreleasePool.h"
#include "base/PoolManager.h"
#include "platform/platform.h"
#include "platform/FileUtils.h"
#include "Application.h"
#include "label_nodes/LabelBMFont.h"
#include "label_nodes/LabelAtlas.h"
#include "actions/ActionManager.h"
#include "base/Configuration.h"
#include "keypad_dispatcher/KeypadDispatcher.h"
#include "Accelerometer.h"
#include "sprite_nodes/AnimationCache.h"
#include "base/Touch.h"
#include "support/user_default/UserDefault.h"
#include "shaders/ccGLStateCache.h"
#include "shaders/ShaderCache.h"
#include "kazmath/kazmath.h"
#include "kazmath/GL/matrix.h"
#include "support/Profiling.h"
#include "platform/Image.h"
#include "platform/EGLViewProtocol.h"
#include "base/Configuration.h"
#include "base/EventDispatcher.h"



/**
 Position of the FPS
 
 Default: 0,0 (bottom-left corner)
 */
#ifndef AX_DIRECTOR_STATS_POSITION
#define AX_DIRECTOR_STATS_POSITION Director::sharedDirector()->getVisibleOrigin()
#endif

using namespace std;

unsigned int g_uNumberOfDraws = 0;

NS_AX_BEGIN
// XXX it should be a Director ivar. Move it there once support for multiple directors is added

// singleton stuff
static DisplayLinkDirector *s_SharedDirector = NULL;

#define kDefaultFPS        60  // 60 frames per second

Director* Director::sharedDirector(void)
{
    if (!s_SharedDirector)
    {
        s_SharedDirector = new DisplayLinkDirector();
        s_SharedDirector->init();
    }

    return s_SharedDirector;
}

Director::Director(void)
{

}

bool Director::init(void)
{
	setDefaultValues();

    // scenes
    _runningScene = NULL;
    m_pNextScene = NULL;

    m_pNotificationNode = NULL;

    m_pobScenesStack = new Array();
    m_pobScenesStack->init();

    // projection delegate if "Custom" projection is used
    m_pProjectionDelegate = NULL;

    // FPS
    m_fAccumDt = 0.0f;
    m_fFrameRate = 0.0f;
    m_pFPSLabel = NULL;
    m_pSPFLabel = NULL;
    m_pDrawsLabel = NULL;
    _totalFrames = m_uFrames = 0;
    m_pszFPS = new char[10];
    m_pLastUpdate = new struct AX_timeval();
    m_fSecondsPerFrame = 0.0f;

    // paused ?
    _paused = false;
   
    // purge ?
    m_bPurgeDirecotorInNextLoop = false;

    m_obWinSizeInPoints = Size::ZERO;    

    _openGLView = NULL;

    m_fContentScaleFactor = 1.0f;

    // scheduler
    m_pScheduler = new Scheduler();
    // action manager
    m_pActionManager = new ActionManager();
    m_pScheduler->scheduleUpdateForTarget(m_pActionManager, kAXPrioritySystem, false);

    // KeypadDispatcher
    m_pKeypadDispatcher = new KeypadDispatcher();

    // Accelerometer
    m_pAccelerometer = new Accelerometer();

    _eventDispatcher = new EventDispatcher();

    // create autorelease pool
    PoolManager::sharedPoolManager()->push();

    return true;
}
    
Director::~Director(void)
{
    AXLOG("cocos2d: deallocing Director %p", this);

    AX_SAFE_RELEASE(m_pFPSLabel);
    AX_SAFE_RELEASE(m_pSPFLabel);
    AX_SAFE_RELEASE(m_pDrawsLabel);
    
    AX_SAFE_RELEASE(_runningScene);
    AX_SAFE_RELEASE(m_pNotificationNode);
    AX_SAFE_RELEASE(m_pobScenesStack);
    AX_SAFE_RELEASE(m_pScheduler);
    AX_SAFE_RELEASE(m_pActionManager);
    AX_SAFE_RELEASE(m_pKeypadDispatcher);
    AX_SAFE_DELETE(m_pAccelerometer);
    AX_SAFE_RELEASE(_eventDispatcher);

    // pop the autorelease pool
    PoolManager::sharedPoolManager()->pop();
    PoolManager::purgePoolManager();

    // delete m_pLastUpdate
    AX_SAFE_DELETE(m_pLastUpdate);
    // delete fps string
    delete []m_pszFPS;

    s_SharedDirector = NULL;
}

void Director::setDefaultValues(void)
{
	Configuration *conf = Configuration::sharedConfiguration();

	// default FPS
	double fps = conf->getNumber("axolotl.fps", kDefaultFPS);
	m_dOldAnimationInterval = _animationInterval = 1.0 / fps;

	// Display FPS
	_displayStats = conf->getBool("axolotl.display_fps", false);

	// GL projection
	const char *projection = conf->getCString("axolotl.gl.projection", "3d");
	if( strcmp(projection, "3d") == 0 )
		_projection = PERSPECTIVE;
	else if (strcmp(projection, "2d") == 0)
		_projection = ORTHOGRAPHIC;
	else if (strcmp(projection, "custom") == 0)
		_projection = CUSTOM;
	else
		AXAssert(false, "Invalid projection value");

	// Default pixel format for PNG images with alpha
	const char *pixel_format = conf->getCString("axolotl.texture.pixel_format_for_png", "rgba8888");
	if( strcmp(pixel_format, "rgba8888") == 0 )
		Texture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
	else if( strcmp(pixel_format, "rgba4444") == 0 )
		Texture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA4444);
	else if( strcmp(pixel_format, "rgba5551") == 0 )
		Texture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGB5A1);

	// PVR v2 has alpha premultiplied ?
	bool pvr_alpha_premultipled = conf->getBool("axolotl.texture.pvrv2_has_alpha_premultiplied", false);
	Texture2D::PVRImagesHavePremultipliedAlpha(pvr_alpha_premultipled);
}

void Director::setGLDefaultValues(void)
{
    // This method SHOULD be called only after openGLView_ was initialized
    AXAssert(_openGLView, "opengl view should not be null");

    setAlphaBlending(true);
    // XXX: Fix me, should enable/disable depth test according the depth format as cocos2d-iphone did
    // [self setDepthTest: view_.depthFormat];
    setDepthTest(false);
    setProjection(_projection);

    // set other opengl default values
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// Draw the Scene
void Director::drawScene(void)
{
    // calculate "global" dt
    calculateDeltaTime();

    //tick before glClear: issue #533
    if (! _paused)
    {
        m_pScheduler->update(m_fDeltaTime);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* to avoid flickr, nextScene MUST be here: after tick and before draw.
     XXX: Which bug is this one. It seems that it can't be reproduced with v0.9 */
    if (m_pNextScene)
    {
        setNextScene();
    }

    kmGLPushMatrix();

    // draw the scene
    if (_runningScene)
    {
        _runningScene->visit();
    }

    // draw the notifications node
    if (m_pNotificationNode)
    {
        m_pNotificationNode->visit();
    }
    
    if (_displayStats)
    {
        showStats();
    }
    
    kmGLPopMatrix();

    _totalFrames++;

    // swap buffers
    if (_openGLView)
    {
        _openGLView->swapBuffers();
    }
    
    if (_displayStats)
    {
        calculateMPF();
    }
}

void Director::calculateDeltaTime(void)
{
    struct AX_timeval now;

    if (Time::gettimeofdayCocos2d(&now, NULL) != 0)
    {
        AXLOG("error in gettimeofday");
        m_fDeltaTime = 0;
        return;
    }

    // new delta time. Re-fixed issue #1277
    if (_nextDeltaTimeZero)
    {
        m_fDeltaTime = 0;
        _nextDeltaTimeZero = false;
    }
    else
    {
        m_fDeltaTime = (now.tv_sec - m_pLastUpdate->tv_sec) + (now.tv_usec - m_pLastUpdate->tv_usec) / 1000000.0f;
        m_fDeltaTime = MAX(0, m_fDeltaTime);
    }

#ifdef DEBUG
    // If we are debugging our code, prevent big delta time
    if(m_fDeltaTime > 0.2f)
    {
        m_fDeltaTime = 1 / 60.0f;
    }
#endif

    *m_pLastUpdate = now;
}
float Director::getDeltaTime()
{
	return m_fDeltaTime;
}
void Director::setOpenGLView(EGLViewProtocol* glView)
{
    AXAssert(glView, "opengl view should not be null");

    if (_openGLView != glView)
    {
		// Configuration. Gather GPU info
		Configuration *conf = Configuration::sharedConfiguration();
		conf->gatherGPUInfo();
		conf->dumpInfo();

        AX_SAFE_RELEASE(_openGLView);
        _openGLView = glView;

        if (_openGLView)
        {
            m_obWinSizeInPoints = _openGLView->getDesignResolutionSize();
            createStatsLabel();
            setGLDefaultValues();
        }
        
        CHECK_GL_ERROR_DEBUG();
    }
}

void Director::setViewport()
{
    if (_openGLView)
    {
        _openGLView->setViewPortInPoints(0, 0, m_obWinSizeInPoints.width, m_obWinSizeInPoints.height);
    }
}

void Director::setNextDeltaTimeZero(bool bNextDeltaTimeZero)
{
    _nextDeltaTimeZero = bNextDeltaTimeZero;
}

void Director::setProjection(Projection projection)
{
    _projection = projection;
    updateProjection();
}

void Director::updateProjection()
{
    Size size = m_obWinSizeInPoints;

    setViewport();

    switch (_projection)
    {
    case ORTHOGRAPHIC:
    {
        kmGLMatrixMode(KM_GL_PROJECTION);
        kmGLLoadIdentity();
#if AX_TARGET_PLATFORM == AX_PLATFORM_WP8
        kmGLMultMatrix(EGLView::sharedEGLView()->getOrientationMatrix());
#endif
        kmMat4 orthoMatrix;
        kmMat4OrthographicProjection(&orthoMatrix, 0, size.width, 0, size.height, -1024, 1024);
        kmGLMultMatrix(&orthoMatrix);
        kmGLMatrixMode(KM_GL_MODELVIEW);
        kmGLLoadIdentity();
    }
    break;

    case PERSPECTIVE:
    {
        float zEye = getZEye();

        kmMat4 matrixPerspective, matrixLookup;

        kmGLMatrixMode(KM_GL_PROJECTION);
        kmGLLoadIdentity();

#if AX_TARGET_PLATFORM == AX_PLATFORM_WP8
        kmGLMultMatrix(EGLView::sharedEGLView()->getOrientationMatrix());
#endif
        kmMat4PerspectiveProjection(&matrixPerspective, 60, (GLfloat)size.width / size.height, 0.1f, zEye * 2);

        kmGLMultMatrix(&matrixPerspective);

        kmGLMatrixMode(KM_GL_MODELVIEW);
        kmGLLoadIdentity();
        kmVec3 eye, center, up;
        kmVec3Fill(&eye, size.width / 2, size.height / 2, zEye);
        kmVec3Fill(&center, size.width / 2, size.height / 2, 0.0f);
        kmVec3Fill(&up, 0.0f, 1.0f, 0.0f);
        kmMat4LookAt(&matrixLookup, &eye, &center, &up);
        kmGLMultMatrix(&matrixLookup);
    }
    break;

    case CUSTOM:
        if (m_pProjectionDelegate)
        {
            m_pProjectionDelegate->updateProjection();
        }
        break;
    default:
        AXLOG("axolotl: Director: unrecognized projection");
        break;
    }

    ccSetProjectionMatrixDirty();
}

void Director::purgeCachedData(void)
{
    LabelBMFont::purgeCachedData();
    if (s_SharedDirector->getOpenGLView())
    {
        SpriteFrameCache::sharedSpriteFrameCache()->purgeSharedSpriteFrameCache();
        TextureCache::sharedTextureCache()->removeUnusedTextures();
#if defined(AXOLOTL_DEBUG) && (AXOLOTL_DEBUG > 0)
        TextureCache::sharedTextureCache()->dumpCachedTextureInfo();
#endif
    }
    FileUtils::sharedFileUtils()->purgeCachedEntries();
}

float Director::getZEye() const
{
    return (m_obWinSizeInPoints.height / 1.1566f);
}

void Director::setAlphaBlending(bool bOn)
{
    if (bOn)
    {
        ccGLBlendFunc(AX_BLEND_SRC, AX_BLEND_DST);
    }
    else
    {
        ccGLBlendFunc(GL_ONE, GL_ZERO);
    }

    CHECK_GL_ERROR_DEBUG();
}

void Director::reshapeProjection(const Size& newWindowSize)
{
	AX_UNUSED_PARAM(newWindowSize);
	if (_openGLView)
	{
		m_obWinSizeInPoints = Size(newWindowSize.width * m_fContentScaleFactor,
			newWindowSize.height * m_fContentScaleFactor);
		
        updateProjection();
	}
}

void Director::setDepthTest(bool on)
{
    if (on)
    {
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    CHECK_GL_ERROR_DEBUG();
}

static void
GLToClipTransform(kmMat4 *transformOut)
{
	kmMat4 projection;
	kmGLGetMatrix(KM_GL_PROJECTION, &projection);

#if AX_TARGET_PLATFORM == AX_PLATFORM_WP8
	kmMat4Multiply(&projection, EGLView::sharedEGLView()->getReverseOrientationMatrix(), &projection);
#endif

	kmMat4 modelview;
	kmGLGetMatrix(KM_GL_MODELVIEW, &modelview);
	
	kmMat4Multiply(transformOut, &projection, &modelview);
}

Vec2 Director::convertToGL(const Vec2& uiPoint)
{
    kmMat4 transform;
	GLToClipTransform(&transform);
	
	kmMat4 transformInv;
	kmMat4Inverse(&transformInv, &transform);
	
	kmScalar zClip = transform.mat[14]/transform.mat[15];
	
    Size glSize = _openGLView->getDesignResolutionSize();
	kmVec3 clipCoord = {2.0f*uiPoint.x/glSize.width - 1.0f, 1.0f - 2.0f*uiPoint.y/glSize.height, zClip};
	
	kmVec3 glCoord;
	kmVec3TransformCoord(&glCoord, &clipCoord, &transformInv);
	
	return Vec2(glCoord.x, glCoord.y);
}

Vec2 Director::convertToUI(const Vec2& glPoint)
{
    kmMat4 transform;
	GLToClipTransform(&transform);
    
	kmVec3 clipCoord;
	// Need to calculate the zero depth from the transform.
	kmVec3 glCoord = {glPoint.x, glPoint.y, 0.0};
	kmVec3TransformCoord(&clipCoord, &glCoord, &transform);
	
	Size glSize = _openGLView->getDesignResolutionSize();
	return Vec2(glSize.width*(clipCoord.x*0.5 + 0.5), glSize.height*(-clipCoord.y*0.5 + 0.5));
}

Size Director::getWinSize(void)
{
    return m_obWinSizeInPoints;
}

Size Director::getWinSizeInPixels()
{
    return Size(m_obWinSizeInPoints.width * m_fContentScaleFactor, m_obWinSizeInPoints.height * m_fContentScaleFactor);
}

Size Director::getVisibleSize()
{
    if (_openGLView)
    {
        return _openGLView->getVisibleSize();
    }
    else 
    {
        return Size::ZERO;
    }
}

Vec2 Director::getVisibleOrigin()
{
    if (_openGLView)
    {
        return _openGLView->getVisibleOrigin();
    }
    else 
    {
        return Vec2::ZERO;
    }
}

// scene management

void Director::runWithScene(Scene *pScene)
{
    AXAssert(pScene != NULL, "This command can only be used to start the Director. There is already a scene present.");
    AXAssert(_runningScene == NULL, "m_pRunningScene should be null");

    pushScene(pScene);
    startAnimation();
}

void Director::replaceScene(Scene *pScene)
{
    AXAssert(_runningScene, "Use runWithScene: instead to start the director");
    AXAssert(pScene != NULL, "the scene should not be null");

    unsigned int index = m_pobScenesStack->count();

    m_bSendCleanupToScene = true;
    m_pobScenesStack->replaceObjectAtIndex(index - 1, pScene);

    m_pNextScene = pScene;
}

void Director::pushScene(Scene *pScene)
{
    AXAssert(pScene, "the scene should not null");

    m_bSendCleanupToScene = false;

    m_pobScenesStack->addObject(pScene);
    m_pNextScene = pScene;
}

void Director::popScene(void)
{
    AXAssert(_runningScene != NULL, "running scene should not null");

    m_pobScenesStack->removeLastObject();
    unsigned int c = m_pobScenesStack->count();

    if (c == 0)
    {
        end();
    }
    else
    {
        m_bSendCleanupToScene = true;
        m_pNextScene = (Scene*)m_pobScenesStack->objectAtIndex(c - 1);
    }
}

void Director::popToRootScene(void)
{
    popToSceneStackLevel(1);
}

void Director::popToSceneStackLevel(int level)
{
    AXAssert(_runningScene != NULL, "A running Scene is needed");
    int c = (int)m_pobScenesStack->count();

    // level 0? -> end
    if (level == 0)
    {
        end();
        return;
    }

    // current level or lower -> nothing
    if (level >= c)
        return;

	// pop stack until reaching desired level
	while (c > level)
    {
		Scene *current = (Scene*)m_pobScenesStack->lastObject();

		if (current->isRunning())
        {
            current->onExitTransitionDidStart();
            current->onExit();
		}

        current->cleanup();
        m_pobScenesStack->removeLastObject();
		c--;
	}

	m_pNextScene = (Scene*)m_pobScenesStack->lastObject();
	m_bSendCleanupToScene = false;
}

void Director::end()
{
    m_bPurgeDirecotorInNextLoop = true;
}

void Director::purgeDirector()
{
    // cleanup scheduler
    getScheduler()->unscheduleAll();

    if (_runningScene)
    {
        _runningScene->onExitTransitionDidStart();
        _runningScene->onExit();
        _runningScene->cleanup();
        _runningScene->release();
    }
    
    _runningScene = NULL;
    m_pNextScene = NULL;

    // remove all objects, but don't release it.
    // runWithScene might be executed after 'end'.
    m_pobScenesStack->removeAllObjects();

    stopAnimation();

    AX_SAFE_RELEASE_NULL(m_pFPSLabel);
    AX_SAFE_RELEASE_NULL(m_pSPFLabel);
    AX_SAFE_RELEASE_NULL(m_pDrawsLabel);

    // purge bitmap cache
    LabelBMFont::purgeCachedData();

    // purge all managed caches
    ccDrawFree();
    AnimationCache::purgeSharedAnimationCache();
    SpriteFrameCache::purgeSharedSpriteFrameCache();
    TextureCache::purgeSharedTextureCache();
    ShaderCache::purgeSharedShaderCache();
    FileUtils::purgeFileUtils();
    Configuration::purgeConfiguration();

    // cocos2d-x specific data structures
    UserDefault::purgeSharedUserDefault();
    NotificationCenter::purgeNotificationCenter();

    ccGLInvalidateStateCache();
    
    CHECK_GL_ERROR_DEBUG();
    
    // OpenGL view
    _openGLView->end();
    _openGLView = nullptr;

    // delete Director
    release();
}

void Director::setNextScene(void)
{
    bool runningIsTransition = dynamic_cast<TransitionScene*>(_runningScene) != NULL;
    bool newIsTransition = dynamic_cast<TransitionScene*>(m_pNextScene) != NULL;

    // If it is not a transition, call onExit/cleanup
     if (! newIsTransition)
     {
         if (_runningScene)
         {
             _runningScene->onExitTransitionDidStart();
             _runningScene->onExit();
         }
 
         // issue #709. the root node (scene) should receive the cleanup message too
         // otherwise it might be leaked.
         if (m_bSendCleanupToScene && _runningScene)
         {
             _runningScene->cleanup();
         }
     }

    if (_runningScene)
    {
        _runningScene->release();
    }
    _runningScene = m_pNextScene;
    m_pNextScene->retain();
    m_pNextScene = NULL;

    if ((! runningIsTransition) && _runningScene)
    {
        _runningScene->onEnter();
        _runningScene->onEnterTransitionDidFinish();
    }
}

void Director::pause(void)
{
    if (_paused)
    {
        return;
    }

    m_dOldAnimationInterval = _animationInterval;

    // when paused, don't consume CPU
    setAnimationInterval(1 / 4.0);
    _paused = true;
}

void Director::resume(void)
{
    if (! _paused)
    {
        return;
    }

    setAnimationInterval(m_dOldAnimationInterval);

    if (Time::gettimeofdayCocos2d(m_pLastUpdate, NULL) != 0)
    {
        AXLOG("cocos2d: Director: Error in gettimeofday");
    }

    _paused = false;
    m_fDeltaTime = 0;
}

// display the FPS using a LabelAtlas
// updates the FPS every frame
void Director::showStats(void)
{
    m_uFrames++;
    m_fAccumDt += m_fDeltaTime;
    
    if (_displayStats)
    {
        if (m_pFPSLabel && m_pSPFLabel && m_pDrawsLabel)
        {
            if (m_fAccumDt > AX_DIRECTOR_STATS_INTERVAL)
            {
                sprintf(m_pszFPS, "%.3f", m_fSecondsPerFrame);
                m_pSPFLabel->setString(m_pszFPS);
                
                m_fFrameRate = m_uFrames / m_fAccumDt;
                m_uFrames = 0;
                m_fAccumDt = 0;
                
                sprintf(m_pszFPS, "%.1f", m_fFrameRate);
                m_pFPSLabel->setString(m_pszFPS);
                
                sprintf(m_pszFPS, "%4lu", (unsigned long)g_uNumberOfDraws);
                m_pDrawsLabel->setString(m_pszFPS);
            }
            
            m_pDrawsLabel->visit();
            m_pFPSLabel->visit();
            m_pSPFLabel->visit();
        }
    }    
    
    g_uNumberOfDraws = 0;
}

void Director::calculateMPF()
{
    struct AX_timeval now;
    Time::gettimeofdayCocos2d(&now, NULL);
    
    m_fSecondsPerFrame = (now.tv_sec - m_pLastUpdate->tv_sec) + (now.tv_usec - m_pLastUpdate->tv_usec) / 1000000.0f;
}

// returns the FPS image data pointer and len
void Director::getFPSImageData(unsigned char** datapointer, unsigned int* length)
{
    // XXX fixed me if it should be used 
    *datapointer = ax_fps_images_png;
	*length = ax_fps_images_len();
}

void Director::createStatsLabel()
{
    Texture2D *texture = NULL;
    TextureCache *textureCache = TextureCache::sharedTextureCache();

    if( m_pFPSLabel && m_pSPFLabel )
    {
        AX_SAFE_RELEASE_NULL(m_pFPSLabel);
        AX_SAFE_RELEASE_NULL(m_pSPFLabel);
        AX_SAFE_RELEASE_NULL(m_pDrawsLabel);
        textureCache->removeTextureForKey("AX_fps_images");
        FileUtils::sharedFileUtils()->purgeCachedEntries();
    }

    CCTexture2DPixelFormat currentFormat = Texture2D::defaultAlphaPixelFormat();
    Texture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA4444);
    unsigned char *data = NULL;
    unsigned int data_len = 0;
    getFPSImageData(&data, &data_len);

    Image* image = new Image();
    bool isOK = image->initWithImageData(data, data_len);
    if (!isOK) {
        AXLOGERROR("%s", "Fails: init fps_images");
        return;
    }

    texture = textureCache->addUIImage(image, "AX_fps_images");
    AX_SAFE_RELEASE(image);

    m_pFPSLabel = new LabelAtlas();
    m_pFPSLabel->initWithString("00.0", texture, 12, 32 , '.');

    m_pSPFLabel = new LabelAtlas();
    m_pSPFLabel->initWithString("0.000", texture, 12, 32, '.');

    m_pDrawsLabel = new LabelAtlas();
    m_pDrawsLabel->initWithString("000", texture, 12, 32, '.');

    Texture2D::setDefaultAlphaPixelFormat(currentFormat);

    auto pos = AX_DIRECTOR_STATS_POSITION;

    m_pFPSLabel->setPosition(pos);
    m_pDrawsLabel->setPosition(
        Vec2(
            m_pFPSLabel->getPositionX(),
            m_pFPSLabel->getPositionY() + m_pDrawsLabel->getContentSize().height / 2
        )
    );
    m_pSPFLabel->setPosition(
        Vec2(
            m_pDrawsLabel->getPositionX(),
            m_pDrawsLabel->getPositionY() + m_pSPFLabel->getContentSize().height / 2
        )
    );
}

float Director::getContentScaleFactor(void)
{
    return m_fContentScaleFactor;
}

void Director::setContentScaleFactor(float scaleFactor)
{
    if (scaleFactor != m_fContentScaleFactor)
    {
        m_fContentScaleFactor = scaleFactor;
        createStatsLabel();
    }
}

Node* Director::getNotificationNode() 
{ 
    return m_pNotificationNode; 
}

void Director::setNotificationNode(Node *node)
{
    AX_SAFE_RELEASE(m_pNotificationNode);
    m_pNotificationNode = node;
    AX_SAFE_RETAIN(m_pNotificationNode);
}

DirectorDelegate* Director::getDelegate() const
{
    return m_pProjectionDelegate;
}

void Director::setDelegate(DirectorDelegate* pDelegate)
{
    m_pProjectionDelegate = pDelegate;
}

void Director::setScheduler(Scheduler* pScheduler)
{
    if (m_pScheduler != pScheduler)
    {
        AX_SAFE_RETAIN(pScheduler);
        AX_SAFE_RELEASE(m_pScheduler);
        m_pScheduler = pScheduler;
    }
}

Scheduler* Director::getScheduler()
{
    return m_pScheduler;
}

void Director::setActionManager(ActionManager* pActionManager)
{
    if (m_pActionManager != pActionManager)
    {
        AX_SAFE_RETAIN(pActionManager);
        AX_SAFE_RELEASE(m_pActionManager);
        m_pActionManager = pActionManager;
    }    
}

ActionManager* Director::getActionManager()
{
    return m_pActionManager;
}

void Director::setKeypadDispatcher(KeypadDispatcher* pKeypadDispatcher)
{
    AX_SAFE_RETAIN(pKeypadDispatcher);
    AX_SAFE_RELEASE(m_pKeypadDispatcher);
    m_pKeypadDispatcher = pKeypadDispatcher;
}

KeypadDispatcher* Director::getKeypadDispatcher()
{
    return m_pKeypadDispatcher;
}

void Director::setAccelerometer(Accelerometer* pAccelerometer)
{
    if (m_pAccelerometer != pAccelerometer)
    {
        AX_SAFE_DELETE(m_pAccelerometer);
        m_pAccelerometer = pAccelerometer;
    }
}

Accelerometer* Director::getAccelerometer()
{
    return m_pAccelerometer;
}

void Director::setEventDispatcher(EventDispatcher* eventDispatcher)
{
    if (_eventDispatcher != eventDispatcher)
    {
        AX_SAFE_DELETE(_eventDispatcher);
        _eventDispatcher = eventDispatcher;
    }
}

EventDispatcher* Director::getEventDispatcher()
{
    return _eventDispatcher;
}

/***************************************************
* implementation of DisplayLinkDirector
**************************************************/

DisplayLinkDirector::DisplayLinkDirector()
    : _invalid(false)
{
}

DisplayLinkDirector::~DisplayLinkDirector()
{
}

void DisplayLinkDirector::startAnimation(void)
{
    if (Time::gettimeofdayCocos2d(m_pLastUpdate, NULL) != 0)
    {
        AXLOG("cocos2d: DisplayLinkDirector: Error on gettimeofday");
    }

    _invalid = false;

    Application::sharedApplication()->setAnimationInterval(_animationInterval);
}

void DisplayLinkDirector::mainLoop(void)
{
    if (m_bPurgeDirecotorInNextLoop)
    {
        m_bPurgeDirecotorInNextLoop = false;
        purgeDirector();
    }
    else if (!_invalid)
     {
         drawScene();
     
         // release the objects
         PoolManager::sharedPoolManager()->pop();        
     }
}

void DisplayLinkDirector::stopAnimation(void)
{
    _invalid = true;
}

void DisplayLinkDirector::setAnimationInterval(double value)
{
    _animationInterval = value;
    if (!_invalid)
    {
        stopAnimation();
        startAnimation();
    }    
}

NS_AX_END

