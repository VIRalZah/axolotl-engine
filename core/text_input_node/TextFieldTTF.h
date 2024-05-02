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

#ifndef __AX_TEXT_FIELD_H__
#define __AX_TEXT_FIELD_H__

#include "label_nodes/LabelTTF.h"
#include "text_input_node/IMEDelegate.h"


NS_AX_BEGIN

class TextFieldTTF;

/**
 * @addtogroup input
 * @{
 * @js NA
 * @lua NA
 */

class AX_DLL TextFieldDelegate
{
public:
    virtual bool onTextFieldAttachWithIME(TextFieldTTF* sender)
    {
        AX_UNUSED_PARAM(sender);
        return false;
    }

    virtual bool onTextFieldDetachWithIME(TextFieldTTF* sender)
    {
        AX_UNUSED_PARAM(sender);
        return false;
    }

    virtual bool onTextFieldInsertText(TextFieldTTF* sender, const std::string& text)
    {
        AX_UNUSED_PARAM(sender);
        AX_UNUSED_PARAM(text);
        return false;
    }

    virtual bool onTextFieldDeleteBackward(TextFieldTTF* sender, const std::string& delText)
    {
        AX_UNUSED_PARAM(sender);
        AX_UNUSED_PARAM(delText);
        return false;
    }

    virtual bool onDraw(TextFieldTTF* sender)
    {
        AX_UNUSED_PARAM(sender);
        return false;
    }
};

class AX_DLL TextFieldTTF : public LabelTTF, public IMEDelegate
{
public:
    TextFieldTTF();
    virtual ~TextFieldTTF();

    static TextFieldTTF* create(const std::string& placeholder, const Size& dimensions, CCTextAlignment alignment, const char* fontName, float fontSize);
    static TextFieldTTF* create(const std::string& placeholder, const char* fontName, float fontSize);

    bool initWithPlaceHolder(const std::string& placeholder, const Size& dimensions, CCTextAlignment alignment, const char *fontName, float fontSize);
    bool initWithPlaceHolder(const std::string& placeholder, const char *fontName, float fontSize);

    virtual bool attachWithIME();
    virtual bool detachWithIME();

    AX_SYNTHESIZE(TextFieldDelegate*, _delegate, Delegate);
    AX_SYNTHESIZE_READONLY(int, _charCount, CharCount);

    virtual const ccColor3B& getPlaceHolderColor();
    virtual void setPlaceHolderColor(const ccColor3B& color);
public:
    virtual void setString(const std::string& text);
    virtual const std::string& getString(void);

    virtual void setPlaceHolder(const std::string& placeholder);
    virtual std::string getPlaceHolder(void);

    virtual void setSecureTextEntry(bool value);
    virtual bool isSecureTextEntry();
protected:
    virtual void draw();

    virtual bool canAttachWithIME();
    virtual bool canDetachWithIME();
    virtual void insertText(const std::string& text);
    virtual void deleteBackward();

    std::string _placeholder;
    ccColor3B _placeholderColor;

    std::string _content;

    bool _secureTextEntry;
};

// end of input group
/// @}

NS_AX_END

#endif    // __AX_TEXT_FIELD_H__
