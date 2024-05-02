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

#include "ScriptSupport.h"
#include "base/Scheduler.h"

bool AX_DLL AX_assert_script_compatible(const char *msg)
{
    axolotl::ScriptEngineProtocol* pEngine = axolotl::ScriptEngineManager::sharedManager()->getScriptEngine();
    if (pEngine && pEngine->handleAssert(msg))
    {
        return true;
    }
    return false;
}

NS_AX_BEGIN

 #if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
// #pragma mark -
// #pragma mark CCScriptHandlerEntry
#endif

CCScriptHandlerEntry* CCScriptHandlerEntry::create(int nHandler)
{
    CCScriptHandlerEntry* entry = new CCScriptHandlerEntry(nHandler);
    entry->autorelease();
    return entry;
}

CCScriptHandlerEntry::~CCScriptHandlerEntry(void)
{
	if (m_nHandler != 0)
	{
        ScriptEngineManager::sharedManager()->getScriptEngine()->removeScriptHandler(m_nHandler);
        m_nHandler = 0;
    }
}

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
// #pragma mark -
// #pragma mark CCSchedulerScriptHandlerEntry
#endif

CCSchedulerScriptHandlerEntry* CCSchedulerScriptHandlerEntry::create(int nHandler, float fInterval, bool bPaused)
{
    CCSchedulerScriptHandlerEntry* pEntry = new CCSchedulerScriptHandlerEntry(nHandler);
    pEntry->init(fInterval, bPaused);
    pEntry->autorelease();
    return pEntry;
}

bool CCSchedulerScriptHandlerEntry::init(float fInterval, bool bPaused)
{
    m_pTimer = new Timer();
    m_pTimer->initWithScriptHandler(m_nHandler, fInterval);
    m_pTimer->autorelease();
    m_pTimer->retain();
    m_bPaused = bPaused;
    LUALOG("[LUA] ADD script schedule: %d, entryID: %d", m_nHandler, m_nEntryId);
    return true;
}

CCSchedulerScriptHandlerEntry::~CCSchedulerScriptHandlerEntry(void)
{
    m_pTimer->release();
    LUALOG("[LUA] DEL script schedule %d, entryID: %d", m_nHandler, m_nEntryId);
}

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
// #pragma mark -
// #pragma mark CCTouchScriptHandlerEntry
#endif

CCTouchScriptHandlerEntry* CCTouchScriptHandlerEntry::create(int nHandler,
                                                             bool bIsMultiTouches,
                                                             int nPriority,
                                                             bool bSwallowsTouches)
{
    CCTouchScriptHandlerEntry* pEntry = new CCTouchScriptHandlerEntry(nHandler);
    pEntry->init(bIsMultiTouches, nPriority, bSwallowsTouches);
    pEntry->autorelease();
    return pEntry;
}

CCTouchScriptHandlerEntry::~CCTouchScriptHandlerEntry(void)
{
    if (m_nHandler != 0)
    {
        ScriptEngineManager::sharedManager()->getScriptEngine()->removeScriptHandler(m_nHandler);
        LUALOG("[LUA] Remove touch event handler: %d", m_nHandler);
        m_nHandler = 0;
    }
}

bool CCTouchScriptHandlerEntry::init(bool bIsMultiTouches, int nPriority, bool bSwallowsTouches)
{
    m_bIsMultiTouches = bIsMultiTouches;
    m_nPriority = nPriority;
    m_bSwallowsTouches = bSwallowsTouches;
    
    return true;
}

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
// #pragma mark -
// #pragma mark ScriptEngineManager
#endif

static ScriptEngineManager* s_pSharedScriptEngineManager = NULL;


ScriptEngineManager::~ScriptEngineManager(void)
{
    removeScriptEngine();
}

void ScriptEngineManager::setScriptEngine(ScriptEngineProtocol *pScriptEngine)
{
    removeScriptEngine();
    m_pScriptEngine = pScriptEngine;
}

void ScriptEngineManager::removeScriptEngine(void)
{
    if (m_pScriptEngine)
    {
        delete m_pScriptEngine;
        m_pScriptEngine = NULL;
    }
}

ScriptEngineManager* ScriptEngineManager::sharedManager(void)
{
    if (!s_pSharedScriptEngineManager)
    {
        s_pSharedScriptEngineManager = new ScriptEngineManager();
    }
    return s_pSharedScriptEngineManager;
}

void ScriptEngineManager::purgeSharedManager(void)
{
    if (s_pSharedScriptEngineManager)
    {
        delete s_pSharedScriptEngineManager;
        s_pSharedScriptEngineManager = NULL;
    }
}

NS_AX_END
