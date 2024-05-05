/****************************************************************************
 Copyright (c) 2012 cocos2d-x.org
 Copyright (c) 2010 Sangwoo Im
 
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

#ifndef __AXSCROLLVIEW_H__
#define __AXSCROLLVIEW_H__

#include "axolotl.h"
#include "ExtensionMacros.h"

NS_AX_EXT_BEGIN

/**
 * @addtogroup GUI
 * @{
 */

typedef enum {
	kCCScrollViewDirectionNone = -1,
    kCCScrollViewDirectionHorizontal = 0,
    kCCScrollViewDirectionVertical,
    kCCScrollViewDirectionBoth
} CCScrollViewDirection;

class CCScrollView;
/**
 *  @js NA
 *  @lua NA
 */
class AX_EX_DLL CCScrollViewDelegate
{
public:
    virtual ~CCScrollViewDelegate() {}
    virtual void scrollViewDidScroll(CCScrollView* view) = 0;
    virtual void scrollViewDidZoom(CCScrollView* view) = 0;
};


/**
 * ScrollView support for cocos2d for iphone.
 * It provides scroll view functionalities to cocos2d projects natively.
 * @lua NA
 */
class AX_EX_DLL CCScrollView : public Layer
{
public:
    /**
     *  @js ctor
     */
    CCScrollView();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~CCScrollView();

    bool init();

    /**
     * Returns an autoreleased scroll view object.
     *
     * @param size view size
     * @param container parent object
     * @return autoreleased scroll view object
     */
    static CCScrollView* create(Size size, Node* container = NULL);

    /**
     * Returns an autoreleased scroll view object.
     *
     * @param size view size
     * @param container parent object
     * @return autoreleased scroll view object
     */
    static CCScrollView* create();

    /**
     * Returns a scroll view object
     *
     * @param size view size
     * @param container parent object
     * @return scroll view object
     */
    bool initWithViewSize(Size size, Node* container = NULL);


    /**
     * Sets a new content offset. It ignores max/min offset. It just sets what's given. (just like UIKit's UIScrollView)
     *
     * @param offset new offset
     * @param If YES, the view scrolls to the new offset
     */
    void setContentOffset(Vec2 offset, bool animated = false);
    Vec2 getContentOffset();
    /**
     * Sets a new content offset. It ignores max/min offset. It just sets what's given. (just like UIKit's UIScrollView)
     * You can override the animation duration with this method.
     *
     * @param offset new offset
     * @param animation duration
     */
    void setContentOffsetInDuration(Vec2 offset, float dt); 

    void setZoomScale(float s);
    /**
     * Sets a new scale and does that for a predefined duration.
     *
     * @param s a new scale vale
     * @param animated if YES, scaling is animated
     */
    void setZoomScale(float s, bool animated);

    float getZoomScale();

    /**
     * Sets a new scale for container in a given duration.
     *
     * @param s a new scale value
     * @param animation duration
     */
    void setZoomScaleInDuration(float s, float dt);
    /**
     * Returns the current container's minimum offset. You may want this while you animate scrolling by yourself
     */
    Vec2 minContainerOffset();
    /**
     * Returns the current container's maximum offset. You may want this while you animate scrolling by yourself
     */
    Vec2 maxContainerOffset(); 
    /**
     * Determines if a given node's bounding box is in visible bounds
     *
     * @return YES if it is in visible bounds
     */
    bool isNodeVisible(Node * node);
    /**
     * Provided to make scroll view compatible with SWLayer's pause method
     */
    void pause(Object* sender);
    /**
     * Provided to make scroll view compatible with SWLayer's resume method
     */
    void resume(Object* sender);


    bool isDragging() {return m_bDragging;}
    bool isTouchMoved() { return m_bTouchMoved; }
    bool isBounceable() { return m_bBounceable; }
    void setBounceable(bool bBounceable) { m_bBounceable = bBounceable; }

    /**
     * size to clip. Node boundingBox uses contentSize directly.
     * It's semantically different what it actually means to common scroll views.
     * Hence, this scroll view will use a separate size property.
     */
    Size getViewSize() { return m_tViewSize; } 
    void setViewSize(Size size);

    Node * getContainer();
    void setContainer(Node * pContainer);

    /**
     * direction allowed to scroll. CCScrollViewDirectionBoth by default.
     */
    CCScrollViewDirection getDirection() { return m_eDirection; }
    virtual void setDirection(CCScrollViewDirection eDirection) { m_eDirection = eDirection; }

    CCScrollViewDelegate* getDelegate() { return _delegate; }
    void setDelegate(CCScrollViewDelegate* pDelegate) { _delegate = pDelegate; }

    /** override functions */
    // optional
    virtual bool ccTouchBegan(Touch *pTouch);
    virtual void ccTouchMoved(Touch *pTouch);
    virtual void ccTouchEnded(Touch *pTouch);
    virtual void ccTouchCancelled(Touch *pTouch);

    virtual void setContentSize(const Size & size);
    virtual const Size& getContentSize() const;

	void updateInset();
    /**
     * Determines whether it clips its children or not.
     */
    bool isClippingToBounds() { return m_bClippingToBounds; }
    void setClippingToBounds(bool bClippingToBounds) { m_bClippingToBounds = bClippingToBounds; }
    /**
     *  @js NA
     */
    virtual void visit();
    virtual void addChild(Node * child, int zOrder, int tag);
    virtual void addChild(Node * child, int zOrder);
    virtual void addChild(Node * child);
    void setTouchEnabled(bool e);
private:
    /**
     * Relocates the container at the proper offset, in bounds of max/min offsets.
     *
     * @param animated If YES, relocation is animated
     */
    void relocateContainer(bool animated);
    /**
     * implements auto-scrolling behavior. change SCROLL_DEAAXEL_RATE as needed to choose
     * deacceleration speed. it must be less than 1.0f.
     *
     * @param dt delta
     */
    void deaccelerateScrolling(float dt);
    /**
     * This method makes sure auto scrolling causes delegate to invoke its method
     */
    void performedAnimatedScroll(float dt);
    /**
     * Expire animated scroll delegate calls
     */
    void stoppedAnimatedScroll(Node* node);
    /**
     * clip this view so that outside of the visible bounds can be hidden.
     */
    void beforeDraw();
    /**
     * retract what's done in beforeDraw so that there's no side effect to
     * other nodes.
     */
    void afterDraw();
    /**
     * Zoom handling
     */
    void handleZoom();

protected:
    Rect getViewRect();
    
    /**
     * current zoom scale
     */
    float m_fZoomScale;
    /**
     * min zoom scale
     */
    float m_fMinZoomScale;
    /**
     * max zoom scale
     */
    float m_fMaxZoomScale;
    /**
     * scroll view delegate
     */
    CCScrollViewDelegate* _delegate;

    CCScrollViewDirection m_eDirection;
    /**
     * If YES, the view is being dragged.
     */
    bool m_bDragging;

    /**
     * Content offset. Note that left-bottom point is the origin
     */
    Vec2 m_tContentOffset;

    /**
     * Container holds scroll view contents, Sets the scrollable container object of the scroll view
     */
    Node* m_pContainer;
    /**
     * Determiens whether user touch is moved after begin phase.
     */
    bool m_bTouchMoved;
    /**
     * max inset point to limit scrolling by touch
     */
    Vec2 m_fMaxInset;
    /**
     * min inset point to limit scrolling by touch
     */
    Vec2 m_fMinInset;
    /**
     * Determines whether the scroll view is allowed to bounce or not.
     */
    bool m_bBounceable;

    bool m_bClippingToBounds;

    /**
     * scroll speed
     */
    Vec2 m_tScrollDistance;
    /**
     * Touch point
     */
    Vec2 m_tTouchPoint;
    /**
     * length between two fingers
     */
    float m_fTouchLength;
    /**
     * UITouch objects to detect multitouch
     */
    Array* m_pTouches;
    /**
     * size to clip. Node boundingBox uses contentSize directly.
     * It's semantically different what it actually means to common scroll views.
     * Hence, this scroll view will use a separate size property.
     */
    Size m_tViewSize;
    /**
     * max and min scale
     */
    float m_fMinScale, m_fMaxScale;
    /**
     * scissor rect for parent, just for restoring GL_SCISSOR_BOX
     */
    Rect m_tParentScissorRect;
    bool m_bScissorRestored;
public:
    enum ScrollViewScriptEventType
    {
        kScrollViewScroll   = 0,
        kScrollViewZoom,
    };
    void registerScriptHandler(int nFunID,int nScriptEventType);
    void unregisterScriptHandler(int nScriptEventType);
    int  getScriptHandler(int nScriptEventType);
private:
    std::map<int,int> m_mapScriptHandler;
};

// end of GUI group
/// @}

NS_AX_EXT_END

#endif /* __AXSCROLLVIEW_H__ */
