/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2009      On-Core
 
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
#ifndef __EFFECTS_AXGRABBER_H__
#define __EFFECTS_AXGRABBER_H__

#include "base/Configuration.h"
#include "base/Object.h"
#include "GL.h"

NS_AX_BEGIN

class Texture2D;

/**
 * @addtogroup effects
 * @{
 */

/** FBO class that grabs the the contents of the screen 
 * @js NA
 * @lua NA
 */
class CCGrabber : public Object
{
public:
    CCGrabber(void);
    ~CCGrabber(void);

    void grab(Texture2D *pTexture);
    void beforeRender(Texture2D *pTexture);
    void afterRender(Texture2D *pTexture);

protected:
    GLuint m_FBO;
    GLint m_oldFBO;
    GLfloat    m_oldClearColor[4];
};

// end of effects group
/// @}

NS_AX_END

#endif // __EFFECTS_AXGRABBER_H__
