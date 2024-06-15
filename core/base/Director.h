/****************************************************************************
Copyright (c) 2024 zahann.ru

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

class LabelTTF;
class Scene;
class GLView;
class Node;
class Renderer;
class Scheduler;
class ActionManager;
class Accelerometer;
class EventDispatcher;

class AX_DLL Director : public Object, public TypeInfo
{
public:
    Director();
    ~Director();

    // get director instance
    static Director* getInstance();

    // class id? :)
    virtual long getClassTypeInfo()
    {
		static const long id = getHashCodeByString(typeid(Director).name());
		return id;
    }

    // running scene return
    Scene* getRunningScene() { return _runningScene; }

    // animation interval funcs
    double getAnimationInterval() const { return _animationInterval; }
    void setAnimationInterval(double value);

    // next delta time zero funcs
    bool isNextDeltaTimeZero() const { return _nextDeltaTimeZero; }
    void setNextDeltaTimeZero(bool nextDeltaTimeZero) { _nextDeltaTimeZero = nextDeltaTimeZero; };

    // paused return
    bool isPaused() const { return _paused; }

    // total frames return
    unsigned int getTotalFrames() const { return _totalFrames; }
    
    // projection funcs
    Projection getProjection() const { return _projection; }
    void setProjection(Projection projection);

    // update projection
    void updateProjection();
    
    // set gl viewport
    void setViewport();

    // send cleanup to scene return
    inline bool isSendCleanupToScene() const { return _sendCleanupToScene; }

    // design size return
    Size getDesignSize() const;
    Size getDesignSizeInPixels() const;
    
    // visible size return
    Size getVisibleSize() const;
    Vec2 getVisibleOrigin() const;

    // convert point
    Vec2 convertToGL(const Vec2& obPoint) const;
    Vec2 convertToUI(const Vec2& obPoint) const;

    // z eye return
    float getZEye() const;

    // scene management
    void runWithScene(Scene* scene);
    void pushScene(Scene* scene);
    void replaceScene(Scene* scene);

    void popScene();
    void popToRootScene();
    void popToSceneStackLevel(int level);

    // sets "_terminateInNextLoop" to true
    void end();

    // pause and resume
    void pause();
    void resume();

    // animation control
    void stopAnimation();
    void startAnimation();

    // draw scene. DO NOT CALL THIS FUNCTION MANUALLY!
    void drawScene();

    // purge cached data
    void purgeCachedData();

    // default values set
    void setDefaultValues();
    void setGLDefaultValues();

    // alpha blending
    void setAlphaBlending(bool on);

    // depth test
    void setDepthTest(bool on);

    // manu loop. DO NOT CALL THIS FUNCTION MANUALLY!
    void mainLoop();

    // content scale factor funcs
    inline float getContentScaleFactor() const;
    virtual void setContentScaleFactor(float scaleFactor);

    // renderer return
    Renderer* getRenderer() const { return _renderer; }

    // glview funcs
    GLView* getGLView() { return _glView; };
    void setGLView(GLView* glView);

    // scheduler return
    Scheduler* getScheduler() const { return _scheduler; }

    // action manager return
    ActionManager* getActionManager() const { return _actionManager; }

    // accelerometer return
    Accelerometer* getAccelerometer() const { return _accelerometer; };

    // event dispatcher return
    EventDispatcher* getEventDispatcher() const { return _eventDispatcher; }

    // original delta time return
    float getDeltaTime() const { return _deltaTime; }
protected:
    // initalizes director
    bool init();

    // "terminate"
    void terminate();

    // stats
    void showStats();
    void updateStatsLabel();
    
    // set next scene
    void setNextScene();

    // calculate delta time
    void calculateDeltaTime();

    // renderer and glview
    Renderer* _renderer;
    GLView* _glView;
    Scheduler* _scheduler;
    ActionManager* _actionManager;
    Accelerometer* _accelerometer;
    EventDispatcher* _eventDispatcher;

    // delta time
    float _deltaTime;

    // terminate in next loop
    bool _terminateInNextLoop;

    // stats
    bool _displayStats;
    LabelTTF* _statsLabel;
    float _elapsedDt;
    bool _shouldUpdateStatsLabel;

    // animation interval
    double _animationInterval;
    double _oldAnimationInterval;
    
    // paused
    bool _paused;

    // total frames
    unsigned int _totalFrames;

    // scenes
    Scene* _runningScene;
    Scene* _nextScene;
    Array* _scenesStack;

    // send cleanup to scene
    bool _sendCleanupToScene;

    // last update :)
    std::chrono::steady_clock::time_point _lastUpdate;
    
    // next delta time zero
    bool _nextDeltaTimeZero;
    
    // current projection
    Projection _projection;

    // design size in points
    Size _designSizeInPoints;
    
    // content scale factor
    float _contentScaleFactor;

    // invalid???
    bool _invalid;

    // mark GLView as a friend class
    friend class GLView;
};

NS_AX_END

#endif // __AX_DIRECTOR_H__