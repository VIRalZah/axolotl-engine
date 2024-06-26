/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2009 Lam Pham
Copyright (c) 2012 Ricardo Quesada

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

#ifndef __AXTRANSITIONPROGRESS_H__
#define __AXTRANSITIONPROGRESS_H__

#include "Transition.h"

NS_AX_BEGIN

class ProgressTimer;
class RenderTexture;

/**
 * @addtogroup transition
 * @{
 */

class AX_DLL TransitionProgress : public TransitionScene
{
public:
    static TransitionProgress* create(float t, Scene* scene);
    /**
     *  @js ctor
     */
    TransitionProgress();
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
protected:
    virtual ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture);
    virtual void setupTransition();
    virtual void sceneOrder();
    float m_fTo;
    float m_fFrom;
    Scene* m_pSceneToBeModified;
};


/** CCTransitionRadialCCW transition.
 A counter clock-wise radial transition to the next scene
 */
class AX_DLL TransitionProgressRadialCCW : public TransitionProgress
{
public:
    static TransitionProgressRadialCCW* create(float t, Scene* scene);
protected:
    virtual ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture);
  
};


/** CCTransitionRadialCW transition.
 A counter clock-wise radial transition to the next scene
*/
class AX_DLL TransitionProgressRadialCW : public TransitionProgress
{
public:
    static TransitionProgressRadialCW* create(float t, Scene* scene);
protected:
    virtual ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture);

};

/** TransitionProgressHorizontal transition.
 A  clock-wise radial transition to the next scene
 */
class AX_DLL TransitionProgressHorizontal : public TransitionProgress
{
public:

    static TransitionProgressHorizontal* create(float t, Scene* scene);
protected:
    virtual ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture);

};

class AX_DLL TransitionProgressVertical : public TransitionProgress
{
public:

    static TransitionProgressVertical* create(float t, Scene* scene);
protected:
    virtual ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture);

};

class AX_DLL TransitionProgressInOut : public TransitionProgress
{
public:

    static TransitionProgressInOut* create(float t, Scene* scene);
protected:
    virtual ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture);
    virtual void sceneOrder();
    virtual void setupTransition();
};

class AX_DLL TransitionProgressOutIn : public TransitionProgress
{
public:

    static TransitionProgressOutIn* create(float t, Scene* scene);
protected:
    virtual ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture);

};

// end of transition group
/// @}

NS_AX_END

#endif /* __AXTRANSITIONPROGRESS_H__ */

