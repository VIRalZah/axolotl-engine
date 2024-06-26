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
#ifndef __AXPARALLAX_NODE_H__
#define __AXPARALLAX_NODE_H__

#include "base/Node.h"
/*#include "support/data_support/axArray.h"*/

NS_AX_BEGIN

struct _axArray;

/**
 * @addtogroup tilemap_parallAX_nodes
 * @{
 */

/** @brief ParallaxNode: A node that simulates a parallax scroller

The children will be moved faster / slower than the parent according the the parallax ratio.

*/
class AX_DLL ParallaxNode : public Node 
{
    /** array that holds the offset / ratio of the children */
    AX_SYNTHESIZE(struct _axArray *, m_pParallaxArray, ParallaxArray)

public:
    /** Adds a child to the container with a z-order, a parallax ratio and a position offset
    It returns self, so you can chain several addChilds.
    @since v0.8
    @js ctor
    */
    ParallaxNode();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~ParallaxNode();

    static ParallaxNode * create();
    virtual void addChild(Node * child, unsigned int z, const Vec2& parallaxRatio, const Vec2& positionOffset);
    // super methods
    virtual void addChild(Node * child, unsigned int zOrder, int tag);
    virtual void removeChild(Node* child, bool cleanup);
    virtual void removeAllChildrenWithCleanup(bool cleanup);
    virtual void visit(void);
private:
    Vec2 absolutePosition();
protected:
    Vec2    m_tLastPosition;
};

// end of tilemap_parallAX_nodes group
/// @}

NS_AX_END

#endif //__AXPARALLAX_NODE_H__


