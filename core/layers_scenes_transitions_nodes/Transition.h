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

#ifndef __AXTRANSITION_H__
#define __AXTRANSITION_H__

#include "Scene.h"
#include "base/Types.h"

NS_AX_BEGIN

/**
 * @addtogroup transition
 * @{
 */

//static creation function macro
//c/c++ don't support object creation of using class name
//so, all classes need creation method.

class ActionInterval;
class Node;

/** @brief TransitionEaseScene can ease the actions of the scene protocol.
@since v0.8.2
*/
class AX_DLL TransitionEaseScene// : public Object
{
public:
    /** returns the Ease action that will be performed on a linear action.
    @since v0.8.2
    */
    virtual ActionInterval * easeActionWithAction(ActionInterval * action) = 0;
};

/** Orientation Type used by some transitions
*/
typedef enum {
    /// An horizontal orientation where the Left is nearer
	kCCTransitionOrientationLeftOver = 0,
	/// An horizontal orientation where the Right is nearer
	kCCTransitionOrientationRightOver = 1,
	/// A vertical orientation where the Up is nearer
	kCCTransitionOrientationUpOver = 0,
	/// A vertical orientation where the Bottom is nearer
	kCCTransitionOrientationDownOver = 1,
    
	// Deprecated
    //	kOrientationLeftOver = kCCTransitionOrientationLeftOver,
    //	kOrientationRightOver = kCCTransitionOrientationRightOver,
    //	kOrientationUpOver = kCCTransitionOrientationUpOver,
    //	kOrientationDownOver = kCCTransitionOrientationDownOver,
} tOrientation;

/** @brief Base class for CCTransition scenes
*/
class AX_DLL TransitionScene : public Scene
{

protected:
    Scene    * m_pInScene;
    Scene    * m_pOutScene;
    float    m_fDuration;
    bool    m_bIsInSceneOnTop;
    bool    m_bIsSendCleanupToScene;

public:
    /**
     *  @js ctor
     */
    TransitionScene();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionScene();
    virtual void draw();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onExit();
    virtual void cleanup();

    /** creates a base transition with duration and incoming scene */
    static TransitionScene * create(float t, Scene *scene);

    /** initializes a transition with duration and incoming scene */
    virtual bool initWithDuration(float t,Scene* scene);

    /** called after the transition finishes */
    void finish(void);

    /** used by some transitions to hide the outer scene */
    void hideOutShowIn(void);

protected:
    virtual void sceneOrder();
private:
    void setNewScene(float dt);

};

/** @brief A CCTransition that supports orientation like.
* Possible orientation: LeftOver, RightOver, UpOver, DownOver
*/
class AX_DLL TransitionSceneOriented : public TransitionScene
{
protected:
    tOrientation m_eOrientation;

public:
    /**
     *  @js ctor
     */
    TransitionSceneOriented();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionSceneOriented();

    /** creates a base transition with duration and incoming scene */
    static TransitionSceneOriented * create(float t,Scene* scene, tOrientation orientation);

    /** initializes a transition with duration and incoming scene */
    virtual bool initWithDuration(float t,Scene* scene,tOrientation orientation);
};

/** @brief TransitionRotoZoom:
Rotate and zoom out the outgoing scene, and then rotate and zoom in the incoming 
*/
class AX_DLL TransitionRotoZoom : public TransitionScene
{
public:
    /**
     *  @js ctor
     */
    TransitionRotoZoom();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionRotoZoom();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();

    static TransitionRotoZoom* create(float t, Scene* scene);
};

/** @brief TransitionJumpZoom:
Zoom out and jump the outgoing scene, and then jump and zoom in the incoming 
*/
class AX_DLL TransitionJumpZoom : public TransitionScene
{
public:
    /**
     *  @js ctor
     */
    TransitionJumpZoom();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionJumpZoom();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();

    static TransitionJumpZoom* create(float t, Scene* scene);
};

/** @brief TransitionMoveInL:
Move in from to the left the incoming scene.
*/
class AX_DLL TransitionMoveInL : public TransitionScene, public TransitionEaseScene
{
public:
    /**
     *  @js ctor
     */
    TransitionMoveInL();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionMoveInL();
    /** initializes the scenes */
    virtual void initScenes(void);
    /** returns the action that will be performed */
    virtual ActionInterval* action(void);

    virtual ActionInterval* easeActionWithAction(ActionInterval * action);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();

    static TransitionMoveInL* create(float t, Scene* scene);
};

/** @brief TransitionMoveInR:
Move in from to the right the incoming scene.
*/
class AX_DLL TransitionMoveInR : public TransitionMoveInL
{
public:
    /**
     *  @js ctor
     */
    TransitionMoveInR();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionMoveInR();
    virtual void initScenes();

    static TransitionMoveInR* create(float t, Scene* scene);
};

/** @brief TransitionMoveInT:
Move in from to the top the incoming scene.
*/
class AX_DLL TransitionMoveInT : public TransitionMoveInL 
{
public:
    /**
     *  @js ctor
     */
    TransitionMoveInT();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionMoveInT();
    virtual void initScenes();

    static TransitionMoveInT* create(float t, Scene* scene);
};

/** @brief TransitionMoveInB:
Move in from to the bottom the incoming scene.
*/
class AX_DLL TransitionMoveInB : public TransitionMoveInL
{
public:
    /**
     *  @js ctor
     */
    TransitionMoveInB();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionMoveInB();
    virtual void initScenes();

    static TransitionMoveInB* create(float t, Scene* scene);
};

/** @brief TransitionSlideInL:
Slide in the incoming scene from the left border.
*/
class AX_DLL TransitionSlideInL : public TransitionScene, public TransitionEaseScene
{
public:
    /**
     *  @js ctor
     */
    TransitionSlideInL();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionSlideInL();

    /** initializes the scenes */
    virtual void initScenes(void);
    /** returns the action that will be performed by the incoming and outgoing scene */
    virtual ActionInterval* action(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();
    
    virtual ActionInterval* easeActionWithAction(ActionInterval * action);

    static TransitionSlideInL* create(float t, Scene* scene);
protected:
    virtual void sceneOrder();
};

/** @brief TransitionSlideInR:
Slide in the incoming scene from the right border.
*/
class AX_DLL TransitionSlideInR : public TransitionSlideInL 
{
public:
    /**
     *  @js ctor
     */
    TransitionSlideInR();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionSlideInR();

    /** initializes the scenes */
    virtual void initScenes(void);
    /** returns the action that will be performed by the incoming and outgoing scene */
    virtual ActionInterval* action(void);

    static TransitionSlideInR* create(float t, Scene* scene);
protected:
    virtual void sceneOrder();
};

/** @brief TransitionSlideInB:
Slide in the incoming scene from the bottom border.
*/
class AX_DLL TransitionSlideInB : public TransitionSlideInL
{
public:
    /**
     *  @js ctor
     */
    TransitionSlideInB();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionSlideInB();

    /** initializes the scenes */
    virtual void initScenes(void);
    /** returns the action that will be performed by the incoming and outgoing scene */
    virtual ActionInterval* action(void);

    static TransitionSlideInB* create(float t, Scene* scene);
protected: 
    virtual void sceneOrder();
};

/** @brief TransitionSlideInT:
Slide in the incoming scene from the top border.
*/
class AX_DLL TransitionSlideInT : public TransitionSlideInL
{
public:
    /**
     *  @js ctor
     */
    TransitionSlideInT();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionSlideInT();

    /** initializes the scenes */
    virtual void initScenes(void);
    /** returns the action that will be performed by the incoming and outgoing scene */
    virtual ActionInterval* action(void);

    static TransitionSlideInT* create(float t, Scene* scene);
protected:
    virtual void sceneOrder();
};

/**
@brief Shrink the outgoing scene while grow the incoming scene
*/
class AX_DLL TransitionShrinkGrow : public TransitionScene , public TransitionEaseScene
{
public:
    /**
     *  @js ctor
     */
    TransitionShrinkGrow();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionShrinkGrow();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();
    virtual ActionInterval* easeActionWithAction(ActionInterval * action);

    static TransitionShrinkGrow* create(float t, Scene* scene);
};

/** @brief TransitionFlipX:
Flips the screen horizontally.
The front face is the outgoing scene and the back face is the incoming scene.
*/
class AX_DLL TransitionFlipX : public TransitionSceneOriented
{
public:
    /**
     *  @js ctor
     */
    TransitionFlipX();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionFlipX();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();

    static TransitionFlipX* create(float t, Scene* s, tOrientation o);
    static TransitionFlipX* create(float t, Scene* s);
};

/** @brief TransitionFlipY:
Flips the screen vertically.
The front face is the outgoing scene and the back face is the incoming scene.
*/
class AX_DLL TransitionFlipY : public TransitionSceneOriented
{
public:
    /**
     *  @js ctor
     */
    TransitionFlipY();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionFlipY();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();

    static TransitionFlipY* create(float t, Scene* s, tOrientation o);
    static TransitionFlipY* create(float t, Scene* s);
};

/** @brief TransitionFlipAngular:
Flips the screen half horizontally and half vertically.
The front face is the outgoing scene and the back face is the incoming scene.
*/
class AX_DLL TransitionFlipAngular : public TransitionSceneOriented
{
public:
    /**
     *  @js ctor
     */
    TransitionFlipAngular();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionFlipAngular();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();
    
    static TransitionFlipAngular* create(float t, Scene* s, tOrientation o);
    static TransitionFlipAngular* create(float t, Scene* s);
};

/** @brief TransitionZoomFlipX:
Flips the screen horizontally doing a zoom out/in
The front face is the outgoing scene and the back face is the incoming scene.
*/
class AX_DLL TransitionZoomFlipX : public TransitionSceneOriented
{
public:
    /**
     *  @js ctor
     */
    TransitionZoomFlipX();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionZoomFlipX();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();

    static TransitionZoomFlipX* create(float t, Scene* s, tOrientation o);
    static TransitionZoomFlipX* create(float t, Scene* s);
};

/** @brief TransitionZoomFlipY:
Flips the screen vertically doing a little zooming out/in
The front face is the outgoing scene and the back face is the incoming scene.
*/
class AX_DLL TransitionZoomFlipY : public TransitionSceneOriented
{
public:
    /**
     *  @js ctor
     */
    TransitionZoomFlipY();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionZoomFlipY();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();

    static TransitionZoomFlipY* create(float t, Scene* s, tOrientation o);
    static TransitionZoomFlipY* create(float t, Scene* s);
};

/** @brief TransitionZoomFlipAngular:
Flips the screen half horizontally and half vertically doing a little zooming out/in.
The front face is the outgoing scene and the back face is the incoming scene.
*/
class AX_DLL TransitionZoomFlipAngular : public TransitionSceneOriented
{
public:
    /**
     *  @js ctor
     */
    TransitionZoomFlipAngular();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionZoomFlipAngular();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();

    static TransitionZoomFlipAngular* create(float t, Scene* s, tOrientation o);
    static TransitionZoomFlipAngular* create(float t, Scene* s);
};

/** @brief TransitionFade:
Fade out the outgoing scene and then fade in the incoming scene.'''
*/
class AX_DLL TransitionFade : public TransitionScene
{
protected:
    ccColor4B    m_tColor;

public:
    /**
     *  @js ctor
     */
    TransitionFade();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionFade();
    
    /** creates the transition with a duration and with an RGB color
    * Example: FadeTransition::create(2, scene, ccc3(255,0,0); // red color
    */
    static TransitionFade* create(float duration,Scene* scene, const ccColor3B& color);
    static TransitionFade* create(float duration,Scene* scene);

    /** initializes the transition with a duration and with an RGB color */
    virtual bool initWithDuration(float t, Scene*scene ,const ccColor3B& color);

    virtual bool initWithDuration(float t,Scene* scene);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onExit();
};

class RenderTexture;
/**
@brief TransitionCrossFade:
Cross fades two scenes using the RenderTexture object.
*/
class AX_DLL TransitionCrossFade : public TransitionScene
{
public :
    /**
     *  @js ctor
     */
    TransitionCrossFade();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionCrossFade();

    virtual void draw();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onExit();

public:
    static TransitionCrossFade* create(float t, Scene* scene);
};

/** @brief TransitionTurnOffTiles:
Turn off the tiles of the outgoing scene in random order
*/
class AX_DLL TransitionTurnOffTiles : public TransitionScene ,public TransitionEaseScene
{
public :
    /**
     *  @js ctor
     */
    TransitionTurnOffTiles();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionTurnOffTiles();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();
    virtual ActionInterval * easeActionWithAction(ActionInterval * action);

public:
    static TransitionTurnOffTiles* create(float t, Scene* scene);
protected:
    virtual void sceneOrder();
};

/** @brief TransitionSplitCols:
The odd columns goes upwards while the even columns goes downwards.
*/
class AX_DLL TransitionSplitCols : public TransitionScene , public TransitionEaseScene
{
public:
    /**
     *  @js ctor
     */
    TransitionSplitCols();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionSplitCols();

    virtual ActionInterval* action(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();
    virtual ActionInterval * easeActionWithAction(ActionInterval * action);

public:

    static TransitionSplitCols* create(float t, Scene* scene);
};

/** @brief TransitionSplitRows:
The odd rows goes to the left while the even rows goes to the right.
*/
class AX_DLL TransitionSplitRows : public TransitionSplitCols
{
public:
    /**
     *  @js ctor
     */
    TransitionSplitRows();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionSplitRows();

    virtual ActionInterval* action(void);

public:

    static TransitionSplitRows* create(float t, Scene* scene);
};

/** @brief TransitionFadeTR:
Fade the tiles of the outgoing scene from the left-bottom corner the to top-right corner.
*/
class AX_DLL TransitionFadeTR : public TransitionScene , public TransitionEaseScene
{
public:
    /**
     *  @js ctor
     */
    TransitionFadeTR();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionFadeTR();
    virtual ActionInterval* actionWithSize(const Size& size);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();
    virtual ActionInterval* easeActionWithAction(ActionInterval * action);

public:

    static TransitionFadeTR* create(float t, Scene* scene);
protected:
    virtual void sceneOrder();
};

/** @brief TransitionFadeBL:
Fade the tiles of the outgoing scene from the top-right corner to the bottom-left corner.
*/
class AX_DLL TransitionFadeBL : public TransitionFadeTR
{
public:
    /**
     *  @js ctor
     */
    TransitionFadeBL();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionFadeBL();
    virtual ActionInterval* actionWithSize(const Size& size);

public:

    static TransitionFadeBL* create(float t, Scene* scene);
};

/** @brief TransitionFadeUp:
* Fade the tiles of the outgoing scene from the bottom to the top.
*/
class AX_DLL TransitionFadeUp : public TransitionFadeTR
{
public:
    /**
     *  @js ctor
     */
    TransitionFadeUp();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionFadeUp();
    virtual ActionInterval* actionWithSize(const Size& size);

public:

    static TransitionFadeUp* create(float t, Scene* scene);
};

/** @brief TransitionFadeDown:
* Fade the tiles of the outgoing scene from the top to the bottom.
*/
class AX_DLL TransitionFadeDown : public TransitionFadeTR
{
public:
    /**
     *  @js ctor
     */
    TransitionFadeDown();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~TransitionFadeDown();
    virtual ActionInterval* actionWithSize(const Size& size);

public:

    static TransitionFadeDown* create(float t, Scene* scene);
};

// end of transition group
/// @}

NS_AX_END

#endif // __AXTRANSITION_H__

