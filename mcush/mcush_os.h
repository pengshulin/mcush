/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_OS_H__
#define __MCUSH_OS_H__

#define OS_FREERTOS  1
#define OS_RTX       2
#define OS_THREADX   3
#define OS_RTTHREAD  4

#ifndef MCUSH_OS
    #define MCUSH_OS  OS_FREERTOS
#endif

#if MCUSH_OS == OS_FREERTOS
    #include "mcush_os_freertos.h"
#elif MCUSH_OS == OS_RTX
    #include "mcush_os_rtx.h"
#elif MCUSH_OS == OS_THREADX
    #include "mcush_os_threadx.h"
#elif MCUSH_OS == OS_RTTHREAD
    #include "mcush_os_rtthread.h"
#else
    #error "MCUSH_OS not defined"
#endif


#endif
