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

#ifndef __AXSHADERCACHE_H__
#define __AXSHADERCACHE_H__

#include "base/Dictionary.h"

NS_AX_BEGIN

class GLProgram;

/**
 * @addtogroup shaders
 * @{
 */

/** ShaderCache
 Singleton that stores manages GL shaders
 @since v2.0
 */
class AX_DLL ShaderCache : public Object 
{
public:
    /**
     * @js ctor
     */
    ShaderCache();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~ShaderCache();
    /** returns the shared instance 
     *  @js sharedEGLView
     */
    static ShaderCache* sharedShaderCache();

    /** purges the cache. It releases the retained instance. */
    static void purgeSharedShaderCache();

    /** loads the default shaders */
    void loadDefaultShaders();
    
    /** reload the default shaders */
    void reloadDefaultShaders();



    /** returns a GL program for a given key 
     *  @js getProgram
     */
    GLProgram * programForKey(const char* key);

    /** adds a GLProgram to the cache for a given name */
    void addProgram(GLProgram* program, const char* key);

private:
    bool init();
    void loadDefaultShader(GLProgram *program, int type);

    Dictionary* m_pPrograms;

};

// end of shaders group
/// @}

NS_AX_END

#endif /* __AXSHADERCACHE_H__ */
