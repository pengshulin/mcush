/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_OS_H__
#define __MCUSH_OS_H__

#define OS_FREERTOS  1
#define OS_RTX       2

#ifndef MCUSH_OS
    #define MCUSH_OS  OS_FREERTOS
#endif

#if MCUSH_OS == OS_FREERTOS
    #include "mcush_os_freertos.h"
#elif MCUSH_OS == OS_RTX
    #include "mcush_os_rtx.h"
#else
    #error "MCUSH_OS not defined"
#endif


#endif
