/****************************************************************************
Copyright (c) 2010-2012 zahann.ru

http://www.zahann.ru

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

#ifndef __AX_DIRECTOR_H__
#define __AX_DIRECTOR_H__

#include "platform/PlatformMacros.h"
#include "Object.h"
#include "Types.h"
#include "Array.h"
#include "GL.h"
#include "kazmath/mat4.h"
#include "label_nodes/LabelAtlas.h"
#include "ccTypeInfo.h"
#include <chrono>

NS_AX_BEGIN

enum Projection
{
    ORTHOGRAPHIC,
    PERSPECTIVE,
    
    DEFAULT_PROJECTION = ORTHOGRAPHIC
};

enum TextureQuality
{
    LOW = 1,
    MEDIUM,
    HIGH,

    DEFAULT_TEXTURE_QUALITY = LOW
};

class LabelAtlas;
class Scene;
class EGLViewProtocol;
class Node;
class Scheduler;
class ActionManager;
class KeypadDispatcher;
class Accelerometer;
class EventDispatcher;

class AX_DLL Director : public Object, public TypeInfo
{
public:
    Director();
    virtual ~Director();

    static Director* sharedDirector();

    virtual bool init();

    virtual long getClassTypeInfo()
    {
		static const long id = axolotl::getHashCodeByString(typeid(axolotl::Director).name());
		return id;
    }

    inline Scene* getRunningScene() { return _runningScene; }

    inline double getAnimationInterval() const { return _animationInterval; }
    virtual void setAnimationInterval(double value);

    inline bool isDisplayStats() const { return _displayStats; }
    virtual void setDisplayStats(bool displayStats) { _displayStats = displayStats; }
    
    inline EGLViewProtocol* getOpenGLView() const { return _openGLView; }
    virtual void setOpenGLView(EGLViewProtocol* glView);

    inline bool isNextDeltaTimeZero() const { return _nextDeltaTimeZero; }
    virtual void setNextDeltaTimeZero(bool bNextDeltaTimeZero);

    inline bool isPaused() const { return _paused; }

    inline unsigned int getTotalFrames() const { return _totalFrames; }
    
    inline Projection getProjection(void) { return _projection; }
    void setProjection(Projection kProjection);

    virtual void updateProjection();

    void reshapeProjection(const Size& newWindowSize);
    
    void setViewport();

    inline bool isSendCleanupToScene() const { return _sendCleanupToScene; }

    inline Node* getNotificationNode();
    virtual void setNotificationNode(Node* node);

    inline Size getWinSize() const;
    inline Size getWinSizeInPixels() const;
    
    inline Size getVisibleSize() const;
    inline Vec2 getVisibleOrigin() const;

    virtual Vec2 convertToGL(const Vec2& obPoint);
    virtual Vec2 convertToUI(const Vec2& obPoint);

    inline float getZEye() const;

    virtual void runWithScene(Scene* scene);
    virtual void pushScene(Scene* scene);

    virtual void popScene(void);
    virtual void popToRootScene(void);
    virtual void popToSceneStackLevel(int level);

    virtual void replaceScene(Scene* scene);

    virtual void end();

    virtual void pause();
    virtual void resume();

    virtual void stopAnimation();
    virtual void startAnimation();

    virtual void drawScene();

    virtual void purgeCachedData();

    virtual void setDefaultValues();

    virtual void setGLDefaultValues();

    virtual void setAlphaBlending(bool on);

    virtual void setDepthTest(bool on);

    virtual void mainLoop();

    inline float getContentScaleFactor() const;
    virtual void setContentScaleFactor(float scaleFactor);
protected:
    virtual void purgeDirector();
    
    virtual void setNextScene();
    
    virtual void showStats();
    virtual void createStatsLabel();

    virtual void getFPSImageData(unsigned char** datapointer, unsigned int* length);
    
    virtual void updateTextureQuality();

    virtual void calculateDeltaTime();

    AX_PROPERTY(Scheduler*, _scheduler, Scheduler);
    AX_PROPERTY(ActionManager*, _actionManager, ActionManager);
    AX_PROPERTY(KeypadDispatcher*, _keypadDispatcher, KeypadDispatcher);
    AX_PROPERTY(Accelerometer*, _accelerometer, Accelerometer);
    AX_PROPERTY(EventDispatcher*, _eventDispatcher, EventDispatcher);

    AX_PROPERTY_READONLY(float, _deltaTime, DeltaTime);

    AX_PROPERTY_READONLY(TextureQuality, _textureQuality, TextureQuality);

    bool _purgeDirectorInNextLoop;

    EGLViewProtocol* _openGLView;

    double _animationInterval;
    double _oldAnimationInterval;

    bool _displayStats;
    float _accumDt;
    float _frameRate;
    
    LabelAtlas* _fpsLabel;
    LabelAtlas* _drawsLabel;
    
    bool _paused;

    unsigned int _totalFrames;
    unsigned int _frames;

    Scene* _runningScene;
    Scene* _nextScene;
    Array* _scenesStack;

    bool _sendCleanupToScene;

    std::chrono::system_clock::time_point _lastUpdate;
    
    bool _nextDeltaTimeZero;
    
    Projection _projection;

    Size _winSizeInPoints;
    
    float _contentScaleFactor;

    Node* _notificationNode;

    bool _invalid;

    friend class EGLViewProtocol;
};

NS_AX_END

#endif // __AX_DIRECTOR_H__