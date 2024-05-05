// Application main file.

#include "Main.h"
#include "../Classes/AppDelegate.h"

// Cocos2dx headers
#include "axolotl.h"

// Marmaladeheaders
#include "IwGL.h"

USING_NS_CC;

int main()
{
	AppDelegate app;

	return axolotl::Application::sharedApplication()->Run();
}
