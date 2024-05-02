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

#ifndef __AX_IME_DISPATCHER_H__
#define __AX_IME_DISPATCHER_H__

#include "IMEDelegate.h"

NS_AX_BEGIN

/**
 * @addtogroup input
 * @{
 */

/**
@brief    Input Method Edit Message Dispatcher.
@js NA
@lua NA
*/
class AX_DLL IMEDispatcher
{
public:
    ~IMEDispatcher();

    /**
    @brief Returns the shared IMEDispatcher object for the system.
    */
    static IMEDispatcher* sharedDispatcher();

//     /**
//     @brief Releases all CCIMEDelegates from the shared dispatcher.
//     */
//     static void purgeSharedDispatcher();

    /**
    @brief Dispatches the input text from IME.
    */
    void dispatchInsertText(std::string text);

    /**
    @brief Dispatches the delete-backward operation.
    */
    void dispatchDeleteBackward();

    /**
    @brief Get the content text from IMEDelegate, retrieved previously from IME.
    */
    const char * getContentText();

    //////////////////////////////////////////////////////////////////////////
    // dispatch keyboard notification
    //////////////////////////////////////////////////////////////////////////
    void dispatchKeyboardWillShow(CCIMEKeyboardNotificationInfo& info);
    void dispatchKeyboardDidShow(CCIMEKeyboardNotificationInfo& info);
    void dispatchKeyboardWillHide(CCIMEKeyboardNotificationInfo& info);
    void dispatchKeyboardDidHide(CCIMEKeyboardNotificationInfo& info);

protected:
    friend class IMEDelegate;

    /**
    @brief Add delegate to receive IME messages.
    */
    void addDelegate(IMEDelegate * pDelegate);

    /**
    @brief Attach the pDelegate to the IME.
    @return If the old delegate can detach from the IME, and the new delegate 
            can attach to the IME, return true, otherwise false.
    */
    bool attachDelegateWithIME(IMEDelegate * pDelegate);
    bool detachDelegateWithIME(IMEDelegate * pDelegate);

    /**
    @brief Remove the delegate from the delegates which receive IME messages.
    */
    void removeDelegate(IMEDelegate * pDelegate);

private:
    IMEDispatcher();
    
    class Impl;
    Impl * m_pImpl;
};

// end of input group
/// @}

NS_AX_END

#endif    // __AX_IME_DISPATCHER_H__
