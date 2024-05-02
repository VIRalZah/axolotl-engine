/*
 * CCAplication.h
 *
 *  Created on: Aug 8, 2011
 *      Author: laschweinski
 */

#ifndef CCAPLICATION_H_
#define CCAPLICATION_H_

#include "platform/Common.h"
#include "platform/ApplicationProtocol.h"
#include <string>

NS_AX_BEGIN
class Rect;

class Application : public ApplicationProtocol
{
public:
	Application();
	virtual ~Application();

	/**
	 @brief	Callback by Director for limit FPS.
	 @interval       The time, which expressed in second in second, between current frame and next.
	 */
	void setAnimationInterval(double interval);

	/**
	 @brief	Run the message loop.
	 */
	int run();

	/**
	 @brief	Get current applicaiton instance.
	 @return Current application instance pointer.
	 */
	static Application* sharedApplication();

	/* override functions */
	virtual ccLanguageType getCurrentLanguage();

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
    
    /**
     @brief Get target platform
     */
    virtual TargetPlatform getTargetPlatform();
protected:
    long       m_nAnimationInterval;  //micro second
    std::string m_resourceRootPath;
    
	static Application * sm_pSharedApplication;
};

NS_AX_END

#endif /* CCAPLICATION_H_ */
