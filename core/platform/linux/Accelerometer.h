/*
 * Accelerometer.h
 *
 *  Created on: Aug 9, 2011
 *      Author: laschweinski
 */

#ifndef AXACCELEROMETER_H_
#define AXACCELEROMETER_H_

#include "platform/AccelerometerDelegate.h"

namespace axolotl {

class Accelerometer
{
public:
    Accelerometer(){}
    ~Accelerometer(){}

    static Accelerometer* sharedAccelerometer() { return NULL; };

    void removeDelegate(AccelerometerDelegate* pDelegate) {AX_UNUSED_PARAM(pDelegate);};
    void addDelegate(AccelerometerDelegate* pDelegate) {AX_UNUSED_PARAM(pDelegate);};
    void setDelegate(AccelerometerDelegate* pDelegate) {AX_UNUSED_PARAM(pDelegate);};
    void setAccelerometerInterval(float interval) {AX_UNUSED_PARAM(interval);};
};

}//namespace   cocos2d

#endif /* AXACCELEROMETER_H_ */
