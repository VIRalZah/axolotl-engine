#include "platform/Device.h"
#include "StdC.h"

NS_AX_BEGIN

int Device::getDPI()
{
    static int dpi = -1;
    if (dpi == -1)
    {
        HDC hScreenDC = GetDC( NULL );
        int PixelsX = GetDeviceCaps( hScreenDC, HORZRES );
        int MMX = GetDeviceCaps( hScreenDC, HORZSIZE );
        ReleaseDC( NULL, hScreenDC );   
        dpi = 254.0f*PixelsX/MMX/10;
    }
    return dpi;
}

NS_AX_END