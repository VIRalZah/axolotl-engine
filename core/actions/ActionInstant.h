/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011 Zynga Inc.

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

#ifndef __AXINSTANT_ACTION_H__
#define __AXINSTANT_ACTION_H__

#include <string>
#include "ccTypeInfo.h"
#include "Action.h"

NS_AX_BEGIN

/**
 * @addtogroup actions
 * @{
 */

/** 
@brief Instant actions are immediate actions. They don't have a duration like
the CCIntervalAction actions.
*/ 
class AX_DLL ActionInstant : public FiniteTimeAction //<NSCopying>
{
public:
    /**
     *  @js ctor
     */
    ActionInstant();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~ActionInstant(){}
    // Action methods
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);
    virtual bool isDone(void);
    virtual void step(float dt);
    virtual void update(float time);
    //FiniteTimeAction method
    virtual FiniteTimeAction * reverse(void);
};

/** @brief Show the node
*/
class AX_DLL Show : public ActionInstant
{
public:
    /**
     *  @js ctor
     *  @lua NA
     */
    Show(){}
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~Show(){}
    //super methods
    virtual void update(float time);
    virtual FiniteTimeAction * reverse(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);
public:

    /** Allocates and initializes the action */
    static Show * create();
};



/** 
@brief Hide the node
*/
class AX_DLL Hide : public ActionInstant
{
public:
    /**
     *  @js ctor
     *  @lua NA
     */
    Hide(){}
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~Hide(){}
    //super methods
    /**
     *  @lua NA
     */
    virtual void update(float time);
    virtual FiniteTimeAction * reverse(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);
public:

    /** Allocates and initializes the action */
    static Hide * create();
};

/** @brief Toggles the visibility of a node
*/
class AX_DLL ToggleVisibility : public ActionInstant
{
public:
    /**
     *  @js ctor
     */
    ToggleVisibility(){}
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~ToggleVisibility(){}
    //super method
    virtual void update(float time);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);
public:

    /** Allocates and initializes the action */
    static ToggleVisibility * create();
};

/** 
 @brief Remove the node
 @js NA
 @lua NA
 */
class AX_DLL RemoveSelf : public ActionInstant
{
public:
	RemoveSelf(){}
	virtual ~RemoveSelf(){}
	//super methods
	virtual void update(float time);
	virtual FiniteTimeAction * reverse(void);
	virtual Object* copyWithZone(Zone *pZone);
public:
	/** create the action */
	static RemoveSelf * create(bool isNeedCleanUp = true);
	/** init the action */
	bool init(bool isNeedCleanUp);
protected:
	bool m_bIsNeedCleanUp;
};

/** 
@brief Flips the sprite horizontally
@since v0.99.0
@js NA
*/
class AX_DLL FlipX : public ActionInstant
{
public:
    /**
     *  @js ctor
     */
    FlipX()
        :m_bFlipX(false)
    {}
    /**
     *  @js  NA
     *  @lua NA
     */
    virtual ~FlipX(){}

    /** create the action */
    static FlipX * create(bool x);

    /** init the action */
    bool initWithFlipX(bool x);
    //super methods
    virtual void update(float time);
    virtual FiniteTimeAction * reverse(void);
    /**
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);

protected:
    bool    m_bFlipX;
};

/** 
@brief Flips the sprite vertically
@since v0.99.0
@js NA
*/
class AX_DLL FlipY : public ActionInstant
{
public:
    /**
     *  @js ctor
     */
    FlipY()
        :m_bFlipY(false)
    {}
    /**
     *  @js  NA
     *  @lua NA
     */
    virtual ~FlipY(){}

    /** create the action */
    static FlipY * create(bool y);

    /** init the action */
    bool initWithFlipY(bool y);
    //super methods
    virtual void update(float time);
    virtual FiniteTimeAction * reverse(void);
    /**
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);

protected:
    bool    m_bFlipY;
};

/** @brief Places the node in a certain position
*/
class AX_DLL Place : public ActionInstant //<NSCopying>
{
public:
    /**
     *  @js ctor
     */
    Place(){}
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~Place(){}

    /** creates a Place action with a position */
    static Place * create(const Vec2& pos);
    /** Initializes a Place action with a position */
    bool initWithPosition(const Vec2& pos);
    //super methods
    virtual void update(float time);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);
protected:
    Vec2 m_tPosition;
};

/** @brief Calls a 'callback'
*/
class AX_DLL CallFunc : public ActionInstant //<NSCopying>
{
public:
    /**
     *  @js ctor
     */
    CallFunc()
        : m_pSelectorTarget(NULL)
        , m_pCallFunc(NULL)
    {
    }
    /**
     * @js NA
     * @lua NA
     */
    virtual ~CallFunc();

    /** creates the action with the callback 

    * typedef void (Object::*SEL_CallFunc)();
    * @lua NA
    */
    static CallFunc * create(Object* pSelectorTarget, SEL_CallFunc selector);

	/** initializes the action with the callback 
    
    * typedef void (Object::*SEL_CallFunc)();
    * @lua NA
    */
    virtual bool initWithTarget(Object* pSelectorTarget);
    /** executes the callback 
     * @lua NA
     */
    virtual void execute();
    /** super methods
     * @lua NA
     */
    virtual void update(float time);
    /**
     * @js  NA
     * @lua NA
     */
    Object * copyWithZone(Zone *pZone);
    /**
     * @lua NA
     */
    inline Object* getTargetCallback()
    {
        return m_pSelectorTarget;
    }
    /**
     * @lua NA
     */
    inline void setTargetCallback(Object* pSel)
    {
        if (pSel != m_pSelectorTarget)
        {
            AX_SAFE_RETAIN(pSel);
            AX_SAFE_RELEASE(m_pSelectorTarget);
            m_pSelectorTarget = pSel; 
        }
    }
protected:
    /** Target that will be called */
    Object*   m_pSelectorTarget;

    union
    {
        SEL_CallFunc    m_pCallFunc;
        SEL_CallFuncN    m_pCallFuncN;
        SEL_CallFuncND    m_pCallFuncND;
        SEL_CallFuncO   m_pCallFuncO;
    };
};

/** 
@brief Calls a 'callback' with the node as the first argument
N means Node
* @js NA
*/
class AX_DLL CallFuncN : public CallFunc, public TypeInfo
{
public:
    /**
     * @js ctor
     * @lua NA
     */
    CallFuncN(){}
    /**
     * @js  NA
     * @lua NA
     */
    virtual ~CallFuncN(){}
    /**
     * @lua NA
     */
    virtual long getClassTypeInfo() {
		static const long id = axolotl::getHashCodeByString(typeid(axolotl::CallFunc).name());
		return id;
    }

    /** creates the action with the callback 

     * typedef void (Object::*SEL_CallFuncN)(Node*);
     * @lua NA
     */
    static CallFuncN * create(Object* pSelectorTarget, SEL_CallFuncN selector);

    /** initializes the action with the callback 

     * typedef void (Object::*SEL_CallFuncN)(Node*);
     * @lua NA
     */
    virtual bool initWithTarget(Object* pSelectorTarget, SEL_CallFuncN selector);
    /** super methods
     * @js  NA
     * @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);
    /**
     * @lua NA
     */
    virtual void execute();
};


/** 
* @brief Calls a 'callback' with the node as the first argument and the 2nd argument is data
* ND means: Node and Data. Data is void *, so it could be anything.
* @js NA
* @lua NA
*/
class AX_DLL CallFuncND : public CallFuncN
{
public:
    virtual long getClassTypeInfo() {
        static const long id = axolotl::getHashCodeByString(typeid(axolotl::CallFunc).name());
		return id;
    }

    /** creates the action with the callback and the data to pass as an argument */
    static CallFuncND * create(Object* pSelectorTarget, SEL_CallFuncND selector, void* d);

    /** initializes the action with the callback and the data to pass as an argument */
    virtual bool initWithTarget(Object* pSelectorTarget, SEL_CallFuncND selector, void* d);
    // super methods
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);
    virtual void execute();

protected:
    void            *m_pData;
};


/**
@brief Calls a 'callback' with an object as the first argument.
O means Object.
@since v0.99.5
@js NA
@lua NA
*/

class AX_DLL CallFuncO : public CallFunc, public TypeInfo
{
public:
    CallFuncO();
    virtual ~CallFuncO();

    virtual long getClassTypeInfo() {
	    static const long id = axolotl::getHashCodeByString(typeid(axolotl::CallFunc).name());
		return id;
    }

    /** creates the action with the callback 

    typedef void (Object::*SEL_CallFuncO)(Object*);
    */
    static CallFuncO * create(Object* pSelectorTarget, SEL_CallFuncO selector, Object* pObject);

    /** initializes the action with the callback 

    typedef void (Object::*SEL_CallFuncO)(Object*);
    */
    virtual bool initWithTarget(Object* pSelectorTarget, SEL_CallFuncO selector, Object* pObject);
    // super methods
    /**
     *  @js NA
     *  @lua NA
     */
    virtual Object* copyWithZone(Zone *pZone);
    virtual void execute();

    inline Object* getObject()
    {
        return m_pObject;
    }

    inline void setObject(Object* pObj)
    {
        if (pObj != m_pObject)
        {
            AX_SAFE_RELEASE(m_pObject);
            m_pObject = pObj;
            AX_SAFE_RETAIN(m_pObject);
        }
    }

protected:
    /** object to be passed as argument */
    Object* m_pObject;
};

// end of actions group
/// @}

NS_AX_END

#endif //__AXINSTANT_ACTION_H__
