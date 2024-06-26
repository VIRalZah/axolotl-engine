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

#include "base/Object.h"
#include "Bool.h"
#include "Integer.h"
#include "Float.h"
#include "Double.h"
#include "String.h"
#include "Array.h"
#include "Dictionary.h"
#include "Set.h"
#include "Value.h"
#include "support/StringUtils.h"

NS_AX_BEGIN

void DataVisitor::visit(const Bool *value)
{
    visitObject(value);
}

void DataVisitor::visit(const Integer *value)
{
    visitObject(value);
}

void DataVisitor::visit(const Float *value)
{
    visitObject(value);
}

void DataVisitor::visit(const Double *value)
{
    visitObject(value);
}

void DataVisitor::visit(const String *value)
{
    visitObject(value);
}

void DataVisitor::visit(const Array *value)
{
    visitObject(value);
}

void DataVisitor::visit(const Dictionary *value)
{
    visitObject(value);
}

void DataVisitor::visit(const Set *value)
{
    visitObject(value);
}

void DataVisitor::visit(const Value* value)
{
    visitObject(value);
}

// PrettyPrinter
PrettyPrinter::PrettyPrinter(int indentLevel/* = 0 */)
{
    setIndentLevel(indentLevel);
}

void PrettyPrinter::clear()
{
    _result.clear();
}

std::string PrettyPrinter::getResult()
{
    return _result;
}

void PrettyPrinter::visitObject(const Object *p)
{
    char buf[50] = {0};
    sprintf(buf, "%p", p);
    _result += buf;
}

void PrettyPrinter::visit(const Bool * p)
{
    char buf[50] = {0};
    sprintf(buf, "%s", p->getValue() ? "true" : "false");
   _result += buf;
}

void PrettyPrinter::visit(const Integer *p)
{
    char buf[50] = {0};
    sprintf(buf, "%d", p->getValue());
    _result += buf;
}

void PrettyPrinter::visit(const Float *p)
{
    char buf[50] = {0};
    sprintf(buf, "%f", p->getValue());
    _result += buf;
}

void PrettyPrinter::visit(const Double *p)
{
    char buf[50] = {0};
    sprintf(buf, "%lf", p->getValue());
    _result += buf;
}

void PrettyPrinter::visit(const String *p)
{
    _result += p->getCString();
}

void PrettyPrinter::visit(const Array *p)
{
    _result += "\n";
    _result += _indentStr;
    _result += "<array>\n";

    setIndentLevel(_indentLevel+1);
    Object* obj;
    int i = 0;
    char buf[50] = {0};
    AXARRAY_FOREACH(p, obj)
    {
        if (i > 0) {
            _result += "\n";
        }
        _result += StringUtils::format("%s%d: ", _indentStr.c_str(), i);
        PrettyPrinter v(_indentLevel);
        obj->acceptVisitor(v);
        _result += v.getResult();
        i++;
    }
    setIndentLevel(_indentLevel-1);
    
    _result += "\n";
    _result += _indentStr;
    _result += "</array>";
}

void PrettyPrinter::visit(const Dictionary *p)
{
    _result += "\n";
    _result += _indentStr;
    _result += "<dict>\n";
    
    setIndentLevel(_indentLevel+1);
    DictElement* element;
    bool bFirstElement = true;
    char buf[1000] = {0};
    CCDICT_FOREACH(p, element)
    {
        if (!bFirstElement) {
            _result += "\n";
        }
        sprintf(buf, "%s%s: ", _indentStr.c_str(),element->getStrKey());
        _result += buf;
        PrettyPrinter v(_indentLevel);
        element->getObject()->acceptVisitor(v);
        _result += v.getResult();
        bFirstElement = false;
    }
    setIndentLevel(_indentLevel-1);
    
    _result += "\n";
    _result += _indentStr;
    _result += "</dict>";
}

void PrettyPrinter::visit(const Set *p)
{
    _result += "\n";
    _result += _indentStr;
    _result += "<set>\n";
    
    setIndentLevel(_indentLevel+1);

    int i = 0;
    Set* tmp = const_cast<Set*>(p);
    SetIterator it = tmp->begin();

    for (; it != tmp->end(); ++it, ++i) {
        if (i > 0) {
            _result += "\n";
        }
        _result += _indentStr.c_str();
        PrettyPrinter v(_indentLevel);
        (*it)->acceptVisitor(v);
        _result += v.getResult();
    }
    setIndentLevel(_indentLevel-1);
    
    _result += "\n";
    _result += _indentStr;
    _result += "</set>\n";
}

void PrettyPrinter::visit(const Value* value)
{
    _result += value->stringValue();
}

void PrettyPrinter::setIndentLevel(int indentLevel)
{
    _indentLevel = indentLevel;
    _indentStr.clear();
    for (int i = 0; i < _indentLevel; ++i) {
        _indentStr += "\t";
    }
}

NS_AX_END
