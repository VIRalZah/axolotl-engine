/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Ricardo Quesada
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

#ifndef __AXGLSTATE_H__
#define __AXGLSTATE_H__

#include "GL.h"
#include "platform/PlatformMacros.h"

NS_AX_BEGIN

/**
 * @addtogroup shaders
 * @{
 */

class GLProgram;

/** vertex attrib flags */
enum {
    kCCVertexAttribFlag_None        = 0,

    kCCVertexAttribFlag_Position    = 1 << 0,
    kCCVertexAttribFlag_Color       = 1 << 1,
    kCCVertexAttribFlag_TexCoords   = 1 << 2,

    kCCVertexAttribFlag_PosColorTex = ( kCCVertexAttribFlag_Position | kCCVertexAttribFlag_Color | kCCVertexAttribFlag_TexCoords ),
};

/** GL server side states */
typedef enum {
//    AX_GL_SCISSOR_TEST = 1 << 0,
//    AX_GL_STENCIL_TEST = 1 << 1,
//    AX_GL_DEPTH_TEST = 1 << 2,
//    AX_GL_BLEND = 1 << 3,
//    AX_GL_DITHER = 1 << 4,

//    AX_GL_ALL = ( AX_GL_SCISSOR_TEST | AX_GL_STENCIL_TEST | AX_GL_DEPTH_TEST | AX_GL_BLEND | AX_GL_DITHER ),
//    AX_GL_ALL = ( AX_GL_BLEND ),
      AX_GL_ALL = 0,

} ccGLServerState;

/** @file ccGLStateCache.h
*/

/** Invalidates the GL state cache.
 If AX_ENABLE_GL_STATE_CACHE it will reset the GL state cache.
 @since v2.0.0
 */
void AX_DLL ccGLInvalidateStateCache(void);

/** Uses the GL program in case program is different than the current one.
 If AX_ENABLE_GL_STATE_CACHE is disabled, it will the glUseProgram() directly.
 @since v2.0.0
 */
void AX_DLL ccGLUseProgram(GLuint program);

/** Deletes the GL program. If it is the one that is being used, it invalidates it.
 If AX_ENABLE_GL_STATE_CACHE is disabled, it will the glDeleteProgram() directly.
 @since v2.0.0
 */
void AX_DLL ccGLDeleteProgram(GLuint program);

/** Uses a blending function in case it not already used.
 If AX_ENABLE_GL_STATE_CACHE is disabled, it will the glBlendFunc() directly.
 @since v2.0.0
 */
void AX_DLL ccGLBlendFunc(GLenum sfactor, GLenum dfactor);

/** Resets the blending mode back to the cached state in case you used glBlendFuncSeparate() or glBlendEquation().
 If AX_ENABLE_GL_STATE_CACHE is disabled, it will just set the default blending mode using GL_FUNC_ADD.
 @since v2.0.0
 */
void AX_DLL ccGLBlendResetToCache(void);

/** sets the projection matrix as dirty
 @since v2.0.0
 */
void AX_DLL ccSetProjectionMatrixDirty(void);

/** Will enable the vertex attribs that are passed as flags.
 Possible flags:

    * kCCVertexAttribFlag_Position
    * kCCVertexAttribFlag_Color
    * kCCVertexAttribFlag_TexCoords

 These flags can be ORed. The flags that are not present, will be disabled.

 @since v2.0.0
 */
void AX_DLL ccGLEnableVertexAttribs(unsigned int flags);

/** If the texture is not already bound to texture unit 0, it binds it.
 If AX_ENABLE_GL_STATE_CACHE is disabled, it will call glBindTexture() directly.
 @since v2.0.0
 */
void AX_DLL ccGLBindTexture2D(GLuint textureId);


/** If the texture is not already bound to a given unit, it binds it.
 If AX_ENABLE_GL_STATE_CACHE is disabled, it will call glBindTexture() directly.
 @since v2.1.0
 */
void AX_DLL ccGLBindTexture2DN(GLuint textureUnit, GLuint textureId);

/** It will delete a given texture. If the texture was bound, it will invalidate the cached.
 If AX_ENABLE_GL_STATE_CACHE is disabled, it will call glDeleteTextures() directly.
 @since v2.0.0
 */
void AX_DLL ccGLDeleteTexture(GLuint textureId);

/** It will delete a given texture. If the texture was bound, it will invalidate the cached for the given texture unit.
 If AX_ENABLE_GL_STATE_CACHE is disabled, it will call glDeleteTextures() directly.
 @since v2.1.0
 */
void AX_DLL ccGLDeleteTextureN(GLuint textureUnit, GLuint textureId);

/** If the vertex array is not already bound, it binds it.
 If AX_ENABLE_GL_STATE_CACHE is disabled, it will call glBindVertexArray() directly.
 @since v2.0.0
 */
void AX_DLL ccGLBindVAO(GLuint vaoId);

/** It will enable / disable the server side GL states.
 If AX_ENABLE_GL_STATE_CACHE is disabled, it will call glEnable() directly.
 @since v2.0.0
 */
void AX_DLL ccGLEnable( ccGLServerState flags );

// end of shaders group
/// @}

NS_AX_END
    

#endif /* __AXGLSTATE_H__ */
