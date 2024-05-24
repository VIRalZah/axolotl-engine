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

#include "BMFontCache.h"
#include "platform/FileUtils.h"
#include "Configuration.h"

NS_AX_BEGIN

// BMFontConfiguration

BMFontConfiguration::BMFontConfiguration()
    : _fontDefDictionary(nullptr)
    , _kerningDictionary(nullptr)
{
}

BMFontConfiguration::~BMFontConfiguration()
{
    AXLOGINFO("axolotl: deallocing BMFontConfiguration %p", this);
    purgeFontDefDictionary();
    purgeKerningDictionary();
    _atlasName.clear();
    _characterSet.clear();
}

BMFontConfiguration* BMFontConfiguration::create(const char* file)
{
	auto ret = new (std::nothrow) BMFontConfiguration();
	if (ret && ret->initWithFile(file))
	{
		ret->autorelease();
		return ret;
	}
	AX_SAFE_DELETE(ret);
	return nullptr;
}

bool BMFontConfiguration::initWithFile(const char* file)
{
	unsigned long size;
	auto data = FileUtils::sharedFileUtils()->getFileData(file, "rb", &size);

	if (data != nullptr && size > 0)
	{
        std::string line;
        std::string string((char*)data, size);
        while (string.size() > 0)
        {
            auto pos = string.find('\n');

            if (pos != std::string::npos)
            {
                line = string.substr(0, pos);
                string = string.substr(pos + 1);
            }
            else
            {
                line = string;
                string.erase();
            }

            if (line.find("info face") == 0)
            {
                parseInfoArguments(line);
            }
            else if (line.find("common lineHeight") == 0)
            {
                parseCommonArguments(line);
            }
            else if (line.find("page id") == 0)
            {
                parseImageFileName(line, file);
            }
            else if (line.find("chars c") == 0)
            {
                // skip
            }
            else if (line.find("char") == 0)
            {
                auto element = (axFontDefHashElement*)malloc(sizeof(axFontDefHashElement));
                parseCharacterDefinition(line, &element->fontDef);

                element->key = element->fontDef.charID;
                HASH_ADD_INT(_fontDefDictionary, key, element);

                _characterSet.insert(element->fontDef.charID);
            }
            else if (line.find("kerning first") == 0)
            {
                parseKerningEntry(line);
            }
        }

		return true;
	}

	return false;
}


void BMFontConfiguration::parseImageFileName(std::string line, const char* fntFile)
{
    int index = line.find('=') + 1;
    int index2 = line.find(' ', index);
    std::string value = line.substr(index, index2 - index);
    AXAssert(atoi(value.c_str()) == 0, "LabelBMFont file could not be found");

    index = line.find('"') + 1;

    _atlasName = FileUtils::sharedFileUtils()->fullPathFromRelativeFile(
        line.substr(
            index,
            line.find('"', index) - index
        ).c_str(),
        fntFile);
}

void BMFontConfiguration::parseInfoArguments(std::string line)
{
    int index = line.find("padding=");
    int index2 = line.find(' ', index);
    std::string value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "padding=%d,%d,%d,%d", &m_tPadding.top, &m_tPadding.right, &m_tPadding.bottom, &m_tPadding.left);
    AXLOG("cocos2d: padding: %d,%d,%d,%d", m_tPadding.left, m_tPadding.top, m_tPadding.right, m_tPadding.bottom);
}

void BMFontConfiguration::parseCommonArguments(std::string line)
{
    int index = line.find("lineHeight=");
    int index2 = line.find(' ', index);
    std::string value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "lineHeight=%d", &_commonHeight);

    auto maxTextureSize = Configuration::sharedConfiguration()->getMaxTextureSize();

    index = line.find("scaleW=") + strlen("scaleW=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2 - index);
    AXAssert(atoi(value.c_str()) <= maxTextureSize, "LabelBMFont: page can't be larger than supported");

    index = line.find("scaleH=") + strlen("scaleH=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2 - index);
    AXAssert(atoi(value.c_str()) <= maxTextureSize, "LabelBMFont: page can't be larger than supported");

    index = line.find("pages=") + strlen("pages=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2 - index);
    AXAssert(atoi(value.c_str()) == 1, "BMFontConfiguration only supports 1 page");
}

void BMFontConfiguration::parseCharacterDefinition(std::string line, axBMFontDef* characterDefinition)
{
    int index = line.find("id=");
    int index2 = line.find(' ', index);
    std::string value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "id=%u", &characterDefinition->charID);

    index = line.find("x=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "x=%f", &characterDefinition->rect.origin.x);

    index = line.find("y=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "y=%f", &characterDefinition->rect.origin.y);

    index = line.find("width=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "width=%f", &characterDefinition->rect.size.width);

    index = line.find("height=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "height=%f", &characterDefinition->rect.size.height);

    index = line.find("xoffset=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "xoffset=%hd", &characterDefinition->xOffset);

    index = line.find("yoffset=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "yoffset=%hd", &characterDefinition->yOffset);

    index = line.find("xadvance=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "xadvance=%hd", &characterDefinition->xAdvance);
}

void BMFontConfiguration::parseKerningEntry(std::string line)
{
    int first;
    int index = line.find("first=");
    int index2 = line.find(' ', index);
    std::string value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "first=%d", &first);

    int second;
    index = line.find("second=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "second=%d", &second);

    int amount;
    index = line.find("amount=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2 - index);
    sscanf(value.c_str(), "amount=%d", &amount);

    auto element = (axKerningHashElement*)malloc(sizeof(axKerningHashElement));
    element->amount = amount;
    element->key = (first << 16) | (second & 0xffff);
    HASH_ADD_INT(_kerningDictionary, key, element);
}

void BMFontConfiguration::purgeKerningDictionary()
{
    axKerningHashElement* current;
    while (_kerningDictionary)
    {
        current = _kerningDictionary;
        HASH_DEL(_kerningDictionary, current);
        free(current);
    }
}

void BMFontConfiguration::purgeFontDefDictionary()
{
    axFontDefHashElement* current, *tmp;

    HASH_ITER(hh, _fontDefDictionary, current, tmp)
    {
        HASH_DEL(_fontDefDictionary, current);
        free(current);
    }
}

// BMFontCache

static BMFontCache* _sharedBMFontCache = nullptr;

BMFontCache::BMFontCache()
	: _configurations(nullptr)
{
}

BMFontCache::~BMFontCache()
{
	AX_SAFE_RELEASE(_configurations);
}

BMFontCache* BMFontCache::sharedBMFontCache()
{
	if (!_sharedBMFontCache)
	{
		_sharedBMFontCache = new (std::nothrow) BMFontCache();
		_sharedBMFontCache->init();
	}
	return _sharedBMFontCache;
}

void BMFontCache::purgeBMFontCache()
{
	AX_SAFE_DELETE(_sharedBMFontCache);
}

void BMFontCache::removeAllConfigurations()
{
    _configurations->removeAllObjects();
}

bool BMFontCache::init()
{
	_configurations = new Dictionary();

	return true;
}

BMFontConfiguration* BMFontCache::addConfigurationWithFile(const char* file)
{
	auto fullPath = FileUtils::sharedFileUtils()->fullPathForFilename(file);
	BMFontConfiguration* ret = static_cast<BMFontConfiguration*>(_configurations->objectForKey(fullPath));
	if (!ret)
	{
		ret = BMFontConfiguration::create(fullPath.c_str());
		_configurations->setObject(ret, fullPath);
	}
	return ret;
}

NS_AX_END