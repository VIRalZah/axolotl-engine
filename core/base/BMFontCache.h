/****************************************************************************
Copyright (c) 2024 zahann.ru

http://www.zahann.ru

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

#ifndef __AX_BMFONT_CACHE_H__
#define __AX_BMFONT_CACHE_H__

#include "Dictionary.h"
#include <set>

NS_AX_BEGIN

typedef struct _axBMFontDef
{
    unsigned int charID;
    Rect rect;
    short xOffset;
    short yOffset;
    short xAdvance;
} axBMFontDef;

typedef struct _axBMFontPadding {
    int left;
    int top;
    int right;
    int bottom;
} axBMFontPadding;

typedef struct _axFontDefHashElement
{
    unsigned int key;
    axBMFontDef	fontDef;
    UT_hash_handle hh;
} axFontDefHashElement;

typedef struct _axKerningHashElement
{
    int	key;
    int	amount;
    UT_hash_handle hh;
} axKerningHashElement;

class AX_DLL BMFontConfiguration : public Object
{
public:
    BMFontConfiguration();
    virtual ~BMFontConfiguration();

    static BMFontConfiguration* create(const char* file);

    virtual bool initWithFile(const char* file);

    axFontDefHashElement* _fontDefDictionary;

    int _commonHeight;

    axBMFontPadding    m_tPadding;

    std::string _atlasName;

    axKerningHashElement* _kerningDictionary;

    std::set<unsigned int> _characterSet;
private:
    void parseCharacterDefinition(std::string line, axBMFontDef* characterDefinition);
    void parseInfoArguments(std::string line);
    void parseCommonArguments(std::string line);
    void parseImageFileName(std::string line, const char* fntFile);
    void parseKerningEntry(std::string line);
    void purgeKerningDictionary();
    void purgeFontDefDictionary();
};

class BMFontCache : public Object
{
protected:
	BMFontCache();
public:
	virtual ~BMFontCache();

	static BMFontCache* sharedBMFontCache();
	static void purgeBMFontCache();

    virtual void removeAllConfigurations();

	bool init();

	virtual BMFontConfiguration* addConfigurationWithFile(const char* file);
protected:
	Dictionary* _configurations;
};

NS_AX_END

#endif // __AX_BMFONT_CACHE_H__