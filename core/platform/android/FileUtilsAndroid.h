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
#ifndef __AX_FILEUTILS_ANDROID_H__
#define __AX_FILEUTILS_ANDROID_H__

#include "platform/FileUtils.h"
#include "platform/PlatformMacros.h"
#include "base/Types.h"
#include "ccTypeInfo.h"
#include <string>
#include <vector>

NS_AX_BEGIN

/**
 * @addtogroup platform
 * @{
 */

//! @brief  Helper class to handle file operations
class AX_DLL FileUtilsAndroid : public FileUtils
{
    friend class FileUtils;
    FileUtilsAndroid();
public:
    virtual ~FileUtilsAndroid();

    /* override funtions */
    bool init();
    virtual unsigned char* getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize);
    virtual std::string getWritablePath();
    virtual bool isFileExist(const std::string& strFilePath);
    virtual bool isAbsolutePath(const std::string& strPath);
    
    /** This function is android specific. It is used for TextureCache::addImageAsync(). 
     Don't use it in your codes.
     */
    unsigned char* getFileDataForAsync(const char* pszFileName, const char* pszMode, unsigned long * pSize);
    
private:
    unsigned char* doGetFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize, bool forAsync);
};

// end of platform group
/// @}

NS_AX_END

#endif    // __AX_FILEUTILS_ANDROID_H__

