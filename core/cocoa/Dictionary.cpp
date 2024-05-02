/****************************************************************************
 Copyright (c) 2012 - 2013 cocos2d-x.org
 
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

#include "Dictionary.h"
#include "String.h"
#include "Integer.h"
#include "platform/FileUtils.h"

using namespace std;

NS_AX_BEGIN

// -----------------------------------------------------------------------
// DictElement

DictElement::DictElement(const char* pszKey, Object* pObject)
{
    AXAssert(pszKey && strlen(pszKey) > 0, "Invalid key value.");
    m_iKey = 0;
    const char* pStart = pszKey;
    
    int len = strlen(pszKey);
    if (len > MAX_KEY_LEN )
    {
        char* pEnd = (char*)&pszKey[len-1];
        pStart = pEnd - (MAX_KEY_LEN-1);
    }
    
    strcpy(m_szKey, pStart);
    
    m_pObject = pObject;
    memset(&hh, 0, sizeof(hh));
}

DictElement::DictElement(intptr_t iKey, Object* pObject)
{
    m_szKey[0] = '\0';
    m_iKey = iKey;
    m_pObject = pObject;
    memset(&hh, 0, sizeof(hh));
}

DictElement::~DictElement()
{

}

// -----------------------------------------------------------------------
// Dictionary

Dictionary::Dictionary()
: m_pElements(NULL)
, m_eDictType(kCCDictUnknown)
{

}

Dictionary::~Dictionary()
{
    removeAllObjects();
}

unsigned int Dictionary::count()
{
    return HASH_COUNT(m_pElements);
}

Array* Dictionary::allKeys()
{
    int iKeyCount = this->count();
    if (iKeyCount <= 0) return NULL;

    Array* pArray = Array::createWithCapacity(iKeyCount);

    DictElement *pElement, *tmp;
    if (m_eDictType == kCCDictStr)
    {
        HASH_ITER(hh, m_pElements, pElement, tmp) 
        {
            String* pOneKey = new String(pElement->m_szKey);
            pArray->addObject(pOneKey);
            AX_SAFE_RELEASE(pOneKey);
        }
    }
    else if (m_eDictType == kCCDictInt)
    {
        HASH_ITER(hh, m_pElements, pElement, tmp) 
        {
            Integer* pOneKey = new Integer(pElement->m_iKey);
            pArray->addObject(pOneKey);
            AX_SAFE_RELEASE(pOneKey);
        }
    }
    
    return pArray;
}

Array* Dictionary::allKeysForObject(Object* object)
{
    int iKeyCount = this->count();
    if (iKeyCount <= 0) return NULL;
    Array* pArray = Array::create();

    DictElement *pElement, *tmp;

    if (m_eDictType == kCCDictStr)
    {
        HASH_ITER(hh, m_pElements, pElement, tmp) 
        {
            if (object == pElement->m_pObject)
            {
                String* pOneKey = new String(pElement->m_szKey);
                pArray->addObject(pOneKey);
                AX_SAFE_RELEASE(pOneKey);
            }
        }
    }
    else if (m_eDictType == kCCDictInt)
    {
        HASH_ITER(hh, m_pElements, pElement, tmp) 
        {
            if (object == pElement->m_pObject)
            {
                Integer* pOneKey = new Integer(pElement->m_iKey);
                pArray->addObject(pOneKey);
                AX_SAFE_RELEASE(pOneKey);
            }
        }
    }
    return pArray;
}

Object* Dictionary::objectForKey(const std::string& key)
{
    // if dictionary wasn't initialized, return NULL directly.
    if (m_eDictType == kCCDictUnknown) return NULL;
    // Dictionary only supports one kind of key, string or integer.
    // This method uses string as key, therefore we should make sure that the key type of this Dictionary is string.
    AXAssert(m_eDictType == kCCDictStr, "this dictionary does not use string as key.");

    Object* pRetObject = NULL;
    DictElement *pElement = NULL;
    HASH_FIND_STR(m_pElements, key.c_str(), pElement);
    if (pElement != NULL)
    {
        pRetObject = pElement->m_pObject;
    }
    return pRetObject;
}

Object* Dictionary::objectForKey(intptr_t key)
{
    // if dictionary wasn't initialized, return NULL directly.
    if (m_eDictType == kCCDictUnknown) return NULL;
    // Dictionary only supports one kind of key, string or integer.
    // This method uses integer as key, therefore we should make sure that the key type of this Dictionary is integer.
    AXAssert(m_eDictType == kCCDictInt, "this dictionary does not use integer as key.");

    Object* pRetObject = NULL;
    DictElement *pElement = NULL;
    HASH_FIND_PTR(m_pElements, &key, pElement);
    if (pElement != NULL)
    {
        pRetObject = pElement->m_pObject;
    }
    return pRetObject;
}

const String* Dictionary::valueForKey(const std::string& key)
{
    String* pStr = dynamic_cast<String*>(objectForKey(key));
    if (pStr == NULL)
    {
        pStr = String::create("");
    }
    return pStr;
}

const String* Dictionary::valueForKey(intptr_t key)
{
    String* pStr = dynamic_cast<String*>(objectForKey(key));
    if (pStr == NULL)
    {
        pStr = String::create("");
    }
    return pStr;
}

void Dictionary::setObject(Object* pObject, const std::string& key)
{
    AXAssert(key.length() > 0 && pObject != NULL, "Invalid Argument!");
    if (m_eDictType == kCCDictUnknown)
    {
        m_eDictType = kCCDictStr;
    }

    AXAssert(m_eDictType == kCCDictStr, "this dictionary doesn't use string as key.");

    DictElement *pElement = NULL;
    HASH_FIND_STR(m_pElements, key.c_str(), pElement);
    if (pElement == NULL)
    {
        setObjectUnSafe(pObject, key);
    }
    else if (pElement->m_pObject != pObject)
    {
        Object* pTmpObj = pElement->m_pObject;
        pTmpObj->retain();
        removeObjectForElememt(pElement);
        setObjectUnSafe(pObject, key);
        pTmpObj->release();
    }
}

void Dictionary::setObject(Object* pObject, intptr_t key)
{
    AXAssert(pObject != NULL, "Invalid Argument!");
    if (m_eDictType == kCCDictUnknown)
    {
        m_eDictType = kCCDictInt;
    }

    AXAssert(m_eDictType == kCCDictInt, "this dictionary doesn't use integer as key.");

    DictElement *pElement = NULL;
    HASH_FIND_PTR(m_pElements, &key, pElement);
    if (pElement == NULL)
    {
        setObjectUnSafe(pObject, key);
    }
    else if (pElement->m_pObject != pObject)
    {
        Object* pTmpObj = pElement->m_pObject;
        pTmpObj->retain();
        removeObjectForElememt(pElement);
        setObjectUnSafe(pObject, key);
        pTmpObj->release();
    }

}

void Dictionary::removeObjectForKey(const std::string& key)
{
    if (m_eDictType == kCCDictUnknown)
    {
        return;
    }
    
    AXAssert(m_eDictType == kCCDictStr, "this dictionary doesn't use string as its key");
    AXAssert(key.length() > 0, "Invalid Argument!");
    DictElement *pElement = NULL;
    HASH_FIND_STR(m_pElements, key.c_str(), pElement);
    removeObjectForElememt(pElement);
}

void Dictionary::removeObjectForKey(intptr_t key)
{
    if (m_eDictType == kCCDictUnknown)
    {
        return;
    }
    
    AXAssert(m_eDictType == kCCDictInt, "this dictionary doesn't use integer as its key");
    DictElement *pElement = NULL;
    HASH_FIND_PTR(m_pElements, &key, pElement);
    removeObjectForElememt(pElement);
}

void Dictionary::setObjectUnSafe(Object* pObject, const std::string& key)
{
    pObject->retain();
    DictElement* pElement = new DictElement(key.c_str(), pObject);
    HASH_ADD_STR(m_pElements, m_szKey, pElement);
}

void Dictionary::setObjectUnSafe(Object* pObject, const intptr_t key)
{
    pObject->retain();
    DictElement* pElement = new DictElement(key, pObject);
    HASH_ADD_PTR(m_pElements, m_iKey, pElement);
}

void Dictionary::removeObjectsForKeys(Array* pKeyArray)
{
    Object* pObj = NULL;
    AXARRAY_FOREACH(pKeyArray, pObj)
    {
        String* pStr = (String*)pObj;
        removeObjectForKey(pStr->getCString());
    }
}

void Dictionary::removeObjectForElememt(DictElement* pElement)
{
    if (pElement != NULL)
    {
        HASH_DEL(m_pElements, pElement);
        pElement->m_pObject->release();
        AX_SAFE_DELETE(pElement);
    }
}

void Dictionary::removeAllObjects()
{
    DictElement *pElement, *tmp;
    HASH_ITER(hh, m_pElements, pElement, tmp) 
    {
        HASH_DEL(m_pElements, pElement);
        pElement->m_pObject->release();
        AX_SAFE_DELETE(pElement);

    }
}

Object* Dictionary::copyWithZone(Zone* pZone)
{
    AXAssert(pZone == NULL, "Dictionary should not be inherited.");
    Dictionary* pNewDict = new Dictionary();

    DictElement* pElement = NULL;
    Object* pTmpObj = NULL;

    if (m_eDictType == kCCDictInt)
    {
        CCDICT_FOREACH(this, pElement)
        {
            pTmpObj = pElement->getObject()->copy();
            pNewDict->setObject(pTmpObj, pElement->getIntKey());
            pTmpObj->release();
        }
    }
    else if (m_eDictType == kCCDictStr)
    {
        CCDICT_FOREACH(this, pElement)
        {
            pTmpObj = pElement->getObject()->copy();
            pNewDict->setObject(pTmpObj, pElement->getStrKey());
            pTmpObj->release();
        }
    }

    return pNewDict;
}

Object* Dictionary::randomObject()
{
    if (m_eDictType == kCCDictUnknown)
    {
        return NULL;
    }
    
    Object* key = allKeys()->randomObject();
    
    if (m_eDictType == kCCDictInt)
    {
        return objectForKey(((Integer*)key)->getValue());
    }
    else if (m_eDictType == kCCDictStr)
    {
        return objectForKey(((String*)key)->getCString());
    }
    else
    {
        return NULL;
    }
}

Dictionary* Dictionary::create()
{
    Dictionary* pRet = new Dictionary();
    if (pRet != NULL)
    {
        pRet->autorelease();
    }
    return pRet;
}

Dictionary* Dictionary::createWithDictionary(Dictionary* srcDict)
{
    Dictionary* pNewDict = (Dictionary*)srcDict->copy();
    pNewDict->autorelease();
    return pNewDict;
}

Dictionary* Dictionary::createWithContentsOfFileThreadSafe(const char *pFileName)
{
    return FileUtils::sharedFileUtils()->createCCDictionaryWithContentsOfFile(pFileName);
}

void Dictionary::acceptVisitor(DataVisitor &visitor)
{
    return visitor.visit(this);
}

Dictionary* Dictionary::createWithContentsOfFile(const char *pFileName)
{
    Dictionary* pRet = createWithContentsOfFileThreadSafe(pFileName);
    pRet->autorelease();
    return pRet;
}

bool Dictionary::writeToFile(const char *fullPath)
{
    return FileUtils::sharedFileUtils()->writeToFile(this, fullPath);
}


NS_AX_END
