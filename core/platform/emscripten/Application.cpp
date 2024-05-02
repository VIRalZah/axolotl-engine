#include "Application.h"
#include "platform/FileUtils.h"
#include "base/Director.h"
#include "EGLView.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <emscripten/emscripten.h>

#define  LOGD(...)  fprintf(stderr, __VA_ARGS__)

NS_AX_BEGIN;

// sharedApplication pointer
Application * Application::sm_pSharedApplication = 0;
long Application::m_animationInterval = 1000;

// convert the timespec into milliseconds
static long time2millis(struct timespec *times)
{
    return times->tv_sec*1000 + times->tv_nsec/1000000;
}

Application::Application()
{
    AX_ASSERT(! sm_pSharedApplication);
    sm_pSharedApplication = this;
}

Application::~Application()
{
    AX_ASSERT(this == sm_pSharedApplication);
    sm_pSharedApplication = NULL;
}


extern "C" void mainLoopIter(void)
{
    EGLView::sharedOpenGLView()->handleEvents();
    Director::sharedDirector()->mainLoop();
}

int Application::run()
{
	struct timespec time_struct;
	long update_time;

	// Initialize instance and cocos2d.
	if (!applicationDidFinishLaunching())
	{
		return 0;
	}

	clock_gettime(CLOCK_REALTIME, &time_struct);
	update_time = time2millis(&time_struct);

    // XXX: Set to 1FPS while debugging
    emscripten_set_main_loop(&mainLoopIter, 0, 1);
	
	return -1;
}

void Application::setAnimationInterval(double interval)
{
	// interval in milliseconds
	m_animationInterval = (long)(interval * 1000);
}

void Application::setResourceRootPath(const std::string& rootResDir)
{
    m_resourceRootPath = rootResDir;
    if (m_resourceRootPath[m_resourceRootPath.length() - 1] != '/')
    {
        m_resourceRootPath += '/';
    }
    FileUtils* pFileUtils = FileUtils::sharedFileUtils();
    std::vector<std::string> searchPaths = pFileUtils->getSearchPaths();
    searchPaths.insert(searchPaths.begin(), m_resourceRootPath);
    pFileUtils->setSearchPaths(searchPaths);
}

const std::string& Application::getResourceRootPath(void)
{
    return m_resourceRootPath;
}

TargetPlatform Application::getTargetPlatform()
{
    return kTargetEmscripten;
}

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
Application* Application::sharedApplication()
{
    AX_ASSERT(sm_pSharedApplication);
    return sm_pSharedApplication;
}

ccLanguageType Application::getCurrentLanguage()
{
    return kLanguageEnglish;
}

NS_AX_END;
