/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.
Copyright (c) Microsoft Open Technologies, Inc.

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

#include "TextureCache.h"
#include "Texture2D.h"
#include "ccMacros.h"
#include "base/Director.h"
#include "platform/platform.h"
#include "platform/FileUtils.h"
#include "platform/Thread.h"
#include "platform/Image.h"
#include "support/ccUtils.h"
#include "base/Scheduler.h"
#include "cocoa/String.h"
#include <errno.h>
#include <stack>
#include <string>
#include <cctype>
#include <queue>
#include <list>

#if (AX_TARGET_PLATFORM != AX_PLATFORM_WINRT) && (AX_TARGET_PLATFORM != AX_PLATFORM_WP8)
#include <pthread.h>
#else
#include "PThreadWinRT.h"
#include <ppl.h>
#include <ppltasks.h>
using namespace concurrency;
#endif

using namespace std;

NS_AX_BEGIN

typedef struct _AsyncStruct
{
    std::string            filename;
    Object    *target;
    SEL_CallFuncO        selector;
} AsyncStruct;

typedef struct _ImageInfo
{
    AsyncStruct *asyncStruct;
    Image        *image;
    Image::EImageFormat imageType;
} ImageInfo;

static pthread_t s_loadingThread;

static pthread_mutex_t		s_SleepMutex;
static pthread_cond_t		s_SleepCondition;

static pthread_mutex_t      s_asyncStructQueueMutex;
static pthread_mutex_t      s_ImageInfoMutex;

#ifdef EMSCRIPTEN
// Hack to get ASM.JS validation (no undefined symbols allowed).
#define pthread_cond_signal(_)
#endif // EMSCRIPTEN

static unsigned long s_nAsyncRefCount = 0;

static bool need_quit = false;

static std::queue<AsyncStruct*>* s_pAsyncStructQueue = NULL;

static std::queue<ImageInfo*>*   s_pImageQueue = NULL;


static Image::EImageFormat computeImageFormatType(string& filename)
{
    Image::EImageFormat ret = Image::kFmtUnKnown;

    if ((std::string::npos != filename.find(".jpg")) || (std::string::npos != filename.find(".jpeg")))
    {
        ret = Image::kFmtJpg;
    }
    else if ((std::string::npos != filename.find(".png")) || (std::string::npos != filename.find(".PNG")))
    {
        ret = Image::kFmtPng;
    }
    else if ((std::string::npos != filename.find(".tiff")) || (std::string::npos != filename.find(".TIFF")))
    {
        ret = Image::kFmtTiff;
    }
#if (AX_TARGET_PLATFORM != AX_PLATFORM_WINRT) && (AX_TARGET_PLATFORM != AX_PLATFORM_WP8)
    else if ((std::string::npos != filename.find(".webp")) || (std::string::npos != filename.find(".WEBP")))
    {
        ret = Image::kFmtWebp;
    }
#endif
   
    return ret;
}

static void loadImageData(AsyncStruct *pAsyncStruct)
{
    const char *filename = pAsyncStruct->filename.c_str();

    // compute image type
    Image::EImageFormat imageType = computeImageFormatType(pAsyncStruct->filename);
    if (imageType == Image::kFmtUnKnown)
    {
        AXLOG("unsupported format %s",filename);
        delete pAsyncStruct;
        return;
    }
        
    // generate image            
    Image *pImage = new Image();
    if (pImage && !pImage->initWithImageFileThreadSafe(filename, imageType))
    {
        AX_SAFE_RELEASE(pImage);
        AXLOG("can not load %s", filename);
        return;
    }

    // generate image info
    ImageInfo *pImageInfo = new ImageInfo();
    pImageInfo->asyncStruct = pAsyncStruct;
    pImageInfo->image = pImage;
    pImageInfo->imageType = imageType;
    // put the image info into the queue
    pthread_mutex_lock(&s_ImageInfoMutex);
    s_pImageQueue->push(pImageInfo);
    pthread_mutex_unlock(&s_ImageInfoMutex);   
}

static void* loadImage(void* data)
{
    AsyncStruct *pAsyncStruct = NULL;

    while (true)
    {
        // create autorelease pool for iOS
        Thread thread;
        thread.createAutoreleasePool();

        std::queue<AsyncStruct*> *pQueue = s_pAsyncStructQueue;
        pthread_mutex_lock(&s_asyncStructQueueMutex);// get async struct from queue
        if (pQueue->empty())
        {
            pthread_mutex_unlock(&s_asyncStructQueueMutex);
            if (need_quit) {
                break;
            }
            else {
                pthread_cond_wait(&s_SleepCondition, &s_SleepMutex);
                continue;
            }
        }
        else
        {
            pAsyncStruct = pQueue->front();
            pQueue->pop();
            pthread_mutex_unlock(&s_asyncStructQueueMutex);
            loadImageData(pAsyncStruct);
        }        
    }
    
    if( s_pAsyncStructQueue != NULL )
    {
        delete s_pAsyncStructQueue;
        s_pAsyncStructQueue = NULL;
        delete s_pImageQueue;
        s_pImageQueue = NULL;

        pthread_mutex_destroy(&s_asyncStructQueueMutex);
        pthread_mutex_destroy(&s_ImageInfoMutex);
        pthread_mutex_destroy(&s_SleepMutex);
        pthread_cond_destroy(&s_SleepCondition);
    }
    
    return 0;
}


// implementation TextureCache

// TextureCache - Alloc, Init & Dealloc
static TextureCache *g_sharedTextureCache = NULL;

TextureCache * TextureCache::sharedTextureCache()
{
    if (!g_sharedTextureCache)
    {
        g_sharedTextureCache = new TextureCache();
    }
    return g_sharedTextureCache;
}

TextureCache::TextureCache()
{
    AXAssert(g_sharedTextureCache == NULL, "Attempted to allocate a second instance of a singleton.");
    
    m_pTextures = new Dictionary();
}

TextureCache::~TextureCache()
{
    AXLOGINFO("cocos2d: deallocing TextureCache.");
    need_quit = true;
    pthread_cond_signal(&s_SleepCondition);
    AX_SAFE_RELEASE(m_pTextures);
}

void TextureCache::purgeSharedTextureCache()
{
    AX_SAFE_RELEASE_NULL(g_sharedTextureCache);
}

const char* TextureCache::description()
{
    return String::createWithFormat("<TextureCache | Number of textures = %u>", m_pTextures->count())->getCString();
}

Dictionary* TextureCache::snapshotTextures()
{ 
    Dictionary* pRet = new Dictionary();
    DictElement* pElement = NULL;
    CCDICT_FOREACH(m_pTextures, pElement)
    {
        pRet->setObject(pElement->getObject(), pElement->getStrKey());
    }
    pRet->autorelease();
    return pRet;
}

void TextureCache::addImageAsync(const char *path, Object *target, SEL_CallFuncO selector)
{
#ifdef EMSCRIPTEN
    AXLOGWARN("Cannot load image %s asynchronously in Emscripten builds.", path);
    return;
#endif // EMSCRIPTEN

    AXAssert(path != NULL, "TextureCache: fileimage MUST not be NULL");    

    Texture2D *texture = NULL;

    // optimization

    std::string pathKey = path;

    pathKey = FileUtils::sharedFileUtils()->fullPathForFilename(pathKey.c_str());

    texture = (Texture2D*)m_pTextures->objectForKey(pathKey.c_str());

    std::string fullpath = pathKey;


    if (texture != NULL)
    {
        if (target && selector)
        {
            (target->*selector)(texture);
        }
        
        return;
    }


    // lazy init
    if (s_pAsyncStructQueue == NULL)
    {             
        s_pAsyncStructQueue = new queue<AsyncStruct*>();
        s_pImageQueue = new queue<ImageInfo*>();        
        
        pthread_mutex_init(&s_asyncStructQueueMutex, NULL);
        pthread_mutex_init(&s_ImageInfoMutex, NULL);
        pthread_mutex_init(&s_SleepMutex, NULL);
        pthread_cond_init(&s_SleepCondition, NULL);
#if (AX_TARGET_PLATFORM != AX_PLATFORM_WINRT) && (AX_TARGET_PLATFORM != AX_PLATFORM_WP8)
        pthread_create(&s_loadingThread, NULL, loadImage, NULL);
#endif
        need_quit = false;
    }

    if (0 == s_nAsyncRefCount)
    {
        Director::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(TextureCache::addImageAsyncCallBack), this, 0, false);
    }

    ++s_nAsyncRefCount;

    if (target)
    {
        target->retain();
    }

    // generate async struct
    AsyncStruct *data = new AsyncStruct();
    data->filename = fullpath.c_str();
    data->target = target;
    data->selector = selector;

#if (AX_TARGET_PLATFORM != AX_PLATFORM_WINRT) && (AX_TARGET_PLATFORM != AX_PLATFORM_WP8)
    // add async struct into queue
    pthread_mutex_lock(&s_asyncStructQueueMutex);
    s_pAsyncStructQueue->push(data);
    pthread_mutex_unlock(&s_asyncStructQueueMutex);
    pthread_cond_signal(&s_SleepCondition);
#else
    // WinRT uses an Async Task to load the image since the ThreadPool has a limited number of threads
    //std::replace( data->filename.begin(), data->filename.end(), '/', '\\'); 
    create_task([this, data] {
        loadImageData(data);
    });
#endif
}

void TextureCache::addImageAsyncCallBack(float dt)
{
    // the image is generated in loading thread
    std::queue<ImageInfo*> *imagesQueue = s_pImageQueue;

    pthread_mutex_lock(&s_ImageInfoMutex);
    if (imagesQueue->empty())
    {
        pthread_mutex_unlock(&s_ImageInfoMutex);
    }
    else
    {
        ImageInfo *pImageInfo = imagesQueue->front();
        imagesQueue->pop();
        pthread_mutex_unlock(&s_ImageInfoMutex);

        AsyncStruct *pAsyncStruct = pImageInfo->asyncStruct;
        Image *pImage = pImageInfo->image;

        Object *target = pAsyncStruct->target;
        SEL_CallFuncO selector = pAsyncStruct->selector;
        const char* filename = pAsyncStruct->filename.c_str();

        // generate texture in render thread
        Texture2D *texture = new Texture2D();
#if 0 //TODO: (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
        texture->initWithImage(pImage, kCCResolutioniPhone);
#else
        texture->initWithImage(pImage);
#endif

#if AX_ENABLE_CACHE_TEXTURE_DATA
       // cache the texture file name
       VolatileTexture::addImageTexture(texture, filename, pImageInfo->imageType);
#endif

        // cache the texture
        m_pTextures->setObject(texture, filename);
        texture->autorelease();

        if (target && selector)
        {
            (target->*selector)(texture);
            target->release();
        }        

        pImage->release();
        delete pAsyncStruct;
        delete pImageInfo;

        --s_nAsyncRefCount;
        if (0 == s_nAsyncRefCount)
        {
            Director::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(TextureCache::addImageAsyncCallBack), this);
        }
    }
}

Texture2D * TextureCache::addImage(const char * path)
{
    AXAssert(path != NULL, "TextureCache: fileimage MUST not be NULL");

    Texture2D * texture = NULL;
    Image* pImage = NULL;
    // Split up directory and filename
    // MUTEX:
    // Needed since addImageAsync calls this method from a different thread
    
    //pthread_mutex_lock(m_pDictLock);

    std::string pathKey = path;

    pathKey = FileUtils::sharedFileUtils()->fullPathForFilename(pathKey.c_str());
    if (pathKey.size() == 0)
    {
        return NULL;
    }
    texture = (Texture2D*)m_pTextures->objectForKey(pathKey.c_str());

    if (! texture) 
    {
		std::string fullpath = pathKey; // (FileUtils::sharedFileUtils()->fullPathFromRelativePath(path));
        std::string lowerCase(pathKey);
        for (unsigned int i = 0; i < lowerCase.length(); ++i)
        {
            lowerCase[i] = tolower(lowerCase[i]);
        }
        // all images are handled by UIImage except PVR extension that is handled by our own handler
        do 
        {
            if (std::string::npos != lowerCase.find(".pvr"))
            {
                texture = this->addPVRImage(fullpath.c_str());
            }
            else if (std::string::npos != lowerCase.find(".pkm"))
            {
                // ETC1 file format, only supportted on Android
                texture = this->addETCImage(fullpath.c_str());
            }
            else
            {
                Image::EImageFormat eImageFormat = Image::kFmtUnKnown;
                if (std::string::npos != lowerCase.find(".png"))
                {
                    eImageFormat = Image::kFmtPng;
                }
                else if (std::string::npos != lowerCase.find(".jpg") || std::string::npos != lowerCase.find(".jpeg"))
                {
                    eImageFormat = Image::kFmtJpg;
                }
                else if (std::string::npos != lowerCase.find(".tif") || std::string::npos != lowerCase.find(".tiff"))
                {
                    eImageFormat = Image::kFmtTiff;
                }
                else if (std::string::npos != lowerCase.find(".webp"))
                {
                    eImageFormat = Image::kFmtWebp;
                }
                
                pImage = new Image();
                AX_BREAK_IF(NULL == pImage);

                bool bRet = pImage->initWithImageFile(fullpath.c_str(), eImageFormat);
                AX_BREAK_IF(!bRet);

                texture = new Texture2D();
                
                if( texture &&
                    texture->initWithImage(pImage) )
                {
#if AX_ENABLE_CACHE_TEXTURE_DATA
                    // cache the texture file name
                    VolatileTexture::addImageTexture(texture, fullpath.c_str(), eImageFormat);
#endif
                    m_pTextures->setObject(texture, pathKey.c_str());
                    texture->release();
                }
                else
                {
                    AXLOG("cocos2d: Couldn't create texture for file:%s in TextureCache", path);
                }
            }
        } while (0);
    }

    AX_SAFE_RELEASE(pImage);

    //pthread_mutex_unlock(m_pDictLock);
    return texture;
}

Texture2D * TextureCache::addPVRImage(const char* path)
{
    AXAssert(path != NULL, "TextureCache: fileimage MUST not be nil");

    Texture2D* texture = NULL;
    std::string key(path);
    
    if( (texture = (Texture2D*)m_pTextures->objectForKey(key.c_str())) ) 
    {
        return texture;
    }

    // Split up directory and filename
    std::string fullpath = FileUtils::sharedFileUtils()->fullPathForFilename(key.c_str());
    texture = new Texture2D();
    if(texture != NULL && texture->initWithPVRFile(fullpath.c_str()) )
    {
#if AX_ENABLE_CACHE_TEXTURE_DATA
        // cache the texture file name
        VolatileTexture::addImageTexture(texture, fullpath.c_str(), Image::kFmtRawData);
#endif
        m_pTextures->setObject(texture, key.c_str());
        texture->autorelease();
    }
    else
    {
        AXLOG("cocos2d: Couldn't add PVRImage:%s in TextureCache",key.c_str());
        AX_SAFE_DELETE(texture);
    }

    return texture;
}

Texture2D* TextureCache::addETCImage(const char* path)
{
    AXAssert(path != NULL, "TextureCache: fileimage MUST not be nil");
    
    Texture2D* texture = NULL;
    std::string key(path);
    
    if( (texture = (Texture2D*)m_pTextures->objectForKey(key.c_str())) )
    {
        return texture;
    }
    
    // Split up directory and filename
    std::string fullpath = FileUtils::sharedFileUtils()->fullPathForFilename(key.c_str());
    texture = new Texture2D();
    if(texture != NULL && texture->initWithETCFile(fullpath.c_str()))
    {
        m_pTextures->setObject(texture, key.c_str());
        texture->autorelease();
    }
    else
    {
        AXLOG("cocos2d: Couldn't add ETCImage:%s in TextureCache",key.c_str());
        AX_SAFE_DELETE(texture);
    }
    
    return texture;
}

Texture2D* TextureCache::addUIImage(Image *image, const char *key)
{
    AXAssert(image != NULL, "TextureCache: image MUST not be nil");

    Texture2D * texture = NULL;
    // textureForKey() use full path,so the key should be full path
    std::string forKey;
    if (key)
    {
        forKey = FileUtils::sharedFileUtils()->fullPathForFilename(key);
    }

    // Don't have to lock here, because addImageAsync() will not 
    // invoke opengl function in loading thread.

    do 
    {
        // If key is nil, then create a new texture each time
        if(key && (texture = (Texture2D *)m_pTextures->objectForKey(forKey.c_str())))
        {
            break;
        }

        // prevents overloading the autorelease pool
        texture = new Texture2D();
        texture->initWithImage(image);

        if(key && texture)
        {
            m_pTextures->setObject(texture, forKey.c_str());
            texture->autorelease();
        }
        else
        {
            AXLOG("cocos2d: Couldn't add UIImage in TextureCache");
        }

    } while (0);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    VolatileTexture::addCCImage(texture, image);
#endif
    
    return texture;
}

bool TextureCache::reloadTexture(const char* fileName)
{
    std::string fullpath = FileUtils::sharedFileUtils()->fullPathForFilename(fileName);
    if (fullpath.size() == 0)
    {
        return false;
    }
    
    Texture2D * texture = (Texture2D*) m_pTextures->objectForKey(fullpath);
    
    bool ret = false;
    if (! texture) {
        texture = this->addImage(fullpath.c_str());
        ret = (texture != NULL);
    }
    else
    {
        do {
            Image* image = new Image();
            AX_BREAK_IF(NULL == image);
            
            bool bRet = image->initWithImageFile(fullpath.c_str());
            AX_BREAK_IF(!bRet);
            
            ret = texture->initWithImage(image);
        } while (0);
    }
    
    return ret;
}

// TextureCache - Remove

void TextureCache::removeAllTextures()
{
    m_pTextures->removeAllObjects();
}

void TextureCache::removeUnusedTextures()
{
    /*
    DictElement* pElement = NULL;
    CCDICT_FOREACH(m_pTextures, pElement)
    {
        AXLOG("cocos2d: TextureCache: texture: %s", pElement->getStrKey());
        Texture2D *value = (Texture2D*)pElement->getObject();
        if (value->retainCount() == 1)
        {
            AXLOG("cocos2d: TextureCache: removing unused texture: %s", pElement->getStrKey());
            m_pTextures->removeObjectForElememt(pElement);
        }
    }
     */
    
    /** Inter engineer zhuoshi sun finds that this way will get better performance
     */    
    if (m_pTextures->count())
    {   
        // find elements to be removed
        DictElement* pElement = NULL;
        list<DictElement*> elementToRemove;
        CCDICT_FOREACH(m_pTextures, pElement)
        {
            AXLOG("cocos2d: TextureCache: texture: %s", pElement->getStrKey());
            Texture2D *value = (Texture2D*)pElement->getObject();
            if (value->retainCount() == 1)
            {
                elementToRemove.push_back(pElement);
            }
        }
        
        // remove elements
        for (list<DictElement*>::iterator iter = elementToRemove.begin(); iter != elementToRemove.end(); ++iter)
        {
            AXLOG("cocos2d: TextureCache: removing unused texture: %s", (*iter)->getStrKey());
            m_pTextures->removeObjectForElememt(*iter);
        }
    }
}

void TextureCache::removeTexture(Texture2D* texture)
{
    if( ! texture )
    {
        return;
    }

    Array* keys = m_pTextures->allKeysForObject(texture);
    m_pTextures->removeObjectsForKeys(keys);
}

void TextureCache::removeTextureForKey(const char *textureKeyName)
{
    if (textureKeyName == NULL)
    {
        return;
    }

    string fullPath = FileUtils::sharedFileUtils()->fullPathForFilename(textureKeyName);
    m_pTextures->removeObjectForKey(fullPath);
}

Texture2D* TextureCache::textureForKey(const char* key)
{
    return (Texture2D*)m_pTextures->objectForKey(FileUtils::sharedFileUtils()->fullPathForFilename(key));
}

void TextureCache::reloadAllTextures()
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    VolatileTexture::reloadAllTextures();
#endif
}

void TextureCache::dumpCachedTextureInfo()
{
    unsigned int count = 0;
    unsigned int totalBytes = 0;

    DictElement* pElement = NULL;
    CCDICT_FOREACH(m_pTextures, pElement)
    {
        Texture2D* tex = (Texture2D*)pElement->getObject();
        unsigned int bpp = tex->bitsPerPixelForFormat();
        // Each texture takes up width * height * bytesPerPixel bytes.
        unsigned int bytes = tex->getPixelsWide() * tex->getPixelsHigh() * bpp / 8;
        totalBytes += bytes;
        count++;
        AXLOG("cocos2d: \"%s\" rc=%lu id=%lu %lu x %lu @ %ld bpp => %lu KB",
               pElement->getStrKey(),
               (long)tex->retainCount(),
               (long)tex->getName(),
               (long)tex->getPixelsWide(),
               (long)tex->getPixelsHigh(),
               (long)bpp,
               (long)bytes / 1024);
    }

    AXLOG("cocos2d: TextureCache dumpDebugInfo: %ld textures, for %lu KB (%.2f MB)", (long)count, (long)totalBytes / 1024, totalBytes / (1024.0f*1024.0f));
}

#if AX_ENABLE_CACHE_TEXTURE_DATA

std::list<VolatileTexture*> VolatileTexture::textures;
bool VolatileTexture::isReloading = false;

VolatileTexture::VolatileTexture(Texture2D *t)
: texture(t)
, m_eCashedImageType(kInvalid)
, m_pTextureData(NULL)
, m_PixelFormat(kTexture2DPixelFormat_RGBA8888)
, m_strFileName("")
, m_FmtImage(Image::kFmtPng)
, m_alignment(kCCTextAlignmentCenter)
, m_vAlignment(kCCVerticalTextAlignmentCenter)
, m_strFontName("")
, m_strText("")
, uiImage(NULL)
, m_fFontSize(0.0f)
{
    m_size = CCSizeMake(0, 0);
    m_texParams.minFilter = GL_LINEAR;
    m_texParams.magFilter = GL_LINEAR;
    m_texParams.wrapS = GL_CLAMP_TO_EDGE;
    m_texParams.wrapT = GL_CLAMP_TO_EDGE;
    textures.push_back(this);
}

VolatileTexture::~VolatileTexture()
{
    textures.remove(this);
    AX_SAFE_RELEASE(uiImage);
}

void VolatileTexture::addImageTexture(Texture2D *tt, const char* imageFileName, Image::EImageFormat format)
{
    if (isReloading)
    {
        return;
    }

    VolatileTexture *vt = findVolotileTexture(tt);

    vt->m_eCashedImageType = kImageFile;
    vt->m_strFileName = imageFileName;
    vt->m_FmtImage    = format;
    vt->m_PixelFormat = tt->getPixelFormat();
}

void VolatileTexture::addCCImage(Texture2D *tt, Image *image)
{
    VolatileTexture *vt = findVolotileTexture(tt);
    image->retain();
    vt->uiImage = image;
    vt->m_eCashedImageType = kImage;
}

VolatileTexture* VolatileTexture::findVolotileTexture(Texture2D *tt)
{
    VolatileTexture *vt = 0;
    std::list<VolatileTexture *>::iterator i = textures.begin();
    while (i != textures.end())
    {
        VolatileTexture *v = *i++;
        if (v->texture == tt) 
        {
            vt = v;
            break;
        }
    }
    
    if (! vt)
    {
        vt = new VolatileTexture(tt);
    }
    
    return vt;
}

void VolatileTexture::addDataTexture(Texture2D *tt, void* data, CCTexture2DPixelFormat pixelFormat, const Size& contentSize)
{
    if (isReloading)
    {
        return;
    }

    VolatileTexture *vt = findVolotileTexture(tt);

    vt->m_eCashedImageType = kImageData;
    vt->m_pTextureData = data;
    vt->m_PixelFormat = pixelFormat;
    vt->m_TextureSize = contentSize;
}

void VolatileTexture::addStringTexture(Texture2D *tt, const char* text, const Size& dimensions, CCTextAlignment alignment, 
                                       CCVerticalTextAlignment vAlignment, const char *fontName, float fontSize)
{
    if (isReloading)
    {
        return;
    }

    VolatileTexture *vt = findVolotileTexture(tt);

    vt->m_eCashedImageType = kString;
    vt->m_size        = dimensions;
    vt->m_strFontName = fontName;
    vt->m_alignment   = alignment;
    vt->m_vAlignment  = vAlignment;
    vt->m_fFontSize   = fontSize;
    vt->m_strText     = text;
}

void VolatileTexture::setTexParameters(Texture2D *t, ccTexParams *texParams) 
{
    VolatileTexture *vt = findVolotileTexture(t);

    if (texParams->minFilter != GL_NONE)
        vt->m_texParams.minFilter = texParams->minFilter;
    if (texParams->magFilter != GL_NONE)
        vt->m_texParams.magFilter = texParams->magFilter;
    if (texParams->wrapS != GL_NONE)
        vt->m_texParams.wrapS = texParams->wrapS;
    if (texParams->wrapT != GL_NONE)
        vt->m_texParams.wrapT = texParams->wrapT;
}

void VolatileTexture::removeTexture(Texture2D *t) 
{

    std::list<VolatileTexture *>::iterator i = textures.begin();
    while (i != textures.end())
    {
        VolatileTexture *vt = *i++;
        if (vt->texture == t) 
        {
            delete vt;
            break;
        }
    }
}

void VolatileTexture::reloadAllTextures()
{
    isReloading = true;

    AXLOG("reload all texture");
    std::list<VolatileTexture *>::iterator iter = textures.begin();

    while (iter != textures.end())
    {
        VolatileTexture *vt = *iter++;

        switch (vt->m_eCashedImageType)
        {
        case kImageFile:
            {
                std::string lowerCase(vt->m_strFileName.c_str());
                for (unsigned int i = 0; i < lowerCase.length(); ++i)
                {
                    lowerCase[i] = tolower(lowerCase[i]);
                }

                if (std::string::npos != lowerCase.find(".pvr")) 
                {
                    CCTexture2DPixelFormat oldPixelFormat = Texture2D::defaultAlphaPixelFormat();
                    Texture2D::setDefaultAlphaPixelFormat(vt->m_PixelFormat);

                    vt->texture->initWithPVRFile(vt->m_strFileName.c_str());
                    Texture2D::setDefaultAlphaPixelFormat(oldPixelFormat);
                } 
                else 
                {
                    Image* pImage = new Image();
                    unsigned long nSize = 0;
                    unsigned char* pBuffer = FileUtils::sharedFileUtils()->getFileData(vt->m_strFileName.c_str(), "rb", &nSize);

                    if (pImage && pImage->initWithImageData((void*)pBuffer, nSize, vt->m_FmtImage))
                    {
                        CCTexture2DPixelFormat oldPixelFormat = Texture2D::defaultAlphaPixelFormat();
                        Texture2D::setDefaultAlphaPixelFormat(vt->m_PixelFormat);
                        vt->texture->initWithImage(pImage);
                        Texture2D::setDefaultAlphaPixelFormat(oldPixelFormat);
                    }

                    AX_SAFE_DELETE_ARRAY(pBuffer);
                    AX_SAFE_RELEASE(pImage);
                }
            }
            break;
        case kImageData:
            {
                vt->texture->initWithData(vt->m_pTextureData, 
                                          vt->m_PixelFormat, 
                                          vt->m_TextureSize.width, 
                                          vt->m_TextureSize.height, 
                                          vt->m_TextureSize);
            }
            break;
        case kString:
            {
                vt->texture->initWithString(vt->m_strText.c_str(),
                                            vt->m_strFontName.c_str(),
                                            vt->m_fFontSize,
                                            vt->m_size,
                                            vt->m_alignment,
                                            vt->m_vAlignment
                                            );
            }
            break;
        case kImage:
            {
                vt->texture->initWithImage(vt->uiImage);
            }
            break;
        default:
            break;
        }
        vt->texture->setTexParameters(&vt->m_texParams);
    }

    isReloading = false;
}

#endif // AX_ENABLE_CACHE_TEXTURE_DATA

NS_AX_END

