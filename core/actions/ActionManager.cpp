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

#include "ActionManager.h"
#include "base/Node.h"
#include "base/Scheduler.h"
#include "ccMacros.h"
#include "support/data_support/axCArray.h"
#include "support/data_support/uthash.h"
#include "base/Set.h"

NS_AX_BEGIN
//
// singleton stuff
//
typedef struct _hashElement
{
    struct _axArray             *actions;
    Object                    *target;
    unsigned int                actionIndex;
    Action                    *currentAction;
    bool                        currentActionSalvaged;
    bool                        paused;
    UT_hash_handle                hh;
} tHashElement;

ActionManager::ActionManager(void)
: m_pTargets(NULL), 
  m_pCurrentTarget(NULL),
  m_bCurrentTargetSalvaged(false)
{

}

ActionManager::~ActionManager(void)
{
    AXLOGINFO("cocos2d: deallocing %p", this);

    removeAllActions();
}

// private

void ActionManager::deleteHashElement(tHashElement *pElement)
{
    axArrayFree(pElement->actions);
    HASH_DEL(m_pTargets, pElement);
    pElement->target->release();
    free(pElement);
}

void ActionManager::actionAllocWithHashElement(tHashElement *pElement)
{
    // 4 actions per Node by default
    if (pElement->actions == NULL)
    {
        pElement->actions = axArrayNew(4);
    }else 
    if (pElement->actions->num == pElement->actions->max)
    {
        axArrayDoubleCapacity(pElement->actions);
    }

}

void ActionManager::removeActionAtIndex(unsigned int uIndex, tHashElement *pElement)
{
    Action *pAction = (Action*)pElement->actions->arr[uIndex];

    if (pAction == pElement->currentAction && (! pElement->currentActionSalvaged))
    {
        pElement->currentAction->retain();
        pElement->currentActionSalvaged = true;
    }

    axArrayRemoveObjectAtIndex(pElement->actions, uIndex, true);

    // update actionIndex in case we are in tick. looping over the actions
    if (pElement->actionIndex >= uIndex)
    {
        pElement->actionIndex--;
    }

    if (pElement->actions->num == 0)
    {
        if (m_pCurrentTarget == pElement)
        {
            m_bCurrentTargetSalvaged = true;
        }
        else
        {
            deleteHashElement(pElement);
        }
    }
}

// pause / resume

void ActionManager::pauseTarget(Object *pTarget)
{
    tHashElement *pElement = NULL;
    HASH_FIND_INT(m_pTargets, &pTarget, pElement);
    if (pElement)
    {
        pElement->paused = true;
    }
}

void ActionManager::resumeTarget(Object *pTarget)
{
    tHashElement *pElement = NULL;
    HASH_FIND_INT(m_pTargets, &pTarget, pElement);
    if (pElement)
    {
        pElement->paused = false;
    }
}

Set* ActionManager::pauseAllRunningActions()
{
    Set *idsWithActions = new Set();
    idsWithActions->autorelease();
    
    for (tHashElement *element=m_pTargets; element != NULL; element = (tHashElement *)element->hh.next) 
    {
        if (! element->paused) 
        {
            element->paused = true;
            idsWithActions->addObject(element->target);
        }
    }    
    
    return idsWithActions;
}

void ActionManager::resumeTargets(axolotl::Set *targetsToResume)
{    
    SetIterator iter;
    for (iter = targetsToResume->begin(); iter != targetsToResume->end(); ++iter)
    {
        resumeTarget(*iter);
    }
}

// run

void ActionManager::addAction(Action *pAction, Node *pTarget, bool paused)
{
    AXAssert(pAction != NULL, "");
    AXAssert(pTarget != NULL, "");

    tHashElement *pElement = NULL;
    // we should convert it to Object*, because we save it as Object*
    Object *tmp = pTarget;
    HASH_FIND_INT(m_pTargets, &tmp, pElement);
    if (! pElement)
    {
        pElement = (tHashElement*)calloc(sizeof(*pElement), 1);
        pElement->paused = paused;
        pTarget->retain();
        pElement->target = pTarget;
        HASH_ADD_INT(m_pTargets, target, pElement);
    }

     actionAllocWithHashElement(pElement);
 
     AXAssert(! axArrayContainsObject(pElement->actions, pAction), "");
     axArrayAppendObject(pElement->actions, pAction);
 
     pAction->startWithTarget(pTarget);
}

// remove

void ActionManager::removeAllActions(void)
{
    for (tHashElement *pElement = m_pTargets; pElement != NULL; )
    {
        Object *pTarget = pElement->target;
        pElement = (tHashElement*)pElement->hh.next;
        removeAllActionsFromTarget(pTarget);
    }
}

void ActionManager::removeAllActionsFromTarget(Object *pTarget)
{
    // explicit null handling
    if (pTarget == NULL)
    {
        return;
    }

    tHashElement *pElement = NULL;
    HASH_FIND_INT(m_pTargets, &pTarget, pElement);
    if (pElement)
    {
        if (axArrayContainsObject(pElement->actions, pElement->currentAction) && (! pElement->currentActionSalvaged))
        {
            pElement->currentAction->retain();
            pElement->currentActionSalvaged = true;
        }

        axArrayRemoveAllObjects(pElement->actions);
        if (m_pCurrentTarget == pElement)
        {
            m_bCurrentTargetSalvaged = true;
        }
        else
        {
            deleteHashElement(pElement);
        }
    }
    else
    {
//        AXLOG("cocos2d: removeAllActionsFromTarget: Target not found");
    }
}

void ActionManager::removeAction(Action *pAction)
{
    // explicit null handling
    if (pAction == NULL)
    {
        return;
    }

    tHashElement *pElement = NULL;
    Object *pTarget = pAction->getOriginalTarget();
    HASH_FIND_INT(m_pTargets, &pTarget, pElement);
    if (pElement)
    {
        unsigned int i = axArrayGetIndexOfObject(pElement->actions, pAction);
        if (UINT_MAX != i)
        {
            removeActionAtIndex(i, pElement);
        }
    }
    else
    {
        AXLOG("cocos2d: removeAction: Target not found");
    }
}

void ActionManager::removeActionByTag(unsigned int tag, Object *pTarget)
{
    AXAssert((int)tag != kCCActionTagInvalid, "");
    AXAssert(pTarget != NULL, "");

    tHashElement *pElement = NULL;
    HASH_FIND_INT(m_pTargets, &pTarget, pElement);

    if (pElement)
    {
        unsigned int limit = pElement->actions->num;
        for (unsigned int i = 0; i < limit; ++i)
        {
            Action *pAction = (Action*)pElement->actions->arr[i];

            if (pAction->getTag() == (int)tag && pAction->getOriginalTarget() == pTarget)
            {
                removeActionAtIndex(i, pElement);
                break;
            }
        }
    }
}

// get

Action* ActionManager::getActionByTag(unsigned int tag, Object *pTarget)
{
    AXAssert((int)tag != kCCActionTagInvalid, "");

    tHashElement *pElement = NULL;
    HASH_FIND_INT(m_pTargets, &pTarget, pElement);

    if (pElement)
    {
        if (pElement->actions != NULL)
        {
            unsigned int limit = pElement->actions->num;
            for (unsigned int i = 0; i < limit; ++i)
            {
                Action *pAction = (Action*)pElement->actions->arr[i];

                if (pAction->getTag() == (int)tag)
                {
                    return pAction;
                }
            }
        }
        AXLOG("cocos2d : getActionByTag(tag = %d): Action not found", tag);
    }
    else
    {
        // AXLOG("cocos2d : getActionByTag: Target not found");
    }

    return NULL;
}

unsigned int ActionManager::numberOfRunningActionsInTarget(Object *pTarget)
{
    tHashElement *pElement = NULL;
    HASH_FIND_INT(m_pTargets, &pTarget, pElement);
    if (pElement)
    {
        return pElement->actions ? pElement->actions->num : 0;
    }

    return 0;
}

// main loop
void ActionManager::update(float dt)
{
    for (tHashElement *elt = m_pTargets; elt != NULL; )
    {
        m_pCurrentTarget = elt;
        m_bCurrentTargetSalvaged = false;

        if (! m_pCurrentTarget->paused)
        {
            // The 'actions' CCMutableArray may change while inside this loop.
            for (m_pCurrentTarget->actionIndex = 0; m_pCurrentTarget->actionIndex < m_pCurrentTarget->actions->num;
                m_pCurrentTarget->actionIndex++)
            {
                m_pCurrentTarget->currentAction = (Action*)m_pCurrentTarget->actions->arr[m_pCurrentTarget->actionIndex];
                if (m_pCurrentTarget->currentAction == NULL)
                {
                    continue;
                }

                m_pCurrentTarget->currentActionSalvaged = false;

                m_pCurrentTarget->currentAction->step(dt);

                if (m_pCurrentTarget->currentActionSalvaged)
                {
                    // The currentAction told the node to remove it. To prevent the action from
                    // accidentally deallocating itself before finishing its step, we retained
                    // it. Now that step is done, it's safe to release it.
                    m_pCurrentTarget->currentAction->release();
                } else
                if (m_pCurrentTarget->currentAction->isDone())
                {
                    m_pCurrentTarget->currentAction->stop();

                    Action *pAction = m_pCurrentTarget->currentAction;
                    // Make currentAction nil to prevent removeAction from salvaging it.
                    m_pCurrentTarget->currentAction = NULL;
                    removeAction(pAction);
                }

                m_pCurrentTarget->currentAction = NULL;
            }
        }

        // elt, at this moment, is still valid
        // so it is safe to ask this here (issue #490)
        elt = (tHashElement*)(elt->hh.next);

        // only delete currentTarget if no actions were scheduled during the cycle (issue #481)
        if (m_bCurrentTargetSalvaged && m_pCurrentTarget->actions->num == 0)
        {
            deleteHashElement(m_pCurrentTarget);
        }
    }

    // issue #635
    m_pCurrentTarget = NULL;
}

NS_AX_END
