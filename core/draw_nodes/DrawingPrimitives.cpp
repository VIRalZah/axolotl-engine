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

#include "DrawingPrimitives.h"
#include "base/Types.h"
#include "ccMacros.h"
#include "GL.h"
#include "base/Director.h"
#include "shaders/ccGLStateCache.h"
#include "shaders/ShaderCache.h"
#include "shaders/GLProgram.h"
#include "actions/ActionCatmullRom.h"
#include "support/PointExtension.h"
#include <string.h>
#include <cmath>

NS_AX_BEGIN
#ifndef M_PI
    #define M_PI       3.14159265358979323846
#endif

static bool s_bInitialized = false;
static GLProgram* s_pShader = NULL;
static int s_nColorLocation = -1;
static ccColor4F s_tColor = {1.0f,1.0f,1.0f,1.0f};
static int s_nPointSizeLocation = -1;
static GLfloat s_fPointSize = 1.0f;

#ifdef EMSCRIPTEN
static GLuint s_bufferObject = 0;
static GLuint s_bufferSize = 0;

static void setGLBufferData(void *buf, GLuint bufSize)
{
    if(s_bufferSize < bufSize)
    {
        if(s_bufferObject)
        {
            glDeleteBuffers(1, &s_bufferObject);
        }
        glGenBuffers(1, &s_bufferObject);
        s_bufferSize = bufSize;

        glBindBuffer(GL_ARRAY_BUFFER, s_bufferObject);
        glBufferData(GL_ARRAY_BUFFER, bufSize, buf, GL_DYNAMIC_DRAW);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, s_bufferObject);
        glBufferSubData(GL_ARRAY_BUFFER, 0, bufSize, buf);
    }
}

#endif // EMSCRIPTEN

static void lazy_init( void )
{
    if( ! s_bInitialized ) {

        //
        // Position and 1 color passed as a uniform (to simulate glColor4ub )
        //
        s_pShader = ShaderCache::sharedShaderCache()->programForKey(kAXShader_Position_uColor);
        s_pShader->retain();
        
        s_nColorLocation = glGetUniformLocation( s_pShader->getProgram(), "u_color");
    CHECK_GL_ERROR_DEBUG();
        s_nPointSizeLocation = glGetUniformLocation( s_pShader->getProgram(), "u_pointSize");
    CHECK_GL_ERROR_DEBUG();

        s_bInitialized = true;
    }
}

// When switching from backround to foreground on android, we want the params to be initialized again
void ccDrawInit()
{
    lazy_init();
}

void ccDrawFree()
{
	AX_SAFE_RELEASE_NULL(s_pShader);
	s_bInitialized = false;
}

void ccDrawPoint( const Vec2& point )
{
    lazy_init();

    ccVertex2F p;
    p.x = point.x;
    p.y = point.y;

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );
    s_pShader->use();
    s_pShader->setUniformsForBuiltins();

    s_pShader->setUniformLocationWith4fv(s_nColorLocation, (GLfloat*) &s_tColor.r, 1);
    s_pShader->setUniformLocationWith1f(s_nPointSizeLocation, s_fPointSize);

#ifdef EMSCRIPTEN
    setGLBufferData(&p, 8);
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, &p);
#endif // EMSCRIPTEN

    glDrawArrays(GL_POINTS, 0, 1);

    AX_INCREMENT_GL_DRAWS(1);
}

void ccDrawPoints( const Vec2 *points, unsigned int numberOfPoints )
{
    lazy_init();

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );
    s_pShader->use();
    s_pShader->setUniformsForBuiltins();
    s_pShader->setUniformLocationWith4fv(s_nColorLocation, (GLfloat*) &s_tColor.r, 1);
    s_pShader->setUniformLocationWith1f(s_nPointSizeLocation, s_fPointSize);

    // XXX: Mac OpenGL error. arrays can't go out of scope before draw is executed
    ccVertex2F* newPoints = new ccVertex2F[numberOfPoints];

    // iPhone and 32-bit machines optimization
    if( sizeof(Vec2) == sizeof(ccVertex2F) )
    {
#ifdef EMSCRIPTEN
        setGLBufferData((void*) points, numberOfPoints * sizeof(Vec2));
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, points);
#endif // EMSCRIPTEN
    }
    else
    {
        // Mac on 64-bit
        for( unsigned int i=0; i<numberOfPoints;i++) {
            newPoints[i].x = points[i].x;
            newPoints[i].y = points[i].y;
        }
#ifdef EMSCRIPTEN
        // Suspect Emscripten won't be emitting 64-bit code for a while yet,
        // but want to make sure this continues to work even if they do.
        setGLBufferData(newPoints, numberOfPoints * sizeof(ccVertex2F));
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, newPoints);
#endif // EMSCRIPTEN
    }

    glDrawArrays(GL_POINTS, 0, (GLsizei) numberOfPoints);

    AX_SAFE_DELETE_ARRAY(newPoints);

    AX_INCREMENT_GL_DRAWS(1);
}


void ccDrawLine( const Vec2& origin, const Vec2& destination )
{
    lazy_init();

    ccVertex2F vertices[2] = {
        {origin.x, origin.y},
        {destination.x, destination.y}
    };

    s_pShader->use();
    s_pShader->setUniformsForBuiltins();
    s_pShader->setUniformLocationWith4fv(s_nColorLocation, (GLfloat*) &s_tColor.r, 1);

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );
#ifdef EMSCRIPTEN
    setGLBufferData(vertices, 16);
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#endif // EMSCRIPTEN
    glDrawArrays(GL_LINES, 0, 2);

    AX_INCREMENT_GL_DRAWS(1);
}

void ccDrawRect( Vec2 origin, Vec2 destination )
{
    ccDrawLine(Vec2(origin.x, origin.y), Vec2(destination.x, origin.y));
    ccDrawLine(Vec2(destination.x, origin.y), Vec2(destination.x, destination.y));
    ccDrawLine(Vec2(destination.x, destination.y), Vec2(origin.x, destination.y));
    ccDrawLine(Vec2(origin.x, destination.y), Vec2(origin.x, origin.y));
}

void ccDrawSolidRect( Vec2 origin, Vec2 destination, ccColor4F color )
{
    Vec2 vertices[] = {
        origin,
        Vec2(destination.x, origin.y),
        destination,
        Vec2(origin.x, destination.y)
    };

    ccDrawSolidPoly(vertices, 4, color );
}

void ccDrawPoly( const Vec2 *poli, unsigned int numberOfPoints, bool closePolygon )
{
    lazy_init();

    s_pShader->use();
    s_pShader->setUniformsForBuiltins();
    s_pShader->setUniformLocationWith4fv(s_nColorLocation, (GLfloat*) &s_tColor.r, 1);

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );

    // iPhone and 32-bit machines optimization
    if( sizeof(Vec2) == sizeof(ccVertex2F) )
    {
#ifdef EMSCRIPTEN
        setGLBufferData((void*) poli, numberOfPoints * sizeof(Vec2));
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, poli);
#endif // EMSCRIPTEN

        if( closePolygon )
            glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) numberOfPoints);
        else
            glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) numberOfPoints);
    }
    else
    {
        // Mac on 64-bit
        // XXX: Mac OpenGL error. arrays can't go out of scope before draw is executed
        ccVertex2F* newPoli = new ccVertex2F[numberOfPoints];
        for( unsigned int i=0; i<numberOfPoints;i++) {
            newPoli[i].x = poli[i].x;
            newPoli[i].y = poli[i].y;
        }
#ifdef EMSCRIPTEN
        setGLBufferData(newPoli, numberOfPoints * sizeof(ccVertex2F));
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, newPoli);
#endif // EMSCRIPTEN

        if( closePolygon )
            glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) numberOfPoints);
        else
            glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) numberOfPoints);

        AX_SAFE_DELETE_ARRAY(newPoli);
    }

    AX_INCREMENT_GL_DRAWS(1);
}

void ccDrawSolidPoly( const Vec2 *poli, unsigned int numberOfPoints, ccColor4F color )
{
    lazy_init();

    s_pShader->use();
    s_pShader->setUniformsForBuiltins();
    s_pShader->setUniformLocationWith4fv(s_nColorLocation, (GLfloat*) &color.r, 1);

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );

    // XXX: Mac OpenGL error. arrays can't go out of scope before draw is executed
    ccVertex2F* newPoli = new ccVertex2F[numberOfPoints];

    // iPhone and 32-bit machines optimization
    if( sizeof(Vec2) == sizeof(ccVertex2F) )
    {
#ifdef EMSCRIPTEN
        setGLBufferData((void*) poli, numberOfPoints * sizeof(Vec2));
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, poli);
#endif // EMSCRIPTEN
    }
    else
    {
        // Mac on 64-bit
        for( unsigned int i=0; i<numberOfPoints;i++)
        {
            newPoli[i] = vertex2( poli[i].x, poli[i].y );
        }
#ifdef EMSCRIPTEN
        setGLBufferData(newPoli, numberOfPoints * sizeof(ccVertex2F));
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, newPoli);
#endif // EMSCRIPTEN
    }    

    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei) numberOfPoints);

    AX_SAFE_DELETE_ARRAY(newPoli);
    AX_INCREMENT_GL_DRAWS(1);
}

void ccDrawCircle( const Vec2& center, float radius, float angle, unsigned int segments, bool drawLineToCenter, float scaleX, float scaleY)
{
    lazy_init();

    int additionalSegment = 1;
    if (drawLineToCenter)
        additionalSegment++;

    const float coef = 2.0f * (float)M_PI/segments;

    GLfloat *vertices = (GLfloat*)calloc( sizeof(GLfloat)*2*(segments+2), 1);
    if( ! vertices )
        return;

    for(unsigned int i = 0;i <= segments; i++) {
        float rads = i*coef;
        GLfloat j = radius * cosf(rads + angle) * scaleX + center.x;
        GLfloat k = radius * sinf(rads + angle) * scaleY + center.y;

        vertices[i*2] = j;
        vertices[i*2+1] = k;
    }
    vertices[(segments+1)*2] = center.x;
    vertices[(segments+1)*2+1] = center.y;

    s_pShader->use();
    s_pShader->setUniformsForBuiltins();
    s_pShader->setUniformLocationWith4fv(s_nColorLocation, (GLfloat*) &s_tColor.r, 1);

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );

#ifdef EMSCRIPTEN
    setGLBufferData(vertices, sizeof(GLfloat)*2*(segments+2));
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#endif // EMSCRIPTEN
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments+additionalSegment);

    free( vertices );

    AX_INCREMENT_GL_DRAWS(1);
}

void AX_DLL ccDrawCircle( const Vec2& center, float radius, float angle, unsigned int segments, bool drawLineToCenter)
{
    ccDrawCircle(center, radius, angle, segments, drawLineToCenter, 1.0f, 1.0f);
}

void ccDrawQuadBezier(const Vec2& origin, const Vec2& control, const Vec2& destination, unsigned int segments)
{
    lazy_init();

    ccVertex2F* vertices = new ccVertex2F[segments + 1];

    float t = 0.0f;
    for(unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1 - t, 2) * origin.x + 2.0f * (1 - t) * t * control.x + t * t * destination.x;
        vertices[i].y = powf(1 - t, 2) * origin.y + 2.0f * (1 - t) * t * control.y + t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;

    s_pShader->use();
    s_pShader->setUniformsForBuiltins();
    s_pShader->setUniformLocationWith4fv(s_nColorLocation, (GLfloat*) &s_tColor.r, 1);

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );

#ifdef EMSCRIPTEN
    setGLBufferData(vertices, (segments + 1) * sizeof(ccVertex2F));
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#endif // EMSCRIPTEN
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments + 1);
    AX_SAFE_DELETE_ARRAY(vertices);

    AX_INCREMENT_GL_DRAWS(1);
}

void ccDrawCatmullRom( PointArray *points, unsigned int segments )
{
    ccDrawCardinalSpline( points, 0.5f, segments );
}

void ccDrawCardinalSpline( PointArray *config, float tension,  unsigned int segments )
{
    lazy_init();

    ccVertex2F* vertices = new ccVertex2F[segments + 1];

    unsigned int p;
    float lt;
    float deltaT = 1.0f / config->count();

    for( unsigned int i=0; i < segments+1;i++) {

        float dt = (float)i / segments;

        // border
        if( dt == 1 ) {
            p = config->count() - 1;
            lt = 1;
        } else {
            p = dt / deltaT;
            lt = (dt - deltaT * (float)p) / deltaT;
        }

        // Interpolate
        Vec2 pp0 = config->getControlPointAtIndex(p-1);
        Vec2 pp1 = config->getControlPointAtIndex(p+0);
        Vec2 pp2 = config->getControlPointAtIndex(p+1);
        Vec2 pp3 = config->getControlPointAtIndex(p+2);

        Vec2 newPos = ccCardinalSplineAt( pp0, pp1, pp2, pp3, tension, lt);
        vertices[i].x = newPos.x;
        vertices[i].y = newPos.y;
    }

    s_pShader->use();
    s_pShader->setUniformsForBuiltins();
    s_pShader->setUniformLocationWith4fv(s_nColorLocation, (GLfloat*)&s_tColor.r, 1);

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );

#ifdef EMSCRIPTEN
    setGLBufferData(vertices, (segments + 1) * sizeof(ccVertex2F));
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#endif // EMSCRIPTEN
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments + 1);

    AX_SAFE_DELETE_ARRAY(vertices);
    AX_INCREMENT_GL_DRAWS(1);
}

void ccDrawCubicBezier(const Vec2& origin, const Vec2& control1, const Vec2& control2, const Vec2& destination, unsigned int segments)
{
    lazy_init();

    ccVertex2F* vertices = new ccVertex2F[segments + 1];

    float t = 0;
    for(unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1 - t, 3) * origin.x + 3.0f * powf(1 - t, 2) * t * control1.x + 3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
        vertices[i].y = powf(1 - t, 3) * origin.y + 3.0f * powf(1 - t, 2) * t * control1.y + 3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;

    s_pShader->use();
    s_pShader->setUniformsForBuiltins();
    s_pShader->setUniformLocationWith4fv(s_nColorLocation, (GLfloat*) &s_tColor.r, 1);

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );

#ifdef EMSCRIPTEN
    setGLBufferData(vertices, (segments + 1) * sizeof(ccVertex2F));
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#endif // EMSCRIPTEN
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments + 1);
    AX_SAFE_DELETE_ARRAY(vertices);

    AX_INCREMENT_GL_DRAWS(1);
}

void ccDrawColor4F( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
    s_tColor.r = r;
    s_tColor.g = g;
    s_tColor.b = b;
    s_tColor.a = a;
}

void ccPointSize( GLfloat pointSize )
{
    s_fPointSize = pointSize * AX_CONTENT_SCALE_FACTOR();

    //TODO :glPointSize( pointSize );

}

void ccDrawColor4B( GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    s_tColor.r = r/255.0f;
    s_tColor.g = g/255.0f;
    s_tColor.b = b/255.0f;
    s_tColor.a = a/255.0f;
}

NS_AX_END
