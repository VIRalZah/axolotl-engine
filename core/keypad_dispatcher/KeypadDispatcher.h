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

#ifndef __AXKEYPAD_DISPATCHER_H__
#define __AXKEYPAD_DISPATCHER_H__

#include "KeypadDelegate.h"
#include "cocoa/Array.h"

NS_AX_BEGIN

/**
 * @addtogroup input
 * @{
 */

typedef enum {
    // the back key clicked msg
    kTypeBackClicked = 1,
    kTypeMenuClicked,
} ccKeypadMSGType;

struct _axCArray;
/**
@class KeypadDispatcher
@brief Dispatch the keypad message from the phone
@js NA
@lua NA
*/
class AX_DLL KeypadDispatcher : public Object
{
public:
    KeypadDispatcher();
    ~KeypadDispatcher();

    /**
    @brief add delegate to concern keypad msg
    */
    void addDelegate(KeypadDelegate* pDelegate);

    /**
    @brief remove the delegate from the delegates who concern keypad msg
    */
    void removeDelegate(KeypadDelegate* pDelegate);

    /**
    @brief force add the delegate
    */
    void forceAddDelegate(KeypadDelegate* pDelegate);

    /**
    @brief force remove the delegate
    */
    void forceRemoveDelegate(KeypadDelegate* pDelegate);

    /**
    @brief dispatch the key pad msg
    */
    bool dispatchKeypadMSG(ccKeypadMSGType nMsgType);

protected:

    Array* m_pDelegates;
    bool m_bLocked;
    bool m_bToAdd;
    bool m_bToRemove;

    struct _axCArray *m_pHandlersToAdd;
    struct _axCArray *m_pHandlersToRemove;
};

// end of input group
/// @} 

NS_AX_END

#endif //__AXKEYPAD_DISPATCHER_H__
