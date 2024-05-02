/*
 * EGLView.h
 *
 *  Created on: Aug 8, 2011
 *      Author: laschweinski
 */

#ifndef CCEGLVIEW_H_
#define CCEGLVIEW_H_

#include "platform/Common.h"
#include "cocoa/Geometry.h"
#include "platform/EGLViewProtocol.h"

bool initExtensions();

NS_AX_BEGIN

class EGLView : public EGLViewProtocol{
public:
	EGLView();
	virtual ~EGLView();

	friend void keyEventHandle(int,int);
	friend void mouseButtonEventHandle(int,int);
	friend void mousePosEventHandle(int,int);
	friend void charEventHandle(int,int);

	/**
	 * iPixelWidth, height: the window's size
	 * iWidth ,height: the point size, which may scale.
	 * iDepth is not the buffer depth of opengl, it indicate how may bits for a pixel
	 */
	virtual void setFrameSize(float width, float height);
	virtual void setViewPortInPoints(float x , float y , float w , float h);
	virtual void setScissorInPoints(float x , float y , float w , float h);

	/*
	 * Set zoom factor for frame. This method is for debugging big resolution (e.g.new ipad) app on desktop.
	 */
	void setFrameZoomFactor(float fZoomFactor);
	float getFrameZoomFactor();
	virtual bool isOpenGLReady();
	virtual void end();
	virtual void swapBuffers();
	virtual void setIMEKeyboardState(bool bOpen);

	/**
	 @brief	get the shared main open gl window
	 */
	static EGLView* getInstance();
private:
	bool initGL();
	void destroyGL();
private:
	//store current mouse point for moving, valid if and only if the mouse pressed
	Point m_mousePoint;
	bool bIsInit;
	float _frameZoomFactor;
};

NS_AX_END

#endif /* CCEGLVIEW_H_ */
