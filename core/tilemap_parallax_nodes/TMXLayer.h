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
#ifndef __AXTMX_LAYER_H__
#define __AXTMX_LAYER_H__

#include "TMXObjectGroup.h"
#include "base/AtlasNode.h"
#include "sprite_nodes/SpriteBatchNode.h"
#include "TMXXMLParser.h"
NS_AX_BEGIN

class TMXMapInfo;
class TMXLayerInfo;
class TMXTilesetInfo;
struct _axCArray;

/**
 * @addtogroup tilemap_parallAX_nodes
 * @{
 */

/** @brief TMXLayer represents the TMX layer.

It is a subclass of SpriteBatchNode. By default the tiles are rendered using a TextureAtlas.
If you modify a tile on runtime, then, that tile will become a Sprite, otherwise no Sprite objects are created.
The benefits of using Sprite objects as tiles are:
- tiles (Sprite) can be rotated/scaled/moved with a nice API

If the layer contains a property named "AX_vertexz" with an integer (in can be positive or negative),
then all the tiles belonging to the layer will use that value as their OpenGL vertex Z for depth.

On the other hand, if the "AX_vertexz" property has the "automatic" value, then the tiles will use an automatic vertex Z value.
Also before drawing the tiles, GL_ALPHA_TEST will be enabled, and disabled after drawing them. The used alpha func will be:

glAlphaFunc( GL_GREATER, value )

"value" by default is 0, but you can change it from Tiled by adding the "AX_alpha_func" property to the layer.
The value 0 should work for most cases, but if you have tiles that are semi-transparent, then you might want to use a different
value, like 0.5.

For further information, please see the programming guide:

http://www.cocos2d-iphone.org/wiki/doku.php/prog_guide:tiled_maps

@since v0.8.1
Tiles can have tile flags for additional properties. At the moment only flip horizontal and flip vertical are used. These bit flags are defined in CCTMXXMLParser.h.

@since 1.1
*/

class AX_DLL TMXLayer : public SpriteBatchNode
{
    /** size of the layer in tiles */
    AX_SYNTHESIZE_PASS_BY_REF(Size, m_tLayerSize, LayerSize);
    /** size of the map's tile (could be different from the tile's size) */
    AX_SYNTHESIZE_PASS_BY_REF(Size, m_tMapTileSize, MapTileSize);
    /** pointer to the map of tiles */
    AX_SYNTHESIZE(unsigned int*, m_pTiles, Tiles);
    /** Tileset information for the layer */
    AX_PROPERTY(TMXTilesetInfo*, m_pTileSet, TileSet);
    /** Layer orientation, which is the same as the map orientation */
    AX_SYNTHESIZE(unsigned int, m_uLayerOrientation, LayerOrientation);
    /** properties from the layer. They can be added using Tiled */
    AX_PROPERTY(Dictionary*, m_pProperties, Properties);
public:
    /**
     * @js ctor
     * @lua NA
     */
    TMXLayer();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~TMXLayer();
  
    /** creates a TMXLayer with an tileset info, a layer info and a map info */
    static TMXLayer * create(TMXTilesetInfo *tilesetInfo, TMXLayerInfo *layerInfo, TMXMapInfo *mapInfo);

    /** initializes a TMXLayer with a tileset info, a layer info and a map info 
     * @lua NA
     */
    bool initWithTilesetInfo(TMXTilesetInfo *tilesetInfo, TMXLayerInfo *layerInfo, TMXMapInfo *mapInfo);

    /** dealloc the map that contains the tile position from memory.
    Unless you want to know at runtime the tiles positions, you can safely call this method.
    If you are going to call layer->tileGIDAt() then, don't release the map
    */
    void releaseMap();

    /** returns the tile (Sprite) at a given a tile coordinate.
    The returned Sprite will be already added to the TMXLayer. Don't add it again.
    The Sprite can be treated like any other Sprite: rotated, scaled, translated, opacity, color, etc.
    You can remove either by calling:
    - layer->removeChild(sprite, cleanup);
    - or layer->removeTileAt(Vec2(x,y));
    @js getTileGIDAt
    */
    Sprite* tileAt(const Vec2& tileCoordinate);

    /** returns the tile gid at a given tile coordinate.
    if it returns 0, it means that the tile is empty.
    This method requires the the tile map has not been previously released (eg. don't call layer->releaseMap())
    @js tileGIDAt
    */
    unsigned int  tileGIDAt(const Vec2& tileCoordinate);

    /** returns the tile gid at a given tile coordinate. It also returns the tile flags.
     This method requires the the tile map has not been previously released (eg. don't call [layer releaseMap])
     @js tileGIDAt
     @lua NA
     */
    unsigned int tileGIDAt(const Vec2& tileCoordinate, ccTMXTileFlags* flags);

    /** sets the tile gid (gid = tile global id) at a given tile coordinate.
    The Tile GID can be obtained by using the method "tileGIDAt" or by using the TMX editor -> Tileset Mgr +1.
    If a tile is already placed at that position, then it will be removed.
    */
    void setTileGID(unsigned int gid, const Vec2& tileCoordinate);

    /** sets the tile gid (gid = tile global id) at a given tile coordinate.
     The Tile GID can be obtained by using the method "tileGIDAt" or by using the TMX editor -> Tileset Mgr +1.
     If a tile is already placed at that position, then it will be removed.
     
     Use withFlags if the tile flags need to be changed as well
     */

    void setTileGID(unsigned int gid, const Vec2& tileCoordinate, ccTMXTileFlags flags);

    /** removes a tile at given tile coordinate */
    void removeTileAt(const Vec2& tileCoordinate);

    /** returns the position in points of a given tile coordinate 
     * @js getPositionAt
     */
    Vec2 positionAt(const Vec2& tileCoordinate);

    /** return the value for the specific property name 
     *  @js getProperty
     */
    String *propertyNamed(const char *propertyName);

    /** Creates the tiles */
    void setupTiles();

    /** TMXLayer doesn't support adding a Sprite manually.
     *  @warning addchild(z, tag); is not supported on TMXLayer. Instead of setTileGID.
     *  @lua NA
     */
    virtual void addChild(Node * child, int zOrder, int tag);
    /** super method
     *  @lua NA
     */
    void removeChild(Node* child, bool cleanup);

    inline const char* getLayerName(){ return m_sLayerName.c_str(); }
    inline void setLayerName(const char *layerName){ m_sLayerName = layerName; }
private:
    Vec2 positionForIsoAt(const Vec2& pos);
    Vec2 positionForOrthoAt(const Vec2& pos);
    Vec2 positionForHexAt(const Vec2& pos);

    Vec2 calculateLayerOffset(const Vec2& offset);

    /* optimization methods */
    Sprite* appendTileForGID(unsigned int gid, const Vec2& pos);
    Sprite* insertTileForGID(unsigned int gid, const Vec2& pos);
    Sprite* updateTileForGID(unsigned int gid, const Vec2& pos);

    /* The layer recognizes some special properties, like AX_vertez */
    void parseInternalProperties();
    void setupTileSprite(Sprite* sprite, Vec2 pos, unsigned int gid);
    Sprite* reusedTileWithRect(Rect rect);
    int vertexZForPos(const Vec2& pos);

    // index
    unsigned int atlasIndexForExistantZ(unsigned int z);
    unsigned int atlasIndexForNewZ(int z);
protected:
    //! name of the layer
    std::string m_sLayerName;
    //! TMX Layer supports opacity
    unsigned char        m_cOpacity;

    unsigned int        m_uMinGID;
    unsigned int        m_uMaxGID;

    //! Only used when vertexZ is used
    int                    m_nVertexZvalue;
    bool                m_bUseAutomaticVertexZ;

    //! used for optimization
    Sprite            *m_pReusedTile;
    axCArray            *m_pAtlasIndexArray;
    
    // used for retina display
    float               _contentScaleFactor;            
};

// end of tilemap_parallAX_nodes group
/// @}

NS_AX_END

#endif //__AXTMX_LAYER_H__

