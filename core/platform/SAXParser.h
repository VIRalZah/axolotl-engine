/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org  http://cocos2d-x.org
 Copyright (c) 2010 Максим Аксенов
 
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

#ifndef __AXSAXPARSER_H__
#define __AXSAXPARSER_H__

#include "PlatformConfig.h"
#include "Common.h"

NS_AX_BEGIN

/**
 * @addtogroup platform
 * @{
 */

typedef unsigned char AX_XML_CHAR;

/**
 * @js NA
 * @lua NA
 */
class AX_DLL SAXDelegator
{
public:
    virtual void startElement(void *ctx, const char *name, const char **atts) = 0;
    virtual void endElement(void *ctx, const char *name) = 0;
    virtual void textHandler(void *ctx, const char *s, int len) = 0;
};

/**
 * @js NA
 * @lua NA
 */
class AX_DLL SAXParser
{
    SAXDelegator*    m_pDelegator;
public:

    SAXParser();
    ~SAXParser(void);

    bool init(const char *pszEncoding);
    bool parse(const char* pXMLData, unsigned int uDataLength);
    bool parse(const char *pszFile);
    void setDelegator(SAXDelegator* pDelegator);

    static void startElement(void *ctx, const AX_XML_CHAR *name, const AX_XML_CHAR **atts);
    static void endElement(void *ctx, const AX_XML_CHAR *name);
    static void textHandler(void *ctx, const AX_XML_CHAR *name, int len);
};

// end of platform group
/// @}

NS_AX_END

#endif //__AXSAXPARSER_H__
