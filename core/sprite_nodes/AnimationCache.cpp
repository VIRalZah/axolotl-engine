/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2010      Ricardo Quesada
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
#include "AnimationCache.h"
#include "ccMacros.h"
#include "Animation.h"
#include "SpriteFrame.h"
#include "SpriteFrameCache.h"
#include "base/String.h"
#include "platform/FileUtils.h"

using namespace std;

NS_AX_BEGIN

AnimationCache* AnimationCache::s_pSharedAnimationCache = NULL;

AnimationCache* AnimationCache::sharedAnimationCache(void)
{
    if (! s_pSharedAnimationCache)
    {
        s_pSharedAnimationCache = new AnimationCache();
        s_pSharedAnimationCache->init();
    }

    return s_pSharedAnimationCache;
}

void AnimationCache::purgeSharedAnimationCache(void)
{
    AX_SAFE_RELEASE_NULL(s_pSharedAnimationCache);
}

bool AnimationCache::init()
{
    m_pAnimations = new Dictionary();
    return true;
}

AnimationCache::AnimationCache()
: m_pAnimations(NULL)
{
}

AnimationCache::~AnimationCache()
{
    AXLOGINFO("cocos2d: deallocing %p", this);
    AX_SAFE_RELEASE(m_pAnimations);
}

void AnimationCache::addAnimation(Animation *animation, const char * name)
{
    m_pAnimations->setObject(animation, name);
}

void AnimationCache::removeAnimationByName(const char* name)
{
    if (! name)
    {
        return;
    }

    m_pAnimations->removeObjectForKey(name);
}

Animation* AnimationCache::animationByName(const char* name)
{
    return (Animation*)m_pAnimations->objectForKey(name);
}

void AnimationCache::parseVersion1(Dictionary* animations)
{
    SpriteFrameCache *frameCache = SpriteFrameCache::sharedSpriteFrameCache();

    DictElement* pElement = NULL;
    CCDICT_FOREACH(animations, pElement)
    {
        Dictionary* animationDict = (Dictionary*)pElement->getObject();
        Array* frameNames = (Array*)animationDict->objectForKey("frames");
        float delay = animationDict->valueForKey("delay")->floatValue();
        Animation* animation = NULL;

        if ( frameNames == NULL ) 
        {
            AXLOG("cocos2d: AnimationCache: Animation '%s' found in dictionary without any frames - cannot add to animation cache.", pElement->getStrKey());
            continue;
        }

        Array* frames = Array::createWithCapacity(frameNames->count());
        frames->retain();

        Object* pObj = NULL;
        AXARRAY_FOREACH(frameNames, pObj)
        {
            const char* frameName = ((String*)pObj)->getCString();
            SpriteFrame* spriteFrame = frameCache->spriteFrameByName(frameName);

            if ( ! spriteFrame ) {
                AXLOG("cocos2d: AnimationCache: Animation '%s' refers to frame '%s' which is not currently in the SpriteFrameCache. This frame will not be added to the animation.", pElement->getStrKey(), frameName);

                continue;
            }

            AnimationFrame* animFrame = new AnimationFrame();
            animFrame->initWithSpriteFrame(spriteFrame, 1, NULL);
            frames->addObject(animFrame);
            animFrame->release();
        }

        if ( frames->count() == 0 ) {
            AXLOG("cocos2d: AnimationCache: None of the frames for animation '%s' were found in the SpriteFrameCache. Animation is not being added to the Animation Cache.", pElement->getStrKey());
            continue;
        } else if ( frames->count() != frameNames->count() ) {
            AXLOG("cocos2d: AnimationCache: An animation in your dictionary refers to a frame which is not in the SpriteFrameCache. Some or all of the frames for the animation '%s' may be missing.", pElement->getStrKey());
        }

        animation = Animation::create(frames, delay, 1);

        AnimationCache::sharedAnimationCache()->addAnimation(animation, pElement->getStrKey());
        frames->release();
    }    
}

void AnimationCache::parseVersion2(Dictionary* animations)
{
    SpriteFrameCache *frameCache = SpriteFrameCache::sharedSpriteFrameCache();

    DictElement* pElement = NULL;
    CCDICT_FOREACH(animations, pElement)
    {
        const char* name = pElement->getStrKey();
        Dictionary* animationDict = (Dictionary*)pElement->getObject();

        const String* loops = animationDict->valueForKey("loops");
        bool restoreOriginalFrame = animationDict->valueForKey("restoreOriginalFrame")->boolValue();

        Array* frameArray = (Array*)animationDict->objectForKey("frames");

        if ( frameArray == NULL ) {
            AXLOG("cocos2d: AnimationCache: Animation '%s' found in dictionary without any frames - cannot add to animation cache.", name);
            continue;
        }

        // Array of AnimationFrames
        Array* array = Array::createWithCapacity(frameArray->count());
        array->retain();

        Object* pObj = NULL;
        AXARRAY_FOREACH(frameArray, pObj)
        {
            Dictionary* entry = (Dictionary*)(pObj);

            const char* spriteFrameName = entry->valueForKey("spriteframe")->getCString();
            SpriteFrame *spriteFrame = frameCache->spriteFrameByName(spriteFrameName);

            if( ! spriteFrame ) {
                AXLOG("cocos2d: AnimationCache: Animation '%s' refers to frame '%s' which is not currently in the SpriteFrameCache. This frame will not be added to the animation.", name, spriteFrameName);

                continue;
            }

            float delayUnits = entry->valueForKey("delayUnits")->floatValue();
            Dictionary* userInfo = (Dictionary*)entry->objectForKey("notification");

            AnimationFrame *animFrame = new AnimationFrame();
            animFrame->initWithSpriteFrame(spriteFrame, delayUnits, userInfo);

            array->addObject(animFrame);
            animFrame->release();
        }

        float delayPerUnit = animationDict->valueForKey("delayPerUnit")->floatValue();
        Animation *animation = new Animation();
        animation->initWithAnimationFrames(array, delayPerUnit, 0 != loops->length() ? loops->intValue() : 1);
        array->release();

        animation->setRestoreOriginalFrame(restoreOriginalFrame);

        AnimationCache::sharedAnimationCache()->addAnimation(animation, name);
        animation->release();
    }
}

void AnimationCache::addAnimationsWithDictionary(Dictionary* dictionary,const char* plist)
{
    Dictionary* animations = (Dictionary*)dictionary->objectForKey("animations");

    if ( animations == NULL ) {
        AXLOG("cocos2d: AnimationCache: No animations were found in provided dictionary.");
        return;
    }

    unsigned int version = 1;
    Dictionary* properties = (Dictionary*)dictionary->objectForKey("properties");
    if( properties )
    {
        version = properties->valueForKey("format")->intValue();
        Array* spritesheets = (Array*)properties->objectForKey("spritesheets");

        Object* pObj = NULL;
        AXARRAY_FOREACH(spritesheets, pObj)
        {
            String* name = (String*)(pObj);
            if (plist)
            {
                const char* path = FileUtils::sharedFileUtils()->fullPathFromRelativeFile(name->getCString(),plist);
                SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(path);
            } 
            else
            {
                SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(name->getCString());
            }            
        }
    }

    switch (version) {
        case 1:
            parseVersion1(animations);
            break;
        case 2:
            parseVersion2(animations);
            break;
        default:
            AXAssert(false, "Invalid animation format");
    }
}

/** Read an NSDictionary from a plist file and parse it automatically for animations */
void AnimationCache::addAnimationsWithFile(const char* plist)
{
    AXAssert( plist, "Invalid texture file name");

    std::string path = FileUtils::sharedFileUtils()->fullPathForFilename(plist);
    Dictionary* dict = Dictionary::createWithContentsOfFile(path.c_str());

    AXAssert( dict, "AnimationCache: File could not be found");

    addAnimationsWithDictionary(dict,plist);
}


NS_AX_END