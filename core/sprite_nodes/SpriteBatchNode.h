/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2009-2010 Ricardo Quesada
Copyright (C) 2009      Matt Oswald
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

#ifndef __AX_SPRITE_BATCH_NODE_H__
#define __AX_SPRITE_BATCH_NODE_H__

#include "base/Node.h"
#include "Protocols.h"
#include "textures/TextureAtlas.h"
#include "ccMacros.h"
#include "base/Array.h"

NS_AX_BEGIN

/**
 * @addtogroup sprite_nodes
 * @{
 */

#define kDefaultSpriteBatchCapacity   29

class Sprite;

/** SpriteBatchNode is like a batch node: if it contains children, it will draw them in 1 single OpenGL call
* (often known as "batch draw").
*
* A SpriteBatchNode can reference one and only one texture (one image file, one texture atlas).
* Only the CCSprites that are contained in that texture can be added to the SpriteBatchNode.
* All CCSprites added to a SpriteBatchNode are drawn in one OpenGL ES draw call.
* If the CCSprites are not added to a SpriteBatchNode then an OpenGL ES draw call will be needed for each one, which is less efficient.
*
*
* Limitations:
*  - The only object that is accepted as child (or grandchild, grand-grandchild, etc...) is Sprite or any subclass of Sprite. eg: particles, labels and layer can't be added to a SpriteBatchNode.
*  - Either all its children are Aliased or Antialiased. It can't be a mix. This is because "alias" is a property of the texture, and all the sprites share the same texture.
* 
* @since v0.7.1
*/
class AX_DLL SpriteBatchNode : public Node, public TextureProtocol
{
public:
    /**
     *  @js ctor
     */
    SpriteBatchNode();
    /**
     * @js NA
     * @lua NA
     */
    ~SpriteBatchNode();

    // property
    
    // retain
    inline TextureAtlas* getTextureAtlas(void) { return m_pobTextureAtlas; }
    inline void setTextureAtlas(TextureAtlas* textureAtlas) 
    { 
        if (textureAtlas != m_pobTextureAtlas)
        {
            AX_SAFE_RETAIN(textureAtlas);
            AX_SAFE_RELEASE(m_pobTextureAtlas);
            m_pobTextureAtlas = textureAtlas;
        }
    }

    inline Array* getDescendants(void) { return m_pobDescendants; }

    /** creates a SpriteBatchNode with a texture2d and capacity of children.
    The capacity will be increased in 33% in runtime if it run out of space.
    */
    static SpriteBatchNode* createWithTexture(Texture2D* tex, unsigned int capacity);
    static SpriteBatchNode* createWithTexture(Texture2D* tex) {
        return SpriteBatchNode::createWithTexture(tex, kDefaultSpriteBatchCapacity);
    }

    /** creates a SpriteBatchNode with a file image (.png, .jpeg, .pvr, etc) and capacity of children.
    The capacity will be increased in 33% in runtime if it run out of space.
    The file will be loaded using the TextureMgr.
    */
    static SpriteBatchNode* create(const char* fileImage, unsigned int capacity);
    static SpriteBatchNode* create(const char* fileImage) {
        return SpriteBatchNode::create(fileImage, kDefaultSpriteBatchCapacity);
    }

    /** initializes a SpriteBatchNode with a texture2d and capacity of children.
    The capacity will be increased in 33% in runtime if it run out of space.
    */
    bool initWithTexture(Texture2D *tex, unsigned int capacity);
    /** initializes a SpriteBatchNode with a file image (.png, .jpeg, .pvr, etc) and a capacity of children.
    The capacity will be increased in 33% in runtime if it run out of space.
    The file will be loaded using the TextureMgr.
    */
    bool initWithFile(const char* fileImage, unsigned int capacity);
    bool init();

    void increaseAtlasCapacity();

    /** removes a child given a certain index. It will also cleanup the running actions depending on the cleanup parameter.
    @warning Removing a child from a SpriteBatchNode is very slow
    */
    void removeChildAtIndex(unsigned int index, bool doCleanup);

    void insertChild(Sprite *child, unsigned int index);
    void appendChild(Sprite* sprite);
    void removeSpriteFromAtlas(Sprite *sprite);

    unsigned int rebuildIndexInOrder(Sprite *parent, unsigned int index);
    unsigned int highestAtlasIndexInChild(Sprite *sprite);
    unsigned int lowestAtlasIndexInChild(Sprite *sprite);
    unsigned int atlasIndexForChild(Sprite *sprite, int z);
    /* Sprites use this to start sortChildren, don't call this manually */
    void reorderBatch(bool reorder);
    // TextureProtocol
    virtual Texture2D* getTexture(void);
    virtual void setTexture(Texture2D *texture);
    virtual void setBlendFunc(ccBlendFunc blendFunc);
    /**
     * @js NA
     */
    virtual ccBlendFunc getBlendFunc(void);

    virtual void visit(void);
    virtual void addChild(Node * child);
    virtual void addChild(Node * child, int zOrder);
    virtual void addChild(Node * child, int zOrder, int tag);
    virtual void reorderChild(Node * child, int zOrder);
        
    virtual void removeChild(Node* child, bool cleanup);
    virtual void removeAllChildrenWithCleanup(bool cleanup);
    virtual void sortAllChildren();
    virtual void draw(void);

protected:
    /** Inserts a quad at a certain index into the texture atlas. The Sprite won't be added into the children array.
     This method should be called only when you are dealing with very big AtlasSrite and when most of the Sprite won't be updated.
     For example: a tile map (CCTMXMap) or a label with lots of characters (LabelBMFont)
     */
    void insertQuadFromSprite(Sprite *sprite, unsigned int index);
    /** Updates a quad at a certain index into the texture atlas. The Sprite won't be added into the children array.
     This method should be called only when you are dealing with very big AtlasSrite and when most of the Sprite won't be updated.
     For example: a tile map (CCTMXMap) or a label with lots of characters (LabelBMFont)
     */
    void updateQuadFromSprite(Sprite *sprite, unsigned int index);
    /* This is the opposite of "addQuadFromSprite.
    It add the sprite to the children and descendants array, but it doesn't update add it to the texture atlas
    */
    SpriteBatchNode * addSpriteWithoutQuad(Sprite*child, unsigned int z, int aTag);

private:
    void updateAtlasIndex(Sprite* sprite, int* curIndex);
    void swap(int oldIndex, int newIndex);
    void updateBlendFunc();

protected:
    TextureAtlas *m_pobTextureAtlas;
    ccBlendFunc m_blendFunc;

    // all descendants: children, gran children, etc...
    Array* m_pobDescendants;
};

// end of sprite_nodes group
/// @}

NS_AX_END

#endif // __AX_SPRITE_BATCH_NODE_H__
