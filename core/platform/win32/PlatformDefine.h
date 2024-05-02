#ifndef __AXPLATFORMDEFINE_H__
#define __AXPLATFORMDEFINE_H__

#ifdef __MINGW32__
#include <string.h>
#endif

#if defined(_USRDLL)
    #define AX_DLL     __declspec(dllexport)
#else         /* use a DLL library */
    #define AX_DLL     __declspec(dllimport)
#endif

#include <assert.h>

#if AX_DISABLE_ASSERT > 0
#define AX_ASSERT(cond)
#else
#define AX_ASSERT(cond)    assert(cond)
#endif
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
