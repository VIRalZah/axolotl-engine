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

#ifndef __PLATFORM_AXACCELEROMETER_BLACKBERRY_H__
#define __PLATFORM_AXACCELEROMETER_BLACKBERRY_H__

#include "platform/AccelerometerDelegate.h"

NS_AX_BEGIN

class AX_DLL Accelerometer
{
public:
    Accelerometer();
    ~Accelerometer();

    void setDelegate(AccelerometerDelegate* pDelegate);
    void setAccelerometerInterval(float interval) {AX_UNUSED_PARAM(interval);};
    void update(long sensorTimeStamp, double x, double y, double z);

private:
    AccelerometerDelegate* m_pAccelDelegate;
    CCAcceleration 			 m_accelerationValue;
    static int				 m_initialOrientationAngle;
};

NS_AX_END

#endif