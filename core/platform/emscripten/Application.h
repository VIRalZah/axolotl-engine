#ifndef __AX_APPLICATION_BLACKBERRY_H__
#define __AX_APPLICATION_BLACKBERRY_H__

#include "platform/Common.h"
#include "platform/ApplicationProtocol.h"
#include <string>
NS_AX_BEGIN;

class Rect;

class AX_DLL Application : public ApplicationProtocol
{
public:
    Application();
    virtual ~Application();

   /**
	@brief    Callback by Director for limit FPS.
	@interval       The time, which expressed in second in second, between current frame and next.
	*/
	void setAnimationInterval(double interval);

	/**
	@brief    Run the message loop.
	*/
	int run();

	/**
	@brief    Get current applicaiton instance.
	@return Current application instance pointer.
	*/
	static Application* sharedApplication();

	/**
	@brief Get current language config
	@return Current language config
	*/
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



protected:
	static Application * sm_pSharedApplication;
	std::string m_resourceRootPath;
	static long   			m_animationInterval;
};

NS_AX_END

#endif	// __AX_APPLICATION_BLACKBERRY_H__
