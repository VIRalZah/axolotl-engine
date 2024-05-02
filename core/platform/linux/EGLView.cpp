/*
 * CCEGLViewlinux.cpp
 *
 *  Created on: Aug 8, 2011
 *      Author: laschweinski
 */

#include "EGLView.h"
#include "GL.h"
#include "GL/glfw.h"
#include "ccMacros.h"
#include "base/Director.h"
#include "base/Touch.h"

#include "text_input_node/IMEDispatcher.h"

PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT = NULL;

PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;
PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;

bool initExtensions() {
#define LOAD_EXTENSION_FUNCTION(TYPE, FN)  FN = (TYPE)glfwGetProcAddress(#FN);
	bool bRet = false;
	do {

//		char* p = (char*) glGetString(GL_EXTENSIONS);
//		printf(p);

		/* Supports frame buffer? */
		if (glfwExtensionSupported("GL_EXT_framebuffer_object") != GL_FALSE)
		{

			/* Loads frame buffer extension functions */
			LOAD_EXTENSION_FUNCTION(PFNGLGENERATEMIPMAPEXTPROC,
					glGenerateMipmapEXT);
			LOAD_EXTENSION_FUNCTION(PFNGLGENFRAMEBUFFERSEXTPROC,
					glGenFramebuffersEXT);
			LOAD_EXTENSION_FUNCTION(PFNGLDELETEFRAMEBUFFERSEXTPROC,
					glDeleteFramebuffersEXT);
			LOAD_EXTENSION_FUNCTION(PFNGLBINDFRAMEBUFFEREXTPROC,
					glBindFramebufferEXT);
			LOAD_EXTENSION_FUNCTION(PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC,
					glCheckFramebufferStatusEXT);
			LOAD_EXTENSION_FUNCTION(PFNGLFRAMEBUFFERTEXTURE2DEXTPROC,
					glFramebufferTexture2DEXT);

		} else {
			break;
		}

		if (glfwExtensionSupported("GL_ARB_vertex_buffer_object") != GL_FALSE) {
			LOAD_EXTENSION_FUNCTION(PFNGLGENBUFFERSARBPROC, glGenBuffersARB);
			LOAD_EXTENSION_FUNCTION(PFNGLBINDBUFFERARBPROC, glBindBufferARB);
			LOAD_EXTENSION_FUNCTION(PFNGLBUFFERDATAARBPROC, glBufferDataARB);
			LOAD_EXTENSION_FUNCTION(PFNGLBUFFERSUBDATAARBPROC,
					glBufferSubDataARB);
			LOAD_EXTENSION_FUNCTION(PFNGLDELETEBUFFERSARBPROC,
					glDeleteBuffersARB);
		} else {
			break;
		}
		bRet = true;
	} while (0);
	return bRet;
}

NS_AX_BEGIN

EGLView::EGLView()
: bIsInit(false)
, m_fFrameZoomFactor(1.0f)
{
}

EGLView::~EGLView()
{
}

void keyEventHandle(int iKeyID,int iKeyState) {
	if (iKeyState ==GLFW_RELEASE) {
		return;
	}

	if (iKeyID == GLFW_KEY_DEL) {
		IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
	} else if (iKeyID == GLFW_KEY_ENTER) {
		IMEDispatcher::sharedDispatcher()->dispatchInsertText("\n", 1);
	} else if (iKeyID == GLFW_KEY_TAB) {

	}
}

void charEventHandle(int iCharID,int iCharState) {
	if (iCharState ==GLFW_RELEASE) {
		return;
	}

	// ascii char
	IMEDispatcher::sharedDispatcher()->dispatchInsertText((const char *)&iCharID, 1);
}

void mouseButtonEventHandle(int iMouseID,int iMouseState) {
	if (iMouseID == GLFW_MOUSE_BUTTON_LEFT) {
        EGLView* pEGLView = EGLView::sharedOpenGLView();
		//get current mouse pos
		int x,y;
		glfwGetMousePos(&x, &y);
		Point oPoint((float)x,(float)y);
		/*
		if (!Rect::CCRectContainsPoint(s_pMainWindow->m_rcViewPort,oPoint))
		{
			AXLOG("not in the viewport");
			return;
		}
		*/
         oPoint.x /= pEGLView->m_fFrameZoomFactor;
         oPoint.y /= pEGLView->m_fFrameZoomFactor;
		int id = 0;
		if (iMouseState == GLFW_PRESS) {
			pEGLView->handleTouchesBegin(1, &id, &oPoint.x, &oPoint.y);

		} else if (iMouseState == GLFW_RELEASE) {
			pEGLView->handleTouchesEnd(1, &id, &oPoint.x, &oPoint.y);
		}
	}
}

void mousePosEventHandle(int iPosX,int iPosY) {
	int iButtonState = glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT);

	//to test move
	if (iButtonState == GLFW_PRESS) {
            EGLView* pEGLView = EGLView::sharedOpenGLView();
            int id = 0;
            float x = (float)iPosX;
            float y = (float)iPosY;
            x /= pEGLView->m_fFrameZoomFactor;
            y /= pEGLView->m_fFrameZoomFactor;
            pEGLView->handleTouchesMove(1, &id, &x, &y);
	}
}

int closeEventHandle() {
	Director::sharedDirector()->end();
	return GL_TRUE;
}

void EGLView::setFrameSize(float width, float height)
{
	bool eResult = false;
	int u32GLFWFlags = GLFW_WINDOW;
	//create the window by glfw.

	//check
	AXAssert(width!=0&&height!=0, "invalid window's size equal 0");

	//Inits GLFW
	eResult = glfwInit() != GL_FALSE;

	if (!eResult) {
		AXAssert(0, "fail to init the glfw");
	}

	/* Updates window hint */
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);

	int iDepth = 16; // set default value
	/* Depending on video depth */
	switch(iDepth)
	{
		/* 16-bit */
		case 16:
		{
			/* Updates video mode */
			eResult = (glfwOpenWindow(width, height, 5, 6, 5, 0, 16, 8, (int)u32GLFWFlags) != false) ? true : false;

			break;
		}

		/* 24-bit */
		case 24:
		{
			/* Updates video mode */
			eResult = (glfwOpenWindow(width, height, 8, 8, 8, 0, 16, 8, (int)u32GLFWFlags) != false) ? true : false;

			break;
		}

		/* 32-bit */
		default:
		case 32:
		{
			/* Updates video mode */
			eResult = (glfwOpenWindow(width, height, 8, 8, 8, 8, 16, 8, (int)u32GLFWFlags) != GL_FALSE) ? true :false;
			break;
		}
	}

	/* Success? */
	if(eResult)
	{

		/* Updates actual size */
	  //		glfwGetWindowSize(&width, &height);

		CCEGLViewProtocol::setFrameSize(width, height);		

		/* Updates its title */
		glfwSetWindowTitle("Cocos2dx-Linux");

		//set the init flag
		bIsInit = true;

		//register the glfw key event
		glfwSetKeyCallback(keyEventHandle);
		//register the glfw char event
		glfwSetCharCallback(charEventHandle);
		//register the glfw mouse event
		glfwSetMouseButtonCallback(mouseButtonEventHandle);
		//register the glfw mouse pos event
		glfwSetMousePosCallback(mousePosEventHandle);

		glfwSetWindowCloseCallback(closeEventHandle);

		//Inits extensions
		eResult = initExtensions();

		if (!eResult) {
			AXAssert(0, "fail to init the extensions of opengl");
		}
		initGL();
	}
}

void EGLView::setFrameZoomFactor(float fZoomFactor)
{
    m_fFrameZoomFactor = fZoomFactor;
    glfwSetWindowSize(m_obScreenSize.width * fZoomFactor, m_obScreenSize.height * fZoomFactor);
    Director::sharedDirector()->setProjection(Director::sharedDirector()->getProjection());
}

float EGLView::getFrameZoomFactor()
{
    return m_fFrameZoomFactor;
}

void EGLView::setViewPortInPoints(float x , float y , float w , float h)
{
    glViewport((GLint)(x * m_fScaleX * m_fFrameZoomFactor+ m_obViewPortRect.origin.x * m_fFrameZoomFactor),
        (GLint)(y * m_fScaleY * m_fFrameZoomFactor + m_obViewPortRect.origin.y * m_fFrameZoomFactor),
        (GLsizei)(w * m_fScaleX * m_fFrameZoomFactor),
        (GLsizei)(h * m_fScaleY * m_fFrameZoomFactor));
}

void EGLView::setScissorInPoints(float x , float y , float w , float h)
{
    glScissor((GLint)(x * m_fScaleX * m_fFrameZoomFactor + m_obViewPortRect.origin.x * m_fFrameZoomFactor),
              (GLint)(y * m_fScaleY * m_fFrameZoomFactor + m_obViewPortRect.origin.y * m_fFrameZoomFactor),
              (GLsizei)(w * m_fScaleX * m_fFrameZoomFactor),
              (GLsizei)(h * m_fScaleY * m_fFrameZoomFactor));
}


bool EGLView::isOpenGLReady()
{
	return bIsInit;
}

void EGLView::end()
{
	/* Exits from GLFW */
	glfwTerminate();
	delete this;
	exit(0);
}

void EGLView::swapBuffers() {
	if (bIsInit) {
		/* Swap buffers */
		glfwSwapBuffers();
	}
}

void EGLView::setIMEKeyboardState(bool bOpen) {

}

bool EGLView::initGL()
{
    GLenum GlewInitResult = glewInit();
    if (GLEW_OK != GlewInitResult) 
    {
        fprintf(stderr,"ERROR: %s\n",glewGetErrorString(GlewInitResult));
        return false;
    }

    if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
    {
        CCLog("Ready for GLSL");
    }
    else 
    {
        CCLog("Not totally ready :(");
    }

    if (glewIsSupported("GL_VERSION_2_0"))
    {
        CCLog("Ready for OpenGL 2.0");
    }
    else
    {
        CCLog("OpenGL 2.0 not supported");
    }

    // Enable point size by default on linux.
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    return true;
}

void EGLView::destroyGL()
{
	/*
    if (m_hDC != NULL && m_hRC != NULL)
    {
        // deselect rendering context and delete it
        wglMakeCurrent(m_hDC, NULL);
        wglDeleteContext(m_hRC);
    }
	*/
}

EGLView* EGLView::sharedOpenGLView()
{
    static EGLView* s_pEglView = NULL;
    if (s_pEglView == NULL)
    {
        s_pEglView = new EGLView();
    }
    return s_pEglView;
}

NS_AX_END
