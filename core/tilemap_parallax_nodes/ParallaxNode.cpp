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
#include "ParallaxNode.h"
#include "support/PointExtension.h"
#include "support/data_support/axCArray.h"

NS_AX_BEGIN

class CCPointObject : Object
{
    AX_SYNTHESIZE(Vec2, m_tRatio, Ratio)
    AX_SYNTHESIZE(Vec2, m_tOffset, Offset)
    AX_SYNTHESIZE(Node *,m_pChild, Child)    // weak ref

public:
    static CCPointObject * pointWithCCPoint(Vec2 ratio, Vec2 offset)
    {
        CCPointObject *pRet = new CCPointObject();
        pRet->initWithCCPoint(ratio, offset);
        pRet->autorelease();
        return pRet;
    }
    bool initWithCCPoint(Vec2 ratio, Vec2 offset)
    {
        m_tRatio = ratio;
        m_tOffset = offset;
        m_pChild = NULL;
        return true;
    }
};

ParallaxNode::ParallaxNode()
{
    m_pParallaxArray = axArrayNew(5);        
    m_tLastPosition = Vec2(-100,-100);
}
ParallaxNode::~ParallaxNode()
{
    if( m_pParallaxArray )
    {
        axArrayFree(m_pParallaxArray);
        m_pParallaxArray = NULL;
    }
}

ParallaxNode * ParallaxNode::create()
{
    ParallaxNode *pRet = new ParallaxNode();
    pRet->autorelease();
    return pRet;
}

void ParallaxNode::addChild(Node * child, unsigned int zOrder, int tag)
{
    AX_UNUSED_PARAM(zOrder);
    AX_UNUSED_PARAM(child);
    AX_UNUSED_PARAM(tag);
    AXAssert(0,"ParallaxNode: use addChild:z:parallaxRatio:positionOffset instead");
}
void ParallaxNode::addChild(Node *child, unsigned int z, const Vec2& ratio, const Vec2& offset)
{
    AXAssert( child != NULL, "Argument must be non-nil");
    CCPointObject *obj = CCPointObject::pointWithCCPoint(ratio, offset);
    obj->setChild(child);
    axArrayAppendObjectWithResize(m_pParallaxArray, (Object*)obj);

    Vec2 pos = m_obPosition;
    pos.x = pos.x * ratio.x + offset.x;
    pos.y = pos.y * ratio.y + offset.y;
    child->setPosition(pos);

    Node::addChild(child, z, child->getTag());
}
void ParallaxNode::removeChild(Node* child, bool cleanup)
{
    for( unsigned int i=0;i < m_pParallaxArray->num;i++)
    {
        CCPointObject *point = (CCPointObject*)m_pParallaxArray->arr[i];
        if( point->getChild()->isEqual(child)) 
        {
            axArrayRemoveObjectAtIndex(m_pParallaxArray, i, true);
            break;
        }
    }
    Node::removeChild(child, cleanup);
}
void ParallaxNode::removeAllChildrenWithCleanup(bool cleanup)
{
    axArrayRemoveAllObjects(m_pParallaxArray);
    Node::removeAllChildrenWithCleanup(cleanup);
}
Vec2 ParallaxNode::absolutePosition()
{
    Vec2 ret = m_obPosition;
    Node *cn = this;
    while (cn->getParent() != NULL)
    {
        cn = cn->getParent();
        ret = PointAdd( ret,  cn->getPosition());
    }
    return ret;
}

/*
The positions are updated at visit because:
- using a timer is not guaranteed that it will called after all the positions were updated
- overriding "draw" will only precise if the children have a z > 0
*/
void ParallaxNode::visit()
{
    //    Vec2 pos = position_;
    //    Vec2    pos = [self convertToWorldSpace:Vec2::ZERO];
    Vec2 pos = this->absolutePosition();
    if( ! pos.equals(m_tLastPosition) )
    {
        for(unsigned int i=0; i < m_pParallaxArray->num; i++ ) 
        {
            CCPointObject *point = (CCPointObject*)m_pParallaxArray->arr[i];
            float x = -pos.x + pos.x * point->getRatio().x + point->getOffset().x;
            float y = -pos.y + pos.y * point->getRatio().y + point->getOffset().y;            
            point->getChild()->setPosition(Vec2(x,y));
        }
        m_tLastPosition = pos;
    }
    Node::visit();
}

NS_AX_END
