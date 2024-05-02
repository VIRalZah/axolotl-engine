#ifndef __AXEXTENSIONEXPORT_H__
#define __AXEXTENSIONEXPORT_H__

#if defined(WIN32) && defined(_WINDOWS)
    #ifdef __MINGW32__
        #include <string.h>
    #endif

    #if defined(_USREXDLL)
        #define AX_EX_DLL     __declspec(dllexport)
    #else         /* use a DLL library */
        #define AX_EX_DLL     __declspec(dllimport)
    #endif


    /* Define NULL pointer value */
    #ifndef NULL
        #ifdef __cplusplus
            #define NULL    0
        #else
            #define NULL    ((void *)0)
        #endif
    #endif
#else
    #define AX_EX_DLL
#endif

#endif /* __AXEXTENSIONEXPORT_H__*/