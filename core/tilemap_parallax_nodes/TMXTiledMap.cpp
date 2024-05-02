/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2009-2010 Ricardo Quesada
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
#include "TMXTiledMap.h"
#include "TMXXMLParser.h"
#include "TMXLayer.h"
#include "sprite_nodes/Sprite.h"
#include "support/PointExtension.h"

NS_AX_BEGIN

// implementation TMXTiledMap

TMXTiledMap * TMXTiledMap::create(const char *tmxFile)
{
    TMXTiledMap *pRet = new TMXTiledMap();
    if (pRet->initWithTMXFile(tmxFile))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

TMXTiledMap* TMXTiledMap::createWithXML(const char* tmxString, const char* resourcePath)
{
    TMXTiledMap *pRet = new TMXTiledMap();
    if (pRet->initWithXML(tmxString, resourcePath))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool TMXTiledMap::initWithTMXFile(const char *tmxFile)
{
    AXAssert(tmxFile != NULL && strlen(tmxFile)>0, "TMXTiledMap: tmx file should not bi NULL");
    
    setContentSize(CCSizeZero);

    TMXMapInfo *mapInfo = TMXMapInfo::formatWithTMXFile(tmxFile);

    if (! mapInfo)
    {
        return false;
    }
    AXAssert( mapInfo->getTilesets()->count() != 0, "TMXTiledMap: Map not found. Please check the filename.");
    buildWithMapInfo(mapInfo);

    return true;
}

bool TMXTiledMap::initWithXML(const char* tmxString, const char* resourcePath)
{
    setContentSize(CCSizeZero);

    TMXMapInfo *mapInfo = TMXMapInfo::formatWithXML(tmxString, resourcePath);

    AXAssert( mapInfo->getTilesets()->count() != 0, "TMXTiledMap: Map not found. Please check the filename.");
    buildWithMapInfo(mapInfo);

    return true;
}

TMXTiledMap::TMXTiledMap()
    :m_tMapSize(CCSizeZero)
    ,m_tTileSize(CCSizeZero)        
    ,m_pObjectGroups(NULL)
    ,m_pProperties(NULL)
    ,m_pTileProperties(NULL)
{
}
TMXTiledMap::~TMXTiledMap()
{
    AX_SAFE_RELEASE(m_pProperties);
    AX_SAFE_RELEASE(m_pObjectGroups);
    AX_SAFE_RELEASE(m_pTileProperties);
}

Array* TMXTiledMap::getObjectGroups()
{
    return m_pObjectGroups;
}

void TMXTiledMap::setObjectGroups(Array* var)
{
    AX_SAFE_RETAIN(var);
    AX_SAFE_RELEASE(m_pObjectGroups);
    m_pObjectGroups = var;
}

Dictionary * TMXTiledMap::getProperties()
{
    return m_pProperties;
}

void TMXTiledMap::setProperties(Dictionary* var)
{
    AX_SAFE_RETAIN(var);
    AX_SAFE_RELEASE(m_pProperties);
    m_pProperties = var;
}

// private
TMXLayer * TMXTiledMap::parseLayer(TMXLayerInfo *layerInfo, TMXMapInfo *mapInfo)
{
    TMXTilesetInfo *tileset = tilesetForLayer(layerInfo, mapInfo);
    TMXLayer *layer = TMXLayer::create(tileset, layerInfo, mapInfo);

    // tell the layerinfo to release the ownership of the tiles map.
    layerInfo->m_bOwnTiles = false;
    layer->setupTiles();

    return layer;
}

TMXTilesetInfo * TMXTiledMap::tilesetForLayer(TMXLayerInfo *layerInfo, TMXMapInfo *mapInfo)
{
    Size size = layerInfo->m_tLayerSize;
    Array* tilesets = mapInfo->getTilesets();
    if (tilesets && tilesets->count()>0)
    {
        TMXTilesetInfo* tileset = NULL;
        Object* pObj = NULL;
        AXARRAY_FOREACH_REVERSE(tilesets, pObj)
        {
            tileset = (TMXTilesetInfo*)pObj;
            if (tileset)
            {
                for( unsigned int y=0; y < size.height; y++ )
                {
                    for( unsigned int x=0; x < size.width; x++ ) 
                    {
                        unsigned int pos = (unsigned int)(x + size.width * y);
                        unsigned int gid = layerInfo->m_pTiles[ pos ];

                        // gid are stored in little endian.
                        // if host is big endian, then swap
                        //if( o == CFByteOrderBigEndian )
                        //    gid = CFSwapInt32( gid );
                        /* We support little endian.*/

                        // XXX: gid == 0 --> empty tile
                        if( gid != 0 ) 
                        {
                            // Optimization: quick return
                            // if the layer is invalid (more than 1 tileset per layer) an AXAssert will be thrown later
                            if( (gid & kCCFlippedMask) >= tileset->m_uFirstGid )
                                return tileset;
                        }
                    }
                }        
            }
        }
    }

    // If all the tiles are 0, return empty tileset
    AXLOG("cocos2d: Warning: TMX Layer '%s' has no tiles", layerInfo->m_sName.c_str());
    return NULL;
}

void TMXTiledMap::buildWithMapInfo(TMXMapInfo* mapInfo)
{
    m_tMapSize = mapInfo->getMapSize();
    m_tTileSize = mapInfo->getTileSize();
    m_nMapOrientation = mapInfo->getOrientation();

    AX_SAFE_RELEASE(m_pObjectGroups);
    m_pObjectGroups = mapInfo->getObjectGroups();
    AX_SAFE_RETAIN(m_pObjectGroups);

    AX_SAFE_RELEASE(m_pProperties);
    m_pProperties = mapInfo->getProperties();
    AX_SAFE_RETAIN(m_pProperties);

    AX_SAFE_RELEASE(m_pTileProperties);
    m_pTileProperties = mapInfo->getTileProperties();
    AX_SAFE_RETAIN(m_pTileProperties);

    int idx=0;

    Array* layers = mapInfo->getLayers();
    if (layers && layers->count()>0)
    {
        TMXLayerInfo* layerInfo = NULL;
        Object* pObj = NULL;
        AXARRAY_FOREACH(layers, pObj)
        {
            layerInfo = (TMXLayerInfo*)pObj;
            if (layerInfo && layerInfo->m_bVisible)
            {
                TMXLayer *child = parseLayer(layerInfo, mapInfo);
                addChild((Node*)child, idx, idx);

                // update content size with the max size
                const Size& childSize = child->getContentSize();
                Size currentSize = this->getContentSize();
                currentSize.width = MAX( currentSize.width, childSize.width );
                currentSize.height = MAX( currentSize.height, childSize.height );
                this->setContentSize(currentSize);

                idx++;
            }
        }
    }
}

// public
TMXLayer * TMXTiledMap::layerNamed(const char *layerName)
{
    AXAssert(layerName != NULL && strlen(layerName) > 0, "Invalid layer name!");
    Object* pObj = NULL;
    AXARRAY_FOREACH(m_pChildren, pObj) 
    {
        TMXLayer* layer = dynamic_cast<TMXLayer*>(pObj);
        if(layer)
        {
            if(0 == strcmp(layer->getLayerName(), layerName))
            {
                return layer;
            }
        }
    }

    // layer not found
    return NULL;
}

TMXObjectGroup * TMXTiledMap::objectGroupNamed(const char *groupName)
{
    AXAssert(groupName != NULL && strlen(groupName) > 0, "Invalid group name!");

    std::string sGroupName = groupName;
    if (m_pObjectGroups && m_pObjectGroups->count()>0)
    {
        TMXObjectGroup* objectGroup = NULL;
        Object* pObj = NULL;
        AXARRAY_FOREACH(m_pObjectGroups, pObj)
        {
            objectGroup = (TMXObjectGroup*)(pObj);
            if (objectGroup && objectGroup->getGroupName() == sGroupName)
            {
                return objectGroup;
            }
        }
    }

    // objectGroup not found
    return NULL;
}

String* TMXTiledMap::propertyNamed(const char *propertyName)
{
    return (String*)m_pProperties->objectForKey(propertyName);
}

Dictionary* TMXTiledMap::propertiesForGID(int GID)
{
    return (Dictionary*)m_pTileProperties->objectForKey(GID);
}
        

NS_AX_END

