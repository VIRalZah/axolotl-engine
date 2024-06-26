/****************************************************************************
Copyright (c) 2024 zahann.ru

http://www.zahann.ru

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

#ifndef __AX_CAMERA_H__
#define __AX_CAMERA_H__

#include "Object.h"
#include "ccMacros.h"
#include "kazmath/mat4.h"
#include <string>

NS_AX_BEGIN

/**
 * @addtogroup base
 * @{
 */

/** 
A Camera is used in every Node.
Useful to look at the object from different views.
The OpenGL gluLookAt() function is used to locate the
camera.

If the object is transformed by any of the scale, rotation or
position attributes, then they will override the camera.

IMPORTANT: Either your use the camera or the rotation/scale/position properties. You can't use both.
World coordinates won't work if you use the camera.

Limitations:

- Some nodes, like ParallaxNode, CCParticle uses world node coordinates, and they won't work properly if you move them (or any of their ancestors)
using the camera.

- It doesn't work on batched nodes like Sprite objects when they are parented to a SpriteBatchNode object.

- It is recommended to use it ONLY if you are going to create 3D effects. For 2D effects, use the action Follow or position/scale/rotate.

*/
class AX_DLL Camera : public Object
{
public:
    /**
     *  @js ctor
     */
    Camera(void);
    /**
     *  @js NA
     *  @lua NA
     */
    ~Camera(void);

    void init(void);
    /**
     *  @js NA
     */
    const char* description(void);

    void restore(void);
    void locate(void);

    virtual bool isDirty() const { return _dirty; }
    virtual void setDirty(bool dirty) { _dirty = dirty; }

    void setEyeXYZ(float fEyeX, float fEyeY, float fEyeZ);
    void setCenterXYZ(float fCenterX, float fCenterY, float fCenterZ);
    void setUpXYZ(float fUpX, float fUpY, float fUpZ);

    void setZoom(float zoom);
    float getZoom() const { return _zoom; };

    void lookAt(Vec2 pos); // 2d

    void getEyeXYZ(float *pEyeX, float *pEyeY, float *pEyeZ);
    void getCenterXYZ(float *pCenterX, float *pCenterY, float *pCenterZ);
    void getUpXYZ(float *pUpX, float *pUpY, float *pUpZ);

    static float getZEye();
protected:
    bool _dirty;

    kmMat4 _matrix;

    float m_fEyeX;
    float m_fEyeY;
    float m_fEyeZ;

    float m_fCenterX;
    float m_fCenterY;
    float m_fCenterZ;

    float m_fUpX;
    float m_fUpY;
    float m_fUpZ;

    float _zoom;
};

NS_AX_END

#endif // __AX_CAMERA_H__
