/****************************************************************************
 Copyright (c) 2012 cocos2d-x.org
 Copyright (c) 2010 Sangwoo Im
 
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

#include "Sorting.h"

NS_AX_EXT_BEGIN

class CCSortedObject : public Object, public CCSortableObject
{
public:
    CCSortedObject() : objectID(0) {}
    virtual void setObjectID(unsigned int objectID) { this->objectID = objectID; }
    virtual unsigned int getObjectID() { return objectID; }
private:
    unsigned int objectID;
};

#if 0
static int _compareObject(const void * val1, const void * val2)
{
    CCSortableObject* operand1;
    CCSortableObject* operand2;

    operand1 = (CCSortableObject*)val1;
    operand2 = (CCSortableObject*)val2;

    if (operand1->getObjectID() > operand2->getObjectID())
    {
        return 1;
    } 
    else if (operand1->getObjectID() < operand2->getObjectID()) {
        return -1;
    }
    return 0;
}
#endif


void axArrayForObjectSorting::insertSortedObject(CCSortableObject* object)
{
    unsigned int idx;
    Object* pObj = dynamic_cast<Object*>(object);
    AXAssert(pObj, "Invalid parameter.");
    idx = this->indexOfSortedObject(object);
    
    this->insertObject(pObj, idx);
}

void axArrayForObjectSorting::removeSortedObject(CCSortableObject* object)
{
    if (this->count() == 0) {
        return;
    }
    unsigned int idx;
    CCSortableObject* foundObj;
    idx = this->indexOfSortedObject(object);
    
    if (idx < this->count() && idx != AX_INVALID_INDEX) {
        foundObj = dynamic_cast<CCSortableObject*>(this->objectAtIndex(idx));
        
        if(foundObj->getObjectID() == object->getObjectID()) {
            this->removeObjectAtIndex(idx);
        }
    }
}

void axArrayForObjectSorting::setObjectID_ofSortedObject(unsigned int tag, CCSortableObject* object)
{
    CCSortableObject* foundObj;
    unsigned int  idx;
    
    idx = this->indexOfSortedObject(object);
    if (idx < this->count() && idx != AX_INVALID_INDEX)
    {
        foundObj = dynamic_cast<CCSortableObject*>(this->objectAtIndex(idx));
        Object* pObj = dynamic_cast<Object*>(foundObj);
        pObj->retain();
        
        if(foundObj->getObjectID() == object->getObjectID()) {
            this->removeObjectAtIndex(idx);
            foundObj->setObjectID(tag);
            this->insertSortedObject(foundObj);
            pObj->release();
        } else {
            pObj->release();
        }
    }
}

CCSortableObject* axArrayForObjectSorting::objectWithObjectID(unsigned int tag)
{
    if (this->count() == 0) {
        return NULL;
    }
    
    unsigned int  idx;
    CCSortableObject* foundObj;
    
    foundObj = new CCSortedObject();
    foundObj->setObjectID(tag);
    
    idx      = this->indexOfSortedObject(foundObj);
    
    ((CCSortedObject*)foundObj)->release();
    foundObj = NULL;
    
    if (idx < this->count() && idx != AX_INVALID_INDEX)
    {
        foundObj = dynamic_cast<CCSortableObject*>(this->objectAtIndex(idx));
        if (foundObj->getObjectID() != tag) {
            foundObj = NULL;
        }
    }
    
    return foundObj;
}

unsigned int axArrayForObjectSorting::indexOfSortedObject(CCSortableObject* object)
{
    unsigned int  idx = 0;
    if (object)
    {
 //       Object* pObj = (Object*)bsearch((Object*)&object, data->arr, data->num, sizeof(Object*), _compareObject);
        // FIXME: need to use binary search to improve performance
        Object* pObj = NULL;
        unsigned int uPrevObjectID = 0;
        unsigned int uOfSortObjectID = object->getObjectID();

        AXARRAY_FOREACH(this, pObj)
        {
            CCSortableObject* pSortableObj = dynamic_cast<CCSortableObject*>(pObj);
            unsigned int uCurObjectID = pSortableObj->getObjectID();
            if (  (uOfSortObjectID == uCurObjectID)
                  || (uOfSortObjectID >= uPrevObjectID && uOfSortObjectID < uCurObjectID))
            {
                break;
            }
            
            uPrevObjectID = uCurObjectID;
            idx++;
        }
    }
    else
    {
        idx = AX_INVALID_INDEX;
    }
    return idx;
}

NS_AX_EXT_END
