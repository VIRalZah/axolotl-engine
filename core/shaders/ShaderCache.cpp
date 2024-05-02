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

#include "ShaderCache.h"
#include "GLProgram.h"
#include "ccMacros.h"
#include "ccShaders.h"


NS_AX_BEGIN

enum {
    kCCShaderType_PositionTextureColor,
    kCCShaderType_PositionTextureColorAlphaTest,
    kCCShaderType_PositionColor,
    kCCShaderType_PositionTexture,
    kCCShaderType_PositionTexture_uColor,
    kCCShaderType_PositionTextureA8Color,
    kCCShaderType_Position_uColor,
    kCCShaderType_PositionLengthTexureColor,
    kCCShaderType_ControlSwitch,
    
    kCCShaderType_MAX,
};

static ShaderCache *_sharedShaderCache = 0;

ShaderCache* ShaderCache::sharedShaderCache()
{
    if (!_sharedShaderCache) {
        _sharedShaderCache = new ShaderCache();
        if (!_sharedShaderCache->init())
        {
            AX_SAFE_DELETE(_sharedShaderCache);
        }
    }
    return _sharedShaderCache;
}

void ShaderCache::purgeSharedShaderCache()
{
    AX_SAFE_RELEASE_NULL(_sharedShaderCache);
}

ShaderCache::ShaderCache()
: m_pPrograms(0)
{

}

ShaderCache::~ShaderCache()
{
    AXLOGINFO("cocos2d deallocing 0x%X", this);
    m_pPrograms->release();
}



bool ShaderCache::init()
{
    m_pPrograms = new Dictionary();
    loadDefaultShaders();
    return true;
}

void ShaderCache::loadDefaultShaders()
{
    // Position Texture Color shader
    GLProgram *p = new GLProgram();
    loadDefaultShader(p, kCCShaderType_PositionTextureColor);

    m_pPrograms->setObject(p, kAXShader_PositionTextureColor);
    p->release();

    // Position Texture Color alpha test
    p = new GLProgram();
    loadDefaultShader(p, kCCShaderType_PositionTextureColorAlphaTest);

    m_pPrograms->setObject(p, kAXShader_PositionTextureColorAlphaTest);
    p->release();

    //
    // Position, Color shader
    //
    p = new GLProgram();
    loadDefaultShader(p, kCCShaderType_PositionColor);

    m_pPrograms->setObject(p, kAXShader_PositionColor);
    p->release();

    //
    // Position Texture shader
    //
    p = new GLProgram();
    loadDefaultShader(p, kCCShaderType_PositionTexture);

    m_pPrograms->setObject(p, kAXShader_PositionTexture);
    p->release();

    //
    // Position, Texture attribs, 1 Color as uniform shader
    //
    p = new GLProgram();
    loadDefaultShader(p, kCCShaderType_PositionTexture_uColor);

    m_pPrograms->setObject(p ,kAXShader_PositionTexture_uColor);
    p->release();

    //
    // Position Texture A8 Color shader
    //
    p = new GLProgram();
    loadDefaultShader(p, kCCShaderType_PositionTextureA8Color);
    
    m_pPrograms->setObject(p, kAXShader_PositionTextureA8Color);
    p->release();

    //
    // Position and 1 color passed as a uniform (to simulate glColor4ub )
    //
    p = new GLProgram();
    loadDefaultShader(p, kCCShaderType_Position_uColor);
    
    m_pPrograms->setObject(p, kAXShader_Position_uColor);
    p->release();
    
    //
	// Position, Legth(TexCoords, Color (used by Draw Node basically )
	//
    p = new GLProgram();
    loadDefaultShader(p, kCCShaderType_PositionLengthTexureColor);
    
    m_pPrograms->setObject(p, kAXShader_PositionLengthTexureColor);
    p->release();

    //
	// ControlSwitch
	//
    p = new GLProgram();
    loadDefaultShader(p, kCCShaderType_ControlSwitch);
    
    m_pPrograms->setObject(p, kAXShader_ControlSwitch);
    p->release();
}



void ShaderCache::reloadDefaultShaders()
{
    // reset all programs and reload them
    
    // Position Texture Color shader
    GLProgram *p = programForKey(kAXShader_PositionTextureColor);    
    p->reset();
    loadDefaultShader(p, kCCShaderType_PositionTextureColor);

    // Position Texture Color alpha test
    p = programForKey(kAXShader_PositionTextureColorAlphaTest);
    p->reset();    
    loadDefaultShader(p, kCCShaderType_PositionTextureColorAlphaTest);
    
    //
    // Position, Color shader
    //
    p = programForKey(kAXShader_PositionColor);
    p->reset();
    loadDefaultShader(p, kCCShaderType_PositionColor);
    
    //
    // Position Texture shader
    //
    p = programForKey(kAXShader_PositionTexture);
    p->reset();
    loadDefaultShader(p, kCCShaderType_PositionTexture);
    
    //
    // Position, Texture attribs, 1 Color as uniform shader
    //
    p = programForKey(kAXShader_PositionTexture_uColor);
    p->reset();
    loadDefaultShader(p, kCCShaderType_PositionTexture_uColor);
    
    //
    // Position Texture A8 Color shader
    //
    p = programForKey(kAXShader_PositionTextureA8Color);
    p->reset();
    loadDefaultShader(p, kCCShaderType_PositionTextureA8Color);
    
    //
    // Position and 1 color passed as a uniform (to simulate glColor4ub )
    //
    p = programForKey(kAXShader_Position_uColor);
    p->reset();
    loadDefaultShader(p, kCCShaderType_Position_uColor);
    
    //
	// Position, Legth(TexCoords, Color (used by Draw Node basically )
	//
    p = programForKey(kAXShader_PositionLengthTexureColor);
    p->reset();
    loadDefaultShader(p, kCCShaderType_PositionLengthTexureColor);
}




void ShaderCache::loadDefaultShader(GLProgram *p, int type)
{
    switch (type) {
        case kCCShaderType_PositionTextureColor:
            p->initWithVertexShaderByteArray(ccPositionTextureColor_vert, ccPositionTextureColor_frag);
            
            p->addAttribute(kAXAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kAXAttributeNameColor, kCCVertexAttrib_Color);
            p->addAttribute(kAXAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
            
            break;
        case kCCShaderType_PositionTextureColorAlphaTest:
            p->initWithVertexShaderByteArray(ccPositionTextureColor_vert, ccPositionTextureColorAlphaTest_frag);
            
            p->addAttribute(kAXAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kAXAttributeNameColor, kCCVertexAttrib_Color);
            p->addAttribute(kAXAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

            break;
        case kCCShaderType_PositionColor:  
            p->initWithVertexShaderByteArray(ccPositionColor_vert ,ccPositionColor_frag);
            
            p->addAttribute(kAXAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kAXAttributeNameColor, kCCVertexAttrib_Color);

            break;
        case kCCShaderType_PositionTexture:
            p->initWithVertexShaderByteArray(ccPositionTexture_vert ,ccPositionTexture_frag);
            
            p->addAttribute(kAXAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kAXAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

            break;
        case kCCShaderType_PositionTexture_uColor:
            p->initWithVertexShaderByteArray(ccPositionTexture_uColor_vert, ccPositionTexture_uColor_frag);
            
            p->addAttribute(kAXAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kAXAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

            break;
        case kCCShaderType_PositionTextureA8Color:
            p->initWithVertexShaderByteArray(ccPositionTextureA8Color_vert, ccPositionTextureA8Color_frag);
            
            p->addAttribute(kAXAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kAXAttributeNameColor, kCCVertexAttrib_Color);
            p->addAttribute(kAXAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

            break;
        case kCCShaderType_Position_uColor:
            p->initWithVertexShaderByteArray(ccPosition_uColor_vert, ccPosition_uColor_frag);    
            
            p->addAttribute("aVertex", kCCVertexAttrib_Position);    
            
            break;
        case kCCShaderType_PositionLengthTexureColor:
            p->initWithVertexShaderByteArray(ccPositionColorLengthTexture_vert, ccPositionColorLengthTexture_frag);
            
            p->addAttribute(kAXAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kAXAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
            p->addAttribute(kAXAttributeNameColor, kCCVertexAttrib_Color);
            
            break;

       case kCCShaderType_ControlSwitch:
            p->initWithVertexShaderByteArray(ccPositionTextureColor_vert, ccExSwitchMask_frag);

            p->addAttribute(kAXAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kAXAttributeNameColor, kCCVertexAttrib_Color);
            p->addAttribute(kAXAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

            break;

        default:
            AXLOG("cocos2d: %s:%d, error shader type", __FUNCTION__, __LINE__);
            return;
    }
    
    p->link();
    p->updateUniforms();
    
    CHECK_GL_ERROR_DEBUG();
}

GLProgram* ShaderCache::programForKey(const char* key)
{
    return (GLProgram*)m_pPrograms->objectForKey(key);
}

void ShaderCache::addProgram(GLProgram* program, const char* key)
{
    m_pPrograms->setObject(program, key);
}

NS_AX_END
