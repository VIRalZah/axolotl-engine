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

#ifndef __AX_VALUE_H__
#define __AX_VALUE_H__

#include "Object.h"

NS_AX_BEGIN

class AX_DLL Value : public Object
{
public:
    Value();
    Value(const Value& other);
    Value(const int& v);
    Value(const unsigned int& v);
    Value(const float& v);
    Value(const double& v);
    Value(const bool& v);
    Value(const char* v);
    Value(const std::string& v);

    virtual ~Value();
    
    Value& operator=(const Value& other);
    Value& operator=(const int& v);
    Value& operator=(const unsigned int& v);
    Value& operator=(const float& v);
    Value& operator=(const double& v);
    Value& operator=(const bool& v);
    Value& operator=(const char* v);
    Value& operator=(const std::string& v);

    virtual void reset();

    int intValue() const;
    unsigned int uintValue() const;
    float floatValue() const;
    double doubleValue() const;
    bool boolValue() const;
    std::string stringValue() const;

    virtual void acceptVisitor(DataVisitor& visitor);

    enum Type
    {
        UNKNOWN = 0,
        INTEGER,
        UNSIGNED,
        FLOAT,
        DOUBLE,
        BOOL,
        STRING
    };

    const Type& getType() const { return _type; }
protected:
    Type _type;
    std::string _string;
};

NS_AX_END

#endif // __AX_VALUE_H__
