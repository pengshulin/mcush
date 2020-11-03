/* compiler definations */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __COMPILER_H__
#define __COMPILER_H__
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/* KEIL MDK-ARM */
#if defined( __ARMCC_VERSION )
    #if defined( __CC_ARM )
        /* older version compiler */
        #define __byte_aligned  __aligned(1)
    #else
        /* latest version compiler */
        #define __byte_aligned  __attribute__((aligned(1)))
    #endif
    #ifndef __weak
        #define __weak  __attribute__((weak)) 
    #endif
    #define __fallthrough  __attribute__((fallthrough));
    #define __signature  __attribute__((section(".signature"))) __attribute__((used))
    #define __init_script  __attribute__((section(".init_script"))) __attribute__((used))

    #define __no_return __attribute__((__noreturn__))

    #if defined(__ARM_BIG_ENDIAN)
        #define __big_endian  1
        #define __little_endian  0
    #else
        #define __big_endian  0
        #define __little_endian  1
    #endif

/* GCC or compatiable */
#elif defined( __GNUC__ )
    #ifndef __weak
        #define __weak  __attribute__((weak)) 
    #endif
    #define __byte_aligned  __attribute__((aligned(1)))
    #define __fallthrough
    //#define __fallthrough  __attribute__((fallthrough));
    #define __signature  __attribute__((section(".signature"))) __attribute__((used))
    #define __init_script  __attribute__((section(".init_script"))) __attribute__((used))
    
    #define __no_return __attribute__((noreturn))
    
    #if defined(__NEWLIB__)    
        #define SUPPORT_NEWLIB   1
    #endif

    #if __BYTE_ORDER___ == __ORDER_BIG_ENDIAN__
        #define __big_endian  1
        #define __little_endian  0
    #else
        #define __big_endian  0
        #define __little_endian  1
    #endif


#endif


#ifdef __cplusplus
}
#endif

#endif
