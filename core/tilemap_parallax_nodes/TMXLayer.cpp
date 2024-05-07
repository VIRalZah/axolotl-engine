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
#include "TMXLayer.h"
#include "TMXXMLParser.h"
#include "TMXTiledMap.h"
#include "sprite_nodes/Sprite.h"
#include "textures/TextureCache.h"
#include "shaders/ShaderCache.h"
#include "shaders/GLProgram.h"
#include "support/PointExtension.h"
#include "support/data_support/axCArray.h"
#include "base/Director.h"

NS_AX_BEGIN


// TMXLayer - init & alloc & dealloc

TMXLayer * TMXLayer::create(TMXTilesetInfo *tilesetInfo, TMXLayerInfo *layerInfo, TMXMapInfo *mapInfo)
{
    TMXLayer *pRet = new TMXLayer();
    if (pRet->initWithTilesetInfo(tilesetInfo, layerInfo, mapInfo))
    {
        pRet->autorelease();
        return pRet;
    }
    return NULL;
}
bool TMXLayer::initWithTilesetInfo(TMXTilesetInfo *tilesetInfo, TMXLayerInfo *layerInfo, TMXMapInfo *mapInfo)
{    
    // XXX: is 35% a good estimate ?
    Size size = layerInfo->m_tLayerSize;
    float totalNumberOfTiles = size.width * size.height;
    float capacity = totalNumberOfTiles * 0.35f + 1; // 35 percent is occupied ?

    Texture2D *texture = NULL;
    if( tilesetInfo )
    {
        texture = TextureCache::sharedTextureCache()->addImage(tilesetInfo->m_sSourceImage.c_str());
    }

    if (SpriteBatchNode::initWithTexture(texture, (unsigned int)capacity))
    {
        // layerInfo
        m_sLayerName = layerInfo->m_sName;
        m_tLayerSize = size;
        m_pTiles = layerInfo->m_pTiles;
        m_uMinGID = layerInfo->m_uMinGID;
        m_uMaxGID = layerInfo->m_uMaxGID;
        m_cOpacity = layerInfo->m_cOpacity;
        setProperties(Dictionary::createWithDictionary(layerInfo->getProperties()));
        _contentScaleFactor = Director::sharedDirector()->getContentScaleFactor(); 

        // tilesetInfo
        m_pTileSet = tilesetInfo;
        AX_SAFE_RETAIN(m_pTileSet);

        // mapInfo
        m_tMapTileSize = mapInfo->getTileSize();
        m_uLayerOrientation = mapInfo->getOrientation();

        // offset (after layer orientation is set);
        Vec2 offset = this->calculateLayerOffset(layerInfo->m_tOffset);
        this->setPosition(AX_POINT_PIXELS_TO_POINTS(offset));

        m_pAtlasIndexArray = axCArrayNew((unsigned int)totalNumberOfTiles);

        this->setContentSize(AX_SIZE_PIXELS_TO_POINTS(Size(m_tLayerSize.width * m_tMapTileSize.width, m_tLayerSize.height * m_tMapTileSize.height)));

        m_bUseAutomaticVertexZ = false;
        m_nVertexZvalue = 0;
        
        return true;
    }
    return false;
}

TMXLayer::TMXLayer()
:m_tLayerSize(Size::ZERO)
,m_tMapTileSize(Size::ZERO)
,m_pTiles(NULL)
,m_pTileSet(NULL)
,m_pProperties(NULL)
,m_sLayerName("")
,m_pReusedTile(NULL)
,m_pAtlasIndexArray(NULL)    
{}

TMXLayer::~TMXLayer()
{
    AX_SAFE_RELEASE(m_pTileSet);
    AX_SAFE_RELEASE(m_pReusedTile);
    AX_SAFE_RELEASE(m_pProperties);

    if (m_pAtlasIndexArray)
    {
        axCArrayFree(m_pAtlasIndexArray);
        m_pAtlasIndexArray = NULL;
    }

    AX_SAFE_DELETE_ARRAY(m_pTiles);
}

TMXTilesetInfo * TMXLayer::getTileSet()
{
    return m_pTileSet;
}

void TMXLayer::setTileSet(TMXTilesetInfo* var)
{
    AX_SAFE_RETAIN(var);
    AX_SAFE_RELEASE(m_pTileSet);
    m_pTileSet = var;
}

void TMXLayer::releaseMap()
{
    if (m_pTiles)
    {
        delete [] m_pTiles;
        m_pTiles = NULL;
    }

    if (m_pAtlasIndexArray)
    {
        axCArrayFree(m_pAtlasIndexArray);
        m_pAtlasIndexArray = NULL;
    }
}

// TMXLayer - setup Tiles
void TMXLayer::setupTiles()
{    
    // Optimization: quick hack that sets the image size on the tileset
    m_pTileSet->m_tImageSize = m_pobTextureAtlas->getTexture()->getContentSizeInPixels();

    // By default all the tiles are aliased
    // pros:
    //  - easier to render
    // cons:
    //  - difficult to scale / rotate / etc.
    m_pobTextureAtlas->getTexture()->setAliasTexParameters();

    //CFByteOrder o = CFByteOrderGetCurrent();

    // Parse cocos2d properties
    this->parseInternalProperties();

    for (unsigned int y=0; y < m_tLayerSize.height; y++) 
    {
        for (unsigned int x=0; x < m_tLayerSize.width; x++) 
        {
            unsigned int pos = (unsigned int)(x + m_tLayerSize.width * y);
            unsigned int gid = m_pTiles[ pos ];

            // gid are stored in little endian.
            // if host is big endian, then swap
            //if( o == CFByteOrderBigEndian )
            //    gid = CFSwapInt32( gid );
            /* We support little endian.*/

            // XXX: gid == 0 --> empty tile
            if (gid != 0) 
            {
                this->appendTileForGID(gid, Vec2(x, y));

                // Optimization: update min and max GID rendered by the layer
                m_uMinGID = MIN(gid, m_uMinGID);
                m_uMaxGID = MAX(gid, m_uMaxGID);
            }
        }
    }

    AXAssert( m_uMaxGID >= m_pTileSet->m_uFirstGid &&
        m_uMinGID >= m_pTileSet->m_uFirstGid, "TMX: Only 1 tileset per layer is supported");    
}

// TMXLayer - Properties
String* TMXLayer::propertyNamed(const char *propertyName)
{
    return (String*)m_pProperties->objectForKey(propertyName);
}

void TMXLayer::parseInternalProperties()
{
    // if AX_vertex=automatic, then tiles will be rendered using vertexz

    String *vertexz = propertyNamed("AX_vertexz");
    if (vertexz) 
    {
        // If "automatic" is on, then parse the "AX_alpha_func" too
        if (vertexz->m_sString == "automatic")
        {
            m_bUseAutomaticVertexZ = true;
            String *alphaFuncVal = propertyNamed("AX_alpha_func");
            float alphaFuncValue = 0.0f;
            if (alphaFuncVal != NULL)
            {
                alphaFuncValue = alphaFuncVal->floatValue();
            }
            setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(kAXShader_PositionTextureColorAlphaTest));

            GLint alphaValueLocation = glGetUniformLocation(getShaderProgram()->getProgram(), kAXUniformAlphaTestValue);

            // NOTE: alpha test shader is hard-coded to use the equivalent of a glAlphaFunc(GL_GREATER) comparison
            getShaderProgram()->setUniformLocationWith1f(alphaValueLocation, alphaFuncValue);
        }
        else
        {
            m_nVertexZvalue = vertexz->intValue();
        }
    }
}

void TMXLayer::setupTileSprite(Sprite* sprite, Vec2 pos, unsigned int gid)
{
    sprite->setPosition(positionAt(pos));
    sprite->setVertexZ((float)vertexZForPos(pos));
    sprite->setAnchorPoint(Vec2::ZERO);
    sprite->setOpacity(m_cOpacity);

    //issue 1264, flip can be undone as well
    sprite->setFlipX(false);
    sprite->setFlipY(false);
    sprite->setRotation(0.0f);
    sprite->setAnchorPoint(Vec2(0,0));

    // Rotation in tiled is achieved using 3 flipped states, flipping across the horizontal, vertical, and diagonal axes of the tiles.
    if (gid & kCCTMXTileDiagonalFlag)
    {
        // put the anchor in the middle for ease of rotation.
        sprite->setAnchorPoint(Vec2(0.5f,0.5f));
        sprite->setPosition(Vec2(positionAt(pos).x + sprite->getContentSize().height/2,
           positionAt(pos).y + sprite->getContentSize().width/2 ) );

        unsigned int flag = gid & (kCCTMXTileHorizontalFlag | kCCTMXTileVerticalFlag );

        // handle the 4 diagonally flipped states.
        if (flag == kCCTMXTileHorizontalFlag)
        {
            sprite->setRotation(90.0f);
        }
        else if (flag == kCCTMXTileVerticalFlag)
        {
            sprite->setRotation(270.0f);
        }
        else if (flag == (kCCTMXTileVerticalFlag | kCCTMXTileHorizontalFlag) )
        {
            sprite->setRotation(90.0f);
            sprite->setFlipX(true);
        }
        else
        {
            sprite->setRotation(270.0f);
            sprite->setFlipX(true);
        }
    }
    else
    {
        if (gid & kCCTMXTileHorizontalFlag)
        {
            sprite->setFlipX(true);
        }

        if (gid & kCCTMXTileVerticalFlag)
        {
            sprite->setFlipY(true);
        }
    }
}

Sprite* TMXLayer::reusedTileWithRect(Rect rect)
{
    if (! m_pReusedTile) 
    {
        m_pReusedTile = new Sprite();
        m_pReusedTile->initWithTexture(m_pobTextureAtlas->getTexture(), rect, false);
        m_pReusedTile->setBatchNode(this);
    }
    else
    {
        // XXX HACK: Needed because if "batch node" is nil,
		// then the Sprite'squad will be reset
        m_pReusedTile->setBatchNode(NULL);
        
		// Re-init the sprite
        m_pReusedTile->setTextureRect(rect, false, rect.size);
        
		// restore the batch node
        m_pReusedTile->setBatchNode(this);
    }

    return m_pReusedTile;
}

// TMXLayer - obtaining tiles/gids
Sprite * TMXLayer::tileAt(const Vec2& pos)
{
    AXAssert(pos.x < m_tLayerSize.width && pos.y < m_tLayerSize.height && pos.x >=0 && pos.y >=0, "TMXLayer: invalid position");
    AXAssert(m_pTiles && m_pAtlasIndexArray, "TMXLayer: the tiles map has been released");

    Sprite *tile = NULL;
    unsigned int gid = this->tileGIDAt(pos);

    // if GID == 0, then no tile is present
    if (gid) 
    {
        int z = (int)(pos.x + pos.y * m_tLayerSize.width);
        tile = (Sprite*) this->getChildByTag(z);

        // tile not created yet. create it
        if (! tile) 
        {
            Rect rect = m_pTileSet->rectForGID(gid);
            rect = AX_RECT_PIXELS_TO_POINTS(rect);

            tile = new Sprite();
            tile->initWithTexture(this->getTexture(), rect);
            tile->setBatchNode(this);
            tile->setPosition(positionAt(pos));
            tile->setVertexZ((float)vertexZForPos(pos));
            tile->setAnchorPoint(Vec2::ZERO);
            tile->setOpacity(m_cOpacity);

            unsigned int indexForZ = atlasIndexForExistantZ(z);
            this->addSpriteWithoutQuad(tile, indexForZ, z);
            tile->release();
        }
    }
    
    return tile;
}

unsigned int TMXLayer::tileGIDAt(const Vec2& pos)
{
    return tileGIDAt(pos, NULL);
}

unsigned int TMXLayer::tileGIDAt(const Vec2& pos, ccTMXTileFlags* flags)
{
    AXAssert(pos.x < m_tLayerSize.width && pos.y < m_tLayerSize.height && pos.x >=0 && pos.y >=0, "TMXLayer: invalid position");
    AXAssert(m_pTiles && m_pAtlasIndexArray, "TMXLayer: the tiles map has been released");

    int idx = (int)(pos.x + pos.y * m_tLayerSize.width);
    // Bits on the far end of the 32-bit global tile ID are used for tile flags
    unsigned int tile = m_pTiles[idx];

    // issue1264, flipped tiles can be changed dynamically
    if (flags) 
    {
        *flags = (ccTMXTileFlags)(tile & kCCFlipedAll);
    }
    
    return (tile & kCCFlippedMask);
}

// TMXLayer - adding helper methods
Sprite * TMXLayer::insertTileForGID(unsigned int gid, const Vec2& pos)
{
    Rect rect = m_pTileSet->rectForGID(gid);
    rect = AX_RECT_PIXELS_TO_POINTS(rect);

    intptr_t z = (intptr_t)(pos.x + pos.y * m_tLayerSize.width);

    Sprite *tile = reusedTileWithRect(rect);

    setupTileSprite(tile, pos, gid);

    // get atlas index
    unsigned int indexForZ = atlasIndexForNewZ(z);

    // Optimization: add the quad without adding a child
    this->insertQuadFromSprite(tile, indexForZ);

    // insert it into the local atlasindex array
    axCArrayInsertValueAtIndex(m_pAtlasIndexArray, (void*)z, indexForZ);

    // update possible children
    if (m_pChildren && m_pChildren->count()>0)
    {
        Object* pObject = NULL;
        AXARRAY_FOREACH(m_pChildren, pObject)
        {
            Sprite* pChild = (Sprite*) pObject;
            if (pChild)
            {
                unsigned int ai = pChild->getAtlasIndex();
                if ( ai >= indexForZ )
                {
                    pChild->setAtlasIndex(ai+1);
                }
            }
        }
    }
    m_pTiles[z] = gid;
    return tile;
}
Sprite * TMXLayer::updateTileForGID(unsigned int gid, const Vec2& pos)    
{
    Rect rect = m_pTileSet->rectForGID(gid);
    rect = Rect(rect.origin.x / _contentScaleFactor, rect.origin.y / _contentScaleFactor, rect.size.width/ _contentScaleFactor, rect.size.height/ _contentScaleFactor);
    int z = (int)(pos.x + pos.y * m_tLayerSize.width);

    Sprite *tile = reusedTileWithRect(rect);

    setupTileSprite(tile ,pos ,gid);

    // get atlas index
    unsigned int indexForZ = atlasIndexForExistantZ(z);
    tile->setAtlasIndex(indexForZ);
    tile->setDirty(true);
    tile->updateTransform();
    m_pTiles[z] = gid;

    return tile;
}

// used only when parsing the map. useless after the map was parsed
// since lot's of assumptions are no longer true
Sprite * TMXLayer::appendTileForGID(unsigned int gid, const Vec2& pos)
{
    Rect rect = m_pTileSet->rectForGID(gid);
    rect = AX_RECT_PIXELS_TO_POINTS(rect);

    intptr_t z = (intptr_t)(pos.x + pos.y * m_tLayerSize.width);

    Sprite *tile = reusedTileWithRect(rect);

    setupTileSprite(tile ,pos ,gid);

    // optimization:
    // The difference between appendTileForGID and insertTileforGID is that append is faster, since
    // it appends the tile at the end of the texture atlas
    unsigned int indexForZ = m_pAtlasIndexArray->num;

    // don't add it using the "standard" way.
    insertQuadFromSprite(tile, indexForZ);

    // append should be after addQuadFromSprite since it modifies the quantity values
    axCArrayInsertValueAtIndex(m_pAtlasIndexArray, (void*)z, indexForZ);

    return tile;
}

// TMXLayer - atlasIndex and Z
static inline int compareInts(const void * a, const void * b)
{
    return ((*(int*)a) - (*(int*)b));
}
unsigned int TMXLayer::atlasIndexForExistantZ(unsigned int z)
{
    int key=z;
    int *item = (int*)bsearch((void*)&key, (void*)&m_pAtlasIndexArray->arr[0], m_pAtlasIndexArray->num, sizeof(void*), compareInts);

    AXAssert(item, "TMX atlas index not found. Shall not happen");

    int index = ((size_t)item - (size_t)m_pAtlasIndexArray->arr) / sizeof(void*);
    return index;
}
unsigned int TMXLayer::atlasIndexForNewZ(int z)
{
    // XXX: This can be improved with a sort of binary search
    unsigned int i=0;
    for (i=0; i< m_pAtlasIndexArray->num ; i++) 
    {
        int val = (size_t) m_pAtlasIndexArray->arr[i];
        if (z < val)
        {
            break;
        }
    } 
    
    return i;
}

// TMXLayer - adding / remove tiles
void TMXLayer::setTileGID(unsigned int gid, const Vec2& pos)
{
    setTileGID(gid, pos, (ccTMXTileFlags)0);
}

void TMXLayer::setTileGID(unsigned int gid, const Vec2& pos, ccTMXTileFlags flags)
{
    AXAssert(pos.x < m_tLayerSize.width && pos.y < m_tLayerSize.height && pos.x >=0 && pos.y >=0, "TMXLayer: invalid position");
    AXAssert(m_pTiles && m_pAtlasIndexArray, "TMXLayer: the tiles map has been released");
    AXAssert(gid == 0 || gid >= m_pTileSet->m_uFirstGid, "TMXLayer: invalid gid" );

    ccTMXTileFlags currentFlags;
    unsigned int currentGID = tileGIDAt(pos, &currentFlags);

    if (currentGID != gid || currentFlags != flags) 
    {
        unsigned gidAndFlags = gid | flags;

        // setting gid=0 is equal to remove the tile
        if (gid == 0)
        {
            removeTileAt(pos);
        }
        // empty tile. create a new one
        else if (currentGID == 0)
        {
            insertTileForGID(gidAndFlags, pos);
        }
        // modifying an existing tile with a non-empty tile
        else 
        {
            unsigned int z = (unsigned int)(pos.x + pos.y * m_tLayerSize.width);
            Sprite *sprite = (Sprite*)getChildByTag(z);
            if (sprite)
            {
                Rect rect = m_pTileSet->rectForGID(gid);
                rect = AX_RECT_PIXELS_TO_POINTS(rect);

                sprite->setTextureRect(rect, false, rect.size);
                if (flags) 
                {
                    setupTileSprite(sprite, sprite->getPosition(), gidAndFlags);
                }
                m_pTiles[z] = gidAndFlags;
            } 
            else 
            {
                updateTileForGID(gidAndFlags, pos);
            }
        }
    }
}
void TMXLayer::addChild(Node * child, int zOrder, int tag)
{
    AX_UNUSED_PARAM(child);
    AX_UNUSED_PARAM(zOrder);
    AX_UNUSED_PARAM(tag);
    AXAssert(0, "addChild: is not supported on TMXLayer. Instead use setTileGID:at:/tileAt:");
}
void TMXLayer::removeChild(Node* node, bool cleanup)
{
    Sprite *sprite = (Sprite*)node;
    // allows removing nil objects
    if (! sprite)
    {
        return;
    }

    AXAssert(m_pChildren->containsObject(sprite), "Tile does not belong to TMXLayer");

    unsigned int atlasIndex = sprite->getAtlasIndex();
    unsigned int zz = (size_t)m_pAtlasIndexArray->arr[atlasIndex];
    m_pTiles[zz] = 0;
    axCArrayRemoveValueAtIndex(m_pAtlasIndexArray, atlasIndex);
    SpriteBatchNode::removeChild(sprite, cleanup);
}
void TMXLayer::removeTileAt(const Vec2& pos)
{
    AXAssert(pos.x < m_tLayerSize.width && pos.y < m_tLayerSize.height && pos.x >=0 && pos.y >=0, "TMXLayer: invalid position");
    AXAssert(m_pTiles && m_pAtlasIndexArray, "TMXLayer: the tiles map has been released");

    unsigned int gid = tileGIDAt(pos);

    if (gid) 
    {
        unsigned int z = (unsigned int)(pos.x + pos.y * m_tLayerSize.width);
        unsigned int atlasIndex = atlasIndexForExistantZ(z);

        // remove tile from GID map
        m_pTiles[z] = 0;

        // remove tile from atlas position array
        axCArrayRemoveValueAtIndex(m_pAtlasIndexArray, atlasIndex);

        // remove it from sprites and/or texture atlas
        Sprite *sprite = (Sprite*)getChildByTag(z);
        if (sprite)
        {
            SpriteBatchNode::removeChild(sprite, true);
        }
        else 
        {
            m_pobTextureAtlas->removeQuadAtIndex(atlasIndex);

            // update possible children
            if (m_pChildren && m_pChildren->count()>0)
            {
                Object* pObject = NULL;
                AXARRAY_FOREACH(m_pChildren, pObject)
                {
                    Sprite* pChild = (Sprite*) pObject;
                    if (pChild)
                    {
                        unsigned int ai = pChild->getAtlasIndex();
                        if ( ai >= atlasIndex )
                        {
                            pChild->setAtlasIndex(ai-1);
                        }
                    }
                }
            }
        }
    }
}

//TMXLayer - obtaining positions, offset
Vec2 TMXLayer::calculateLayerOffset(const Vec2& pos)
{
    Vec2 ret = Vec2::ZERO;
    switch (m_uLayerOrientation) 
    {
    case CCTMXOrientationOrtho:
        ret = Vec2( pos.x * m_tMapTileSize.width, -pos.y *m_tMapTileSize.height);
        break;
    case CCTMXOrientationIso:
        ret = Vec2((m_tMapTileSize.width /2) * (pos.x - pos.y),
                  (m_tMapTileSize.height /2 ) * (-pos.x - pos.y));
        break;
    case CCTMXOrientationHex:
        AXAssert(pos.equals(Vec2::ZERO), "offset for hexagonal map not implemented yet");
        break;
    }
    return ret;    
}
Vec2 TMXLayer::positionAt(const Vec2& pos)
{
    Vec2 ret = Vec2::ZERO;
    switch (m_uLayerOrientation)
    {
    case CCTMXOrientationOrtho:
        ret = positionForOrthoAt(pos);
        break;
    case CCTMXOrientationIso:
        ret = positionForIsoAt(pos);
        break;
    case CCTMXOrientationHex:
        ret = positionForHexAt(pos);
        break;
    }
    ret = AX_POINT_PIXELS_TO_POINTS( ret );
    return ret;
}
Vec2 TMXLayer::positionForOrthoAt(const Vec2& pos)
{
    Vec2 xy = Vec2(pos.x * m_tMapTileSize.width,
                            (m_tLayerSize.height - pos.y - 1) * m_tMapTileSize.height);
    return xy;
}
Vec2 TMXLayer::positionForIsoAt(const Vec2& pos)
{
    Vec2 xy = Vec2(m_tMapTileSize.width /2 * (m_tLayerSize.width + pos.x - pos.y - 1),
                             m_tMapTileSize.height /2 * ((m_tLayerSize.height * 2 - pos.x - pos.y) - 2));
    return xy;
}
Vec2 TMXLayer::positionForHexAt(const Vec2& pos)
{
    float diffY = 0;
    if ((int)pos.x % 2 == 1)
    {
        diffY = -m_tMapTileSize.height/2 ;
    }

    Vec2 xy = Vec2(pos.x * m_tMapTileSize.width*3/4,
                            (m_tLayerSize.height - pos.y - 1) * m_tMapTileSize.height + diffY);
    return xy;
}
int TMXLayer::vertexZForPos(const Vec2& pos)
{
    int ret = 0;
    unsigned int maxVal = 0;
    if (m_bUseAutomaticVertexZ)
    {
        switch (m_uLayerOrientation) 
        {
        case CCTMXOrientationIso:
            maxVal = (unsigned int)(m_tLayerSize.width + m_tLayerSize.height);
            ret = (int)(-(maxVal - (pos.x + pos.y)));
            break;
        case CCTMXOrientationOrtho:
            ret = (int)(-(m_tLayerSize.height-pos.y));
            break;
        case CCTMXOrientationHex:
            AXAssert(0, "TMX Hexa zOrder not supported");
            break;
        default:
            AXAssert(0, "TMX invalid value");
            break;
        }
    } 
    else
    {
        ret = m_nVertexZvalue;
    }
    
    return ret;
}

Dictionary * TMXLayer::getProperties()
{
    return m_pProperties;
}
void TMXLayer::setProperties(Dictionary* var)
{
    AX_SAFE_RETAIN(var);
    AX_SAFE_RELEASE(m_pProperties);
    m_pProperties = var;
}

NS_AX_END

