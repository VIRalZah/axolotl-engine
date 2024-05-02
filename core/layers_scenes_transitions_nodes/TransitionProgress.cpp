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


#include "TransitionProgress.h"
#include "base/Director.h"
#include "misc_nodes/RenderTexture.h"
#include "misc_nodes/ProgressTimer.h"
#include "Layer.h"
#include "actions/ActionInstant.h"
#include "actions/ActionProgressTimer.h"
#include "support/PointExtension.h"

NS_AX_BEGIN

enum {
    kCCSceneRadial = 0xc001,
};

TransitionProgress::TransitionProgress()
: m_fTo(0.0f)
, m_fFrom(0.0f)
, m_pSceneToBeModified(NULL)
{

}

TransitionProgress* TransitionProgress::create(float t, Scene* scene)
{
    TransitionProgress* pScene = new TransitionProgress();
    if(pScene && pScene->initWithDuration(t, scene))
    {
        pScene->autorelease();
        return pScene;
    }
    AX_SAFE_DELETE(pScene);
    return NULL;
}

// TransitionProgress
void TransitionProgress::onEnter()
{
    TransitionScene::onEnter();

    setupTransition();
    
    // create a transparent color layer
    // in which we are going to add our rendertextures
    Size size = Director::sharedDirector()->getWinSize();

    // create the second render texture for outScene
    RenderTexture *texture = RenderTexture::create((int)size.width, (int)size.height);
    texture->getSprite()->setAnchorPoint(Point(0.5f,0.5f));
    texture->setPosition(Point(size.width/2, size.height/2));
    texture->setAnchorPoint(Point(0.5f,0.5f));

    // render outScene to its texturebuffer
    texture->clear(0, 0, 0, 1);
    texture->begin();
    m_pSceneToBeModified->visit();
    texture->end();


    //    Since we've passed the outScene to the texture we don't need it.
    if (m_pSceneToBeModified == m_pOutScene)
    {
        hideOutShowIn();
    }
    //    We need the texture in RenderTexture.
    ProgressTimer *pNode = progressTimerNodeWithRenderTexture(texture);

    // create the blend action
    ActionInterval* layerAction = (ActionInterval*)Sequence::create(
        ProgressFromTo::create(m_fDuration, m_fFrom, m_fTo),
        CallFunc::create(this, callfunc_selector(TransitionProgress::finish)), 
        NULL);
    // run the blend action
    pNode->runAction(layerAction);

    // add the layer (which contains our two rendertextures) to the scene
    addChild(pNode, 2, kCCSceneRadial);
}

// clean up on exit
void TransitionProgress::onExit()
{
    // remove our layer and release all containing objects
    removeChildByTag(kCCSceneRadial, true);
    TransitionScene::onExit();
}

void TransitionProgress::sceneOrder()
{
    m_bIsInSceneOnTop = false;
}

void TransitionProgress::setupTransition()
{
    m_pSceneToBeModified = m_pOutScene;
    m_fFrom = 100;
    m_fTo = 0;
}

ProgressTimer* TransitionProgress::progressTimerNodeWithRenderTexture(RenderTexture* texture)
{
    AXAssert(false, "override me - abstract class");
    return NULL;
}


// TransitionProgressRadialCCW

ProgressTimer* TransitionProgressRadialCCW::progressTimerNodeWithRenderTexture(RenderTexture* texture)
{
    Size size = Director::sharedDirector()->getWinSize();

    ProgressTimer* pNode = ProgressTimer::create(texture->getSprite());

    // but it is flipped upside down so we flip the sprite
    pNode->getSprite()->setFlipY(true);
    pNode->setType(kCCProgressTimerTypeRadial);

    //    Return the radial type that we want to use
    pNode->setReverseDirection(false);
    pNode->setPercentage(100);
    pNode->setPosition(Point(size.width/2, size.height/2));
    pNode->setAnchorPoint(Point(0.5f,0.5f));
    
    return pNode;
}

TransitionProgressRadialCCW* TransitionProgressRadialCCW::create(float t, Scene* scene)
{
    TransitionProgressRadialCCW* pScene = new TransitionProgressRadialCCW();
    if(pScene && pScene->initWithDuration(t, scene))
    {
        pScene->autorelease();
        return pScene;
    }
    AX_SAFE_DELETE(pScene);
    return NULL;
}

// TransitionProgressRadialCW
TransitionProgressRadialCW* TransitionProgressRadialCW::create(float t, Scene* scene)
{
    TransitionProgressRadialCW* pScene = new TransitionProgressRadialCW();
    if(pScene && pScene->initWithDuration(t, scene))
    {
        pScene->autorelease();
        return pScene;
    }
    AX_SAFE_DELETE(pScene);
    return NULL;
}

ProgressTimer* TransitionProgressRadialCW::progressTimerNodeWithRenderTexture(RenderTexture* texture)
{
    Size size = Director::sharedDirector()->getWinSize();
    
    ProgressTimer* pNode = ProgressTimer::create(texture->getSprite());
    
    // but it is flipped upside down so we flip the sprite
    pNode->getSprite()->setFlipY(true);
    pNode->setType( kCCProgressTimerTypeRadial );
    
    //    Return the radial type that we want to use
    pNode->setReverseDirection(true);
    pNode->setPercentage(100);
    pNode->setPosition(Point(size.width/2, size.height/2));
    pNode->setAnchorPoint(Point(0.5f,0.5f));
    
    return pNode;
}

// TransitionProgressHorizontal
TransitionProgressHorizontal* TransitionProgressHorizontal::create(float t, Scene* scene)
{
    TransitionProgressHorizontal* pScene = new TransitionProgressHorizontal();
    if(pScene && pScene->initWithDuration(t, scene))
    {
        pScene->autorelease();
        return pScene;
    }
    AX_SAFE_DELETE(pScene);
    return NULL;
}

ProgressTimer* TransitionProgressHorizontal::progressTimerNodeWithRenderTexture(RenderTexture* texture)
{    
    Size size = Director::sharedDirector()->getWinSize();

    ProgressTimer* pNode = ProgressTimer::create(texture->getSprite());
    
    // but it is flipped upside down so we flip the sprite
    pNode->getSprite()->setFlipY(true);
    pNode->setType( kCCProgressTimerTypeBar);
    
    pNode->setMidpoint(Point(1, 0));
    pNode->setBarChangeRate(Point(1,0));
    
    pNode->setPercentage(100);
    pNode->setPosition(Point(size.width/2, size.height/2));
    pNode->setAnchorPoint(Point(0.5f,0.5f));

    return pNode;
}

// TransitionProgressVertical
TransitionProgressVertical* TransitionProgressVertical::create(float t, Scene* scene)
{
    TransitionProgressVertical* pScene = new TransitionProgressVertical();
    if(pScene && pScene->initWithDuration(t, scene))
    {
        pScene->autorelease();
        return pScene;
    }
    AX_SAFE_DELETE(pScene);
    return NULL;
}

ProgressTimer* TransitionProgressVertical::progressTimerNodeWithRenderTexture(RenderTexture* texture)
{    
    Size size = Director::sharedDirector()->getWinSize();
    
    ProgressTimer* pNode = ProgressTimer::create(texture->getSprite());
    
    // but it is flipped upside down so we flip the sprite
    pNode->getSprite()->setFlipY(true);
    pNode->setType(kCCProgressTimerTypeBar);
    
    pNode->setMidpoint(Point(0, 0));
    pNode->setBarChangeRate(Point(0,1));
    
    pNode->setPercentage(100);
    pNode->setPosition(Point(size.width/2, size.height/2));
    pNode->setAnchorPoint(Point(0.5f,0.5f));
    
    return pNode;
}


// TransitionProgressInOut
TransitionProgressInOut* TransitionProgressInOut::create(float t, Scene* scene)
{
    TransitionProgressInOut* pScene = new TransitionProgressInOut();
    if(pScene && pScene->initWithDuration(t, scene))
    {
        pScene->autorelease();
        return pScene;
    }
    AX_SAFE_DELETE(pScene);
    return NULL;
}

void TransitionProgressInOut::sceneOrder()
{
    m_bIsInSceneOnTop = false;
}

void TransitionProgressInOut::setupTransition()
{
    m_pSceneToBeModified = m_pInScene;
    m_fFrom = 0;
    m_fTo = 100;    
}

ProgressTimer* TransitionProgressInOut::progressTimerNodeWithRenderTexture(RenderTexture* texture)
{    
    Size size = Director::sharedDirector()->getWinSize();
    
    ProgressTimer* pNode = ProgressTimer::create(texture->getSprite());
    
    // but it is flipped upside down so we flip the sprite
    pNode->getSprite()->setFlipY(true);
    pNode->setType( kCCProgressTimerTypeBar);
    
    pNode->setMidpoint(Point(0.5f, 0.5f));
    pNode->setBarChangeRate(Point(1, 1));
    
    pNode->setPercentage(0);
    pNode->setPosition(Point(size.width/2, size.height/2));
    pNode->setAnchorPoint(Point(0.5f,0.5f));
    
    return pNode;
}


// TransitionProgressOutIn
TransitionProgressOutIn* TransitionProgressOutIn::create(float t, Scene* scene)
{
    TransitionProgressOutIn* pScene = new TransitionProgressOutIn();
    if(pScene && pScene->initWithDuration(t, scene))
    {
        pScene->autorelease();
        return pScene;
    }
    AX_SAFE_DELETE(pScene);
    return NULL;
}

ProgressTimer* TransitionProgressOutIn::progressTimerNodeWithRenderTexture(RenderTexture* texture)
{    
    Size size = Director::sharedDirector()->getWinSize();
    
    ProgressTimer* pNode = ProgressTimer::create(texture->getSprite());
    
    // but it is flipped upside down so we flip the sprite
    pNode->getSprite()->setFlipY(true);
    pNode->setType( kCCProgressTimerTypeBar );
    
    pNode->setMidpoint(Point(0.5f, 0.5f));
    pNode->setBarChangeRate(Point(1, 1));
    
    pNode->setPercentage(100);
    pNode->setPosition(Point(size.width/2, size.height/2));
    pNode->setAnchorPoint(Point(0.5f,0.5f));
    
    return pNode;
}

NS_AX_END

