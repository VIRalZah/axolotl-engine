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
#include "Animation.h"
#include "textures/TextureCache.h"
#include "textures/Texture2D.h"
#include "ccMacros.h"
#include "sprite_nodes/SpriteFrame.h"
#include "base/Zone.h"

NS_AX_BEGIN

AnimationFrame::AnimationFrame()
: m_pSpriteFrame(NULL)
, m_fDelayUnits(0.0f)
, m_pUserInfo(NULL)
{

}

bool AnimationFrame::initWithSpriteFrame(SpriteFrame* spriteFrame, float delayUnits, Dictionary* userInfo)
{
    setSpriteFrame(spriteFrame);
    setDelayUnits(delayUnits);
    setUserInfo(userInfo);

    return true;
}

AnimationFrame::~AnimationFrame()
{    
    AXLOGINFO( "cocos2d: deallocing %s", this);

    AX_SAFE_RELEASE(m_pSpriteFrame);
    AX_SAFE_RELEASE(m_pUserInfo);
}

Object* AnimationFrame::copyWithZone(Zone* pZone)
{
    Zone* pNewZone = NULL;
    AnimationFrame* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (AnimationFrame*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new AnimationFrame();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithSpriteFrame((SpriteFrame*)m_pSpriteFrame->copy()->autorelease(),
        m_fDelayUnits, m_pUserInfo != NULL ? (Dictionary*)m_pUserInfo->copy()->autorelease() : NULL);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

// implementation of Animation

Animation* Animation::create(void)
{
    Animation *pAnimation = new Animation();
    pAnimation->init();
    pAnimation->autorelease();

    return pAnimation;
} 

Animation* Animation::createWithSpriteFrames(Array *frames, float delay/* = 0.0f*/)
{
    Animation *pAnimation = new Animation();
    pAnimation->initWithSpriteFrames(frames, delay);
    pAnimation->autorelease();

    return pAnimation;
}

Animation* Animation::create(Array* arrayOfAnimationFrameNames, float delayPerUnit, unsigned int loops)
{
    Animation *pAnimation = new Animation();
    pAnimation->initWithAnimationFrames(arrayOfAnimationFrameNames, delayPerUnit, loops);
    pAnimation->autorelease();
    return pAnimation;
}

bool Animation::init()
{
    return initWithSpriteFrames(NULL, 0.0f);
}

bool Animation::initWithSpriteFrames(Array *pFrames, float delay/* = 0.0f*/)
{
    AXARRAY_VERIFY_TYPE(pFrames, SpriteFrame*);

    m_uLoops = 1;
    m_fDelayPerUnit = delay;
    Array* pTmpFrames = Array::create();
    setFrames(pTmpFrames);

    if (pFrames != NULL)
    {
        Object* pObj = NULL;
        AXARRAY_FOREACH(pFrames, pObj)
        {
            SpriteFrame* frame = (SpriteFrame*)pObj;
            AnimationFrame *animFrame = new AnimationFrame();
            animFrame->initWithSpriteFrame(frame, 1, NULL);
            m_pFrames->addObject(animFrame);
            animFrame->release();

            m_fTotalDelayUnits++;
        }
    }

    return true;
}

bool Animation::initWithAnimationFrames(Array* arrayOfAnimationFrames, float delayPerUnit, unsigned int loops)
{
    AXARRAY_VERIFY_TYPE(arrayOfAnimationFrames, AnimationFrame*);

    m_fDelayPerUnit = delayPerUnit;
    m_uLoops = loops;

    setFrames(Array::createWithArray(arrayOfAnimationFrames));

    Object* pObj = NULL;
    AXARRAY_FOREACH(m_pFrames, pObj)
    {
        AnimationFrame* animFrame = (AnimationFrame*)pObj;
        m_fTotalDelayUnits += animFrame->getDelayUnits();
    }
    return true;
}

Animation::Animation()
: m_fTotalDelayUnits(0.0f)
, m_fDelayPerUnit(0.0f)
, m_fDuration(0.0f)
, m_pFrames(NULL)
, m_bRestoreOriginalFrame(false)
, m_uLoops(0)
{

}

Animation::~Animation(void)
{
    AXLOGINFO("cocos2d, deallocing %p", this);
    AX_SAFE_RELEASE(m_pFrames);
}

void Animation::addSpriteFrame(SpriteFrame *pFrame)
{
    AnimationFrame *animFrame = new AnimationFrame();
    animFrame->initWithSpriteFrame(pFrame, 1.0f, NULL);
    m_pFrames->addObject(animFrame);
    animFrame->release();

    // update duration
    m_fTotalDelayUnits++;
}

void Animation::addSpriteFrameWithFileName(const char *pszFileName)
{
    Texture2D *pTexture = TextureCache::sharedTextureCache()->addImage(pszFileName);
    Rect rect = Rect::ZERO;
    rect.size = pTexture->getContentSize();
    SpriteFrame *pFrame = SpriteFrame::createWithTexture(pTexture, rect);
    addSpriteFrame(pFrame);
}

void Animation::addSpriteFrameWithTexture(Texture2D *pobTexture, const Rect& rect)
{
    SpriteFrame *pFrame = SpriteFrame::createWithTexture(pobTexture, rect);
    addSpriteFrame(pFrame);
}

float Animation::getDuration(void)
{
    return m_fTotalDelayUnits * m_fDelayPerUnit;
}

Object* Animation::copyWithZone(Zone* pZone)
{
    Zone* pNewZone = NULL;
    Animation* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (Animation*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new Animation();
        pNewZone = new Zone(pCopy);
    }

    pCopy->initWithAnimationFrames(m_pFrames, m_fDelayPerUnit, m_uLoops);
    pCopy->setRestoreOriginalFrame(m_bRestoreOriginalFrame);

    AX_SAFE_DELETE(pNewZone);
    return pCopy;
}

NS_AX_END