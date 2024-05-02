#include "platform/Device.h"
#include "jni/DPIJni.h"

NS_AX_BEGIN

int Device::getDPI()
{
    static int dpi = -1;
    if (dpi == -1)
    {
        dpi = (int)getDPIJNI();
    }
    return dpi;
}

NS_AX_END
