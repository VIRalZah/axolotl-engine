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

#ifndef __AX_COMMON_H__
#define __AX_COMMON_H__

#include "PlatformMacros.h"

#define MAX_LEN (axolotl::kMaxLogLen + 1)

NS_AX_BEGIN

/**
 * @addtogroup platform
 * @{
 */

/// The max length of log message.
static const int kMaxLogLen = 16*1024;

/**
@brief Output Debug message.
*/
void AX_DLL log(const char* format, ...) AX_FORMAT_PRINTF(1, 2);

/**
@brief Pop out a message box
*/
void AX_DLL axMessageBox(const char* msg, const char* title);

/**
@brief Enum the language type supported now
*/
typedef enum LanguageType
{
    kLanguageEnglish = 0,
    kLanguageChinese,
    kLanguageFrench,
    kLanguageItalian,
    kLanguageGerman,
    kLanguageSpanish,
    kLanguageDutch,
    kLanguageRussian,
    kLanguageKorean,
    kLanguageJapanese,
    kLanguageHungarian,
    kLanguagePortuguese,
    kLanguageArabic
} ccLanguageType;

// end of platform group
/// @}

NS_AX_END

#endif    // __AX_COMMON_H__
