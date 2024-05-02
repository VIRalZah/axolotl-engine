/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 
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

#ifndef __SCRIPT_SUPPORT_H__
#define __SCRIPT_SUPPORT_H__

#include "platform/Common.h"
#include "Accelerometer.h"
#include "base/Touch.h"
#include "cocoa/Set.h"
#include "Accelerometer.h"
#include <map>
#include <string>
#include <list>

typedef struct lua_State lua_State;

NS_AX_BEGIN

class Timer;
class Layer;
class MenuItem;
class NotificationCenter;
class CallFunc;
class Acceleration;

enum ccScriptType {
    kScriptTypeNone = 0,
    kScriptTypeLua,
    kScriptTypeJavascript
};
/**
 * @js NA
 * @lua NA
 */
class CCScriptHandlerEntry : public Object
{
public:
    static CCScriptHandlerEntry* create(int nHandler);
    ~CCScriptHandlerEntry(void);
    
    int getHandler(void) {
        return m_nHandler;
    }
    
    int getEntryId(void) {
        return m_nEntryId;
    }
    
protected:
    CCScriptHandlerEntry(int nHandler)
    : m_nHandler(nHandler)
    {
        static int newEntryId = 0;
        newEntryId++;
        m_nEntryId = newEntryId;
    }
    
    int m_nHandler;
    int m_nEntryId;
};

/**
 * @addtogroup script_support
 * @{
 * @js NA
 * @lua NA
 */

class CCSchedulerScriptHandlerEntry : public CCScriptHandlerEntry
{
public:
    // nHandler return by tolua_ref_function(), called from LuaCocos2d.cpp
    static CCSchedulerScriptHandlerEntry* create(int nHandler, float fInterval, bool bPaused);
    ~CCSchedulerScriptHandlerEntry(void);
    
    axolotl::Timer* getTimer(void) {
        return m_pTimer;
    }
    
    bool isPaused(void) {
        return m_bPaused;
    }
    
    void markedForDeletion(void) {
        m_bMarkedForDeletion = true;
    }
    
    bool isMarkedForDeletion(void) {
        return m_bMarkedForDeletion;
    }
    
private:
    CCSchedulerScriptHandlerEntry(int nHandler)
    : CCScriptHandlerEntry(nHandler)
    , m_pTimer(NULL)
    , m_bPaused(false)
    , m_bMarkedForDeletion(false)
    {
    }
    bool init(float fInterval, bool bPaused);
    
    axolotl::Timer*   m_pTimer;
    bool                m_bPaused;
    bool                m_bMarkedForDeletion;
};


/**
 * @js NA
 * @lua NA
 */
class CCTouchScriptHandlerEntry : public CCScriptHandlerEntry
{
public:
    static CCTouchScriptHandlerEntry* create(int nHandler, bool bIsMultiTouches, int nPriority, bool bSwallowsTouches);
    ~CCTouchScriptHandlerEntry(void);
    
    bool isMultiTouches(void) {
        return m_bIsMultiTouches;
    }
    
    int getPriority(void) {
        return m_nPriority;
    }
    
    bool getSwallowsTouches(void) {
        return m_bSwallowsTouches;
    }
    
private:
    CCTouchScriptHandlerEntry(int nHandler)
    : CCScriptHandlerEntry(nHandler)
    , m_bIsMultiTouches(false)
    , m_nPriority(0)
    , m_bSwallowsTouches(false)
    {
    }
    bool init(bool bIsMultiTouches, int nPriority, bool bSwallowsTouches);
    
    bool    m_bIsMultiTouches;
    int     m_nPriority;
    bool    m_bSwallowsTouches;
};


// Don't make ScriptEngineProtocol inherits from Object since setScriptEngine is invoked only once in AppDelegate.cpp,
// It will affect the lifecycle of ScriptCore instance, the autorelease pool will be destroyed before destructing ScriptCore.
// So a crash will appear on Win32 if you click the close button.
/**
 * @js NA
 * @lua NA
 */
class AX_DLL ScriptEngineProtocol
{
public:
    virtual ~ScriptEngineProtocol() {};
    
    /** Get script type */
    virtual ccScriptType getScriptType() { return kScriptTypeNone; };

    /** Remove script object. */
    virtual void removeScriptObjectByCCObject(Object* pObj) = 0;
    
    /** Remove script function handler, only CCLuaEngine class need to implement this function. */
    virtual void removeScriptHandler(int nHandler) {};
    
    /** Reallocate script function handler, only CCLuaEngine class need to implement this function. */
    virtual int reallocateScriptHandler(int nHandler) { return -1;}
    
    /**
     @brief Execute script code contained in the given string.
     @param codes holding the valid script code that should be executed.
     @return 0 if the string is executed correctly.
     @return other if the string is executed wrongly.
     */
    virtual int executeString(const char* codes) = 0;
    
    /**
     @brief Execute a script file.
     @param filename String object holding the filename of the script file that is to be executed
     */
    virtual int executeScriptFile(const char* filename) = 0;
    
    /**
     @brief Execute a scripted global function.
     @brief The function should not take any parameters and should return an integer.
     @param functionName String object holding the name of the function, in the global script environment, that is to be executed.
     @return The integer value returned from the script function.
     */
    virtual int executeGlobalFunction(const char* functionName) = 0;
    
    /**
     @brief Execute a node event function
     @param pNode which node produce this event
     @param nAction kCCNodeOnEnter,kCCNodeOnExit,kCCMenuItemActivated,kCCNodeOnEnterTransitionDidFinish,kCCNodeOnExitTransitionDidStart
     @return The integer value returned from the script function.
     */
    virtual int executeNodeEvent(Node* pNode, int nAction) = 0;
    
    virtual int executeMenuItemEvent(MenuItem* pMenuItem) = 0;
    /** Execute a notification event function */
    virtual int executeNotificationEvent(NotificationCenter* pNotificationCenter, const char* pszName) = 0;
    
    /** execute a callfun event */
    virtual int executeCallFuncActionEvent(CallFunc* pAction, Object* pTarget = NULL) = 0;
    /** execute a schedule function */
    virtual int executeSchedule(int nHandler, float dt, Node* pNode = NULL) = 0;
    
    /** functions for executing touch event */
    virtual int executeLayerTouchesEvent(Layer* pLayer, int eventType, Set *pTouches) = 0;
    virtual int executeLayerTouchEvent(Layer* pLayer, int eventType, Touch *pTouch) = 0;

    /** functions for keypad event */
    virtual int executeLayerKeypadEvent(Layer* pLayer, int eventType) = 0;

    /** execute a accelerometer event */
    virtual int executeAccelerometerEvent(Layer* pLayer, Acceleration* pAccelerationValue) = 0;

    /** function for common event */
    virtual int executeEvent(int nHandler, const char* pEventName, Object* pEventSource = NULL, const char* pEventSourceClassName = NULL) = 0;
    
    /** function for c++ call back lua funtion */
    virtual int executeEventWithArgs(int nHandler, Array* pArgs) { return 0; }

    /** called by AXAssert to allow scripting engine to handle failed assertions
     * @return true if the assert was handled by the script engine, false otherwise.
     */
    virtual bool handleAssert(const char *msg) = 0;
    
    /**
     *
     */
    enum ConfigType
    {
        NONE,
        COCOSTUDIO,
    };
    virtual bool parseConfig(ConfigType type, const std::string& str) = 0;
};

/**
 ScriptEngineManager is a singleton which holds an object instance of CCScriptEngineProtocl
 It helps cocos2d-x and the user code to find back LuaEngine object
 @since v0.99.5-x-0.8.5
 @js NA
 @lua NA
 */
class AX_DLL ScriptEngineManager
{
public:
    ~ScriptEngineManager(void);
    
    ScriptEngineProtocol* getScriptEngine(void) {
        return m_pScriptEngine;
    }
    void setScriptEngine(ScriptEngineProtocol *pScriptEngine);
    void removeScriptEngine(void);
    
    static ScriptEngineManager* sharedManager(void);
    static void purgeSharedManager(void);
    
private:
    ScriptEngineManager(void)
    : m_pScriptEngine(NULL)
    {
    }
    
    ScriptEngineProtocol *m_pScriptEngine;
};

// end of script_support group
/// @}

NS_AX_END

#endif // __SCRIPT_SUPPORT_H__
