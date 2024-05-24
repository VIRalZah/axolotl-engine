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

#include "TextFieldTTF.h"

#include "base/Director.h"
#include "GLViewImpl.h"

NS_AX_BEGIN

static int _calcCharCount(const char * pszText)
{
    int n = 0;
    char ch = 0;
    while ((ch = *pszText))
    {
        AX_BREAK_IF(! ch);

        if (0x80 != (0xC0 & ch))
        {
            ++n;
        }
        ++pszText;
    }
    return n;
}

//////////////////////////////////////////////////////////////////////////
// constructor and destructor
//////////////////////////////////////////////////////////////////////////

TextFieldTTF::TextFieldTTF()
: _delegate(0)
, _charCount(0)
, _placeholderColor(ccGRAY)
, _secureTextEntry(false)
{
}

TextFieldTTF::~TextFieldTTF()
{
}

TextFieldTTF * TextFieldTTF::create(const std::string& placeholder, const Size& dimensions, CCTextAlignment alignment, const char* fontName, float fontSize)
{
    TextFieldTTF *pRet = new TextFieldTTF();
    if(pRet && pRet->initWithPlaceHolder("", dimensions, alignment, fontName, fontSize))
    {
        pRet->autorelease();
        pRet->setPlaceHolder(placeholder);
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

TextFieldTTF * TextFieldTTF::create(const std::string& placeholder, const char* fontName, float fontSize)
{
    TextFieldTTF *pRet = new TextFieldTTF();
    if(pRet && pRet->initWithString("", fontName, fontSize))
    {
        pRet->autorelease();
        pRet->setPlaceHolder(placeholder);
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
// initialize
//////////////////////////////////////////////////////////////////////////

bool TextFieldTTF::initWithPlaceHolder(const std::string& placeholder, const Size& dimensions, CCTextAlignment alignment, const char *fontName, float fontSize)
{
    return LabelTTF::initWithString(placeholder, fontName, fontSize, dimensions, alignment);
}
bool TextFieldTTF::initWithPlaceHolder(const std::string& placeholder, const char *fontName, float fontSize)
{
    return LabelTTF::initWithString(placeholder, fontName, fontSize);
}

bool TextFieldTTF::attachWithIME()
{
    bool bRet = IMEDelegate::attachWithIME();
    if (bRet)
    {
        auto glView = Director::sharedDirector()->getOpenGLView();
        if (glView)
        {
            glView->setIMEKeyboardState(true);
        }
    }
    return bRet;
}

bool TextFieldTTF::detachWithIME()
{
    bool bRet = IMEDelegate::detachWithIME();
    if (bRet)
    {
        auto glView = Director::sharedDirector()->getOpenGLView();
        if (glView)
        {
            glView->setIMEKeyboardState(false);
        }
    }
    return bRet;
}

bool TextFieldTTF::canAttachWithIME()
{
    return (_delegate) ? (! _delegate->onTextFieldAttachWithIME(this)) : true;
}

bool TextFieldTTF::canDetachWithIME()
{
    return (_delegate) ? (! _delegate->onTextFieldDetachWithIME(this)) : true;
}

void TextFieldTTF::insertText(const std::string& text)
{
    auto insert = text;
    auto len = insert.length();

    int pos = insert.find('\n');
    if ((int)insert.npos != pos)
    {
        len = pos;
        insert.erase(pos);
    }

    if (len > 0)
    {
        if (_delegate && _delegate->onTextFieldInsertText(this, insert))
        {
            return;
        }

        _charCount += _calcCharCount(insert.c_str());
        setString(_content + insert);
    }

    if ((int)text.npos == pos) {
        return;
    }

    // '\n' inserted, let delegate process first
    if (_delegate && _delegate->onTextFieldInsertText(this, "\n"))
    {
        return;
    }

    // if delegate hasn't processed, detach from IME by default
    detachWithIME();
}

void TextFieldTTF::deleteBackward()
{
    int nStrLen = _content.length();
    if (!nStrLen)
    {
        return;
    }

    int nDeleteLen = 1;

    while(0x80 == (0xC0 & _content.at(nStrLen - nDeleteLen)))
    {
        ++nDeleteLen;
    }

    if (_delegate && _delegate->onTextFieldDeleteBackward(this, _content.c_str() + nStrLen - nDeleteLen))
    {
        return;
    }

    if (nStrLen <= nDeleteLen)
    {
        _content.clear();
        _charCount = 0;

        LabelTTF::setString(_placeholder.c_str());
        return;
    }

    std::string text(_content.c_str(), nStrLen - nDeleteLen);
    setString(text);
}

void TextFieldTTF::draw()
{
    if (_delegate && _delegate->onDraw(this))
    {
        return;
    }

    if (_content.length())
    {
        LabelTTF::draw();
        return;
    }

    ccColor3B color = getColor();
    setColor(_placeholderColor);
    LabelTTF::draw();
    setColor(color);
}

const ccColor3B& TextFieldTTF::getPlaceHolderColor()
{
    return _placeholderColor;
}

void TextFieldTTF::setPlaceHolderColor(const ccColor3B& color)
{
    _placeholderColor = color;
}

void TextFieldTTF::setString(const std::string& text)
{
    static char bulletString[] = {(char)0xe2, (char)0x80, (char)0xa2, (char)0x00};
    std::string displayText;
    int length;

    _content.clear();

    if (!text.empty())
    {
        _content.assign(text);
        displayText = _content;
        if (_secureTextEntry)
        {
            displayText = "";
            length = _content.length();
            while (length)
            {
                displayText.append(bulletString);
                --length;
            }
        }
    }

    if (!_content.length())
    {
        LabelTTF::setString(_placeholder);
    }
    else
    {
        LabelTTF::setString(displayText.c_str());
    }

    _charCount = _calcCharCount(_content.c_str());
}

const std::string& TextFieldTTF::getString(void)
{
    return _content;
}

// place holder text property
void TextFieldTTF::setPlaceHolder(const std::string& placeholder)
{
    _placeholder.assign(placeholder);
    if (!_content.length())
    {
        LabelTTF::setString(_placeholder);
    }
}

std::string TextFieldTTF::getPlaceHolder(void)
{
    return _placeholder;
}

// secureTextEntry
void TextFieldTTF::setSecureTextEntry(bool value)
{
    if (_secureTextEntry != value)
    {
        _secureTextEntry = value;
        setString(getString());
    }
}

bool TextFieldTTF::isSecureTextEntry()
{
    return _secureTextEntry;
}

NS_AX_END
