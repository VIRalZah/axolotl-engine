/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org
 
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

#ifndef __AX_EGLVIEW_MAC_H__
#define __AX_EGLVIEW_MAC_H__

#include "platform/Common.h"
#include "platform/EGLViewProtocol.h"

NS_AX_BEGIN

class Set;
class Touch;
class Size;

class AX_DLL EGLView : public EGLViewProtocol
{
public:
    static EGLView* getInstance(void);
    
    virtual ~EGLView(void);
    
    virtual bool isOpenGLReady(void);
    virtual bool setContentScaleFactor(float contentScaleFactor);
    virtual void end();
    virtual void swapBuffers(void);
    /**
     * Set opengl view port rectangle with points.
     */
    virtual void setViewPortInPoints(float x , float y , float w , float h);
    virtual void setScissorInPoints(float x , float y , float w , float h);
    
    virtual void setIMEKeyboardState(bool bOpen);
	virtual void setMultiTouchMask(bool mask);
    
private:
    static EGLView* s_sharedView;
    
    EGLView(void);
};

NS_AX_END   // end of namespace   cocos2d

#endif	// end of __AX_EGLVIEW_MAC_H__
