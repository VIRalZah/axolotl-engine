/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2007      Scott Lembcke
Copyright (c) 2010      Lam Pham

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

#ifndef __SUPPORT_CGPOINTEXTENSION_H__
#define __SUPPORT_CGPOINTEXTENSION_H__

/**
 @file
 Vec2 extensions based on Chipmunk's cpVect file.
 These extensions work both with Vec2 and cpVect.
 
 The "Vec2" prefix means: "CoCos2d Vec2"
 
 Examples:
  - PointAdd( Vec2(1,1), Vec2(2,2) ); // preferred cocos2d way
  - PointAdd( Vec2(1,1), Vec2(2,2) ); // also ok but more verbose
  
  - cpvadd( cpv(1,1), cpv(2,2) ); // way of the chipmunk
  - PointAdd( cpv(1,1), cpv(2,2) ); // mixing chipmunk and cocos2d (avoid)
  - cpvadd( Vec2(1,1), Vec2(2,2) ); // mixing chipmunk and CG (avoid)
 */


#include <math.h>
#include "base/Types.h"

NS_AX_BEGIN

/**
 * @addtogroup data_structures
 * @{
 */

/** Returns opposite of point.
 @return Vec2
 @since v0.7.2
 */
static inline Vec2
PointNeg(const Vec2& v)
{
    return -v;
}

/** Calculates sum of two points.
 @return Vec2
 @since v0.7.2
 */
static inline Vec2
PointAdd(const Vec2& v1, const Vec2& v2)
{
    return v1 + v2;
}

/** Calculates difference of two points.
 @return Vec2
 @since v0.7.2
 */
static inline Vec2
PointSub(const Vec2& v1, const Vec2& v2)
{
    return v1 - v2;
}

/** Returns point multiplied by given factor.
 @return Vec2
 @since v0.7.2
 */
static inline Vec2
PointMult(const Vec2& v, const float s)
{
    return v * s;
}

/** Calculates midpoint between two points.
 @return Vec2
 @since v0.7.2
 */
static inline Vec2
PointMidpoint(const Vec2& v1, const Vec2& v2)
{
    return (v1 + v2) / 2.f;
}

/** Calculates dot product of two points.
 @return float
 @since v0.7.2
 */
static inline float
PointDot(const Vec2& v1, const Vec2& v2)
{
    return v1.dot(v2);
}

/** Calculates cross product of two points.
 @return float
 @since v0.7.2
 */
static inline float
PointCross(const Vec2& v1, const Vec2& v2)
{
    return v1.cross(v2);
}

/** Calculates perpendicular of v, rotated 90 degrees counter-clockwise -- cross(v, perp(v)) >= 0
 @return Vec2
 @since v0.7.2
 */
static inline Vec2
PointPerp(const Vec2& v)
{
    return v.getPerp();
}

/** Calculates perpendicular of v, rotated 90 degrees clockwise -- cross(v, rperp(v)) <= 0
 @return Vec2
 @since v0.7.2
 */
static inline Vec2
PointRPerp(const Vec2& v)
{
    return v.getRPerp();
}

/** Calculates the projection of v1 over v2.
 @return Vec2
 @since v0.7.2
 */
static inline Vec2
PointProject(const Vec2& v1, const Vec2& v2)
{
    return v1.project(v2);
}

/** Rotates two points.
 @return Vec2
 @since v0.7.2
 */
static inline Vec2
PointRotate(const Vec2& v1, const Vec2& v2)
{
    return v1.rotate(v2);
}

/** Unrotates two points.
 @return Vec2
 @since v0.7.2
 */
static inline Vec2
PointUnrotate(const Vec2& v1, const Vec2& v2)
{
    return v1.unrotate(v2);
}

/** Calculates the square length of a Vec2 (not calling sqrt() )
 @return float
 @since v0.7.2
 */
static inline float
PointLengthSQ(const Vec2& v)
{
    return v.getLengthSq();
}


/** Calculates the square distance between two points (not calling sqrt() )
 @return float
 @since v1.1
*/
static inline float
PointDistanceSQ(const Vec2 p1, const Vec2 p2)
{
    return (p1 - p2).getLengthSq();
}


/** Calculates distance between point an origin
 @return float
 @since v0.7.2
 */
float AX_DLL PointLength(const Vec2& v);

/** Calculates the distance between two points
 @return float
 @since v0.7.2
 */
float AX_DLL PointDistance(const Vec2& v1, const Vec2& v2);

/** Returns point multiplied to a length of 1.
 @return Vec2
 @since v0.7.2
 */
Vec2 AX_DLL PointNormalize(const Vec2& v);

/** Converts radians to a normalized vector.
 @return Vec2
 @since v0.7.2
 */
Vec2 AX_DLL PointForAngle(const float a);

/** Converts a vector to radians.
 @return float
 @since v0.7.2
 */
float AX_DLL PointToAngle(const Vec2& v);


/** Clamp a value between from and to.
 @since v0.99.1
 */
float AX_DLL clampf(float value, float min_inclusive, float max_inclusive);

/** Clamp a point between from and to.
 @since v0.99.1
 */
Vec2 AX_DLL PointClamp(const Vec2& p, const Vec2& from, const Vec2& to);

/** Quickly convert Size to a Vec2
 @since v0.99.1
 */
Vec2 AX_DLL PointFromSize(const Size& s);

/** Run a math operation function on each point component
 * absf, fllorf, ceilf, roundf
 * any function that has the signature: float func(float);
 * For example: let's try to take the floor of x,y
 * PointCompOp(p,floorf);
 @since v0.99.1
 */
Vec2 AX_DLL PointCompOp(const Vec2& p, float (*opFunc)(float));

/** Linear Interpolation between two points a and b
 @returns
    alpha == 0 ? a
    alpha == 1 ? b
    otherwise a value between a..b
 @since v0.99.1
 */
Vec2 AX_DLL PointLerp(const Vec2& a, const Vec2& b, float alpha);


/** @returns if points have fuzzy equality which means equal with some degree of variance.
 @since v0.99.1
 */
bool AX_DLL PointFuzzyEqual(const Vec2& a, const Vec2& b, float variance);


/** Multiplies a and b components, a.x*b.x, a.y*b.y
 @returns a component-wise multiplication
 @since v0.99.1
 */
Vec2 AX_DLL PointCompMult(const Vec2& a, const Vec2& b);

/** @returns the signed angle in radians between two vector directions
 @since v0.99.1
 */
float AX_DLL PointAngleSigned(const Vec2& a, const Vec2& b);

/** @returns the angle in radians between two vector directions
 @since v0.99.1
*/
float AX_DLL PointAngle(const Vec2& a, const Vec2& b);

/** Rotates a point counter clockwise by the angle around a pivot
 @param v is the point to rotate
 @param pivot is the pivot, naturally
 @param angle is the angle of rotation cw in radians
 @returns the rotated point
 @since v0.99.1
 */
Vec2 AX_DLL PointRotateByAngle(const Vec2& v, const Vec2& pivot, float angle);

/** A general line-line intersection test
 @param p1 
    is the startpoint for the first line P1 = (p1 - p2)
 @param p2 
    is the endpoint for the first line P1 = (p1 - p2)
 @param p3 
    is the startpoint for the second line P2 = (p3 - p4)
 @param p4 
    is the endpoint for the second line P2 = (p3 - p4)
 @param s 
    is the range for a hitpoint in P1 (pa = p1 + s*(p2 - p1))
 @param t
    is the range for a hitpoint in P3 (pa = p2 + t*(p4 - p3))
 @return bool 
    indicating successful intersection of a line
    note that to truly test intersection for segments we have to make 
    sure that s & t lie within [0..1] and for rays, make sure s & t > 0
    the hit point is        p3 + t * (p4 - p3);
    the hit point also is    p1 + s * (p2 - p1);
 @since v0.99.1
 */
bool AX_DLL PointLineIntersect(const Vec2& p1, const Vec2& p2, 
                      const Vec2& p3, const Vec2& p4,
                      float *s, float *t);

/*
PointSegmentIntersect returns YES if Segment A-B intersects with segment C-D
@since v1.0.0
*/
bool AX_DLL PointSegmentIntersect(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D);

/*
PointIntersectPoint returns the intersection point of line A-B, C-D
@since v1.0.0
*/
Vec2 AX_DLL PointIntersectPoint(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D);

// end of data_structures group
/// @}

NS_AX_END

#endif // __SUPPORT_CGPOINTEXTENSION_H__

