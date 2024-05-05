/****************************************************************************
Copyright (c) 2010 zahann.ru

http://www.zahann.ru

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

#include "Touch.h"
#include "base/Director.h"

NS_AX_BEGIN

Touch::Touch()
    : _id(0),
    _startPointCaptured(false)
{
}

Touch::~Touch()
{
}

Vec2 Touch::getLocation() const
{
    return Director::sharedDirector()->convertToGL(_point);
}

Vec2 Touch::getPreviousLocation() const
{
    return Director::sharedDirector()->convertToGL(_prevPoint);
}

Vec2 Touch::getStartLocation() const
{
    return Director::sharedDirector()->convertToGL(_startPoint);
}

Vec2 Touch::getDelta() const
{
    return getLocation() - getPreviousLocation();
}

Vec2 Touch::getLocationInView() const 
{ 
    return _point; 
}

Vec2 Touch::getPreviousLocationInView() const 
{ 
    return _prevPoint; 
}

Vec2 Touch::getStartLocationInView() const 
{ 
    return _startPoint; 
}

NS_AX_END