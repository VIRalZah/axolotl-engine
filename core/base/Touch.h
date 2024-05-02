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

#ifndef __AX_TOUCH_H__
#define __AX_TOUCH_H__

#include "cocoa/Object.h"
#include "cocoa/Geometry.h"

NS_AX_BEGIN

class AX_DLL Touch : public Object
{
public:
    Touch();
    virtual ~Touch();

    Point getLocation() const;
    Point getPreviousLocation() const;
    Point getStartLocation() const;
    Point getDelta() const;

    Point getLocationInView() const;
    Point getPreviousLocationInView() const;
    Point getStartLocationInView() const;
    
    void setTouchInfo(int id, float x, float y)
    {
        _id = id;
        _prevPoint = _point;
        _point.x   = x;
        _point.y   = y;
        if (!_startPointCaptured)
        {
            _startPoint = _point;
            _startPointCaptured = true;
        }
    }

    int getID() const
    {
        return _id;
    }
private:
    int _id;

    bool _startPointCaptured;
    Point _startPoint;
    Point _point;
    Point _prevPoint;
};

NS_AX_END

#endif // __AX_TOUCH_H__