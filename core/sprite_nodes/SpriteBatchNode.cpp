/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2009-2010 Ricardo Quesada
Copyright (c) 2009      Matt Oswald
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
#include "SpriteBatchNode.h"
#include "ccConfig.h"
#include "Sprite.h"
#include "effects/Grid.h"
#include "draw_nodes/DrawingPrimitives.h"
#include "textures/TextureCache.h"
#include "support/PointExtension.h"
#include "shaders/ShaderCache.h"
#include "shaders/GLProgram.h"
#include "shaders/ccGLStateCache.h"
#include "base/Director.h"
#include "support/TransformUtils.h"
#include "support/Profiling.h"
// external
#include "kazmath/GL/matrix.h"

NS_AX_BEGIN

/*
* creation with Texture2D
*/

SpriteBatchNode* SpriteBatchNode::createWithTexture(Texture2D* tex, unsigned int capacity/* = kDefaultSpriteBatchCapacity*/)
{
    SpriteBatchNode *batchNode = new SpriteBatchNode();
    batchNode->initWithTexture(tex, capacity);
    batchNode->autorelease();

    return batchNode;
}

/*
* creation with File Image
*/

SpriteBatchNode* SpriteBatchNode::create(const char *fileImage, unsigned int capacity/* = kDefaultSpriteBatchCapacity*/)
{
    SpriteBatchNode *batchNode = new SpriteBatchNode();
    batchNode->initWithFile(fileImage, capacity);
    batchNode->autorelease();

    return batchNode;
}

/*
* init with Texture2D
*/
bool SpriteBatchNode::initWithTexture(Texture2D *tex, unsigned int capacity)
{
    m_blendFunc.src = AX_BLEND_SRC;
    m_blendFunc.dst = AX_BLEND_DST;
    m_pobTextureAtlas = new TextureAtlas();

    if (0 == capacity)
    {
        capacity = kDefaultSpriteBatchCapacity;
    }
    
    m_pobTextureAtlas->initWithTexture(tex, capacity);

    updateBlendFunc();

    // no lazy alloc in this node
    m_pChildren = new Array();
    m_pChildren->initWithCapacity(capacity);

    m_pobDescendants = new Array();
    m_pobDescendants->initWithCapacity(capacity);

    setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(kAXShader_PositionTextureColor));
    return true;
}

bool SpriteBatchNode::init()
{
    Texture2D * texture = new Texture2D();
    texture->autorelease();
    return this->initWithTexture(texture, 0);
}

/*
* init with FileImage
*/
bool SpriteBatchNode::initWithFile(const char* fileImage, unsigned int capacity)
{
    Texture2D *pTexture2D = TextureCache::sharedTextureCache()->addImage(fileImage);
    return initWithTexture(pTexture2D, capacity);
}

SpriteBatchNode::SpriteBatchNode()
: m_pobTextureAtlas(NULL)
, m_pobDescendants(NULL)
{
}

SpriteBatchNode::~SpriteBatchNode()
{
    AX_SAFE_RELEASE(m_pobTextureAtlas);
    AX_SAFE_RELEASE(m_pobDescendants);
}

// override visit
// don't call visit on it's children
void SpriteBatchNode::visit(void)
{
    AX_PROFILER_START_CATEGORY(kCCProfilerCategoryBatchSprite, "SpriteBatchNode - visit");

    // CAREFUL:
    // This visit is almost identical to CocosNode#visit
    // with the exception that it doesn't call visit on it's children
    //
    // The alternative is to have a void Sprite#visit, but
    // although this is less maintainable, is faster
    //
    if (! m_bVisible)
    {
        return;
    }

    kmGLPushMatrix();

    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->beforeDraw();
        transformAncestors();
    }

    sortAllChildren();
    transform();

    draw();

    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->afterDraw(this);
    }

    kmGLPopMatrix();
    setOrderOfArrival(0);

    AX_PROFILER_STOP_CATEGORY(kCCProfilerCategoryBatchSprite, "SpriteBatchNode - visit");

}

void SpriteBatchNode::addChild(Node *child, int zOrder, int tag)
{
    AXAssert(child != NULL, "child should not be null");
    AXAssert(dynamic_cast<Sprite*>(child) != NULL, "SpriteBatchNode only supports CCSprites as children");
    Sprite *pSprite = (Sprite*)(child);
    // check Sprite is using the same texture id
    AXAssert(pSprite->getTexture()->getName() == m_pobTextureAtlas->getTexture()->getName(), "Sprite is not using the same texture id");

    Node::addChild(child, zOrder, tag);

    appendChild(pSprite);
}

void SpriteBatchNode::addChild(Node *child)
{
    Node::addChild(child);
}

void SpriteBatchNode::addChild(Node *child, int zOrder)
{
    Node::addChild(child, zOrder);
}

// override reorderChild
void SpriteBatchNode::reorderChild(Node *child, int zOrder)
{
    AXAssert(child != NULL, "the child should not be null");
    AXAssert(m_pChildren->containsObject(child), "Child doesn't belong to Sprite");

    if (zOrder == child->getZOrder())
    {
        return;
    }

    //set the z-order and sort later
    Node::reorderChild(child, zOrder);
}

// override remove child
void SpriteBatchNode::removeChild(Node *child, bool cleanup)
{
    Sprite *pSprite = (Sprite*)(child);

    // explicit null handling
    if (pSprite == NULL)
    {
        return;
    }

    AXAssert(m_pChildren->containsObject(pSprite), "sprite batch node should contain the child");

    // cleanup before removing
    removeSpriteFromAtlas(pSprite);

    Node::removeChild(pSprite, cleanup);
}

void SpriteBatchNode::removeChildAtIndex(unsigned int uIndex, bool bDoCleanup)
{
    removeChild((Sprite*)(m_pChildren->objectAtIndex(uIndex)), bDoCleanup);
}

void SpriteBatchNode::removeAllChildrenWithCleanup(bool bCleanup)
{
    // Invalidate atlas index. issue #569
    // useSelfRender should be performed on all descendants. issue #1216
    arrayMakeObjectsPerformSelectorWithObject(m_pobDescendants, setBatchNode, NULL, Sprite*);

    Node::removeAllChildrenWithCleanup(bCleanup);

    m_pobDescendants->removeAllObjects();
    m_pobTextureAtlas->removeAllQuads();
}

//override sortAllChildren
void SpriteBatchNode::sortAllChildren()
{
    if (m_bReorderChildDirty)
    {
        int i = 0,j = 0,length = m_pChildren->data->num;
        Node ** x = (Node**)m_pChildren->data->arr;
        Node *tempItem = NULL;

        //insertion sort
        for(i=1; i<length; i++)
        {
            tempItem = x[i];
            j = i-1;

            //continue moving element downwards while zOrder is smaller or when zOrder is the same but orderOfArrival is smaller
            while(j>=0 && ( tempItem->getZOrder() < x[j]->getZOrder() || ( tempItem->getZOrder() == x[j]->getZOrder() && tempItem->getOrderOfArrival() < x[j]->getOrderOfArrival() ) ) )
            {
                x[j+1] = x[j];
                j--;
            }

            x[j+1] = tempItem;
        }

        //sorted now check all children
        if (m_pChildren->count() > 0)
        {
            //first sort all children recursively based on zOrder
            arrayMakeObjectsPerformSelector(m_pChildren, sortAllChildren, Sprite*);

            int index=0;

            Object* pObj = NULL;
            //fast dispatch, give every child a new atlasIndex based on their relative zOrder (keep parent -> child relations intact)
            // and at the same time reorder descendants and the quads to the right index
            AXARRAY_FOREACH(m_pChildren, pObj)
            {
                Sprite* pChild = (Sprite*)pObj;
                updateAtlasIndex(pChild, &index);
            }
        }

        m_bReorderChildDirty=false;
    }
}

void SpriteBatchNode::updateAtlasIndex(Sprite* sprite, int* curIndex)
{
    unsigned int count = 0;
    Array* pArray = sprite->getChildren();
    if (pArray != NULL)
    {
        count = pArray->count();
    }
    
    int oldIndex = 0;

    if( count == 0 )
    {
        oldIndex = sprite->getAtlasIndex();
        sprite->setAtlasIndex(*curIndex);
        sprite->setOrderOfArrival(0);
        if (oldIndex != *curIndex){
            swap(oldIndex, *curIndex);
        }
        (*curIndex)++;
    }
    else
    {
        bool needNewIndex=true;

        if (((Sprite*) (pArray->data->arr[0]))->getZOrder() >= 0)
        {
            //all children are in front of the parent
            oldIndex = sprite->getAtlasIndex();
            sprite->setAtlasIndex(*curIndex);
            sprite->setOrderOfArrival(0);
            if (oldIndex != *curIndex)
            {
                swap(oldIndex, *curIndex);
            }
            (*curIndex)++;

            needNewIndex = false;
        }

        Object* pObj = NULL;
        AXARRAY_FOREACH(pArray,pObj)
        {
            Sprite* child = (Sprite*)pObj;
            if (needNewIndex && child->getZOrder() >= 0)
            {
                oldIndex = sprite->getAtlasIndex();
                sprite->setAtlasIndex(*curIndex);
                sprite->setOrderOfArrival(0);
                if (oldIndex != *curIndex) {
                    this->swap(oldIndex, *curIndex);
                }
                (*curIndex)++;
                needNewIndex = false;

            }

            updateAtlasIndex(child, curIndex);
        }

        if (needNewIndex)
        {//all children have a zOrder < 0)
            oldIndex=sprite->getAtlasIndex();
            sprite->setAtlasIndex(*curIndex);
            sprite->setOrderOfArrival(0);
            if (oldIndex!=*curIndex) {
                swap(oldIndex, *curIndex);
            }
            (*curIndex)++;
        }
    }
}

void SpriteBatchNode::swap(int oldIndex, int newIndex)
{
    Object** x = m_pobDescendants->data->arr;
    ccV3F_C4B_T2F_Quad* quads = m_pobTextureAtlas->getQuads();

    Object* tempItem = x[oldIndex];
    ccV3F_C4B_T2F_Quad tempItemQuad=quads[oldIndex];

    //update the index of other swapped item
    ((Sprite*) x[newIndex])->setAtlasIndex(oldIndex);

    x[oldIndex]=x[newIndex];
    quads[oldIndex]=quads[newIndex];
    x[newIndex]=tempItem;
    quads[newIndex]=tempItemQuad;
}

void SpriteBatchNode::reorderBatch(bool reorder)
{
    m_bReorderChildDirty=reorder;
}

// draw
void SpriteBatchNode::draw(void)
{
    AX_PROFILER_START("SpriteBatchNode - draw");

    // Optimization: Fast Dispatch
    if( m_pobTextureAtlas->getTotalQuads() == 0 )
    {
        return;
    }

    AX_NODE_DRAW_SETUP();

    arrayMakeObjectsPerformSelector(m_pChildren, updateTransform, Sprite*);

    ccGLBlendFunc( m_blendFunc.src, m_blendFunc.dst );

    m_pobTextureAtlas->drawQuads();

    AX_PROFILER_STOP("SpriteBatchNode - draw");
}

void SpriteBatchNode::increaseAtlasCapacity(void)
{
    // if we're going beyond the current TextureAtlas's capacity,
    // all the previously initialized sprites will need to redo their texture coords
    // this is likely computationally expensive
    unsigned int quantity = (m_pobTextureAtlas->getCapacity() + 1) * 4 / 3;

    AXLOG("cocos2d: SpriteBatchNode: resizing TextureAtlas capacity from [%lu] to [%lu].",
        (long)m_pobTextureAtlas->getCapacity(),
        (long)quantity);

    if (! m_pobTextureAtlas->resizeCapacity(quantity))
    {
        // serious problems
        AXLOGWARN("cocos2d: WARNING: Not enough memory to resize the atlas");
        AXAssert(false, "Not enough memory to resize the atlas");
    }
}

unsigned int SpriteBatchNode::rebuildIndexInOrder(Sprite *pobParent, unsigned int uIndex)
{
    Array *pChildren = pobParent->getChildren();

    if (pChildren && pChildren->count() > 0)
    {
        Object* pObject = NULL;
        AXARRAY_FOREACH(pChildren, pObject)
        {
            Sprite* pChild = (Sprite*) pObject;
            if (pChild && (pChild->getZOrder() < 0))
            {
                uIndex = rebuildIndexInOrder(pChild, uIndex);
            }
        }
    }    

    // ignore self (batch node)
    if (! pobParent->isEqual(this))
    {
        pobParent->setAtlasIndex(uIndex);
        uIndex++;
    }

    if (pChildren && pChildren->count() > 0)
    {
        Object* pObject = NULL;
        AXARRAY_FOREACH(pChildren, pObject)
        {
            Sprite* pChild = (Sprite*) pObject;
            if (pChild && (pChild->getZOrder() >= 0))
            {
                uIndex = rebuildIndexInOrder(pChild, uIndex);
            }
        }
    }

    return uIndex;
}

unsigned int SpriteBatchNode::highestAtlasIndexInChild(Sprite *pSprite)
{
    Array *pChildren = pSprite->getChildren();

    if (! pChildren || pChildren->count() == 0)
    {
        return pSprite->getAtlasIndex();
    }
    else
    {
        return highestAtlasIndexInChild((Sprite*)(pChildren->lastObject()));
    }
}

unsigned int SpriteBatchNode::lowestAtlasIndexInChild(Sprite *pSprite)
{
    Array *pChildren = pSprite->getChildren();

    if (! pChildren || pChildren->count() == 0)
    {
        return pSprite->getAtlasIndex();
    }
    else
    {
        return lowestAtlasIndexInChild((Sprite*)(pChildren->objectAtIndex(0)));
    }
}

unsigned int SpriteBatchNode::atlasIndexForChild(Sprite *pobSprite, int nZ)
{
    Array *pBrothers = pobSprite->getParent()->getChildren();
    unsigned int uChildIndex = pBrothers->indexOfObject(pobSprite);

    // ignore parent Z if parent is spriteSheet
    bool bIgnoreParent = (SpriteBatchNode*)(pobSprite->getParent()) == this;
    Sprite *pPrevious = NULL;
    if (uChildIndex > 0 &&
        uChildIndex < UINT_MAX)
    {
        pPrevious = (Sprite*)(pBrothers->objectAtIndex(uChildIndex - 1));
    }

    // first child of the sprite sheet
    if (bIgnoreParent)
    {
        if (uChildIndex == 0)
        {
            return 0;
        }

        return highestAtlasIndexInChild(pPrevious) + 1;
    }

    // parent is a Sprite, so, it must be taken into account

    // first child of an Sprite ?
    if (uChildIndex == 0)
    {
        Sprite *p = (Sprite*)(pobSprite->getParent());

        // less than parent and brothers
        if (nZ < 0)
        {
            return p->getAtlasIndex();
        }
        else
        {
            return p->getAtlasIndex() + 1;
        }
    }
    else
    {
        // previous & sprite belong to the same branch
        if ((pPrevious->getZOrder() < 0 && nZ < 0) || (pPrevious->getZOrder() >= 0 && nZ >= 0))
        {
            return highestAtlasIndexInChild(pPrevious) + 1;
        }

        // else (previous < 0 and sprite >= 0 )
        Sprite *p = (Sprite*)(pobSprite->getParent());
        return p->getAtlasIndex() + 1;
    }

    // Should not happen. Error calculating Z on SpriteSheet
    AXAssert(0, "should not run here");
    return 0;
}

// add child helper

void SpriteBatchNode::insertChild(Sprite *pSprite, unsigned int uIndex)
{
    pSprite->setBatchNode(this);
    pSprite->setAtlasIndex(uIndex);
    pSprite->setDirty(true);

    if(m_pobTextureAtlas->getTotalQuads() == m_pobTextureAtlas->getCapacity())
    {
        increaseAtlasCapacity();
    }

    ccV3F_C4B_T2F_Quad quad = pSprite->getQuad();
    m_pobTextureAtlas->insertQuad(&quad, uIndex);

    axArray *descendantsData = m_pobDescendants->data;

    axArrayInsertObjectAtIndex(descendantsData, pSprite, uIndex);

    // update indices
    unsigned int i = uIndex+1;
    
    Sprite* pChild = NULL;
    for(; i<descendantsData->num; i++){
        pChild = (Sprite*)descendantsData->arr[i];
        pChild->setAtlasIndex(pChild->getAtlasIndex() + 1);
    }

    // add children recursively
    Object* pObj = NULL;
    AXARRAY_FOREACH(pSprite->getChildren(), pObj)
    {
        pChild = (Sprite*)pObj;
        unsigned int idx = atlasIndexForChild(pChild, pChild->getZOrder());
        insertChild(pChild, idx);
    }
}

// addChild helper, faster than insertChild
void SpriteBatchNode::appendChild(Sprite* sprite)
{
    m_bReorderChildDirty=true;
    sprite->setBatchNode(this);
    sprite->setDirty(true);

    if(m_pobTextureAtlas->getTotalQuads() == m_pobTextureAtlas->getCapacity()) {
        increaseAtlasCapacity();
    }

    axArray *descendantsData = m_pobDescendants->data;

    axArrayAppendObjectWithResize(descendantsData, sprite);

    unsigned int index=descendantsData->num-1;

    sprite->setAtlasIndex(index);

    ccV3F_C4B_T2F_Quad quad = sprite->getQuad();
    m_pobTextureAtlas->insertQuad(&quad, index);

    // add children recursively
    
    Object* pObj = NULL;
    AXARRAY_FOREACH(sprite->getChildren(), pObj)
    {
        Sprite* child = (Sprite*)pObj;
        appendChild(child);
    }
}

void SpriteBatchNode::removeSpriteFromAtlas(Sprite *pobSprite)
{
    // remove from TextureAtlas
    m_pobTextureAtlas->removeQuadAtIndex(pobSprite->getAtlasIndex());

    // Cleanup sprite. It might be reused (issue #569)
    pobSprite->setBatchNode(NULL);

    unsigned int uIndex = m_pobDescendants->indexOfObject(pobSprite);
    if (uIndex != UINT_MAX)
    {
        m_pobDescendants->removeObjectAtIndex(uIndex);

        // update all sprites beyond this one
        unsigned int count = m_pobDescendants->count();
        
        for(; uIndex < count; ++uIndex)
        {
            Sprite* s = (Sprite*)(m_pobDescendants->objectAtIndex(uIndex));
            s->setAtlasIndex( s->getAtlasIndex() - 1 );
        }
    }

    // remove children recursively
    Array *pChildren = pobSprite->getChildren();
    if (pChildren && pChildren->count() > 0)
    {
        Object* pObject = NULL;
        AXARRAY_FOREACH(pChildren, pObject)
        {
            Sprite* pChild = (Sprite*) pObject;
            if (pChild)
            {
                removeSpriteFromAtlas(pChild);
            }
        }
    }
}

void SpriteBatchNode::updateBlendFunc(void)
{
    if (! m_pobTextureAtlas->getTexture()->hasPremultipliedAlpha())
    {
        m_blendFunc.src = GL_SRC_ALPHA;
        m_blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
    }
    else
    {
        m_blendFunc.src = AX_BLEND_SRC;
        m_blendFunc.dst = AX_BLEND_DST;
    }
}

// CocosNodeTexture protocol
void SpriteBatchNode::setBlendFunc(ccBlendFunc blendFunc)
{
    m_blendFunc = blendFunc;
}

ccBlendFunc SpriteBatchNode::getBlendFunc(void)
{
    return m_blendFunc;
}

Texture2D* SpriteBatchNode::getTexture(void)
{
    return m_pobTextureAtlas->getTexture();
}

void SpriteBatchNode::setTexture(Texture2D *texture)
{
    m_pobTextureAtlas->setTexture(texture);
    updateBlendFunc();
}


// CCSpriteSheet Extension
//implementation CCSpriteSheet (TMXTiledMapExtension)

void SpriteBatchNode::insertQuadFromSprite(Sprite *sprite, unsigned int index)
{
    AXAssert( sprite != NULL, "Argument must be non-NULL");
    AXAssert( dynamic_cast<Sprite*>(sprite), "SpriteBatchNode only supports CCSprites as children");

    // make needed room
    while(index >= m_pobTextureAtlas->getCapacity() || m_pobTextureAtlas->getCapacity() == m_pobTextureAtlas->getTotalQuads())
    {
        this->increaseAtlasCapacity();
    }
    //
    // update the quad directly. Don't add the sprite to the scene graph
    //
    sprite->setBatchNode(this);
    sprite->setAtlasIndex(index);

    ccV3F_C4B_T2F_Quad quad = sprite->getQuad();
    m_pobTextureAtlas->insertQuad(&quad, index);

    // XXX: updateTransform will update the textureAtlas too, using updateQuad.
    // XXX: so, it should be AFTER the insertQuad
    sprite->setDirty(true);
    sprite->updateTransform();
}

void SpriteBatchNode::updateQuadFromSprite(Sprite *sprite, unsigned int index)
{
    AXAssert(sprite != NULL, "Argument must be non-nil");
    AXAssert(dynamic_cast<Sprite*>(sprite) != NULL, "SpriteBatchNode only supports CCSprites as children");
    
	// make needed room
	while (index >= m_pobTextureAtlas->getCapacity() || m_pobTextureAtlas->getCapacity() == m_pobTextureAtlas->getTotalQuads())
    {
		this->increaseAtlasCapacity();
    }
    
	//
	// update the quad directly. Don't add the sprite to the scene graph
	//
	sprite->setBatchNode(this);
    sprite->setAtlasIndex(index);
    
	sprite->setDirty(true);
	
	// UpdateTransform updates the textureAtlas quad
	sprite->updateTransform();
}

SpriteBatchNode * SpriteBatchNode::addSpriteWithoutQuad(Sprite*child, unsigned int z, int aTag)
{
    AXAssert( child != NULL, "Argument must be non-NULL");
    AXAssert( dynamic_cast<Sprite*>(child), "SpriteBatchNode only supports CCSprites as children");

    // quad index is Z
    child->setAtlasIndex(z);

    // XXX: optimize with a binary search
    int i=0;
 
    Object* pObject = NULL;
    AXARRAY_FOREACH(m_pobDescendants, pObject)
    {
        Sprite* pChild = (Sprite*) pObject;
        if (pChild && (pChild->getAtlasIndex() >= z))
            break;
        ++i;
    }
    
    m_pobDescendants->insertObject(child, i);

    // IMPORTANT: Call super, and not self. Avoid adding it to the texture atlas array
    Node::addChild(child, z, aTag);
    //#issue 1262 don't use lazy sorting, tiles are added as quads not as sprites, so sprites need to be added in order
    reorderBatch(false);

    return this;
}

NS_AX_END
