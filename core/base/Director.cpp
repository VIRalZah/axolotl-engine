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
#include "platform/GLView.h"
#include "base/Configuration.h"
#include "base/EventDispatcher.h"
#include "support/StringUtils.h"

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

    AX_SAFE_RELEASE(_infoLabel);

    AX_SAFE_RELEASE(_runningScene);
    AX_SAFE_RELEASE(_notificationNode);
    AX_SAFE_RELEASE(_scenesStack);
    AX_SAFE_RELEASE(_scheduler);
    AX_SAFE_RELEASE(_actionManager);
    AX_SAFE_RELEASE(_keypadDispatcher);
    AX_SAFE_DELETE(_accelerometer);
    AX_SAFE_RELEASE(_eventDispatcher);

    PoolManager::sharedPoolManager()->pop();
    PoolManager::purgePoolManager();

    _sharedDirector = NULL;
}

Director* Director::sharedDirector(void)
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

    _runningScene = nullptr;
    _nextScene = nullptr;

    _notificationNode = nullptr;

    _scenesStack = new Array();
    _scenesStack->init();

    _accumDt = 0.0f;
    _frameRate = 0.0f;
    _defaultLabelFont = "Times New Roman";
    _infoLabel = nullptr;
    _totalFrames = _frames = 0;

    _paused = false;
   
    _purgeApplicationInNextLoop = false;

    _winSizeInPoints = Size::ZERO;    

    _openGLView = nullptr;

    _contentScaleFactor = 1.0f;

    _scheduler = new Scheduler();

    _actionManager = new ActionManager();
    _scheduler->scheduleUpdateForTarget(_actionManager, kAXPrioritySystem, false);

    _keypadDispatcher = new KeypadDispatcher();

    _accelerometer = new Accelerometer();

    _eventDispatcher = new EventDispatcher();

    _invalid = false;

    _textureQuality = TextureQuality::DEFAULT_TEXTURE_QUALITY;

    PoolManager::sharedPoolManager()->push();

    return true;
}

void Director::setDefaultValues(void)
{
	auto config = Configuration::sharedConfiguration();

	double fps = config->getNumber("axolotl.fps", 60.0);
	_oldAnimationInterval = _animationInterval = 1.0 / fps;

	_displayDebugInfo = config->getBool("axolotl.display_stats", false);

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
    AXAssert(_openGLView, "opengl view should not be null");

    setAlphaBlending(true);

    setDepthTest(false);
    updateProjection();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    updateTextureQuality();
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

    if (_notificationNode)
    {
        _notificationNode->visit();
    }
    
    if (_displayDebugInfo)
    {
        showDebugInfo();
    }

    kmGLPopMatrix();

    _totalFrames++;

    if (_openGLView)
    {
        _openGLView->swapBuffers();
    }
}

void Director::calculateDeltaTime(void)
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

float Director::getDeltaTime()
{
	return _deltaTime;
}

void Director::setOpenGLView(GLView* glView)
{
    AXAssert(glView, "opengl view should not be null");

    if (_openGLView != glView)
    {
		Configuration* config = Configuration::sharedConfiguration();
		config->gatherGPUInfo();
		config->dumpInfo();

        AX_SAFE_RELEASE(_openGLView);
        _openGLView = glView;

        if (_openGLView)
        {
            _winSizeInPoints = _openGLView->getDesignResolutionSize();
            createDebugInfoLabel();
            setGLDefaultValues();
        }
        
        CHECK_GL_ERROR_DEBUG();
    }
}

void Director::setViewport()
{
    if (_openGLView)
    {
        _openGLView->setViewPortInPoints(0, 0, _winSizeInPoints.width, _winSizeInPoints.height);
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
    Size size = _winSizeInPoints;

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
    if (_sharedDirector->getOpenGLView())
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
    return (_winSizeInPoints.height / 1.1566f);
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
		_winSizeInPoints = Size(newWindowSize.width * _contentScaleFactor,
			newWindowSize.height * _contentScaleFactor);
		
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

Size Director::getWinSize(void) const
{
    return _winSizeInPoints;
}

Size Director::getWinSizeInPixels() const
{
    return Size(_winSizeInPoints.width * _contentScaleFactor, _winSizeInPoints.height * _contentScaleFactor);
}

Size Director::getVisibleSize() const
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

Vec2 Director::getVisibleOrigin() const
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
    _purgeApplicationInNextLoop = true;
}

void Director::setDefaultLabelFont(const std::string& defaultLabelFont)
{
    _defaultLabelFont = defaultLabelFont;
    createDebugInfoLabel();
}

void Director::purgeApplication()
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

    AX_SAFE_RELEASE_NULL(_infoLabel);

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
    
    _openGLView->end();
    _openGLView = nullptr;

    release();
}

void Director::setNextScene(void)
{
    bool runningIsTransition = dynamic_cast<TransitionScene*>(_runningScene) != NULL;
    bool newIsTransition = dynamic_cast<TransitionScene*>(_nextScene) != NULL;

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
         if (_sendCleanupToScene && _runningScene)
         {
             _runningScene->cleanup();
         }
     }

    if (_runningScene)
    {
        _runningScene->release();
    }
    _runningScene = _nextScene;
    _nextScene->retain();
    _nextScene = NULL;

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

void Director::showDebugInfo()
{
    _frames++;
    _accumDt += _deltaTime;
    
    if (_displayDebugInfo)
    {
        if (_infoLabel)
        {
            if (_accumDt >= AX_DIRECTOR_STATS_INTERVAL)
            {   
                _frameRate = _frames / _accumDt;
                _frames = 0;
                _accumDt = 0;

                _infoLabel->setString(StringUtils::format(
                    "%u\n%.1f",
                    _numberOfDraws, // open gl draws
                    _frameRate // frame rate
                ));
                _infoLabel->setPosition(getVisibleOrigin() + _infoLabel->getContentSize() / 2);
            }

            _infoLabel->visit();
        }
    }    
    
    _numberOfDraws = 0;
}

void Director::updateTextureQuality()
{
    auto min = MIN(_winSizeInPoints.width, _winSizeInPoints.height);

    if (min <= 480.0f)
    {
        _textureQuality = TextureQuality::LOW;
    }
    else if (min <= 960.0f)
    {
        _textureQuality = TextureQuality::MEDIUM;
    }
    else
    {
        _textureQuality = TextureQuality::HIGH;
    }
}

void Director::createDebugInfoLabel()
{
    if (_infoLabel)
    {
        AX_SAFE_RELEASE_NULL(_infoLabel);
    }

    _infoLabel = new LabelTTF();
    _infoLabel->initWithString("0\n0.0", _defaultLabelFont.c_str(), 24);
    _infoLabel->setPosition(getVisibleOrigin() + _infoLabel->getContentSize() / 2);
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
        createDebugInfoLabel();
    }
}

Node* Director::getNotificationNode() 
{ 
    return _notificationNode; 
}

void Director::setNotificationNode(Node *node)
{
    AX_SAFE_RELEASE(_notificationNode);
    _notificationNode = node;
    AX_SAFE_RETAIN(_notificationNode);
}

void Director::setScheduler(Scheduler* pScheduler)
{
    if (_scheduler != pScheduler)
    {
        AX_SAFE_RETAIN(pScheduler);
        AX_SAFE_RELEASE(_scheduler);
        _scheduler = pScheduler;
    }
}

Scheduler* Director::getScheduler()
{
    return _scheduler;
}

void Director::setActionManager(ActionManager* pActionManager)
{
    if (_actionManager != pActionManager)
    {
        AX_SAFE_RETAIN(pActionManager);
        AX_SAFE_RELEASE(_actionManager);
        _actionManager = pActionManager;
    }    
}

ActionManager* Director::getActionManager()
{
    return _actionManager;
}

void Director::setKeypadDispatcher(KeypadDispatcher* pKeypadDispatcher)
{
    AX_SAFE_RETAIN(pKeypadDispatcher);
    AX_SAFE_RELEASE(_keypadDispatcher);
    _keypadDispatcher = pKeypadDispatcher;
}

KeypadDispatcher* Director::getKeypadDispatcher()
{
    return _keypadDispatcher;
}

void Director::setAccelerometer(Accelerometer* pAccelerometer)
{
    if (_accelerometer != pAccelerometer)
    {
        AX_SAFE_DELETE(_accelerometer);
        _accelerometer = pAccelerometer;
    }
}

Accelerometer* Director::getAccelerometer()
{
    return _accelerometer;
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

TextureQuality Director::getTextureQuality()
{
    return _textureQuality;
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
    if (_purgeApplicationInNextLoop)
    {
        _purgeApplicationInNextLoop = false;
        purgeApplication();
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

