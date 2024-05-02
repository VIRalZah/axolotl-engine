/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2009      Jason Booth
Copyright (c) 2009      Robert J Payne
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

#include "cocoa/NS.h"
#include "ccMacros.h"
#include "textures/TextureCache.h"
#include "SpriteFrameCache.h"
#include "SpriteFrame.h"
#include "Sprite.h"
#include "support/TransformUtils.h"
#include "platform/FileUtils.h"
#include "cocoa/String.h"
#include "cocoa/Array.h"
#include "cocoa/Dictionary.h"
#include <vector>

using namespace std;

NS_AX_BEGIN

static SpriteFrameCache *pSharedSpriteFrameCache = NULL;

SpriteFrameCache* SpriteFrameCache::sharedSpriteFrameCache(void)
{
    if (! pSharedSpriteFrameCache)
    {
        pSharedSpriteFrameCache = new SpriteFrameCache();
        pSharedSpriteFrameCache->init();
    }

    return pSharedSpriteFrameCache;
}

void SpriteFrameCache::purgeSharedSpriteFrameCache(void)
{
    AX_SAFE_RELEASE_NULL(pSharedSpriteFrameCache);
}

bool SpriteFrameCache::init(void)
{
    m_pSpriteFrames= new Dictionary();
    m_pSpriteFramesAliases = new Dictionary();
    m_pLoadedFileNames = new std::set<std::string>();
    return true;
}

SpriteFrameCache::~SpriteFrameCache(void)
{
    AX_SAFE_RELEASE(m_pSpriteFrames);
    AX_SAFE_RELEASE(m_pSpriteFramesAliases);
    AX_SAFE_DELETE(m_pLoadedFileNames);
}

void SpriteFrameCache::addSpriteFramesWithDictionary(Dictionary* dictionary, Texture2D *pobTexture)
{
    /*
    Supported Zwoptex Formats:

    ZWTCoordinatesFormatOptionXMLLegacy = 0, // Flash Version
    ZWTCoordinatesFormatOptionXML1_0 = 1, // Desktop Version 0.0 - 0.4b
    ZWTCoordinatesFormatOptionXML1_1 = 2, // Desktop Version 1.0.0 - 1.0.1
    ZWTCoordinatesFormatOptionXML1_2 = 3, // Desktop Version 1.0.2+
    */

    Dictionary *metadataDict = (Dictionary*)dictionary->objectForKey("metadata");
    Dictionary *framesDict = (Dictionary*)dictionary->objectForKey("frames");
    int format = 0;

    // get the format
    if(metadataDict != NULL) 
    {
        format = metadataDict->valueForKey("format")->intValue();
    }

    // check the format
    AXAssert(format >=0 && format <= 3, "format is not supported for SpriteFrameCache addSpriteFramesWithDictionary:textureFilename:");

    DictElement* pElement = NULL;
    CCDICT_FOREACH(framesDict, pElement)
    {
        Dictionary* frameDict = (Dictionary*)pElement->getObject();
        std::string spriteFrameName = pElement->getStrKey();
        SpriteFrame* spriteFrame = (SpriteFrame*)m_pSpriteFrames->objectForKey(spriteFrameName);
        if (spriteFrame)
        {
            continue;
        }
        
        if(format == 0) 
        {
            float x = frameDict->valueForKey("x")->floatValue();
            float y = frameDict->valueForKey("y")->floatValue();
            float w = frameDict->valueForKey("width")->floatValue();
            float h = frameDict->valueForKey("height")->floatValue();
            float ox = frameDict->valueForKey("offsetX")->floatValue();
            float oy = frameDict->valueForKey("offsetY")->floatValue();
            int ow = frameDict->valueForKey("originalWidth")->intValue();
            int oh = frameDict->valueForKey("originalHeight")->intValue();
            // check ow/oh
            if(!ow || !oh)
            {
                AXLOGWARN("cocos2d: WARNING: originalWidth/Height not found on the SpriteFrame. AnchorPoint won't work as expected. Regenrate the .plist");
            }
            // abs ow/oh
            ow = abs(ow);
            oh = abs(oh);
            // create frame
            spriteFrame = new SpriteFrame();
            spriteFrame->initWithTexture(pobTexture, 
                                        CCRectMake(x, y, w, h), 
                                        false,
                                        Point(ox, oy),
                                        CCSizeMake((float)ow, (float)oh)
                                        );
        } 
        else if(format == 1 || format == 2) 
        {
            Rect frame = CCRectFromString(frameDict->valueForKey("frame")->getCString());
            bool rotated = false;

            // rotation
            if (format == 2)
            {
                rotated = frameDict->valueForKey("rotated")->boolValue();
            }

            Point offset = CCPointFromString(frameDict->valueForKey("offset")->getCString());
            Size sourceSize = CCSizeFromString(frameDict->valueForKey("sourceSize")->getCString());

            // create frame
            spriteFrame = new SpriteFrame();
            spriteFrame->initWithTexture(pobTexture, 
                frame,
                rotated,
                offset,
                sourceSize
                );
        } 
        else if (format == 3)
        {
            // get values
            Size spriteSize = CCSizeFromString(frameDict->valueForKey("spriteSize")->getCString());
            Point spriteOffset = CCPointFromString(frameDict->valueForKey("spriteOffset")->getCString());
            Size spriteSourceSize = CCSizeFromString(frameDict->valueForKey("spriteSourceSize")->getCString());
            Rect textureRect = CCRectFromString(frameDict->valueForKey("textureRect")->getCString());
            bool textureRotated = frameDict->valueForKey("textureRotated")->boolValue();

            // get aliases
            Array* aliases = (Array*) (frameDict->objectForKey("aliases"));
            String * frameKey = new String(spriteFrameName);

            Object* pObj = NULL;
            AXARRAY_FOREACH(aliases, pObj)
            {
                std::string oneAlias = ((String*)pObj)->getCString();
                if (m_pSpriteFramesAliases->objectForKey(oneAlias.c_str()))
                {
                    AXLOGWARN("cocos2d: WARNING: an alias with name %s already exists", oneAlias.c_str());
                }

                m_pSpriteFramesAliases->setObject(frameKey, oneAlias.c_str());
            }
            frameKey->release();
            // create frame
            spriteFrame = new SpriteFrame();
            spriteFrame->initWithTexture(pobTexture,
                            CCRectMake(textureRect.origin.x, textureRect.origin.y, spriteSize.width, spriteSize.height),
                            textureRotated,
                            spriteOffset,
                            spriteSourceSize);
        }

        // add sprite frame
        m_pSpriteFrames->setObject(spriteFrame, spriteFrameName);
        spriteFrame->release();
    }
}

void SpriteFrameCache::addSpriteFramesWithFile(const char *pszPlist, Texture2D *pobTexture)
{
    if (m_pLoadedFileNames->find(pszPlist) != m_pLoadedFileNames->end())
    {
        return;//We already added it
    }
    std::string fullPath = FileUtils::sharedFileUtils()->fullPathForFilename(pszPlist);
    Dictionary *dict = Dictionary::createWithContentsOfFileThreadSafe(fullPath.c_str());

    addSpriteFramesWithDictionary(dict, pobTexture);
    m_pLoadedFileNames->insert(pszPlist);

    dict->release();
}

void SpriteFrameCache::addSpriteFramesWithFile(const char* plist, const char* textureFileName)
{
    AXAssert(textureFileName, "texture name should not be null");
    Texture2D *texture = TextureCache::sharedTextureCache()->addImage(textureFileName);

    if (texture)
    {
        addSpriteFramesWithFile(plist, texture);
    }
    else
    {
        AXLOG("cocos2d: SpriteFrameCache: couldn't load texture file. File not found %s", textureFileName);
    }
}

void SpriteFrameCache::addSpriteFramesWithFile(const char *pszPlist)
{
    AXAssert(pszPlist, "plist filename should not be NULL");

    if (m_pLoadedFileNames->find(pszPlist) == m_pLoadedFileNames->end())
    {
        std::string fullPath = FileUtils::sharedFileUtils()->fullPathForFilename(pszPlist);
        Dictionary *dict = Dictionary::createWithContentsOfFileThreadSafe(fullPath.c_str());

        string texturePath("");

        Dictionary* metadataDict = (Dictionary*)dict->objectForKey("metadata");
        if (metadataDict)
        {
            // try to read  texture file name from meta data
            texturePath = metadataDict->valueForKey("textureFileName")->getCString();
        }

        if (! texturePath.empty())
        {
            // build texture path relative to plist file
            texturePath = FileUtils::sharedFileUtils()->fullPathFromRelativeFile(texturePath.c_str(), pszPlist);
        }
        else
        {
            // build texture path by replacing file extension
            texturePath = pszPlist;

            // remove .xxx
            size_t startPos = texturePath.find_last_of("."); 
            texturePath = texturePath.erase(startPos);

            // append .png
            texturePath = texturePath.append(".png");

            AXLOG("cocos2d: SpriteFrameCache: Trying to use file %s as texture", texturePath.c_str());
        }

        Texture2D *pTexture = TextureCache::sharedTextureCache()->addImage(texturePath.c_str());

        if (pTexture)
        {
            addSpriteFramesWithDictionary(dict, pTexture);
            m_pLoadedFileNames->insert(pszPlist);
        }
        else
        {
            AXLOG("cocos2d: SpriteFrameCache: Couldn't load texture");
        }

        dict->release();
    }

}

void SpriteFrameCache::addSpriteFrame(SpriteFrame *pobFrame, const char *pszFrameName)
{
    m_pSpriteFrames->setObject(pobFrame, pszFrameName);
}

void SpriteFrameCache::removeSpriteFrames(void)
{
    m_pSpriteFrames->removeAllObjects();
    m_pSpriteFramesAliases->removeAllObjects();
    m_pLoadedFileNames->clear();
}

void SpriteFrameCache::removeUnusedSpriteFrames(void)
{
    bool bRemoved = false;
    DictElement* pElement = NULL;
    CCDICT_FOREACH(m_pSpriteFrames, pElement)
    {
        SpriteFrame* spriteFrame = (SpriteFrame*)pElement->getObject();
        if( spriteFrame->retainCount() == 1 ) 
        {
            AXLOG("cocos2d: SpriteFrameCache: removing unused frame: %s", pElement->getStrKey());
            m_pSpriteFrames->removeObjectForElememt(pElement);
            bRemoved = true;
        }
    }

    // XXX. Since we don't know the .plist file that originated the frame, we must remove all .plist from the cache
    if( bRemoved )
    {
        m_pLoadedFileNames->clear();
    }
}


void SpriteFrameCache::removeSpriteFrameByName(const char *pszName)
{
    // explicit nil handling
    if( ! pszName )
    {
        return;
    }

    // Is this an alias ?
    String* key = (String*)m_pSpriteFramesAliases->objectForKey(pszName);

    if (key)
    {
        m_pSpriteFrames->removeObjectForKey(key->getCString());
        m_pSpriteFramesAliases->removeObjectForKey(key->getCString());
    }
    else
    {
        m_pSpriteFrames->removeObjectForKey(pszName);
    }

    // XXX. Since we don't know the .plist file that originated the frame, we must remove all .plist from the cache
    m_pLoadedFileNames->clear();
}

void SpriteFrameCache::removeSpriteFramesFromFile(const char* plist)
{
    std::string fullPath = FileUtils::sharedFileUtils()->fullPathForFilename(plist);
    Dictionary* dict = Dictionary::createWithContentsOfFileThreadSafe(fullPath.c_str());

    removeSpriteFramesFromDictionary((Dictionary*)dict);

    // remove it from the cache
    set<string>::iterator ret = m_pLoadedFileNames->find(plist);
    if (ret != m_pLoadedFileNames->end())
    {
        m_pLoadedFileNames->erase(ret);
    }

    dict->release();
}

void SpriteFrameCache::removeSpriteFramesFromDictionary(Dictionary* dictionary)
{
    Dictionary* framesDict = (Dictionary*)dictionary->objectForKey("frames");
    Array* keysToRemove = Array::create();

    DictElement* pElement = NULL;
    CCDICT_FOREACH(framesDict, pElement)
    {
        if (m_pSpriteFrames->objectForKey(pElement->getStrKey()))
        {
            keysToRemove->addObject(String::create(pElement->getStrKey()));
        }
    }

    m_pSpriteFrames->removeObjectsForKeys(keysToRemove);
}

void SpriteFrameCache::removeSpriteFramesFromTexture(Texture2D* texture)
{
    Array* keysToRemove = Array::create();

    DictElement* pElement = NULL;
    CCDICT_FOREACH(m_pSpriteFrames, pElement)
    {
        string key = pElement->getStrKey();
        SpriteFrame* frame = (SpriteFrame*)m_pSpriteFrames->objectForKey(key.c_str());
        if (frame && (frame->getTexture() == texture))
        {
            keysToRemove->addObject(String::create(pElement->getStrKey()));
        }
    }

    m_pSpriteFrames->removeObjectsForKeys(keysToRemove);
}

SpriteFrame* SpriteFrameCache::spriteFrameByName(const char *pszName)
{
    SpriteFrame* frame = (SpriteFrame*)m_pSpriteFrames->objectForKey(pszName);
    if (!frame)
    {
        // try alias dictionary
        String *key = (String*)m_pSpriteFramesAliases->objectForKey(pszName);  
        if (key)
        {
            frame = (SpriteFrame*)m_pSpriteFrames->objectForKey(key->getCString());
            if (! frame)
            {
                AXLOG("cocos2d: SpriteFrameCache: Frame '%s' not found", pszName);
            }
        }
    }
    return frame;
}

NS_AX_END
