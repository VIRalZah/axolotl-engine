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
#include "BMFontCache.h"
#include "label_nodes/LabelTTF.h"
#include "actions/ActionManager.h"
#include "base/Configuration.h"
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
#include "platform/GLView.h"
#include "base/Configuration.h"
#include "base/EventDispatcher.h"
#include "support/StringUtils.h"
#include "Renderer.h"

using namespace std;

unsigned int _numberOfDraws = 0;

NS_AX_BEGIN

static Director* _sharedDirector = NULL;

Director::Director(void)
{
}

Director::~Director(void)
{
    AXLOG("axolotl: deallocing Director %p", this);

    AX_SAFE_RELEASE(_statsLabel);

    AX_SAFE_RELEASE(_runningScene);
    AX_SAFE_RELEASE(_scenesStack);
    AX_SAFE_RELEASE(_scheduler);
    AX_SAFE_RELEASE(_actionManager);
    AX_SAFE_DELETE(_accelerometer);
    AX_SAFE_RELEASE(_eventDispatcher);

    PoolManager::sharedPoolManager()->pop();
    PoolManager::purgePoolManager();

    _sharedDirector = NULL;
}

Director* Director::getInstance(void)
{
    if (!_sharedDirector)
    {
        _sharedDirector = new Director();
        _sharedDirector->init();
    }
    return _sharedDirector;
}

bool Director::init()
{
	setDefaultValues();

    _renderer = new Renderer();
    _glView = nullptr;

    _terminateInNextLoop = false;

    _statsLabel = nullptr;
    _elapsedDt = 0.0f;
    _shouldUpdateStatsLabel = false;

    _runningScene = nullptr;
    _nextScene = nullptr;

    _scenesStack = new Array();
    _scenesStack->init();

    _totalFrames = 0;

    _paused = false;

    _designSizeInPoints = Size::ZERO;    

    _contentScaleFactor = 1.0f;

    _scheduler = new Scheduler();

    _actionManager = new ActionManager();
    _scheduler->scheduleUpdateForTarget(_actionManager, kAXPrioritySystem, false);

    _accelerometer = new Accelerometer();

    _eventDispatcher = new EventDispatcher();

    _invalid = false;

    PoolManager::sharedPoolManager()->push();

    return true;
}

void Director::setDefaultValues(void)
{
	auto config = Configuration::sharedConfiguration();

	double fps = config->getNumber("axolotl.fps", 60.0);
	_oldAnimationInterval = _animationInterval = 1.0 / fps;

	_displayStats = config->getBool("axolotl.display_stats", true);

    auto& projection = config->getString("axolotl.gl.projection", "2d");
    if (projection == "2d")
    {
        _projection = Projection::ORTHOGRAPHIC;
    }
    else if (projection == "3d")
    {
        _projection = Projection::PERSPECTIVE;
    }
    else
    {
        _projection = Projection::DEFAULT_PROJECTION;
    }

	const char* pixel_format = config->getCString("axolotl.texture.pixel_format_for_png", "rgba8888");
	if( strcmp(pixel_format, "rgba8888") == 0 )
		Texture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
	else if( strcmp(pixel_format, "rgba4444") == 0 )
		Texture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA4444);
	else if( strcmp(pixel_format, "rgba5551") == 0 )
		Texture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGB5A1);

	bool pvr_alpha_premultipled = config->getBool("axolotl.texture.pvrv2_has_alpha_premultiplied", false);
	Texture2D::PVRImagesHavePremultipliedAlpha(pvr_alpha_premultipled);
}

void Director::setGLDefaultValues(void)
{
    AXAssert(_glView, "opengl view should not be null");

    setAlphaBlending(true);

    setDepthTest(false);
    updateProjection();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Director::drawScene(void)
{
    calculateDeltaTime();

    if (!_paused)
    {
        _scheduler->update(_deltaTime);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_nextScene)
    {
        setNextScene();
    }

    kmGLPushMatrix();

    if (_runningScene)
    {
        _runningScene->visit();
    }

    _renderer->render();

    showStats();

    kmGLPopMatrix();

    _totalFrames++;

    if (_glView)
    {
        _glView->swapBuffers();
    }
}

void Director::calculateDeltaTime()
{
    auto now = std::chrono::steady_clock::now();

    if (_nextDeltaTimeZero)
    {
        _nextDeltaTimeZero = false;
        _deltaTime = 0.0f;
    }
    else
    {
        _deltaTime = MAX(
            0.0f, 
            std::chrono::duration_cast<std::chrono::duration<float>>(now - _lastUpdate).count()
        );
    }

    _lastUpdate = now;
}

void Director::setViewport()
{
    if (_glView)
    {
        _glView->setViewPortInPoints(0, 0, _designSizeInPoints.width, _designSizeInPoints.height);
    }
}

void Director::setProjection(Projection projection)
{
    _projection = projection;
    updateProjection();
}

void Director::updateProjection()
{
    Size size = _designSizeInPoints;

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
    default:
        AXLOG("axolotl: Director: unrecognized projection");
        break;
    }

    ccSetProjectionMatrixDirty();
}

void Director::purgeCachedData(void)
{
    BMFontCache::sharedBMFontCache()->removeAllConfigurations();
    if (_sharedDirector->getGLView())
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
    return (_designSizeInPoints.height / 1.1566f);
}

void Director::setAlphaBlending(bool on)
{
    if (on)
    {
        ccGLBlendFunc(AX_BLEND_SRC, AX_BLEND_DST);
    }
    else
    {
        ccGLBlendFunc(GL_ONE, GL_ZERO);
    }

    CHECK_GL_ERROR_DEBUG();
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

Vec2 Director::convertToGL(const Vec2& uiPoint) const
{
    kmMat4 transform;
	GLToClipTransform(&transform);
	
	kmMat4 transformInv;
	kmMat4Inverse(&transformInv, &transform);
	
	kmScalar zClip = transform.mat[14]/transform.mat[15];
	
    Size glSize = _glView->getDesignResolutionSize();
	kmVec3 clipCoord = {2.0f*uiPoint.x/glSize.width - 1.0f, 1.0f - 2.0f*uiPoint.y/glSize.height, zClip};
	
	kmVec3 glCoord;
	kmVec3TransformCoord(&glCoord, &clipCoord, &transformInv);
	
	return Vec2(glCoord.x, glCoord.y);
}

Vec2 Director::convertToUI(const Vec2& glPoint) const
{
    kmMat4 transform;
	GLToClipTransform(&transform);
    
	kmVec3 clipCoord;
	// Need to calculate the zero depth from the transform.
	kmVec3 glCoord = {glPoint.x, glPoint.y, 0.0};
	kmVec3TransformCoord(&clipCoord, &glCoord, &transform);
	
	Size glSize = _glView->getDesignResolutionSize();
	return Vec2(glSize.width*(clipCoord.x*0.5 + 0.5), glSize.height*(-clipCoord.y*0.5 + 0.5));
}

Size Director::getDesignSize(void) const
{
    return _designSizeInPoints;
}

Size Director::getDesignSizeInPixels() const
{
    return Size(_designSizeInPoints.width * _contentScaleFactor, _designSizeInPoints.height * _contentScaleFactor);
}

Size Director::getVisibleSize() const
{
    if (_glView)
    {
        return _glView->getVisibleSize();
    }
    else 
    {
        return Size::ZERO;
    }
}

Vec2 Director::getVisibleOrigin() const
{
    if (_glView)
    {
        return _glView->getVisibleOrigin();
    }
    else 
    {
        return Vec2::ZERO;
    }
}

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

    unsigned int index = _scenesStack->count();

    _sendCleanupToScene = true;
    _scenesStack->replaceObjectAtIndex(index - 1, pScene);

    _nextScene = pScene;
}

void Director::pushScene(Scene *pScene)
{
    AXAssert(pScene, "the scene should not null");

    _sendCleanupToScene = false;

    _scenesStack->addObject(pScene);
    _nextScene = pScene;
}

void Director::popScene()
{
    AXAssert(_runningScene != NULL, "running scene should not null");

    _scenesStack->removeLastObject();
    unsigned int c = _scenesStack->count();

    if (c == 0)
    {
        end();
    }
    else
    {
        _sendCleanupToScene = true;
        _nextScene = (Scene*)_scenesStack->objectAtIndex(c - 1);
    }
}

void Director::popToRootScene(void)
{
    popToSceneStackLevel(1);
}

void Director::popToSceneStackLevel(int level)
{
    AXAssert(_runningScene != NULL, "A running Scene is needed");
    int c = (int)_scenesStack->count();

    if (level == 0)
    {
        end();
        return;
    }

    if (level >= c)
        return;

	while (c > level)
    {
		Scene* current = (Scene*)_scenesStack->lastObject();

		if (current->isRunning())
        {
            current->onExitTransitionDidStart();
            current->onExit();
		}

        current->cleanup();
        _scenesStack->removeLastObject();
		c--;
	}

	_nextScene = (Scene*)_scenesStack->lastObject();
	_sendCleanupToScene = false;
}

void Director::end()
{
    _terminateInNextLoop = true;
}

void Director::setGLView(GLView* glView)
{
    AXAssert(glView, "opengl view should not be null");

    if (_glView != glView)
    {
        Configuration* config = Configuration::sharedConfiguration();
        config->gatherGPUInfo();
        config->dumpInfo();

        AX_SAFE_RELEASE(_glView);
        _glView = glView;

        if (_glView)
        {
            _designSizeInPoints = _glView->getDesignResolutionSize();
            setGLDefaultValues();
        }

        CHECK_GL_ERROR_DEBUG();
    }
}

void Director::terminate()
{
    Application::sharedApplication()->applicationWillTerminate();

    getScheduler()->unscheduleAll();

    if (_runningScene)
    {
        _runningScene->onExitTransitionDidStart();
        _runningScene->onExit();
        _runningScene->cleanup();
        _runningScene->release();
    }
    
    _runningScene = NULL;
    _nextScene = NULL;

    _scenesStack->removeAllObjects();

    stopAnimation();

    BMFontCache::purgeBMFontCache();

    ccDrawFree();
    AnimationCache::purgeSharedAnimationCache();
    SpriteFrameCache::purgeSharedSpriteFrameCache();
    TextureCache::purgeSharedTextureCache();
    ShaderCache::purgeSharedShaderCache();
    FileUtils::purgeFileUtils();
    Configuration::purgeConfiguration();

    UserDefault::purgeSharedUserDefault();
    NotificationCenter::purgeNotificationCenter();

    ccGLInvalidateStateCache();
    
    CHECK_GL_ERROR_DEBUG();
    
    _glView->end();
    _glView = nullptr;

    release();
}

void Director::showStats()
{
    if (_displayStats)
    {
        if (!_statsLabel)
        {
            _statsLabel = new LabelTTF();
            _statsLabel->initWithString("", "Arial", 32);
            _statsLabel->setAnchorPoint(Vec2::ZERO);

            _shouldUpdateStatsLabel = true;
        }
        else
        {
            _elapsedDt += _deltaTime;
            if (_elapsedDt >= AX_DIRECTOR_STATS_INTERVAL)
            {
                _elapsedDt = 0.0f;

                _shouldUpdateStatsLabel = true;
            }
        }

        if (_shouldUpdateStatsLabel)
        {
            _shouldUpdateStatsLabel = false;

            updateStatsLabel();
        }

        _statsLabel->visit();
    }

    _numberOfDraws = 0;
}

void Director::updateStatsLabel()
{
    if (!_statsLabel) return;

    _statsLabel->setString(StringUtils::format(
        "GL draws: %d\nFPS: %.1f",
        _numberOfDraws,
        1.0 / _deltaTime
    ).c_str());
    _statsLabel->setPosition(getVisibleOrigin());
}

void Director::setNextScene(void)
{
    bool runningIsTransition = dynamic_cast<TransitionScene*>(_runningScene) != nullptr;
    bool newIsTransition = dynamic_cast<TransitionScene*>(_nextScene) != nullptr;

    if (!newIsTransition && _runningScene)
    {
        _runningScene->onExitTransitionDidStart();
        _runningScene->onExit();

        if (_sendCleanupToScene)
        {
            _runningScene->cleanup();
        }
    }

    if (_runningScene)
    {
        _runningScene->release();
    }

    _runningScene = _nextScene;
    _runningScene->retain();
    _nextScene = nullptr;

    if (!runningIsTransition && _runningScene)
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

    _oldAnimationInterval = _animationInterval;

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

    setAnimationInterval(_oldAnimationInterval);

    _lastUpdate = std::chrono::steady_clock::now();

    _paused = false;
    _deltaTime = 0;
}

float Director::getContentScaleFactor() const
{
    return _contentScaleFactor;
}

void Director::setContentScaleFactor(float scaleFactor)
{
    if (scaleFactor != _contentScaleFactor)
    {
        _contentScaleFactor = scaleFactor;
    }
}

void Director::stopAnimation()
{
    _invalid = true;
}

void Director::startAnimation()
{
    _lastUpdate = std::chrono::steady_clock::now();

    _invalid = false;

    Application::sharedApplication()->setAnimationInterval(_animationInterval);
}

void Director::mainLoop()
{
    if (_terminateInNextLoop)
    {
        _terminateInNextLoop = false;
        terminate();
    }
    else if (!_invalid)
    {
        drawScene();
        PoolManager::sharedPoolManager()->pop();        
    }
}

void Director::setAnimationInterval(double value)
{
    _animationInterval = value;
    if (!_invalid)
    {
        stopAnimation();
        startAnimation();
    }    
}

NS_AX_END

