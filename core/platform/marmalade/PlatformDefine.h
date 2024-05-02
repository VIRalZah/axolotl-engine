#ifndef __AXPLATFORMDEFINE_H__
#define __AXPLATFORMDEFINE_H__

/*
#if defined(_USRDLL)
    #define AX_DLL     __declspec(dllexport)
#else         // use a DLL library
    #define AX_DLL     __declspec(dllimport)
#endif
*/
// shared library declartor
#define AX_DLL 

#include <assert.h>
#define AX_ASSERT(cond)    assert(cond)
#define AX_UNUSED_PARAM(unusedparam) (void)unusedparam

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif



#endif /* __AXPLATFORMDEFINE_H__*/
