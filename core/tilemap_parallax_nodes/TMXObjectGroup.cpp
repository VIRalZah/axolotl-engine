/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2010      Neophit
Copyright (c) 2010      Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

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
#include "TMXObjectGroup.h"
#include "ccMacros.h"

NS_AX_BEGIN

//implementation TMXObjectGroup

TMXObjectGroup::TMXObjectGroup()
    :m_tPositionOffset(Point::ZERO)
    ,m_sGroupName("")        
{
    m_pObjects = Array::create();
    m_pObjects->retain();
    m_pProperties = new Dictionary();
}
TMXObjectGroup::~TMXObjectGroup()
{
    AXLOGINFO( "cocos2d: deallocing.");
    AX_SAFE_RELEASE(m_pObjects);
    AX_SAFE_RELEASE(m_pProperties);
}
Dictionary* TMXObjectGroup::objectNamed(const char *objectName)
{
    if (m_pObjects && m_pObjects->count() > 0)
    {
        Object* pObj = NULL;
        AXARRAY_FOREACH(m_pObjects, pObj)
        {
            Dictionary* pDict = (Dictionary*)pObj;
            String *name = (String*)pDict->objectForKey("name");
            if (name && name->m_sString == objectName)
            {
                return pDict;
            }
        }
    }
    // object not found
    return NULL;    
}
String* TMXObjectGroup::propertyNamed(const char* propertyName)
{
    return (String*)m_pProperties->objectForKey(propertyName);
}

Dictionary* TMXObjectGroup::getProperties()
{ 
    return m_pProperties;
}
void TMXObjectGroup::setProperties(Dictionary * properties)
{
    AX_SAFE_RETAIN(properties);
    AX_SAFE_RELEASE(m_pProperties);
    m_pProperties = properties;
}
Array* TMXObjectGroup::getObjects()
{
    return m_pObjects;
}
void TMXObjectGroup::setObjects(Array* objects)
{
    AX_SAFE_RETAIN(objects);
    AX_SAFE_RELEASE(m_pObjects);
    m_pObjects = objects;
}

NS_AX_END
