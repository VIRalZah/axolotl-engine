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

#ifndef __COCOA_CGAFFINETRANSFORM_H__
#define __COCOA_CGAFFINETRANSFORM_H__

#include "base/Types.h"

NS_AX_BEGIN

struct AffineTransform {
  float a, b, c, d;
  float tx, ty;
};

AX_DLL AffineTransform __AffineTransformMake(float a, float b, float c, float d, float tx, float ty);
#define AffineTransformMake __AffineTransformMake

AX_DLL Vec2 __CCPointApplyAffineTransform(const Vec2& point, const AffineTransform& t);
#define CCPointApplyAffineTransform __CCPointApplyAffineTransform

AX_DLL Size __CCSizeApplyAffineTransform(const Size& size, const AffineTransform& t);
#define CCSizeApplyAffineTransform __CCSizeApplyAffineTransform

AX_DLL AffineTransform AffineTransformMakeIdentity();
AX_DLL Rect CCRectApplyAffineTransform(const Rect& rect, const AffineTransform& anAffineTransform);

AX_DLL AffineTransform AffineTransformTranslate(const AffineTransform& t, float tx, float ty);
AX_DLL AffineTransform AffineTransformRotate(const AffineTransform& aTransform, float anAngle);
AX_DLL AffineTransform AffineTransformScale(const AffineTransform& t, float sx, float sy);
AX_DLL AffineTransform AffineTransformConcat(const AffineTransform& t1, const AffineTransform& t2);
AX_DLL bool AffineTransformEqualToTransform(const AffineTransform& t1, const AffineTransform& t2);
AX_DLL AffineTransform AffineTransformInvert(const AffineTransform& t);

extern AX_DLL const AffineTransform AffineTransformIdentity;

NS_AX_END

#endif // __COCOA_CGAFFINETRANSFORM_H__
