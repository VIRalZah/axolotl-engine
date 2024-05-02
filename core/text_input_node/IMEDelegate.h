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

#ifndef __AX_IME_DELEGATE_H__
#define __AX_IME_DELEGATE_H__

#include "cocoa/Geometry.h"

NS_AX_BEGIN

/**
 * @addtogroup input
 * @{
 */

typedef struct
{
    Rect  begin;              // the soft keyboard rectangle when animation begins
    Rect  end;                // the soft keyboard rectangle when animation ends
    float     duration;           // the soft keyboard animation duration
} CCIMEKeyboardNotificationInfo;

/**
@brief    Input method editor delegate.
@js NA
@lua NA
*/
class AX_DLL IMEDelegate
{
public:
    virtual ~IMEDelegate();

    virtual bool attachWithIME();
    virtual bool detachWithIME();

protected:
    friend class IMEDispatcher;

    /**
    @brief    Decide if the delegate instance is ready to receive an IME message.

    Called by IMEDispatcher.
    */
    virtual bool canAttachWithIME() { return false; }
    /**
    @brief    When the delegate detaches from the IME, this method is called by IMEDispatcher.
    */
    virtual void didAttachWithIME() {}

    /**
    @brief    Decide if the delegate instance can stop receiving IME messages.
    */
    virtual bool canDetachWithIME() { return false; }

    /**
    @brief    When the delegate detaches from the IME, this method is called by IMEDispatcher.
    */
    virtual void didDetachWithIME() {}

    /**
    @brief    Called by IMEDispatcher when text input received from the IME.
    */
    virtual void insertText(const std::string& text) {AX_UNUSED_PARAM(text);}

    /**
    @brief    Called by IMEDispatcher after the user clicks the backward key.
    */
    virtual void deleteBackward() {}

    /**
    @brief    Called by IMEDispatcher for text stored in delegate.
    */
    virtual const char * getContentText() { return 0; }

    //////////////////////////////////////////////////////////////////////////
    // keyboard show/hide notification
    //////////////////////////////////////////////////////////////////////////
    virtual void keyboardWillShow(CCIMEKeyboardNotificationInfo& info)   {AX_UNUSED_PARAM(info);}
    virtual void keyboardDidShow(CCIMEKeyboardNotificationInfo& info)    {AX_UNUSED_PARAM(info);}
    virtual void keyboardWillHide(CCIMEKeyboardNotificationInfo& info)   {AX_UNUSED_PARAM(info);}
    virtual void keyboardDidHide(CCIMEKeyboardNotificationInfo& info)    {AX_UNUSED_PARAM(info);}

protected:
    IMEDelegate();
};

// end of input group
/// @}

NS_AX_END

#endif    // __AX_IME_DELEGATE_H__
