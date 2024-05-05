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

NS_AX_BEGIN

enum Projection
{
    ORTHOGRAPHIC,
    PERSPECTIVE,
    CUSTOM,
    
    DEFAULT = ORTHOGRAPHIC
};

class LabelAtlas;
class Scene;
class EGLViewProtocol;
class DirectorDelegate;
class Node;
class Scheduler;
class ActionManager;
class KeypadDispatcher;
class Accelerometer;
class EventDispatcher;

class AX_DLL Director : public Object, public TypeInfo
{
public:
    Director(void);
    virtual ~Director(void);

    virtual bool init(void);

    virtual long getClassTypeInfo()
    {
		static const long id = axolotl::getHashCodeByString(typeid(axolotl::Director).name());
		return id;
    }

    inline Scene* getRunningScene() { return _runningScene; }

    inline double getAnimationInterval() const { return _animationInterval; }
    virtual void setAnimationInterval(double value) = 0;

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

    /** How many frames were called since the director started */
    
    
    /** Whether or not the replaced scene will receive the cleanup message.
     If the new scene is pushed, then the old scene won't receive the "cleanup" message.
     If the new scene replaces the old one, the it will receive the "cleanup" message.
     @since v0.99.0
     */
    inline bool isSendCleanupToScene(void) { return m_bSendCleanupToScene; }

    /** This object will be visited after the main scene is visited.
     This object MUST implement the "visit" selector.
     Useful to hook a notification object, like CCNotifications (http://github.com/manucorporat/CCNotifications)
     @since v0.99.5
     */
    Node* getNotificationNode();
    void setNotificationNode(Node *node);
    
    /** Director delegate. It shall implemente the DirectorDelegate protocol
     @since v0.99.5
     */
    DirectorDelegate* getDelegate() const;
    void setDelegate(DirectorDelegate* pDelegate);

    // window size

    /** returns the size of the OpenGL view in points.
    */
    Size getWinSize(void);

    /** returns the size of the OpenGL view in pixels.
    */
    Size getWinSizeInPixels(void);
    
    /** returns visible size of the OpenGL view in points.
     *  the value is equal to getWinSize if don't invoke
     *  EGLView::setDesignResolutionSize()
     */
    Size getVisibleSize();
    
    /** returns visible origin of the OpenGL view in points.
     */
    Vec2 getVisibleOrigin();

    /** converts a UIKit coordinate to an OpenGL coordinate
     Useful to convert (multi) touch coordinates to the current layout (portrait or landscape)
     */
    Vec2 convertToGL(const Vec2& obPoint);

    /** converts an OpenGL coordinate to a UIKit coordinate
     Useful to convert node points to window points for calls such as glScissor
     */
    Vec2 convertToUI(const Vec2& obPoint);

    inline float getZEye(void) const;

    // Scene Management

    /** Enters the Director's main loop with the given Scene.
     * Call it to run only your FIRST scene.
     * Don't call it if there is already a running scene.
     *
     * It will call pushScene: and then it will call startAnimation
     */
    void runWithScene(Scene *pScene);

    /** Suspends the execution of the running scene, pushing it on the stack of suspended scenes.
     * The new scene will be executed.
     * Try to avoid big stacks of pushed scenes to reduce memory allocation. 
     * ONLY call it if there is a running scene.
     */
    void pushScene(Scene *pScene);

    /** Pops out a scene from the queue.
     * This scene will replace the running one.
     * The running scene will be deleted. If there are no more scenes in the stack the execution is terminated.
     * ONLY call it if there is a running scene.
     */
    void popScene(void);

    /** Pops out all scenes from the queue until the root scene in the queue.
     * This scene will replace the running one.
     * Internally it will call `popToSceneStackLevel(1)`
     */
    void popToRootScene(void);

    /** Pops out all scenes from the queue until it reaches `level`.
     If level is 0, it will end the director.
     If level is 1, it will pop all scenes until it reaches to root scene.
     If level is <= than the current stack level, it won't do anything.
     */
 	void popToSceneStackLevel(int level);

    /** Replaces the running scene with a new one. The running scene is terminated.
     * ONLY call it if there is a running scene.
     */
    void replaceScene(Scene *pScene);

    /** Ends the execution, releases the running scene.
     It doesn't remove the OpenGL view from its parent. You have to do it manually.
     */
    void end(void);

    /** Pauses the running scene.
     The running scene will be _drawed_ but all scheduled timers will be paused
     While paused, the draw rate will be 4 FPS to reduce CPU consumption
     */
    void pause(void);

    /** Resumes the paused scene
     The scheduled timers will be activated again.
     The "delta time" will be 0 (as if the game wasn't paused)
     */
    void resume(void);

    /** Stops the animation. Nothing will be drawn. The main loop won't be triggered anymore.
     If you don't want to pause your animation call [pause] instead.
     */
    virtual void stopAnimation(void) = 0;

    /** The main loop is triggered again.
     Call this function only if [stopAnimation] was called earlier
     @warning Don't call this function to start the main loop. To run the main loop call runWithScene
     */
    virtual void startAnimation(void) = 0;

    /** Draw the scene.
    This method is called every frame. Don't call it manually.
    */
    void drawScene(void);

    // Memory Helper

    /** Removes cached all cocos2d cached data.
     It will purge the TextureCache, SpriteFrameCache, LabelBMFont cache
     @since v0.99.3
     */
    void purgeCachedData(void);

	/** sets the default values based on the Configuration info */
    void setDefaultValues(void);

    // OpenGL Helper

    /** sets the OpenGL default values */
    void setGLDefaultValues(void);

    /** enables/disables OpenGL alpha blending */
    void setAlphaBlending(bool bOn);

    /** enables/disables OpenGL depth test */
    void setDepthTest(bool bOn);

    virtual void mainLoop(void) = 0;

    /** The size in pixels of the surface. It could be different than the screen size.
    High-res devices might have a higher surface size than the screen size.
    Only available when compiled using SDK >= 4.0.
    @since v0.99.4
    */
    void setContentScaleFactor(float scaleFactor);
    float getContentScaleFactor(void);

public:
    /** Scheduler associated with this director
     @since v2.0
     */
    AX_PROPERTY(Scheduler*, m_pScheduler, Scheduler);

    /** ActionManager associated with this director
     @since v2.0
     */
    AX_PROPERTY(ActionManager*, m_pActionManager, ActionManager);

    /** KeypadDispatcher associated with this director
     @since v2.0
     */
    AX_PROPERTY(KeypadDispatcher*, m_pKeypadDispatcher, KeypadDispatcher);

    /** Accelerometer associated with this director
     @since v2.0
     @js NA
     @lua NA
     */
    AX_PROPERTY(Accelerometer*, m_pAccelerometer, Accelerometer);

    AX_PROPERTY(EventDispatcher*, _eventDispatcher, EventDispatcher);

    /* delta time since last tick to main loop */
	AX_PROPERTY_READONLY(float, m_fDeltaTime, DeltaTime);
	
public:
    /** returns a shared instance of the director 
     *  @js sharedEGLView
     */
    static Director* sharedDirector(void);

protected:

    void purgeDirector();
    bool m_bPurgeDirecotorInNextLoop; // this flag will be set to true in end()
    
    void setNextScene(void);
    
    void showStats();
    void createStatsLabel();
    void calculateMPF();
    void getFPSImageData(unsigned char** datapointer, unsigned int* length);
    
    /** calculates delta time since last time it was called */    
    void calculateDeltaTime();
protected:
    /* The EGLView, where everything is rendered */
    EGLViewProtocol* _openGLView;

    double _animationInterval;
    double m_dOldAnimationInterval;

    /* landscape mode ? */
    bool m_bLandscape;
    
    bool _displayStats;
    float m_fAccumDt;
    float m_fFrameRate;
    
    LabelAtlas *m_pFPSLabel;
    LabelAtlas *m_pSPFLabel;
    LabelAtlas *m_pDrawsLabel;
    
    /** Whether or not the Director is paused */
    bool _paused;

    /* How many frames were called since the director started */
    unsigned int _totalFrames;
    unsigned int m_uFrames;
    float m_fSecondsPerFrame;
     
    /* The running scene */
    Scene *_runningScene;
    
    /* will be the next 'runningScene' in the next frame
     nextScene is a weak reference. */
    Scene *m_pNextScene;
    
    /* If YES, then "old" scene will receive the cleanup message */
    bool    m_bSendCleanupToScene;

    /* scheduled scenes */
    Array* m_pobScenesStack;
    
    /* last time the main loop was updated */
    struct AX_timeval *m_pLastUpdate;

    /* whether or not the next delta time will be zero */
    bool _nextDeltaTimeZero;
    
    /* projection used */
    Projection _projection;

    /* window size in points */
    Size    m_obWinSizeInPoints;
    
    /* content scale factor */
    float    m_fContentScaleFactor;

    /* store the fps string */
    char *m_pszFPS;

    /* This object will be visited after the scene. Useful to hook a notification node */
    Node *m_pNotificationNode;

    /* Projection protocol delegate */
    DirectorDelegate *m_pProjectionDelegate;
    
    // EGLViewProtocol will recreate stats labels to fit visible rect
    friend class EGLViewProtocol;
};

/** 
 @brief DisplayLinkDirector is a Director that synchronizes timers with the refresh rate of the display.
 
 Features and Limitations:
  - Scheduled timers & drawing are synchronizes with the refresh rate of the display
  - Only supports animation intervals of 1/60 1/30 & 1/15
 
 @since v0.8.2
 @js NA
 @lua NA
 */
class DisplayLinkDirector : public Director
{
public:
    DisplayLinkDirector();
    virtual ~DisplayLinkDirector();

    virtual void mainLoop();

    virtual void setAnimationInterval(double value);

    virtual void startAnimation(void);
    virtual void stopAnimation();
protected:
    bool _invalid;
};

// end of base_node group
/// @}

NS_AX_END

#endif // __AX_DIRECTOR_H__
