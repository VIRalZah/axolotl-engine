/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2014 Chukong Technologies Inc.

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

#ifndef __cocos2dx__ccUTF8__
#define __cocos2dx__ccUTF8__

#include "platform/PlatformMacros.h"
#include <vector>
#include <string>

NS_AX_BEGIN


/** Trims the space characters at the end of UTF8 string */
AX_DLL void AX_utf8_trim_ws(std::vector<unsigned short>* str);

/**
 * Whether the character is a whitespace character.
 *
 * @param ch    the unicode character
 * @returns     whether the character is a white space character.
 *
 * @see http://en.wikipedia.org/wiki/Whitespace_character#Unicode
 * */
AX_DLL bool isspace_unicode(unsigned short ch);

/**
 * Whether the character is a Chinese/Japanese/Korean character.
 *
 * @param ch    the unicode character
 * @returns     whether the character is a Chinese character.
 *
 * @see http://www.searchtb.com/2012/04/chinese_encode.html
 * @see http://tieba.baidu.com/p/748765987
 * */
AX_DLL bool iscjk_unicode(unsigned short ch);


/**
 * Returns the character count in UTF16 string
 * @param str pointer to the start of a UTF-16 encoded string. It must be an NULL terminal UTF8 string.
 */
AX_DLL int AX_wcslen(const unsigned short* str);

/**
 * Returns the length of the string in characters.
 *
 * @param p pointer to the start of a UTF-8 encoded string. It must be an NULL terminal UTF8 string.
 *
 * @returns the length of the string in characters
 **/
AX_DLL long AX_utf8_strlen (const char * p);

/**
 * Find the last character that is not equal to the character given.
 *
 * @param str   the string to be searched.
 * @param c     the character to be searched for.
 *
 * @returns the index of the last character that is not \p c.
 * */
AX_DLL unsigned int AX_utf8_find_last_not_char(const std::vector<unsigned short>& str, unsigned short c);

/** Gets UTF16 character vector from UTF16 string */
AX_DLL std::vector<unsigned short> AX_utf16_vec_from_utf16_str(const unsigned short* str);

/**
 * Creates an utf8 string from a c string. The result will be null terminated.
 *
 * @param utf8 pointer to the start of a C string. It must be an NULL terminal UTF8 string.
 * @param outUTF16CharacterCount The character count in the return UTF16 string.
 *
 * @returns the newly created utf16 string, it must be released with `delete[]`,
 *          If an error occurs, %NULL will be returned.
 * */
AX_DLL unsigned short* AX_utf8_to_utf16(const char* utf8, int* outUTF16CharacterCount = NULL);

/**
 * Converts a string from UTF-16 to UTF-8. The result will be null terminated.
 *
 * @param utf16 an UTF-16 encoded string, It must be an NULL terminal UTF16 string.
 * @param outUTF8CharacterCount The character count in the return UTF8 string.
 *
 * @returns a pointer to a newly allocated UTF-8 string. This value must be
 *          released with `delete[]`. If an error occurs, %NULL will be returned.
 **/
AX_DLL char* AX_utf16_to_utf8(const unsigned short* utf16, int* outUTF8CharacterCount = NULL);

AX_DLL char* AX_codepoint_to_utf8(const unsigned int codepoint);


NS_AX_END

#endif /* defined(__cocos2dx__ccUTF8__) */
