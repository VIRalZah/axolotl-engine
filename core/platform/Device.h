#ifndef __AXDEVICE_H__
#define __AXDEVICE_H__

#include "PlatformMacros.h"

NS_AX_BEGIN
/**
 @js NA
 @lua NA
 */
class AX_DLL Device
{
private:
    Device();
public:
    /**
     *  Gets the DPI of device
     *  @return The DPI of device.
     */
    static int getDPI();
};


NS_AX_END

#endif /* __AXDEVICE_H__ */
