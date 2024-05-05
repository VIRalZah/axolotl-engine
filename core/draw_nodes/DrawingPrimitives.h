/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

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

/*
 *
 * IMPORTANT       IMPORTANT        IMPORTANT        IMPORTANT
 *
 *
 * LEGACY FUNCTIONS
 *
 * USE DrawNode instead
 *
 */


#ifndef __AXDRAWING_PRIMITIVES__
#define __AXDRAWING_PRIMITIVES__
    

#include "base/Types.h"
#include "ccMacros.h"
    // for Vec2

/**
 @file
 Drawing OpenGL ES primitives.
 - ccDrawPoint, ccDrawPoints
 - ccDrawLine
 - ccDrawRect, ccDrawSolidRect
 - ccDrawPoly, ccDrawSolidPoly
 - ccDrawCircle
 - ccDrawQuadBezier
 - ccDrawCubicBezier
 - ccDrawCatmullRom
 - ccDrawCardinalSpline
 
 You can change the color, point size, width by calling:
 - ccDrawColor4B(), ccDrawColor4F()
 - ccPointSize()
 - glLineWidth()
 
 @warning These functions draws the Line, Vec2, Polygon, immediately. They aren't batched. If you are going to make a game that depends on these primitives, I suggest creating a batch. Instead you should use DrawNode
 
 */

NS_AX_BEGIN

/**
 * @addtogroup global
 * @{
 */

class PointArray;

/** Initializes the drawing primitives */
void AX_DLL ccDrawInit();

/** Frees allocated resources by the drawing primitives */
void AX_DLL ccDrawFree();

/** draws a point given x and y coordinate measured in points */
void AX_DLL ccDrawPoint( const Vec2& point );

/** draws an array of points.
 @since v0.7.2
 */
void AX_DLL ccDrawPoints( const Vec2 *points, unsigned int numberOfPoints );

/** draws a line given the origin and destination point measured in points */
void AX_DLL ccDrawLine( const Vec2& origin, const Vec2& destination );

/** draws a rectangle given the origin and destination point measured in points. */
void AX_DLL ccDrawRect( Vec2 origin, Vec2 destination );

/** draws a solid rectangle given the origin and destination point measured in points.
    @since 1.1
 */
void AX_DLL ccDrawSolidRect( Vec2 origin, Vec2 destination, ccColor4F color );

/** draws a polygon given a pointer to Vec2 coordinates and the number of vertices measured in points.
The polygon can be closed or open
*/
void AX_DLL ccDrawPoly( const Vec2 *vertices, unsigned int numOfVertices, bool closePolygon );

/** draws a solid polygon given a pointer to CGPoint coordinates, the number of vertices measured in points, and a color.
 */
void AX_DLL ccDrawSolidPoly( const Vec2 *poli, unsigned int numberOfPoints, ccColor4F color );

/** draws a circle given the center, radius and number of segments. */
void AX_DLL ccDrawCircle( const Vec2& center, float radius, float angle, unsigned int segments, bool drawLineToCenter, float scaleX, float scaleY);
void AX_DLL ccDrawCircle( const Vec2& center, float radius, float angle, unsigned int segments, bool drawLineToCenter);

/** draws a quad bezier path
 @warning This function could be pretty slow. Use it only for debugging purposes.
 @since v0.8
 */
void AX_DLL ccDrawQuadBezier(const Vec2& origin, const Vec2& control, const Vec2& destination, unsigned int segments);

/** draws a cubic bezier path
 @warning This function could be pretty slow. Use it only for debugging purposes.
 @since v0.8
 */
void AX_DLL ccDrawCubicBezier(const Vec2& origin, const Vec2& control1, const Vec2& control2, const Vec2& destination, unsigned int segments);

/** draws a Catmull Rom path.
 @warning This function could be pretty slow. Use it only for debugging purposes.
 @since v2.0
 */
void AX_DLL ccDrawCatmullRom( PointArray *arrayOfControlPoints, unsigned int segments );

/** draws a Cardinal Spline path.
 @warning This function could be pretty slow. Use it only for debugging purposes.
 @since v2.0
 */
void AX_DLL ccDrawCardinalSpline( PointArray *config, float tension,  unsigned int segments );

/** set the drawing color with 4 unsigned bytes
 @since v2.0
 */
void AX_DLL ccDrawColor4B( GLubyte r, GLubyte g, GLubyte b, GLubyte a );

/** set the drawing color with 4 floats
 @since v2.0
 */
void AX_DLL ccDrawColor4F( GLfloat r, GLfloat g, GLfloat b, GLfloat a );

/** set the point size in points. Default 1.
 @since v2.0
 */
void AX_DLL ccPointSize( GLfloat pointSize );

// end of global group
/// @}

NS_AX_END

#endif // __AXDRAWING_PRIMITIVES__
