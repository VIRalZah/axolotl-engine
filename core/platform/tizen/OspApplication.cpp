/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) 2013 Lee, Jae-Hong

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

#include "OspApplication.h"
#include "OspForm.h"
#include "axolotl.h"

USING_NS_CC;
using namespace Tizen::App;
using namespace Tizen::System;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Base::Runtime;

CCOspApplication * CCOspApplication::sm_pSharedCCOspApplication = 0;
CCOspApplicationInitialized CCOspApplication::sm_pApplicationInitialized = 0;
Orientation CCOspApplication::sm_eScreenOrientation = ORIENTATION_NONE;

CCOspApplication::CCOspApplication()
    : __pForm(null)
{
    AXAssert(! sm_pSharedCCOspApplication, "");
    sm_pSharedCCOspApplication = this;
}

CCOspApplication::~CCOspApplication()
{
    AXAssert(this == sm_pSharedCCOspApplication, "");
    sm_pSharedCCOspApplication = NULL;
}

Application*
CCOspApplication::CreateInstance(void)
{
    // Create the instance through the constructor.
    return new (std::nothrow) CCOspApplication();
}

void
CCOspApplication::SetApplicationInitializedCallback(CCOspApplicationInitialized p)
{
    sm_pApplicationInitialized = p;
}

void
CCOspApplication::SetScreenOrientation(Orientation orientation)
{
    sm_eScreenOrientation = orientation;
}

CCOspApplication*
CCOspApplication::GetInstance(void)
{
    AXAssert(sm_pSharedCCOspApplication, "");
    return sm_pSharedCCOspApplication;
}

Tizen::Ui::Controls::Form*
CCOspApplication::getCCOspForm()
{
    return __pForm;
}

bool
CCOspApplication::OnAppInitializing(AppRegistry& appRegistry)
{
    AXLOG("OnAppInitializing");

    result r = E_SUCCESS;

    __pFrame = new (std::nothrow) Frame();
    TryReturn(__pFrame != null, E_FAILURE, "[CCOspApplication] Generating a frame failed.");

    r = __pFrame->Construct();
    TryReturn(!IsFailed(r), E_FAILURE, "[CCOspApplication] pAppFrame->Construct() failed.");

    this->AddFrame(*__pFrame);

    __pForm = new (std::nothrow) CCOspForm;
    TryCatch(__pForm != null, , "[CCOspApplication] Allocation of CCOspForm failed.");

    r = __pForm->Construct(FORM_STYLE_NORMAL);
    TryCatch(!IsFailed(r), , "[CCOspApplication] __pForm->Construct(FORM_STYLE_NORMAL) failed.");

    r = GetAppFrame()->GetFrame()->AddControl(*__pForm);
    TryCatch(!IsFailed(r), , "[CCOspApplication] GetAppFrame()->GetFrame()->AddControl(*__pForm) failed.");

    return true;

CATCH:
    delete __pForm;
    __pForm = null;

    return false;
}

bool
CCOspApplication::OnAppInitialized(void)
{
    sm_pApplicationInitialized();
    __pFrame->SetOrientation(sm_eScreenOrientation);
    return true;
}

bool
CCOspApplication::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
    EGLView::sharedOpenGLView()->cleanup();

    return true;
}

void
CCOspApplication::OnForeground(void)
{
    Timer* timer = EGLView::sharedOpenGLView()->getTimer();

    if (timer != null)
    {
        timer->Start(Application::sharedApplication()->getAnimationInterval());
    }

    if (Director::sharedDirector()->getOpenGLView())
    {
        Application::sharedApplication()->applicationWillEnterForeground();
    }
}

void
CCOspApplication::OnBackground(void)
{
    Timer* timer = EGLView::sharedOpenGLView()->getTimer();

    if (timer != null)
    {
        timer->Cancel();
    }

    Application::sharedApplication()->applicationDidEnterBackground();
}

void
CCOspApplication::OnLowMemory(void)
{
}

void
CCOspApplication::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
}

void
CCOspApplication::OnScreenOn(void)
{
}

void
CCOspApplication::OnScreenOff(void)
{
}
