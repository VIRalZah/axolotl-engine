#ifndef __AX_APPLICATION_WIN32_H__
#define __AX_APPLICATION_WIN32_H__

#include "StdC.h"
#include "platform/Common.h"
#include "platform/ApplicationProtocol.h"
#include <string>

NS_AX_BEGIN

class Rect;

class AX_DLL Application : public ApplicationProtocol
{
public:
    Application();
    virtual ~Application();

    /**
    @brief    Run the message loop.
    */
    virtual int run();

    /**
    @brief    Get current applicaiton instance.
    @return Current application instance pointer.
    */
    static Application* sharedApplication();

    /* override functions */
    virtual void setAnimationInterval(double interval);
    virtual ccLanguageType getCurrentLanguage();
    
    /**
     @brief Get target platform
     */
    virtual TargetPlatform getTargetPlatform();

    /**
     *  Sets the Resource root path.
     *  @deprecated Please use FileUtils::sharedFileUtils()->setSearchPaths() instead.
     */
    AX_DEPRECATED_ATTRIBUTE void setResourceRootPath(const std::string& rootResDir);

    /** 
     *  Gets the Resource root path.
     *  @deprecated Please use FileUtils::sharedFileUtils()->getSearchPaths() instead. 
     */
    AX_DEPRECATED_ATTRIBUTE const std::string& getResourceRootPath(void);

    void setStartupScriptFilename(const std::string& startupScriptFile);

    const std::string& getStartupScriptFilename(void)
    {
        return m_startupScriptFilename;
    }

protected:
    HINSTANCE           m_hInstance;
    HACCEL              m_hAccelTable;
    LARGE_INTEGER       m_nAnimationInterval;
    std::string         m_resourceRootPath;
    std::string         m_startupScriptFilename;

    static Application * sm_pSharedApplication;
};

NS_AX_END

#endif    // __AX_APPLICATION_WIN32_H__
