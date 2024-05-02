/****************************************************************************
Copyright (c) 2010 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.

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

#include "EGLView.h"
#include "cocoa/Set.h"
#include "ccMacros.h"
#include "base/Director.h"
#include "base/Touch.h"

#include "text_input_node/IMEDispatcher.h"
#include "keypad_dispatcher/KeypadDispatcher.h"
#include "support/PointExtension.h"
#include "Application.h"
#include "WinRTUtils.h"

#if (_MSC_VER >= 1800)
#include <d3d11_2.h>
#endif


using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Input;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::System;
using namespace Windows::UI::ViewManagement;

NS_AX_BEGIN

static EGLView* s_pEglView = NULL;

//////////////////////////////////////////////////////////////////////////
// impliment EGLView
//////////////////////////////////////////////////////////////////////////

// Initialize the DirectX resources required to run.
void WinRTWindow::Initialize(CoreWindow^ window, SwapChainBackgroundPanel^ panel)
{
	m_window = window;

 	esInitContext ( &m_esContext );

    ANGLE_D3D_FEATURE_LEVEL featureLevel = ANGLE_D3D_FEATURE_LEVEL::ANGLE_D3D_FEATURE_LEVEL_9_1;

#if (_MSC_VER >= 1800)
    // WinRT on Windows 8.1 can compile shaders at run time so we don't care about the DirectX feature level
    featureLevel = ANGLE_D3D_FEATURE_LEVEL::ANGLE_D3D_FEATURE_LEVEL_ANY;
#endif


    HRESULT result = CreateWinrtEglWindow(WINRT_EGL_IUNKNOWN(panel), featureLevel, m_eglWindow.GetAddressOf());
	
	if (!SUCCEEDED(result))
	{
		AXLOG("Unable to create Angle EGL Window: %d", result);
		return;
	}

	m_esContext.hWnd = m_eglWindow;
    // width and height are ignored and determined from the CoreWindow the SwapChainBackgroundPanel is in.
    esCreateWindow ( &m_esContext, TEXT("Cocos2d-x"), 0, 0, ES_WINDOW_RGB | ES_WINDOW_ALPHA | ES_WINDOW_DEPTH | ES_WINDOW_STENCIL );

	m_pointerPressedEvent = m_window->PointerPressed += 
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WinRTWindow::OnPointerPressed);
        
    m_window->PointerReleased +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WinRTWindow::OnPointerReleased);
    m_window->PointerMoved +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WinRTWindow::OnPointerMoved);
    m_window->PointerWheelChanged +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WinRTWindow::OnPointerWheelChanged);

	m_dummy = ref new Button();
	m_dummy->Opacity = 0.0;
	m_dummy->Width=1;
	m_dummy->Height=1;
	m_dummy->IsEnabled = true;
	panel->Children->Append(m_dummy);

	m_textBox = ref new TextBox();
	m_textBox->Opacity = 0.0;
	m_textBox->Width=1;
	m_textBox->Height=1;
	m_textBox->MaxLength = 1;

	panel->Children->Append(m_textBox);
	m_textBox->AddHandler(UIElement::KeyDownEvent, ref new KeyEventHandler(this, &WinRTWindow::OnTextKeyDown), true);
	m_textBox->AddHandler(UIElement::KeyUpEvent, ref new KeyEventHandler(this, &WinRTWindow::OnTextKeyUp), true);
	m_textBox->IsEnabled = false;

	auto keyboard = InputPane::GetForCurrentView();
	keyboard->Showing += ref new TypedEventHandler<InputPane^, InputPaneVisibilityEventArgs^>(this, &WinRTWindow::ShowKeyboard);
	keyboard->Hiding += ref new TypedEventHandler<InputPane^, InputPaneVisibilityEventArgs^>(this, &WinRTWindow::HideKeyboard);
	setIMEKeyboardState(false);
}

WinRTWindow::WinRTWindow(CoreWindow^ window) :
	m_lastPointValid(false),
	m_textInputEnabled(false)
{
	window->SizeChanged += 
	ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &WinRTWindow::OnWindowSizeChanged);

	DisplayProperties::LogicalDpiChanged +=
		ref new DisplayPropertiesEventHandler(this, &WinRTWindow::OnLogicalDpiChanged);

	DisplayProperties::OrientationChanged +=
        ref new DisplayPropertiesEventHandler(this, &WinRTWindow::OnOrientationChanged);

	DisplayProperties::DisplayContentsInvalidated +=
		ref new DisplayPropertiesEventHandler(this, &WinRTWindow::OnDisplayContentsInvalidated);
	
	m_eventToken = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &WinRTWindow::OnRendering));
}


void WinRTWindow::swapBuffers()
{
	eglSwapBuffers(m_esContext.eglDisplay, m_esContext.eglSurface);  
}



void WinRTWindow::OnSuspending()
{
#if (_MSC_VER >= 1800)
    Microsoft::WRL::ComPtr<ID3D11Device> device = m_eglWindow->GetAngleD3DDevice();
    Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
    HRESULT result = device.As(&dxgiDevice);
    if (SUCCEEDED(result))
    {
        dxgiDevice->Trim();
    }
#endif
}


void WinRTWindow::ResizeWindow()
{
     EGLView::sharedOpenGLView()->UpdateForWindowSizeChange();
}

Point WinRTWindow::GetCCPoint(PointerEventArgs^ args) {
	auto p = args->CurrentPoint;
	float x = getScaledDPIValue(p->Position.X);
	float y = getScaledDPIValue(p->Position.Y);
    Point pt(x, y);

	float zoomFactor = EGLView::sharedOpenGLView()->getFrameZoomFactor();

	if(zoomFactor > 0.0f) {
		pt.x /= zoomFactor;
		pt.y /= zoomFactor;
	}
	return pt;
}

void WinRTWindow::ShowKeyboard(InputPane^ inputPane, InputPaneVisibilityEventArgs^ args)
{
    EGLView::sharedOpenGLView()->ShowKeyboard(args->OccludedRect);
}

void WinRTWindow::HideKeyboard(InputPane^ inputPane, InputPaneVisibilityEventArgs^ args)
{
    EGLView::sharedOpenGLView()->HideKeyboard(args->OccludedRect);
}

void WinRTWindow::setIMEKeyboardState(bool bOpen)
{
	m_textInputEnabled = bOpen;
	if(m_textInputEnabled)
	{
		m_textBox->IsEnabled = true;
		m_textBox->Focus(FocusState::Pointer);
	}
	else
	{
		m_dummy->Focus(FocusState::Pointer);
		m_textBox->IsEnabled = false;
	}
}



void WinRTWindow::OnTextKeyDown(Object^ sender, KeyRoutedEventArgs^ args)
{
#if 0
	if(!m_textInputEnabled)
	{
		return;
	}

    auto key = args->Key;

    switch(key)
    {
    default:
        break;
    }
#endif
}

void WinRTWindow::OnTextKeyUp(Object^ sender, KeyRoutedEventArgs^ args)
{
	if(!m_textInputEnabled)
	{
		return;
	}

	args->Handled = true;

    auto key = args->Key;

    switch(key)
    {
    case VirtualKey::Escape:
        Director::sharedDirector()->getKeypadDispatcher()->dispatchKeypadMSG(kTypeBackClicked);
		args->Handled = true;
        break;
	case VirtualKey::Back:
        IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
        break;
    case VirtualKey::Enter:
		setIMEKeyboardState(false);
        IMEDispatcher::sharedDispatcher()->dispatchInsertText("\n", 1);
        break;
    default:
        char szUtf8[8] = {0};
        int nLen = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_textBox->Text->Data(), 1, szUtf8, sizeof(szUtf8), NULL, NULL);
        IMEDispatcher::sharedDispatcher()->dispatchInsertText(szUtf8, nLen);
        break;
    }	
	m_textBox->Text = "";
}


void WinRTWindow::OnPointerWheelChanged(CoreWindow^ sender, PointerEventArgs^ args)
{
    float direction = (float)args->CurrentPoint->Properties->MouseWheelDelta;
    int id = 0;
    Point p(0.0f,0.0f);
    EGLView::sharedOpenGLView()->handleTouchesBegin(1, &id, &p.x, &p.y);
    p.y += direction;
    EGLView::sharedOpenGLView()->handleTouchesMove(1, &id, &p.x, &p.y);
    EGLView::sharedOpenGLView()->handleTouchesEnd(1, &id, &p.x, &p.y);
}


void WinRTWindow::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
    int id = args->CurrentPoint->PointerId;
    Point pt = GetCCPoint(args);
    EGLView::sharedOpenGLView()->handleTouchesBegin(1, &id, &pt.x, &pt.y);
}

void WinRTWindow::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{
	auto currentPoint = args->CurrentPoint;
	if (currentPoint->IsInContact)
	{
		if (m_lastPointValid)
		{
			int id = args->CurrentPoint->PointerId;
			Point p = GetCCPoint(args);
			EGLView::sharedOpenGLView()->handleTouchesMove(1, &id, &p.x, &p.y);
		}
		m_lastPoint = currentPoint->Position;
		m_lastPointValid = true;
	}
	else
	{
		m_lastPointValid = false;
	}
}

void WinRTWindow::OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
{
    int id = args->CurrentPoint->PointerId;
    Point pt = GetCCPoint(args);
    EGLView::sharedOpenGLView()->handleTouchesEnd(1, &id, &pt.x, &pt.y);
}

void WinRTWindow::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
	ResizeWindow();
	EGLView::sharedOpenGLView()->UpdateForWindowSizeChange();
}

void WinRTWindow::OnLogicalDpiChanged(Object^ sender)
{
	EGLView::sharedOpenGLView()->UpdateForWindowSizeChange();
}

void WinRTWindow::OnOrientationChanged(Object^ sender)
{
	ResizeWindow();
	EGLView::sharedOpenGLView()->UpdateForWindowSizeChange();
}

void WinRTWindow::OnDisplayContentsInvalidated(Object^ sender)
{
	EGLView::sharedOpenGLView()->UpdateForWindowSizeChange();
}

void WinRTWindow::OnRendering(Object^ sender, Object^ args)
{
	EGLView::sharedOpenGLView()->OnRendering();
}

void WinRTWindow::ReleasePointerPressed()
{
	m_window->PointerPressed -= m_pointerPressedEvent;
}

void WinRTWindow::EnablePointerPressed()
{
	m_pointerPressedEvent = m_window->PointerPressed += 
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WinRTWindow::OnPointerPressed);
}


EGLView::EGLView()
	: m_window(nullptr)
	, m_fFrameZoomFactor(1.0f)
	, m_bSupportTouch(false)
	, m_lastPointValid(false)
	, m_running(false)
	, m_winRTWindow(nullptr)
	, m_initialized(false)
{
	s_pEglView = this;
    strcpy(m_szViewName, "Cocos2dxWinRT");
}

EGLView::~EGLView()
{
	AX_ASSERT(this == s_pEglView);
    s_pEglView = NULL;

	// TODO: cleanup 
}

bool EGLView::Create(CoreWindow^ window, SwapChainBackgroundPanel^ panel)
{
    bool bRet = false;
	m_window = window;

	m_bSupportTouch = true;
	m_winRTWindow = ref new WinRTWindow(window);
	m_winRTWindow->Initialize(window, panel);
    m_initialized = false;
	UpdateForWindowSizeChange();
    return bRet;
}

bool EGLView::isOpenGLReady()
{
	// TODO: need to revisit this
    return (m_window.Get() != nullptr);
}

void EGLView::end()
{
	// TODO: need to implement

}

void EGLView::swapBuffers()
{
	m_winRTWindow->swapBuffers();
}


void EGLView::setIMEKeyboardState(bool bOpen)
{
	if(m_winRTWindow) 
	{
		m_winRTWindow->setIMEKeyboardState(bOpen);
	}
}


void EGLView::resize(int width, int height)
{

}

void EGLView::setFrameZoomFactor(float fZoomFactor)
{
    m_fFrameZoomFactor = fZoomFactor;
    resize((int)(m_obScreenSize.width * fZoomFactor), (int)(m_obScreenSize.height * fZoomFactor));
    centerWindow();
    Director::sharedDirector()->setProjection(Director::sharedDirector()->getProjection());
}


float EGLView::getFrameZoomFactor()
{
    return m_fFrameZoomFactor;
}

void EGLView::setFrameSize(float width, float height)
{
	// not implemented in WinRT. Window is always full screen
    // CCEGLViewProtocol::setFrameSize(width, height);
}

void EGLView::centerWindow()
{
	// not implemented in WinRT. Window is always full screen
}

void EGLView::OnSuspending()
{
    if (m_winRTWindow)
    {
        m_winRTWindow->OnSuspending();
    }
}

EGLView* EGLView::sharedOpenGLView()
{
    return s_pEglView;
}

int EGLView::Run() 
{
	m_running = true; 

	return 0;
};


void EGLView::OnRendering()
{
	if(m_running && m_initialized)
	{
		Director::sharedDirector()->mainLoop();
	}
}

void EGLView::HideKeyboard(Rect r)
{
    return; // not implemented
	float height = m_keyboardRect.Height;
	float factor = m_fScaleY / AX_CONTENT_SCALE_FACTOR();
	height = (float)height / factor;

	Rect rect_end(0, 0, 0, 0);
	Rect rect_begin(0, 0, m_obScreenSize.width / factor, height);

    CCIMEKeyboardNotificationInfo info;
    info.begin = rect_begin;
    info.end = rect_end;
    info.duration = 0;
    IMEDispatcher::sharedDispatcher()->dispatchKeyboardWillHide(info);
    IMEDispatcher::sharedDispatcher()->dispatchKeyboardDidHide(info);
}

void EGLView::ShowKeyboard(Rect r)
{
    return; // not implemented
	float height = r.Height;
	float factor = m_fScaleY / AX_CONTENT_SCALE_FACTOR();
	height = (float)height / factor;

	Rect rect_begin(0.0f, 0.0f - height, m_obScreenSize.width / factor, height);
	Rect rect_end(0.0f, 0.0f, m_obScreenSize.width / factor, height);

    CCIMEKeyboardNotificationInfo info;
    info.begin = rect_begin;
    info.end = rect_end;
    info.duration = 0;
    IMEDispatcher::sharedDispatcher()->dispatchKeyboardWillShow(info);
    IMEDispatcher::sharedDispatcher()->dispatchKeyboardDidShow(info);
	m_keyboardRect = r;
}


void EGLView::UpdateForWindowSizeChange()
{
    float width = ConvertDipsToPixels(m_window->Bounds.Width);
    float height = ConvertDipsToPixels(m_window->Bounds.Height);

	if(!m_initialized)
    {
        m_initialized = true;
        CCEGLViewProtocol::setFrameSize(width, height);
    }
    else
    {
        m_obScreenSize = CCSizeMake(width, height);
        Size designSize = getDesignResolutionSize();
        EGLView::sharedOpenGLView()->setDesignResolutionSize(designSize.width, designSize.height, kResolutionShowAll);
        Director::sharedDirector()->setProjection(Director::sharedDirector()->getProjection());
   }
}

void EGLView::openEditBox(CCEditBoxParam^ param)
{
	m_winRTWindow->ReleasePointerPressed();

	if (m_editBoxhandler)
	{		
		m_editBoxhandler->Invoke(nullptr, param);
	}	
}

void EGLView::SetCocosEditBoxHandler( EventHandler<Object^>^ handler )
{
	m_editBoxhandler = handler;
}

void EGLView::OnCloseEditBox()
{
	m_winRTWindow->EnablePointerPressed();
}




NS_AX_END
