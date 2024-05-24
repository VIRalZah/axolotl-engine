/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2009      Valentin Milea
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
#include "base/String.h"
#include "Node.h"
#include "support/PointExtension.h"
#include "support/TransformUtils.h"
#include "base/Camera.h"
#include "effects/Grid.h"
#include "base/Director.h"
#include "base/Scheduler.h"
#include "base/Touch.h"
#include "actions/ActionManager.h"
#include "shaders/GLProgram.h"
// externals
#include "kazmath/GL/matrix.h"
#include "support/component/Component.h"
#include "support/component/ComponentContainer.h"

#if AX_NODE_RENDER_SUBPIXEL
#define RENDER_IN_SUBPIXEL
#else
#define RENDER_IN_SUBPIXEL(__ARGS__) (ceil(__ARGS__))
#endif

NS_AX_BEGIN

// XXX: Yes, nodes might have a sort problem once every 15 days if the game runs at 60 FPS and each frame sprites are reordered.
static int s_globalOrderOfArrival = 1;

Node::Node(void)
: m_fRotationX(0.0f)
, m_fRotationY(0.0f)
, _scaleX(1.0f)
, _scaleY(1.0f)
, m_fVertexZ(0.0f)
, m_obPosition(Vec2::ZERO)
, m_fSkewX(0.0f)
, m_fSkewY(0.0f)
, m_obAnchorPointInPoints(Vec2::ZERO)
, m_obAnchorPoint(Vec2::ZERO)
, m_obContentSize(Size::ZERO)
, m_sAdditionalTransform(AffineTransformMakeIdentity())
, m_pCamera(NULL)
// children (lazy allocs)
// lazy alloc
, m_pGrid(NULL)
, m_nZOrder(0)
, m_pChildren(NULL)
, m_pParent(NULL)
// "whole screen" objects. like Scenes and Layers, should set m_bIgnoreAnchorPointForPosition to true
, m_nTag(kCCNodeTagInvalid)
// userData is always inited as nil
, m_pUserData(NULL)
, m_pUserObject(NULL)
, m_pShaderProgram(NULL)
, m_eGLServerState(ccGLServerState(0))
, m_uOrderOfArrival(0)
, m_bRunning(false)
, m_bTransformDirty(true)
, m_bInverseDirty(true)
, m_bAdditionalTransformDirty(false)
, m_bVisible(true)
, m_bIgnoreAnchorPointForPosition(false)
, m_bReorderChildDirty(false)
, m_pComponentContainer(NULL)
{
    Director* director = Director::sharedDirector();
    _actionManager = director->getActionManager();
    _actionManager->retain();
    _scheduler = director->getScheduler();
    _scheduler->retain();

    m_pComponentContainer = new ComponentContainer(this);
}

Node::~Node(void)
{
    AXLOGINFO( "cocos2d: deallocing" );

    AX_SAFE_RELEASE(_actionManager);
    AX_SAFE_RELEASE(_scheduler);
    // attributes
    AX_SAFE_RELEASE(m_pCamera);

    AX_SAFE_RELEASE(m_pGrid);
    AX_SAFE_RELEASE(m_pShaderProgram);
    AX_SAFE_RELEASE(m_pUserObject);

    // m_pComsContainer
    m_pComponentContainer->removeAll();
    AX_SAFE_DELETE(m_pComponentContainer);

    if(m_pChildren && m_pChildren->count() > 0)
    {
        Object* child;
        AXARRAY_FOREACH(m_pChildren, child)
        {
            Node* pChild = (Node*) child;
            if (pChild)
            {
                pChild->m_pParent = NULL;
            }
        }
    }

    // children
    AX_SAFE_RELEASE(m_pChildren);
}

bool Node::init()
{
    return true;
}

float Node::getSkewX()
{
    return m_fSkewX;
}

void Node::setSkewX(float newSkewX)
{
    m_fSkewX = newSkewX;
    m_bTransformDirty = m_bInverseDirty = true;
}

float Node::getSkewY()
{
    return m_fSkewY;
}

void Node::setSkewY(float newSkewY)
{
    m_fSkewY = newSkewY;

    m_bTransformDirty = m_bInverseDirty = true;
}

/// zOrder getter
int Node::getZOrder()
{
    return m_nZOrder;
}

/// zOrder setter : private method
/// used internally to alter the zOrder variable. DON'T call this method manually 
void Node::_setZOrder(int z)
{
    m_nZOrder = z;
}

void Node::setZOrder(int z)
{
    _setZOrder(z);
    if (m_pParent)
    {
        m_pParent->reorderChild(this, z);
    }
}

/// vertexZ getter
float Node::getVertexZ()
{
    return m_fVertexZ;
}


/// vertexZ setter
void Node::setVertexZ(float var)
{
    m_fVertexZ = var;
}


/// rotation getter
float Node::getRotation()
{
    AXAssert(m_fRotationX == m_fRotationY, "Node#rotation. RotationX != RotationY. Don't know which one to return");
    return m_fRotationX;
}

/// rotation setter
void Node::setRotation(float newRotation)
{
    m_fRotationX = m_fRotationY = newRotation;
    m_bTransformDirty = m_bInverseDirty = true;
}

float Node::getRotationX()
{
    return m_fRotationX;
}

void Node::setRotationX(float fRotationX)
{
    m_fRotationX = fRotationX;
    m_bTransformDirty = m_bInverseDirty = true;
}

float Node::getRotationY()
{
    return m_fRotationY;
}

void Node::setRotationY(float fRotationY)
{
    m_fRotationY = fRotationY;
    m_bTransformDirty = m_bInverseDirty = true;
}

/// scale getter
float Node::getScale(void)
{
    AXAssert( _scaleX == _scaleY, "Node#scale. ScaleX != ScaleY. Don't know which one to return");
    return _scaleX;
}

/// scale setter
void Node::setScale(float scale)
{
    _scaleX = _scaleY = scale;
    m_bTransformDirty = m_bInverseDirty = true;
}

/// scale setter
void Node::setScale(float fScaleX,float fScaleY)
{
    _scaleX = fScaleX;
    _scaleY = fScaleY;
    m_bTransformDirty = m_bInverseDirty = true;
}

/// scaleX getter
float Node::getScaleX()
{
    return _scaleX;
}

/// scaleX setter
void Node::setScaleX(float newScaleX)
{
    _scaleX = newScaleX;
    m_bTransformDirty = m_bInverseDirty = true;
}

/// scaleY getter
float Node::getScaleY()
{
    return _scaleY;
}

/// scaleY setter
void Node::setScaleY(float newScaleY)
{
    _scaleY = newScaleY;
    m_bTransformDirty = m_bInverseDirty = true;
}

/// position getter
const Vec2& Node::getPosition()
{
    return m_obPosition;
}

/// position setter
void Node::setPosition(const Vec2& newPosition)
{
    setPosition(newPosition.x, newPosition.y);
}

void Node::getPosition(float* x, float* y)
{
    *x = m_obPosition.x;
    *y = m_obPosition.y;
}

void Node::setPosition(float x, float y)
{
    m_obPosition.setVec2(x, y);
    m_bTransformDirty = m_bInverseDirty = true;
}

float Node::getPositionX(void)
{
    return m_obPosition.x;
}

float Node::getPositionY(void)
{
    return  m_obPosition.y;
}

void Node::setPositionX(float x)
{
    setPosition(Vec2(x, m_obPosition.y));
}

void Node::setPositionY(float y)
{
    setPosition(Vec2(m_obPosition.x, y));
}

/// children getter
Array* Node::getChildren()
{
    return m_pChildren;
}

unsigned int Node::getChildrenCount(void) const
{
    return m_pChildren ? m_pChildren->count() : 0;
}

/// camera getter: lazy alloc
Camera* Node::getCamera()
{
    if (!m_pCamera)
    {
        m_pCamera = new Camera();
    }
    
    return m_pCamera;
}


/// grid getter
GridBase* Node::getGrid()
{
    return m_pGrid;
}

/// grid setter
void Node::setGrid(GridBase* pGrid)
{
    AX_SAFE_RETAIN(pGrid);
    AX_SAFE_RELEASE(m_pGrid);
    m_pGrid = pGrid;
}


/// isVisible getter
bool Node::isVisible()
{
    return m_bVisible;
}

/// isVisible setter
void Node::setVisible(bool var)
{
    m_bVisible = var;
}

const Vec2& Node::getAnchorPointInPoints()
{
    return m_obAnchorPointInPoints;
}

/// anchorPoint getter
const Vec2& Node::getAnchorPoint()
{
    return m_obAnchorPoint;
}

void Node::setAnchorPoint(const Vec2& point)
{
    if( ! point.equals(m_obAnchorPoint))
    {
        m_obAnchorPoint = point;
        m_obAnchorPointInPoints = Vec2(m_obContentSize.width * m_obAnchorPoint.x, m_obContentSize.height * m_obAnchorPoint.y );
        m_bTransformDirty = m_bInverseDirty = true;
    }
}

/// contentSize getter
const Size& Node::getContentSize() const
{
    return m_obContentSize;
}

const Size& Node::getScaledContentSize() const
{
    return Size(m_obContentSize.width * _scaleX, m_obContentSize.height * _scaleY);
}

void Node::setContentSize(const Size & size)
{
    if ( ! size.equals(m_obContentSize))
    {
        m_obContentSize = size;

        m_obAnchorPointInPoints = Vec2(m_obContentSize.width * m_obAnchorPoint.x, m_obContentSize.height * m_obAnchorPoint.y );
        m_bTransformDirty = m_bInverseDirty = true;
    }
}

// isRunning getter
bool Node::isRunning()
{
    return m_bRunning;
}

/// parent getter
Node * Node::getParent()
{
    return m_pParent;
}
/// parent setter
void Node::setParent(Node * var)
{
    m_pParent = var;
}

/// isRelativeAnchorPoint getter
bool Node::isIgnoreAnchorPointForPosition()
{
    return m_bIgnoreAnchorPointForPosition;
}
/// isRelativeAnchorPoint setter
void Node::ignoreAnchorPointForPosition(bool newValue)
{
    if (newValue != m_bIgnoreAnchorPointForPosition) 
    {
		m_bIgnoreAnchorPointForPosition = newValue;
		m_bTransformDirty = m_bInverseDirty = true;
	}
}

/// tag getter
int Node::getTag() const
{
    return m_nTag;
}

/// tag setter
void Node::setTag(int var)
{
    m_nTag = var;
}

/// userData getter
void * Node::getUserData()
{
    return m_pUserData;
}

/// userData setter
void Node::setUserData(void *var)
{
    m_pUserData = var;
}

unsigned int Node::getOrderOfArrival()
{
    return m_uOrderOfArrival;
}

void Node::setOrderOfArrival(unsigned int uOrderOfArrival)
{
    m_uOrderOfArrival = uOrderOfArrival;
}

GLProgram* Node::getShaderProgram()
{
    return m_pShaderProgram;
}

Object* Node::getUserObject()
{
    return m_pUserObject;
}

ccGLServerState Node::getGLServerState()
{
    return m_eGLServerState;
}

void Node::setGLServerState(ccGLServerState glServerState)
{
    m_eGLServerState = glServerState;
}

void Node::setUserObject(Object *pUserObject)
{
    AX_SAFE_RETAIN(pUserObject);
    AX_SAFE_RELEASE(m_pUserObject);
    m_pUserObject = pUserObject;
}

void Node::setShaderProgram(GLProgram *pShaderProgram)
{
    AX_SAFE_RETAIN(pShaderProgram);
    AX_SAFE_RELEASE(m_pShaderProgram);
    m_pShaderProgram = pShaderProgram;
}

Rect Node::boundingBox()
{
    Rect rect = Rect(0, 0, m_obContentSize.width, m_obContentSize.height);
    return CCRectApplyAffineTransform(rect, nodeToParentTransform());
}

Node * Node::create(void)
{
	Node * pRet = new Node();
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

void Node::cleanup()
{
    // actions
    this->stopAllActions();
    this->unscheduleAllSelectors();
    
    // timers
    arrayMakeObjectsPerformSelector(m_pChildren, cleanup, Node*);
}


const char* Node::description()
{
    return String::createWithFormat("<Node | Tag = %d>", m_nTag)->getCString();
}

// lazy allocs
void Node::childrenAlloc(void)
{
    m_pChildren = Array::createWithCapacity(4);
    m_pChildren->retain();
}

Node* Node::getChildByTag(int aTag)
{
    AXAssert( aTag != kCCNodeTagInvalid, "Invalid tag");

    if(m_pChildren && m_pChildren->count() > 0)
    {
        Object* child;
        AXARRAY_FOREACH(m_pChildren, child)
        {
            Node* pNode = (Node*) child;
            if(pNode && pNode->m_nTag == aTag)
                return pNode;
        }
    }
    return NULL;
}

/* "add" logic MUST only be on this method
* If a class want's to extend the 'addChild' behavior it only needs
* to override this method
*/
void Node::addChild(Node *child, int zOrder, int tag)
{    
    AXAssert( child != NULL, "Argument must be non-nil");
    AXAssert( child->m_pParent == NULL, "child already added. It can't be added again");

    if( ! m_pChildren )
    {
        this->childrenAlloc();
    }

    this->insertChild(child, zOrder);

    child->m_nTag = tag;

    child->setParent(this);
    child->setOrderOfArrival(s_globalOrderOfArrival++);

    if( m_bRunning )
    {
        child->onEnter();
        child->onEnterTransitionDidFinish();
    }
}

void Node::addChild(Node *child, int zOrder)
{
    AXAssert( child != NULL, "Argument must be non-nil");
    this->addChild(child, zOrder, child->m_nTag);
}

void Node::addChild(Node *child)
{
    AXAssert( child != NULL, "Argument must be non-nil");
    this->addChild(child, child->m_nZOrder, child->m_nTag);
}

void Node::removeFromParent()
{
    this->removeFromParentAndCleanup(true);
}

void Node::removeFromParentAndCleanup(bool cleanup)
{
    if (m_pParent != NULL)
    {
        m_pParent->removeChild(this,cleanup);
    } 
}

void Node::removeChild(Node* child)
{
    this->removeChild(child, true);
}

/* "remove" logic MUST only be on this method
* If a class want's to extend the 'removeChild' behavior it only needs
* to override this method
*/
void Node::removeChild(Node* child, bool cleanup)
{
    // explicit nil handling
    if (m_pChildren == NULL)
    {
        return;
    }

    if ( m_pChildren->containsObject(child) )
    {
        this->detachChild(child,cleanup);
    }
}

void Node::removeChildByTag(int tag)
{
    this->removeChildByTag(tag, true);
}

void Node::removeChildByTag(int tag, bool cleanup)
{
    AXAssert( tag != kCCNodeTagInvalid, "Invalid tag");

    Node *child = this->getChildByTag(tag);

    if (child == NULL)
    {
        AXLOG("cocos2d: removeChildByTag(tag = %d): child not found!", tag);
    }
    else
    {
        this->removeChild(child, cleanup);
    }
}

void Node::removeAllChildren()
{
    this->removeAllChildrenWithCleanup(true);
}

void Node::removeAllChildrenWithCleanup(bool cleanup)
{
    // not using detachChild improves speed here
    if ( m_pChildren && m_pChildren->count() > 0 )
    {
        Object* child;
        AXARRAY_FOREACH(m_pChildren, child)
        {
            Node* pNode = (Node*) child;
            if (pNode)
            {
                // IMPORTANT:
                //  -1st do onExit
                //  -2nd cleanup
                if(m_bRunning)
                {
                    pNode->onExitTransitionDidStart();
                    pNode->onExit();
                }

                if (cleanup)
                {
                    pNode->cleanup();
                }
                // set parent nil at the end
                pNode->setParent(NULL);
            }
        }
        
        m_pChildren->removeAllObjects();
    }
    
}

void Node::detachChild(Node *child, bool doCleanup)
{
    // IMPORTANT:
    //  -1st do onExit
    //  -2nd cleanup
    if (m_bRunning)
    {
        child->onExitTransitionDidStart();
        child->onExit();
    }

    // If you don't do cleanup, the child's actions will not get removed and the
    // its scheduledSelectors_ dict will not get released!
    if (doCleanup)
    {
        child->cleanup();
    }

    // set parent nil at the end
    child->setParent(NULL);

    m_pChildren->removeObject(child);
}


// helper used by reorderChild & add
void Node::insertChild(Node* child, int z)
{
    m_bReorderChildDirty = true;
    axArrayAppendObjectWithResize(m_pChildren->data, child);
    child->_setZOrder(z);
}

void Node::reorderChild(Node *child, int zOrder)
{
    AXAssert( child != NULL, "Child must be non-nil");
    m_bReorderChildDirty = true;
    child->setOrderOfArrival(s_globalOrderOfArrival++);
    child->_setZOrder(zOrder);
}

void Node::sortAllChildren()
{
    if (m_bReorderChildDirty)
    {
        int i,j,length = m_pChildren->data->num;
        Node ** x = (Node**)m_pChildren->data->arr;
        Node *tempItem;

        // insertion sort
        for(i=1; i<length; i++)
        {
            tempItem = x[i];
            j = i-1;

            //continue moving element downwards while zOrder is smaller or when zOrder is the same but mutatedIndex is smaller
            while(j>=0 && ( tempItem->m_nZOrder < x[j]->m_nZOrder || ( tempItem->m_nZOrder== x[j]->m_nZOrder && tempItem->m_uOrderOfArrival < x[j]->m_uOrderOfArrival ) ) )
            {
                x[j+1] = x[j];
                j = j-1;
            }
            x[j+1] = tempItem;
        }

        //don't need to check children recursively, that's done in visit of each child

        m_bReorderChildDirty = false;
    }
}


 void Node::draw()
 {
     //AXAssert(0);
     // override me
     // Only use- this function to draw your stuff.
     // DON'T draw your stuff outside this method
 }

void Node::visit()
{
    // quick return if not visible. children won't be drawn.
    if (!m_bVisible)
    {
        return;
    }
    kmGLPushMatrix();

     if (m_pGrid && m_pGrid->isActive())
     {
         m_pGrid->beforeDraw();
     }

    this->transform();

    Node* pNode = NULL;
    unsigned int i = 0;

    if(m_pChildren && m_pChildren->count() > 0)
    {
        sortAllChildren();
        // draw children zOrder < 0
        axArray *arrayData = m_pChildren->data;
        for( ; i < arrayData->num; i++ )
        {
            pNode = (Node*) arrayData->arr[i];

            if ( pNode && pNode->m_nZOrder < 0 ) 
            {
                pNode->visit();
            }
            else
            {
                break;
            }
        }
        // self draw
        this->draw();

        for( ; i < arrayData->num; i++ )
        {
            pNode = (Node*) arrayData->arr[i];
            if (pNode)
            {
                pNode->visit();
            }
        }        
    }
    else
    {
        this->draw();
    }

    // reset for next frame
    m_uOrderOfArrival = 0;

     if (m_pGrid && m_pGrid->isActive())
     {
         m_pGrid->afterDraw(this);
    }
 
    kmGLPopMatrix();
}

void Node::transformAncestors()
{
    if( m_pParent != NULL  )
    {
        m_pParent->transformAncestors();
        m_pParent->transform();
    }
}

void Node::transform()
{    
    kmMat4 transfrom4x4;

    // Convert 3x3 into 4x4 matrix
    nodeToParentTransform();
    CGAffineToGL(&m_sTransform, transfrom4x4.mat);

    // Update Z vertex manually
    transfrom4x4.mat[14] = m_fVertexZ;

    kmGLMultMatrix( &transfrom4x4 );


    // XXX: Expensive calls. Camera should be integrated into the cached affine matrix
    if ( m_pCamera != NULL && !(m_pGrid != NULL && m_pGrid->isActive()) )
    {
        bool translate = (m_obAnchorPointInPoints.x != 0.0f || m_obAnchorPointInPoints.y != 0.0f);

        if( translate )
            kmGLTranslatef(RENDER_IN_SUBPIXEL(m_obAnchorPointInPoints.x), RENDER_IN_SUBPIXEL(m_obAnchorPointInPoints.y), 0 );

        m_pCamera->locate();

        if( translate )
            kmGLTranslatef(RENDER_IN_SUBPIXEL(-m_obAnchorPointInPoints.x), RENDER_IN_SUBPIXEL(-m_obAnchorPointInPoints.y), 0 );
    }

}


void Node::onEnter()
{
    //fix setTouchEnabled not take effect when called the function in onEnter in JSBinding.
    m_bRunning = true;

    //Judge the running state for prevent called onEnter method more than once,it's possible that this function called by addChild  
    if (m_pChildren && m_pChildren->count() > 0)
    {
        Object* child;
        Node* node;
        AXARRAY_FOREACH(m_pChildren, child)
        {
            node = (Node*)child;
            if (!node->isRunning())
            {
                node->onEnter();
            }            
        }
    }

    this->resumeSchedulerAndActions();   
}

void Node::onEnterTransitionDidFinish()
{
    arrayMakeObjectsPerformSelector(m_pChildren, onEnterTransitionDidFinish, Node*);
}

void Node::onExitTransitionDidStart()
{
    arrayMakeObjectsPerformSelector(m_pChildren, onExitTransitionDidStart, Node*);
}

void Node::onExit()
{
    this->pauseSchedulerAndActions();

    m_bRunning = false;

    arrayMakeObjectsPerformSelector(m_pChildren, onExit, Node*);
}

void Node::setActionManager(ActionManager* actionManager)
{
    if( actionManager != _actionManager ) {
        this->stopAllActions();
        AX_SAFE_RETAIN(actionManager);
        AX_SAFE_RELEASE(_actionManager);
        _actionManager = actionManager;
    }
}

ActionManager* Node::getActionManager()
{
    return _actionManager;
}

Action * Node::runAction(Action* action)
{
    AXAssert( action != NULL, "Argument must be non-nil");
    _actionManager->addAction(action, this, !m_bRunning);
    return action;
}

void Node::stopAllActions()
{
    _actionManager->removeAllActionsFromTarget(this);
}

void Node::stopAction(Action* action)
{
    _actionManager->removeAction(action);
}

void Node::stopActionByTag(int tag)
{
    AXAssert( tag != kCCActionTagInvalid, "Invalid tag");
    _actionManager->removeActionByTag(tag, this);
}

Action * Node::getActionByTag(int tag)
{
    AXAssert( tag != kCCActionTagInvalid, "Invalid tag");
    return _actionManager->getActionByTag(tag, this);
}

unsigned int Node::numberOfRunningActions()
{
    return _actionManager->numberOfRunningActionsInTarget(this);
}

// Node - Callbacks

void Node::setScheduler(Scheduler* scheduler)
{
    if( scheduler != _scheduler ) {
        this->unscheduleAllSelectors();
        AX_SAFE_RETAIN(scheduler);
        AX_SAFE_RELEASE(_scheduler);
        _scheduler = scheduler;
    }
}

Scheduler* Node::getScheduler()
{
    return _scheduler;
}

void Node::scheduleUpdate()
{
    scheduleUpdateWithPriority(0);
}

void Node::scheduleUpdateWithPriority(int priority)
{
    _scheduler->scheduleUpdateForTarget(this, priority, !m_bRunning);
}

void Node::unscheduleUpdate()
{
    _scheduler->unscheduleUpdateForTarget(this);
}

void Node::schedule(SEL_SCHEDULE selector)
{
    this->schedule(selector, 0.0f, kAXRepeatForever, 0.0f);
}

void Node::schedule(SEL_SCHEDULE selector, float interval)
{
    this->schedule(selector, interval, kAXRepeatForever, 0.0f);
}

void Node::schedule(SEL_SCHEDULE selector, float interval, unsigned int repeat, float delay)
{
    AXAssert( selector, "Argument must be non-nil");
    AXAssert( interval >=0, "Argument must be positive");

    _scheduler->scheduleSelector(selector, this, interval , repeat, delay, !m_bRunning);
}

void Node::scheduleOnce(SEL_SCHEDULE selector, float delay)
{
    this->schedule(selector, 0.0f, 0, delay);
}

void Node::unschedule(SEL_SCHEDULE selector)
{
    // explicit nil handling
    if (selector == 0)
        return;

    _scheduler->unscheduleSelector(selector, this);
}

void Node::unscheduleAllSelectors()
{
    _scheduler->unscheduleAllForTarget(this);
}

void Node::resumeSchedulerAndActions()
{
    _scheduler->resumeTarget(this);
    _actionManager->resumeTarget(this);
}

void Node::pauseSchedulerAndActions()
{
    _scheduler->pauseTarget(this);
    _actionManager->pauseTarget(this);
}

// override me
void Node::update(float fDelta)
{
    if (m_pComponentContainer && !m_pComponentContainer->isEmpty())
    {
        m_pComponentContainer->visit(fDelta);
    }
}

const AffineTransform& Node::nodeToParentTransform(void)
{
    if (m_bTransformDirty) 
    {

        // Translate values
        float x = m_obPosition.x;
        float y = m_obPosition.y;

        if (m_bIgnoreAnchorPointForPosition) 
        {
            x += m_obAnchorPointInPoints.x;
            y += m_obAnchorPointInPoints.y;
        }

        // Rotation values
		// Change rotation code to handle X and Y
		// If we skew with the exact same value for both x and y then we're simply just rotating
        float cx = 1, sx = 0, cy = 1, sy = 0;
        if (m_fRotationX || m_fRotationY)
        {
            float radiansX = -AX_DEGREES_TO_RADIANS(m_fRotationX);
            float radiansY = -AX_DEGREES_TO_RADIANS(m_fRotationY);
            cx = cosf(radiansX);
            sx = sinf(radiansX);
            cy = cosf(radiansY);
            sy = sinf(radiansY);
        }

        bool needsSkewMatrix = ( m_fSkewX || m_fSkewY );


        // optimization:
        // inline anchor point calculation if skew is not needed
        // Adjusted transform calculation for rotational skew
        if (! needsSkewMatrix && !m_obAnchorPointInPoints.equals(Vec2::ZERO))
        {
            x += cy * -m_obAnchorPointInPoints.x * _scaleX + -sx * -m_obAnchorPointInPoints.y * _scaleY;
            y += sy * -m_obAnchorPointInPoints.x * _scaleX +  cx * -m_obAnchorPointInPoints.y * _scaleY;
        }


        // Build Transform Matrix
        // Adjusted transform calculation for rotational skew
        m_sTransform = AffineTransformMake( cy * _scaleX,  sy * _scaleX,
            -sx * _scaleY, cx * _scaleY,
            x, y );

        // XXX: Try to inline skew
        // If skew is needed, apply skew and then anchor point
        if (needsSkewMatrix) 
        {
            AffineTransform skewMatrix = AffineTransformMake(1.0f, tanf(AX_DEGREES_TO_RADIANS(m_fSkewY)),
                tanf(AX_DEGREES_TO_RADIANS(m_fSkewX)), 1.0f,
                0.0f, 0.0f );
            m_sTransform = AffineTransformConcat(skewMatrix, m_sTransform);

            // adjust anchor point
            if (!m_obAnchorPointInPoints.equals(Vec2::ZERO))
            {
                m_sTransform = AffineTransformTranslate(m_sTransform, -m_obAnchorPointInPoints.x, -m_obAnchorPointInPoints.y);
            }
        }
        
        if (m_bAdditionalTransformDirty)
        {
            m_sTransform = AffineTransformConcat(m_sTransform, m_sAdditionalTransform);
            m_bAdditionalTransformDirty = false;
        }

        m_bTransformDirty = false;
    }

    return m_sTransform;
}

void Node::setAdditionalTransform(const AffineTransform& additionalTransform)
{
    m_sAdditionalTransform = additionalTransform;
    m_bTransformDirty = true;
    m_bAdditionalTransformDirty = true;
}

const AffineTransform& Node::parentToNodeTransform(void)
{
    if ( m_bInverseDirty ) {
        m_sInverse = AffineTransformInvert(this->nodeToParentTransform());
        m_bInverseDirty = false;
    }

    return m_sInverse;
}

AffineTransform Node::nodeToWorldTransform()
{
    AffineTransform t = this->nodeToParentTransform();

    for (Node *p = m_pParent; p != NULL; p = p->getParent())
        t = AffineTransformConcat(t, p->nodeToParentTransform());

    return t;
}

AffineTransform Node::worldToNodeTransform(void)
{
    return AffineTransformInvert(this->nodeToWorldTransform());
}

Vec2 Node::convertToNodeSpace(const Vec2& worldPoint)
{
    Vec2 ret = CCPointApplyAffineTransform(worldPoint, worldToNodeTransform());
    return ret;
}

Vec2 Node::convertToWorldSpace(const Vec2& nodePoint)
{
    Vec2 ret = CCPointApplyAffineTransform(nodePoint, nodeToWorldTransform());
    return ret;
}

Vec2 Node::convertToNodeSpaceAR(const Vec2& worldPoint)
{
    Vec2 nodePoint = convertToNodeSpace(worldPoint);
    return PointSub(nodePoint, m_obAnchorPointInPoints);
}

Vec2 Node::convertToWorldSpaceAR(const Vec2& nodePoint)
{
    Vec2 pt = PointAdd(nodePoint, m_obAnchorPointInPoints);
    return convertToWorldSpace(pt);
}

Vec2 Node::convertToWindowSpace(const Vec2& nodePoint)
{
    Vec2 worldPoint = this->convertToWorldSpace(nodePoint);
    return Director::sharedDirector()->convertToUI(worldPoint);
}

// convenience methods which take a Touch instead of Vec2
Vec2 Node::convertTouchToNodeSpace(Touch *touch)
{
    Vec2 point = touch->getLocation();
    return this->convertToNodeSpace(point);
}
Vec2 Node::convertTouchToNodeSpaceAR(Touch *touch)
{
    Vec2 point = touch->getLocation();
    return this->convertToNodeSpaceAR(point);
}

void Node::updateTransform()
{
    // Recursively iterate over children
    arrayMakeObjectsPerformSelector(m_pChildren, updateTransform, Node*);
}

Component* Node::getComponent(const char *pName) const
{
    return m_pComponentContainer->get(pName);
}

bool Node::addComponent(Component *pComponent)
{
    return m_pComponentContainer->add(pComponent);
}

bool Node::removeComponent(const char *pName)
{
    return m_pComponentContainer->remove(pName);
}

bool Node::removeComponent(Component *pComponent)
{
    return m_pComponentContainer->remove(pComponent);
}

void Node::removeAllComponents()
{
    m_pComponentContainer->removeAll();
}

// NodeRGBA
NodeRGBA::NodeRGBA()
: _displayedOpacity(255)
, _realOpacity(255)
, _displayedColor(ccWHITE)
, _realColor(ccWHITE)
, _cascadeColorEnabled(false)
, _cascadeOpacityEnabled(false)
{}

NodeRGBA::~NodeRGBA() {}

bool NodeRGBA::init()
{
    if (Node::init())
    {
        _displayedOpacity = _realOpacity = 255;
        _displayedColor = _realColor = ccWHITE;
        _cascadeOpacityEnabled = _cascadeColorEnabled = false;
        return true;
    }
    return false;
}

NodeRGBA * NodeRGBA::create(void)
{
	NodeRGBA * pRet = new NodeRGBA();
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

GLubyte NodeRGBA::getOpacity(void)
{
	return _realOpacity;
}

GLubyte NodeRGBA::getDisplayedOpacity(void)
{
	return _displayedOpacity;
}

void NodeRGBA::setOpacity(GLubyte opacity)
{
    _displayedOpacity = _realOpacity = opacity;
    
	if (_cascadeOpacityEnabled)
    {
		GLubyte parentOpacity = 255;
        RGBAProtocol* pParent = dynamic_cast<RGBAProtocol*>(m_pParent);
        if (pParent && pParent->isCascadeOpacityEnabled())
        {
            parentOpacity = pParent->getDisplayedOpacity();
        }
        this->updateDisplayedOpacity(parentOpacity);
	}
}

void NodeRGBA::updateDisplayedOpacity(GLubyte parentOpacity)
{
	_displayedOpacity = _realOpacity * parentOpacity/255.0;
	
    if (_cascadeOpacityEnabled)
    {
        Object* pObj;
        AXARRAY_FOREACH(m_pChildren, pObj)
        {
            RGBAProtocol* item = dynamic_cast<RGBAProtocol*>(pObj);
            if (item)
            {
                item->updateDisplayedOpacity(_displayedOpacity);
            }
        }
    }
}

bool NodeRGBA::isCascadeOpacityEnabled(void)
{
    return _cascadeOpacityEnabled;
}

void NodeRGBA::setCascadeOpacityEnabled(bool cascadeOpacityEnabled)
{
    _cascadeOpacityEnabled = cascadeOpacityEnabled;
}

const ccColor3B& NodeRGBA::getColor(void)
{
	return _realColor;
}

const ccColor3B& NodeRGBA::getDisplayedColor()
{
	return _displayedColor;
}

void NodeRGBA::setColor(const ccColor3B& color)
{
	_displayedColor = _realColor = color;
	
	if (_cascadeColorEnabled)
    {
		ccColor3B parentColor = ccWHITE;
        RGBAProtocol *parent = dynamic_cast<RGBAProtocol*>(m_pParent);
		if (parent && parent->isCascadeColorEnabled())
        {
            parentColor = parent->getDisplayedColor(); 
        }
        
        updateDisplayedColor(parentColor);
	}
}

void NodeRGBA::updateDisplayedColor(const ccColor3B& parentColor)
{
	_displayedColor.r = _realColor.r * parentColor.r/255.0;
	_displayedColor.g = _realColor.g * parentColor.g/255.0;
	_displayedColor.b = _realColor.b * parentColor.b/255.0;
    
    if (_cascadeColorEnabled)
    {
        Object *obj = NULL;
        AXARRAY_FOREACH(m_pChildren, obj)
        {
            RGBAProtocol *item = dynamic_cast<RGBAProtocol*>(obj);
            if (item)
            {
                item->updateDisplayedColor(_displayedColor);
            }
        }
    }
}

bool NodeRGBA::isCascadeColorEnabled(void)
{
    return _cascadeColorEnabled;
}

void NodeRGBA::setCascadeColorEnabled(bool cascadeColorEnabled)
{
    _cascadeColorEnabled = cascadeColorEnabled;
}

NS_AX_END
