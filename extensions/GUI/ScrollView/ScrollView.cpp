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

#include "ScrollView.h"

NS_AX_EXT_BEGIN

#define SCROLL_DEAAXEL_RATE  0.95f
#define SCROLL_DEAAXEL_DIST  1.0f
#define BOUNCE_DURATION      0.15f
#define INSET_RATIO          0.2f
#define MOVE_INCH            7.0f/160.0f

static float convertDistanceFromPointToInch(float pointDis)
{
    float factor = ( EGLView::sharedEGLView()->getScaleX() + EGLView::sharedEGLView()->getScaleY() ) / 2;
    return pointDis * factor / Device::getDPI();
}


CCScrollView::CCScrollView()
: m_fZoomScale(0.0f)
, m_fMinZoomScale(0.0f)
, m_fMaxZoomScale(0.0f)
, _delegate(NULL)
, m_eDirection(kCCScrollViewDirectionBoth)
, m_bDragging(false)
, m_pContainer(NULL)
, m_bTouchMoved(false)
, m_bBounceable(false)
, m_bClippingToBounds(false)
, m_fTouchLength(0.0f)
, m_pTouches(NULL)
, m_fMinScale(0.0f)
, m_fMaxScale(0.0f)
{

}

CCScrollView::~CCScrollView()
{
    AX_SAFE_RELEASE(m_pTouches);
    this->unregisterScriptHandler(kScrollViewScroll);
    this->unregisterScriptHandler(kScrollViewZoom);
}

CCScrollView* CCScrollView::create(Size size, Node* container/* = NULL*/)
{
    CCScrollView* pRet = new CCScrollView();
    if (pRet && pRet->initWithViewSize(size, container))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }
    return pRet;
}

CCScrollView* CCScrollView::create()
{
    CCScrollView* pRet = new CCScrollView();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }
    return pRet;
}


bool CCScrollView::initWithViewSize(Size size, Node *container/* = NULL*/)
{
    if (Layer::init())
    {
        m_pContainer = container;
        
        if (!this->m_pContainer)
        {
            m_pContainer = Layer::create();
            this->m_pContainer->ignoreAnchorPointForPosition(false);
            this->m_pContainer->setAnchorPoint(Point(0.0f, 0.0f));
        }

        this->setViewSize(size);

        setTouchEnabled(true);
        m_pTouches = new Array();
        _delegate = NULL;
        m_bBounceable = true;
        m_bClippingToBounds = true;
        //m_pContainer->setContentSize(CCSizeZero);
        m_eDirection  = kCCScrollViewDirectionBoth;
        m_pContainer->setPosition(Point(0.0f, 0.0f));
        m_fTouchLength = 0.0f;
        
        this->addChild(m_pContainer);
        m_fMinScale = m_fMaxScale = 1.0f;
        m_mapScriptHandler.clear();
        return true;
    }
    return false;
}

bool CCScrollView::init()
{
    return this->initWithViewSize(CCSizeMake(200, 200), NULL);
}

bool CCScrollView::isNodeVisible(Node* node)
{
    const Point offset = this->getContentOffset();
    const Size  size   = this->getViewSize();
    const float   scale  = this->getZoomScale();
    
    Rect viewRect;
    
    viewRect = CCRectMake(-offset.x/scale, -offset.y/scale, size.width/scale, size.height/scale); 
    
    return viewRect.intersectsRect(node->boundingBox());
}

void CCScrollView::pause(Object* sender)
{
    m_pContainer->pauseSchedulerAndActions();

    Object* pObj = NULL;
    Array* pChildren = m_pContainer->getChildren();

    AXARRAY_FOREACH(pChildren, pObj)
    {
        Node* pChild = (Node*)pObj;
        pChild->pauseSchedulerAndActions();
    }
}

void CCScrollView::resume(Object* sender)
{
    Object* pObj = NULL;
    Array* pChildren = m_pContainer->getChildren();

    AXARRAY_FOREACH(pChildren, pObj)
    {
        Node* pChild = (Node*)pObj;
        pChild->resumeSchedulerAndActions();
    }

    m_pContainer->resumeSchedulerAndActions();
}

void CCScrollView::setTouchEnabled(bool e)
{
    Layer::setTouchEnabled(e);
    if (!e)
    {
        m_bDragging = false;
        m_bTouchMoved = false;
        m_pTouches->removeAllObjects();
    }
}

void CCScrollView::setContentOffset(Point offset, bool animated/* = false*/)
{
    if (animated)
    { //animate scrolling
        this->setContentOffsetInDuration(offset, BOUNCE_DURATION);
    } 
    else
    { //set the container position directly
        if (!m_bBounceable)
        {
            const Point minOffset = this->minContainerOffset();
            const Point maxOffset = this->maxContainerOffset();
            
            offset.x = MAX(minOffset.x, MIN(maxOffset.x, offset.x));
            offset.y = MAX(minOffset.y, MIN(maxOffset.y, offset.y));
        }

        m_pContainer->setPosition(offset);

        if (_delegate != NULL)
        {
            _delegate->scrollViewDidScroll(this);   
        }
    }
}

void CCScrollView::setContentOffsetInDuration(Point offset, float dt)
{
    FiniteTimeAction *scroll, *expire;
    
    scroll = MoveTo::create(dt, offset);
    expire = CallFuncN::create(this, callfuncN_selector(CCScrollView::stoppedAnimatedScroll));
    m_pContainer->runAction(Sequence::create(scroll, expire, NULL));
    this->schedule(schedule_selector(CCScrollView::performedAnimatedScroll));
}

Point CCScrollView::getContentOffset()
{
    return m_pContainer->getPosition();
}

void CCScrollView::setZoomScale(float s)
{
    if (m_pContainer->getScale() != s)
    {
        Point oldCenter, newCenter;
        Point center;
        
        if (m_fTouchLength == 0.0f) 
        {
            center = Point(m_tViewSize.width*0.5f, m_tViewSize.height*0.5f);
            center = this->convertToWorldSpace(center);
        }
        else
        {
            center = m_tTouchPoint;
        }
        
        oldCenter = m_pContainer->convertToNodeSpace(center);
        m_pContainer->setScale(MAX(m_fMinScale, MIN(m_fMaxScale, s)));
        newCenter = m_pContainer->convertToWorldSpace(oldCenter);
        
        const Point offset = PointSub(center, newCenter);
        if (_delegate != NULL)
        {
            _delegate->scrollViewDidZoom(this);
        }
        this->setContentOffset(PointAdd(m_pContainer->getPosition(),offset));
    }
}

float CCScrollView::getZoomScale()
{
    return m_pContainer->getScale();
}

void CCScrollView::setZoomScale(float s, bool animated)
{
    if (animated)
    {
        this->setZoomScaleInDuration(s, BOUNCE_DURATION);
    }
    else
    {
        this->setZoomScale(s);
    }
}

void CCScrollView::setZoomScaleInDuration(float s, float dt)
{
    if (dt > 0)
    {
        if (m_pContainer->getScale() != s)
        {
            ActionTween *scaleAction;
            scaleAction = ActionTween::create(dt, "zoomScale", m_pContainer->getScale(), s);
            this->runAction(scaleAction);
        }
    }
    else
    {
        this->setZoomScale(s);
    }
}

void CCScrollView::setViewSize(Size size)
{
    m_tViewSize = size;
    Layer::setContentSize(size);
}

Node * CCScrollView::getContainer()
{
    return this->m_pContainer;
}

void CCScrollView::setContainer(Node * pContainer)
{
    // Make sure that 'm_pContainer' has a non-NULL value since there are
    // lots of logic that use 'm_pContainer'.
    if (NULL == pContainer)
        return;

    this->removeAllChildrenWithCleanup(true);
    this->m_pContainer = pContainer;

    this->m_pContainer->ignoreAnchorPointForPosition(false);
    this->m_pContainer->setAnchorPoint(Point(0.0f, 0.0f));

    this->addChild(this->m_pContainer);

    this->setViewSize(this->m_tViewSize);
}

void CCScrollView::relocateContainer(bool animated)
{
    Point oldPoint, min, max;
    float newX, newY;
    
    min = this->minContainerOffset();
    max = this->maxContainerOffset();
    
    oldPoint = m_pContainer->getPosition();

    newX     = oldPoint.x;
    newY     = oldPoint.y;
    if (m_eDirection == kCCScrollViewDirectionBoth || m_eDirection == kCCScrollViewDirectionHorizontal)
    {
        newX     = MAX(newX, min.x);
        newX     = MIN(newX, max.x);
    }

    if (m_eDirection == kCCScrollViewDirectionBoth || m_eDirection == kCCScrollViewDirectionVertical)
    {
        newY     = MIN(newY, max.y);
        newY     = MAX(newY, min.y);
    }

    if (newY != oldPoint.y || newX != oldPoint.x)
    {
        this->setContentOffset(Point(newX, newY), animated);
    }
}

Point CCScrollView::maxContainerOffset()
{
    return Point(0.0f, 0.0f);
}

Point CCScrollView::minContainerOffset()
{
    return Point(m_tViewSize.width - m_pContainer->getContentSize().width*m_pContainer->getScaleX(), 
               m_tViewSize.height - m_pContainer->getContentSize().height*m_pContainer->getScaleY());
}

void CCScrollView::deaccelerateScrolling(float dt)
{
    if (m_bDragging)
    {
        this->unschedule(schedule_selector(CCScrollView::deaccelerateScrolling));
        return;
    }
    
    float newX, newY;
    Point maxInset, minInset;
    
    m_pContainer->setPosition(PointAdd(m_pContainer->getPosition(), m_tScrollDistance));
    
    if (m_bBounceable)
    {
        maxInset = m_fMaxInset;
        minInset = m_fMinInset;
    }
    else
    {
        maxInset = this->maxContainerOffset();
        minInset = this->minContainerOffset();
    }
    
    //check to see if offset lies within the inset bounds
    newX     = MIN(m_pContainer->getPosition().x, maxInset.x);
    newX     = MAX(newX, minInset.x);
    newY     = MIN(m_pContainer->getPosition().y, maxInset.y);
    newY     = MAX(newY, minInset.y);
    
    newX = m_pContainer->getPosition().x;
    newY = m_pContainer->getPosition().y;
    
    m_tScrollDistance     = PointSub(m_tScrollDistance, Point(newX - m_pContainer->getPosition().x, newY - m_pContainer->getPosition().y));
    m_tScrollDistance     = PointMult(m_tScrollDistance, SCROLL_DEAAXEL_RATE);
    this->setContentOffset(Point(newX,newY));
    
    if ((fabsf(m_tScrollDistance.x) <= SCROLL_DEAAXEL_DIST &&
         fabsf(m_tScrollDistance.y) <= SCROLL_DEAAXEL_DIST) ||
        newY > maxInset.y || newY < minInset.y ||
        newX > maxInset.x || newX < minInset.x ||
        newX == maxInset.x || newX == minInset.x ||
        newY == maxInset.y || newY == minInset.y)
    {
        this->unschedule(schedule_selector(CCScrollView::deaccelerateScrolling));
        this->relocateContainer(true);
    }
}

void CCScrollView::stoppedAnimatedScroll(Node * node)
{
    this->unschedule(schedule_selector(CCScrollView::performedAnimatedScroll));
    // After the animation stopped, "scrollViewDidScroll" should be invoked, this could fix the bug of lack of tableview cells.
    if (_delegate != NULL)
    {
        _delegate->scrollViewDidScroll(this);
    }
}

void CCScrollView::performedAnimatedScroll(float dt)
{
    if (m_bDragging)
    {
        this->unschedule(schedule_selector(CCScrollView::performedAnimatedScroll));
        return;
    }

    if (_delegate != NULL)
    {
        _delegate->scrollViewDidScroll(this);
    }
}


const Size& CCScrollView::getContentSize() const
{
	return m_pContainer->getContentSize();
}

void CCScrollView::setContentSize(const Size & size)
{
    if (this->getContainer() != NULL)
    {
        this->getContainer()->setContentSize(size);
		this->updateInset();
    }
}

void CCScrollView::updateInset()
{
	if (this->getContainer() != NULL)
	{
		m_fMaxInset = this->maxContainerOffset();
		m_fMaxInset = Point(m_fMaxInset.x + m_tViewSize.width * INSET_RATIO,
			m_fMaxInset.y + m_tViewSize.height * INSET_RATIO);
		m_fMinInset = this->minContainerOffset();
		m_fMinInset = Point(m_fMinInset.x - m_tViewSize.width * INSET_RATIO,
			m_fMinInset.y - m_tViewSize.height * INSET_RATIO);
	}
}

/**
 * make sure all children go to the container
 */
void CCScrollView::addChild(Node * child, int zOrder, int tag)
{
    child->ignoreAnchorPointForPosition(false);
    child->setAnchorPoint(Point(0.0f, 0.0f));
    if (m_pContainer != child) {
        m_pContainer->addChild(child, zOrder, tag);
    } else {
        Layer::addChild(child, zOrder, tag);
    }
}

void CCScrollView::addChild(Node * child, int zOrder)
{
    this->addChild(child, zOrder, child->getTag());
}

void CCScrollView::addChild(Node * child)
{
    this->addChild(child, child->getZOrder(), child->getTag());
}

/**
 * clip this view so that outside of the visible bounds can be hidden.
 */
void CCScrollView::beforeDraw()
{
    if (m_bClippingToBounds)
    {
		m_bScissorRestored = false;
        Rect frame = getViewRect();
        if (EGLView::sharedEGLView()->isScissorEnabled()) {
            m_bScissorRestored = true;
            m_tParentScissorRect = EGLView::sharedEGLView()->getScissorRect();
            //set the intersection of m_tParentScissorRect and frame as the new scissor rect
            if (frame.intersectsRect(m_tParentScissorRect)) {
                float x = MAX(frame.origin.x, m_tParentScissorRect.origin.x);
                float y = MAX(frame.origin.y, m_tParentScissorRect.origin.y);
                float xx = MIN(frame.origin.x+frame.size.width, m_tParentScissorRect.origin.x+m_tParentScissorRect.size.width);
                float yy = MIN(frame.origin.y+frame.size.height, m_tParentScissorRect.origin.y+m_tParentScissorRect.size.height);
                EGLView::sharedEGLView()->setScissorInPoints(x, y, xx-x, yy-y);
            }
        }
        else {
            glEnable(GL_SCISSOR_TEST);
            EGLView::sharedEGLView()->setScissorInPoints(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
        }
    }
}

/**
 * retract what's done in beforeDraw so that there's no side effect to
 * other nodes.
 */
void CCScrollView::afterDraw()
{
    if (m_bClippingToBounds)
    {
        if (m_bScissorRestored) {//restore the parent's scissor rect
            EGLView::sharedEGLView()->setScissorInPoints(m_tParentScissorRect.origin.x, m_tParentScissorRect.origin.y, m_tParentScissorRect.size.width, m_tParentScissorRect.size.height);
        }
        else {
            glDisable(GL_SCISSOR_TEST);
        }
    }
}

void CCScrollView::visit()
{
	// quick return if not visible
	if (!isVisible())
    {
		return;
    }

	kmGLPushMatrix();
	
    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->beforeDraw();
        this->transformAncestors();
    }

	this->transform();
    this->beforeDraw();

	if(m_pChildren)
    {
		axArray *arrayData = m_pChildren->data;
		unsigned int i=0;
		
		// draw children zOrder < 0
		for( ; i < arrayData->num; i++ )
        {
			Node *child =  (Node*)arrayData->arr[i];
			if ( child->getZOrder() < 0 )
            {
				child->visit();
			}
            else
            {
				break;
            }
		}
		
		// this draw
		this->draw();
		
		// draw children zOrder >= 0
		for( ; i < arrayData->num; i++ )
        {
			Node* child = (Node*)arrayData->arr[i];
			child->visit();
		}
        
	}
    else
    {
		this->draw();
    }

    this->afterDraw();
	if ( m_pGrid && m_pGrid->isActive())
    {
		m_pGrid->afterDraw(this);
    }

	kmGLPopMatrix();
}

bool CCScrollView::ccTouchBegan(Touch* touch)
{
    if (!this->isVisible())
    {
        return false;
    }
    
    Rect frame = getViewRect();

    //dispatcher does not know about clipping. reject touches outside visible bounds.
    if (m_pTouches->count() > 2 ||
        m_bTouchMoved          ||
        !frame.containsPoint(m_pContainer->convertToWorldSpace(m_pContainer->convertTouchToNodeSpace(touch))))
    {
        return false;
    }

    if (!m_pTouches->containsObject(touch))
    {
        m_pTouches->addObject(touch);
    }

    if (m_pTouches->count() == 1)
    { // scrolling
        m_tTouchPoint     = this->convertTouchToNodeSpace(touch);
        m_bTouchMoved     = false;
        m_bDragging     = true; //dragging started
        m_tScrollDistance = Point(0.0f, 0.0f);
        m_fTouchLength    = 0.0f;
    }
    else if (m_pTouches->count() == 2)
    {
        m_tTouchPoint  = PointMidpoint(this->convertTouchToNodeSpace((Touch*)m_pTouches->objectAtIndex(0)),
                                   this->convertTouchToNodeSpace((Touch*)m_pTouches->objectAtIndex(1)));
        m_fTouchLength = PointDistance(m_pContainer->convertTouchToNodeSpace((Touch*)m_pTouches->objectAtIndex(0)),
                                   m_pContainer->convertTouchToNodeSpace((Touch*)m_pTouches->objectAtIndex(1)));
        m_bDragging  = false;
    } 
    return true;
}

void CCScrollView::ccTouchMoved(Touch* touch)
{
    if (!this->isVisible())
    {
        return;
    }

    if (m_pTouches->containsObject(touch))
    {
        if (m_pTouches->count() == 1 && m_bDragging)
        { // scrolling
            Point moveDistance, newPoint, maxInset, minInset;
            Rect  frame;
            float newX, newY;
            
            frame = getViewRect();

            newPoint     = this->convertTouchToNodeSpace((Touch*)m_pTouches->objectAtIndex(0));
            moveDistance = PointSub(newPoint, m_tTouchPoint);
            
            float dis = 0.0f;
            if (m_eDirection == kCCScrollViewDirectionVertical)
            {
                dis = moveDistance.y;
            }
            else if (m_eDirection == kCCScrollViewDirectionHorizontal)
            {
                dis = moveDistance.x;
            }
            else
            {
                dis = sqrtf(moveDistance.x*moveDistance.x + moveDistance.y*moveDistance.y);
            }

            if (!m_bTouchMoved && fabs(convertDistanceFromPointToInch(dis)) < MOVE_INCH )
            {
                //AXLOG("Invalid movement, distance = [%f, %f], disInch = %f", moveDistance.x, moveDistance.y);
                return;
            }
            
            if (!m_bTouchMoved)
            {
                moveDistance = Point::ZERO;
            }
            
            m_tTouchPoint = newPoint;
            m_bTouchMoved = true;
            
            if (frame.containsPoint(this->convertToWorldSpace(newPoint)))
            {
                switch (m_eDirection)
                {
                    case kCCScrollViewDirectionVertical:
                        moveDistance = Point(0.0f, moveDistance.y);
                        break;
                    case kCCScrollViewDirectionHorizontal:
                        moveDistance = Point(moveDistance.x, 0.0f);
                        break;
                    default:
                        break;
                }
                
                maxInset = m_fMaxInset;
                minInset = m_fMinInset;

                newX     = m_pContainer->getPosition().x + moveDistance.x;
                newY     = m_pContainer->getPosition().y + moveDistance.y;

                m_tScrollDistance = moveDistance;
                this->setContentOffset(Point(newX, newY));
            }
        }
        else if (m_pTouches->count() == 2 && !m_bDragging)
        {
            const float len = PointDistance(m_pContainer->convertTouchToNodeSpace((Touch*)m_pTouches->objectAtIndex(0)),
                                            m_pContainer->convertTouchToNodeSpace((Touch*)m_pTouches->objectAtIndex(1)));
            this->setZoomScale(this->getZoomScale()*len/m_fTouchLength);
        }
    }
}

void CCScrollView::ccTouchEnded(Touch* touch)
{
    if (!this->isVisible())
    {
        return;
    }
    if (m_pTouches->containsObject(touch))
    {
        if (m_pTouches->count() == 1 && m_bTouchMoved)
        {
            this->schedule(schedule_selector(CCScrollView::deaccelerateScrolling));
        }
        m_pTouches->removeObject(touch);
    } 

    if (m_pTouches->count() == 0)
    {
        m_bDragging = false;    
        m_bTouchMoved = false;
    }
}

void CCScrollView::ccTouchCancelled(Touch* touch)
{
    if (!this->isVisible())
    {
        return;
    }
    m_pTouches->removeObject(touch); 
    if (m_pTouches->count() == 0)
    {
        m_bDragging = false;    
        m_bTouchMoved = false;
    }
}

Rect CCScrollView::getViewRect()
{
    Point screenPos = this->convertToWorldSpace(Point::ZERO);
    
    float scaleX = this->getScaleX();
    float scaleY = this->getScaleY();
    
    for (Node *p = m_pParent; p != NULL; p = p->getParent()) {
        scaleX *= p->getScaleX();
        scaleY *= p->getScaleY();
    }

    // Support negative scaling. Not doing so causes intersectsRect calls
    // (eg: to check if the touch was within the bounds) to return false.
    // Note, Node::getScale will assert if X and Y scales are different.
    if(scaleX<0.f) {
        screenPos.x += m_tViewSize.width*scaleX;
        scaleX = -scaleX;
    }
    if(scaleY<0.f) {
        screenPos.y += m_tViewSize.height*scaleY;
        scaleY = -scaleY;
    }

    return CCRectMake(screenPos.x, screenPos.y, m_tViewSize.width*scaleX, m_tViewSize.height*scaleY);
}

void CCScrollView::registerScriptHandler(int nFunID,int nScriptEventType)
{
    this->unregisterScriptHandler(nScriptEventType);
    m_mapScriptHandler[nScriptEventType] = nFunID;
}
void CCScrollView::unregisterScriptHandler(int nScriptEventType)
{
    std::map<int,int>::iterator iter = m_mapScriptHandler.find(nScriptEventType);
    
    if (m_mapScriptHandler.end() != iter)
    {
        m_mapScriptHandler.erase(iter);
    }
}
int  CCScrollView::getScriptHandler(int nScriptEventType)
{
    std::map<int,int>::iterator iter = m_mapScriptHandler.find(nScriptEventType);
    
    if (m_mapScriptHandler.end() != iter)
        return iter->second;
    
    return 0;
}
NS_AX_EXT_END
