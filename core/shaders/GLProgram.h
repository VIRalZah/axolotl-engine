/****************************************************************************
Copyright 2012 cocos2d-x.org
Copyright 2011 Jeff Lamarche
Copyright 2012 Goffredo Marocchi
Copyright 2012 Ricardo Quesada

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

#ifndef __AXGLPROGRAM_H__
#define __AXGLPROGRAM_H__

#include "ccMacros.h"
#include "base/Object.h"

#include "GL.h"

NS_AX_BEGIN

/**
 * @addtogroup shaders
 * @{
 */

enum {
    kCCVertexAttrib_Position,
    kCCVertexAttrib_Color,
    kCCVertexAttrib_TexCoords,

    kCCVertexAttrib_MAX,
};

enum {
	kCCUniformPMatrix,
	kCCUniformMVMatrix,
	kCCUniformMVPMatrix,
	kCCUniformTime,
	kCCUniformSinTime,
	kCCUniformCosTime,
	kCCUniformRandom01,
	kCCUniformSampler,
    
	kCCUniform_MAX,
};

#define kAXShader_PositionTextureColor              "ShaderPositionTextureColor"
#define kAXShader_PositionTextureColorAlphaTest     "ShaderPositionTextureColorAlphaTest"
#define kAXShader_PositionColor                     "ShaderPositionColor"
#define kAXShader_PositionTexture                   "ShaderPositionTexture"
#define kAXShader_PositionTexture_uColor            "ShaderPositionTexture_uColor"
#define kAXShader_PositionTextureA8Color            "ShaderPositionTextureA8Color"
#define kAXShader_Position_uColor                   "ShaderPosition_uColor"
#define kAXShader_PositionLengthTexureColor         "ShaderPositionLengthTextureColor"
#define kAXShader_ControlSwitch                     "Shader_ControlSwitch"

// uniform names
#define kAXUniformPMatrix_s				"AX_PMatrix"
#define kAXUniformMVMatrix_s			"AX_MVMatrix"
#define kAXUniformMVPMatrix_s			"AX_MVPMatrix"
#define kAXUniformTime_s				"AX_Time"
#define kAXUniformSinTime_s				"AX_SinTime"
#define kAXUniformCosTime_s				"AX_CosTime"
#define kAXUniformRandom01_s			"AX_Random01"
#define kAXUniformSampler_s				"AX_Texture0"
#define kAXUniformAlphaTestValue		"AX_alpha_value"

// Attribute names
#define    kAXAttributeNameColor           "a_color"
#define    kAXAttributeNamePosition        "a_position"
#define    kAXAttributeNameTexCoord        "a_texCoord"

struct _hashUniformEntry;

typedef void (*GLInfoFunction)(GLuint program, GLenum pname, GLint* params);
typedef void (*GLLogFunction) (GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog);

/** GLProgram
 Class that implements a glProgram
 
 
 @since v2.0.0
 */
class AX_DLL GLProgram : public Object
{
public:
    /**
     * @js  ctor
     * @lua NA
     */
    GLProgram();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~GLProgram();
    /** Initializes the GLProgram with a vertex and fragment with bytes array 
     * @js  initWithString
     * @lua NA
     */
    bool initWithVertexShaderByteArray(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray);

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT) || (AX_TARGET_PLATFORM == AX_PLATFORM_WP8)
    /** Initializes the GLProgram with precompiled shader program */
    bool initWithPrecompiledProgramByteArray(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray);
#endif
    /** Initializes the GLProgram with a vertex and fragment with contents of filenames 
     * @js  init
     * @lua NA
     */
    bool initWithVertexShaderFilename(const char* vShaderFilename, const char* fShaderFilename);
    /**  It will add a new attribute to the shader 
     * @lua NA
     */
    void addAttribute(const char* attributeName, GLuint index);
    /** links the glProgram 
     * @lua NA
     */
    bool link();
    /** it will call glUseProgram() 
     * @lua NA
     */
    void use();
/** It will create 4 uniforms:
    - kCCUniformPMatrix
    - kCCUniformMVMatrix
    - kCCUniformMVPMatrix
    - kCCUniformSampler

 And it will bind "kCCUniformSampler" to 0
 * @lua NA
 */
    void updateUniforms();
    
    /** calls retrieves the named uniform location for this shader program. 
     * @lua NA
     */
    GLint getUniformLocationForName(const char* name);
    
    /** calls glUniform1i only if the values are different than the previous call for this same shader program. 
     *  @js  setUniformLocationI32
     *  @lua NA
     */
    void setUniformLocationWith1i(GLint location, GLint i1);
    
    /** calls glUniform2i only if the values are different than the previous call for this same shader program. 
     * @lua NA
     */
    void setUniformLocationWith2i(GLint location, GLint i1, GLint i2);
    
    /** calls glUniform3i only if the values are different than the previous call for this same shader program. 
     * @lua NA
     */
    void setUniformLocationWith3i(GLint location, GLint i1, GLint i2, GLint i3);
    
    /** calls glUniform4i only if the values are different than the previous call for this same shader program.
     * @lua NA
     */
    void setUniformLocationWith4i(GLint location, GLint i1, GLint i2, GLint i3, GLint i4);
    
    /** calls glUniform2iv only if the values are different than the previous call for this same shader program. 
     * @lua NA
     */
    void setUniformLocationWith2iv(GLint location, GLint* ints, unsigned int numberOfArrays);
    
    /** calls glUniform3iv only if the values are different than the previous call for this same shader program.
     * @lua NA
     */
    void setUniformLocationWith3iv(GLint location, GLint* ints, unsigned int numberOfArrays);
    
    /** calls glUniform4iv only if the values are different than the previous call for this same shader program. 
     * @lua NA
     */
    
    void setUniformLocationWith4iv(GLint location, GLint* ints, unsigned int numberOfArrays);

    /** calls glUniform1f only if the values are different than the previous call for this same shader program. 
     *  @js  setUniformLocationF32
     *  @lua NA
     */
    void setUniformLocationWith1f(GLint location, GLfloat f1);

    /** calls glUniform2f only if the values are different than the previous call for this same shader program. 
     *  @js  setUniformLocationF32
     *  @lua NA
     */
    void setUniformLocationWith2f(GLint location, GLfloat f1, GLfloat f2);

    /** calls glUniform3f only if the values are different than the previous call for this same shader program. 
     *  @js  setUniformLocationF32
     *  @lua NA
     */
    void setUniformLocationWith3f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3);

    /** calls glUniform4f only if the values are different than the previous call for this same shader program. 
     *  @js  setUniformLocationF32
     *  @lua NA
     */
    void setUniformLocationWith4f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4);

    /** calls glUniform2fv only if the values are different than the previous call for this same shader program. 
     *  @js  NA
     *  @lua NA
     */
    void setUniformLocationWith2fv(GLint location, GLfloat* floats, unsigned int numberOfArrays);

    /** calls glUniform3fv only if the values are different than the previous call for this same shader program. 
     *  @js  NA
     *  @lua NA
     */
    void setUniformLocationWith3fv(GLint location, GLfloat* floats, unsigned int numberOfArrays);

    /** calls glUniform4fv only if the values are different than the previous call for this same shader program. 
     *  @js  NA
     *  @lua NA
     */
    void setUniformLocationWith4fv(GLint location, GLfloat* floats, unsigned int numberOfArrays);

    /** calls glUniformMatrix4fv only if the values are different than the previous call for this same shader program. 
     *  @js  NA
     *  @lua NA
     */
    void setUniformLocationWithMatrix4fv(GLint location, GLfloat* matrixArray, unsigned int numberOfMatrices);
    
    /** will update the builtin uniforms if they are different than the previous call for this same shader program. 
     *  @lua NA
     */
    void setUniformsForBuiltins();

    /** returns the vertexShader error log 
     *  @js  getVertexShaderLog
     *  @lua NA
     */
    const char* vertexShaderLog();
    /** returns the fragmentShader error log 
     *  @js  getFragmentShaderLog
     *  @lua NA
     */
    const char* fragmentShaderLog();
    /** returns the program error log 
     *  @js  getProgramLog
     *  @lua NA
     */
    const char* programLog();
    
    /** reload all shaders, this function is designed for android
     *  when opengl context lost, so don't call it.
     *  @lua NA
     */
    void reset();
    /**
     * @lua NA
     */
    inline const GLuint getProgram() { return m_uProgram; }

private:
    bool updateUniformLocation(GLint location, GLvoid* data, unsigned int bytes);
    const char* description();
    bool compileShader(GLuint * shader, GLenum type, const GLchar* source);
    const char* logForOpenGLObject(GLuint object, GLInfoFunction infoFunc, GLLogFunction logFunc);

private:
    GLuint            m_uProgram;
    GLuint            m_uVertShader;
    GLuint            m_uFragShader;
    GLint             m_uUniforms[kCCUniform_MAX];
    struct _hashUniformEntry* m_pHashForUniforms;
    bool              m_bUsesTime;
    bool              m_hasShaderCompiler;

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT) || (AX_TARGET_PLATFORM == AX_PLATFORM_WP8)
    std::string       m_shaderId;
#endif
};

// end of shaders group
/// @}

NS_AX_END

#endif /* __AXGLPROGRAM_H__ */
