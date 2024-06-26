/****************************************************************************
Copyright (c) 2013 cocos2d-x.org

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

#ifndef __AX_FRAMEWORK_COMCONTAINER_H__
#define __AX_FRAMEWORK_COMCONTAINER_H__

#include "base/Dictionary.h"

NS_AX_BEGIN

class Component;
class Node;
/**
 *  @js NA
 *  @lua NA
 */
class AX_DLL ComponentContainer
{
protected:
    ComponentContainer(Node *pNode);
    
public:
    virtual ~ComponentContainer(void);
    virtual Component* get(const char *pName) const;
    virtual bool add(Component *pCom);
    virtual bool remove(const char *pName);
    virtual bool remove(Component *pCom);
    virtual void removeAll();
    virtual void visit(float fDelta);
public:
    bool isEmpty() const;
    
private:
    void alloc(void);
    
private:
    Dictionary *m_pComponents;        ///< Dictionary of components
    Node *m_pOwner;
    
    friend class Node;
};

NS_AX_END

#endif  // __FUNDATION__CCCOMPONENT_H__
