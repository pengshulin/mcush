/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_H__
#define __MCUSH_H__
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "compiler.h"
#include "hal.h"
#include "shell.h"
#include "mcush_os.h"
#include "shell_str.h"
#include "mcush_ver.h"
#include "mcush_opt.h"
#include "mcush_vfs.h"
#include "mcush_base64.h"
#include "mcush_lib.h"
#include "mcush_lib_crc.h"
#include "mcush_lib_fs.h"


#ifdef __cplusplus
extern "C" {
#endif



/* task priority
 * OS_PRIORITY_HIGHEST --- highest (timer task)
 * ...
 * OS_PRIORITY_HIGHER  --- reserved
 * OS_PRIORITY_HIGH    --- reserved
 * OS_PRIORITY_NORMAL  --- shell task default
 * OS_PRIORITY_LOW     --- reserved
 * OS_PRIORITY_LOWER   --- reserved
 * ...
 * OS_PRIORITY_LOWEST  --- lowest (idle task)
 */

#ifndef MCUSH_PRIORITY
    #define MCUSH_PRIORITY  (OS_PRIORITY_NORMAL)
#endif

#ifndef MCUSH_STACK_SIZE
    #define MCUSH_STACK_SIZE  (4*1024)
#endif

#ifndef MCUSH_FREERTOS_PEEK_API
    #define MCUSH_FREERTOS_PEEK_API   1
#endif

#if ! MCUSH_FREERTOS_PEEK_API
    #define mcushTaskAddToRegistered( a ) 
    #ifdef USE_CMD_SYSTEM
        #undef USE_CMD_SYSTEM
    #endif
    #define USE_CMD_SYSTEM  0
#endif

#ifndef MCUSH_VFS
    #define MCUSH_VFS  0
#endif

#ifndef MCUSH_ROMFS
    #define MCUSH_ROMFS  0
#endif

#ifndef MCUSH_FCFS
    #define MCUSH_FCFS  0
#endif

#ifndef MCUSH_SPIFFS
    #define MCUSH_SPIFFS  0
#endif

#ifndef MCUSH_FATFS
    #define MCUSH_FATFS  0
#endif

#ifndef MCUSH_POSIX
    #if MCUSH_OS == OS_POSIX
        #define MCUSH_POSIX  1
    #else
        #define MCUSH_POSIX  0
    #endif
#endif


#if !MCUSH_VFS
    #ifdef MCUSH_ROMFS
        #undef MCUSH_ROMFS
        #define MCUSH_ROMFS  0
    #endif
    #ifdef MCUSH_FCFS
        #undef MCUSH_FCFS
        #define MCUSH_RCFS  0
    #endif
    #ifdef MCUSH_SPIFFS
        #undef MCUSH_SPIFFS
        #define MCUSH_SPIFFS  0
    #endif
    #ifdef MCUSH_FATFS
        #undef MCUSH_FATFS
        #define MCUSH_FATFS  0
    #endif
    #ifdef MCUSH_POSIX
        #undef MCUSH_POSIX
        #define MCUSH_POSIX  0
    #endif
#endif

#ifndef TASK_IDLE_PRIORITY
    #define TASK_IDLE_PRIORITY  OS_PRIORITY_IDLE
#endif



#ifndef USE_CMD_HELP
    #define USE_CMD_HELP  1
#endif
#ifndef USE_CMD_SCPI_IDN
    #define USE_CMD_SCPI_IDN  1
#endif
#ifndef USE_CMD_SCPI_RST
    #define USE_CMD_SCPI_RST  1
#endif
#ifndef USE_CMD_REBOOT
    #define USE_CMD_REBOOT  1
#endif
#ifndef USE_CMD_UPGRADE
    #define USE_CMD_UPGRADE  0
#endif
#ifndef USE_CMD_GPIO
    #define USE_CMD_GPIO  1
#endif
#ifndef USE_CMD_LED
    #define USE_CMD_LED  1
#endif
#ifndef USE_CMD_DUMP
    #define USE_CMD_DUMP  1
#endif
#ifndef USE_CMD_WRITE
    #define USE_CMD_WRITE  1
#endif
#ifndef USE_CMD_MFILL
    #define USE_CMD_MFILL  0
#endif
#ifndef USE_CMD_DELAY
    #define USE_CMD_DELAY  1
#endif
#ifndef USE_CMD_ECHO
    #define USE_CMD_ECHO  0  /* to be obseleted */
#endif
#ifndef USE_CMD_MKBUF
    #define USE_CMD_MKBUF  0
#endif
#ifndef USE_CMD_UPTIME
    #define USE_CMD_UPTIME  1
#endif

#ifndef USE_CMD_SYSTEM
    #define USE_CMD_SYSTEM  1
#endif
#if USE_CMD_SYSTEM
    #ifndef USE_CMD_SYSTEM_QUEUE
        #define USE_CMD_SYSTEM_QUEUE  1
    #endif
    #ifndef USE_CMD_SYSTEM_KERNEL
        #define USE_CMD_SYSTEM_KERNEL  1
    #endif
    #ifndef USE_CMD_SYSTEM_HEAP
        #define USE_CMD_SYSTEM_HEAP  0
    #endif
    #ifndef USE_CMD_SYSTEM_STACK
        #define USE_CMD_SYSTEM_STACK  0
    #endif
    #ifndef USE_CMD_SYSTEM_IDLE
        #define USE_CMD_SYSTEM_IDLE  1
    #endif
    #ifndef USE_CMD_SYSTEM_VFS
        #define USE_CMD_SYSTEM_VFS  0
    #endif
#endif

#ifndef USE_CMD_MAPI
    #define USE_CMD_MAPI  1
#endif
#if USE_CMD_MAPI
    #ifndef USE_CMD_MAPI_INFO
        #define USE_CMD_MAPI_INFO  0
    #endif
    #ifndef USE_CMD_MAPI_REALLOC
        #define USE_CMD_MAPI_REALLOC  0
    #endif
    #ifndef MAPI_TEST_MALLOC_SIZE
        #define MAPI_TEST_MALLOC_SIZE  65536
    #endif
#endif

#ifndef USE_CMD_BEEP
    #define USE_CMD_BEEP  0
#endif
#ifndef USE_CMD_SGPIO
    #define USE_CMD_SGPIO  0
#endif
#ifndef USE_CMD_POWER
    #define USE_CMD_POWER  0
#endif
#ifndef USE_CMD_I2C
    #define USE_CMD_I2C  1
    #define SUPPORT_I2C  1
#endif
#ifndef USE_CMD_SPI
    #define USE_CMD_SPI  1
    #define SUPPORT_SPI  1
#endif
#ifndef USE_CMD_PULSE
    #define USE_CMD_PULSE  0
    #define SUPPORT_PULSE  0
#endif
#ifndef USE_CMD_DS1W
    #define USE_CMD_DS1W  0
    #define SUPPORT_DS1W  0
#endif
#ifndef USE_CMD_PWM
    #define USE_CMD_PWM  0
#endif
#ifndef USE_CMD_RTC
    #define USE_CMD_RTC  0
#endif
#ifndef USE_CMD_CAN
    #if HAL_CAN
        #define USE_CMD_CAN  1
    #else
        #define USE_CMD_CAN  0
    #endif
#endif

#ifndef USE_CMD_LOOP
    #define USE_CMD_LOOP  1
#endif

#ifndef USE_CMD_TEST
    #define USE_CMD_TEST  1
#endif


#if MCUSH_FCFS
    #ifndef USE_CMD_FCFS
        #define USE_CMD_FCFS  1
    #endif
#else
    #ifdef USE_CMD_FCFS
        #undef USE_CMD_FCFS
    #endif
    #define USE_CMD_FCFS  0
#endif

#if MCUSH_SPIFFS
    #ifndef USE_CMD_SPIFFS
        #define USE_CMD_SPIFFS  1
    #endif
#else
    #ifdef USE_CMD_SPIFFS
        #undef USE_CMD_SPIFFS
    #endif
    #define USE_CMD_SPIFFS  0
#endif

#if MCUSH_FATFS
    #ifndef USE_CMD_FATFS
        #define USE_CMD_FATFS  1
    #endif
#else
    #ifdef USE_CMD_FATFS
        #undef USE_CMD_FATFS
    #endif
    #define USE_CMD_FATFS  0
#endif


#if MCUSH_VFS
    #ifndef USE_CMD_LS
        #define USE_CMD_LS  1
    #endif
    #ifndef USE_CMD_CAT
        #define USE_CMD_CAT  1
    #endif
    #ifndef USE_CMD_CP
        #define USE_CMD_CP  1
    #endif
    #ifndef USE_CMD_RM
        #define USE_CMD_RM  1
    #endif
    #ifndef USE_CMD_RENAME
        #define USE_CMD_RENAME  1
    #endif
    #ifndef USE_CMD_LOAD
        #if MCUSH_FCFS
            #define USE_CMD_LOAD  1
        #else
            #define USE_CMD_LOAD  0
        #endif
    #endif
    #ifndef USE_CMD_STOP
        #define USE_CMD_STOP  USE_CMD_LOAD
    #endif
    #ifndef USE_CMD_CRC
        #define USE_CMD_CRC  0
    #endif
#else
    #ifdef USE_CMD_CAT
        #undef USE_CMD_CAT
    #endif
    #define USE_CMD_CAT  0
    #ifdef USE_CMD_RM
        #undef USE_CMD_RM
    #endif
    #define USE_CMD_RM  0
    #ifdef USE_CMD_RENAME
        #undef USE_CMD_RENAME
    #endif
    #define USE_CMD_RENAME  0
    #ifdef USE_CMD_CP
        #undef USE_CMD_CP
    #endif
    #define USE_CMD_CP  0
    #ifdef USE_CMD_LS
        #undef USE_CMD_LS
    #endif
    #define USE_CMD_LS  0
    #ifdef USE_CMD_LOAD
        #undef USE_CMD_LOAD
    #endif
    #define USE_CMD_LOAD  0
    #ifdef USE_CMD_CRC
        #undef USE_CMD_CRC
    #endif
    #define USE_CMD_CRC  0

    #undef USE_CMD_UPGRADE
    #define USE_CMD_UPGRADE  0
#endif

#if USE_CMD_CAT
    #ifndef USE_CMD_CAT_WRITE
        #if MCUSH_SPIFFS || MCUSH_FATFS || MCUSH_POSIX
            #define USE_CMD_CAT_WRITE  1
        #else
            #define USE_CMD_CAT_WRITE  0
        #endif
    #endif
    #ifndef USE_CMD_CAT_B64
        #define USE_CMD_CAT_B64  1
    #endif
#endif

#include "hal_core.h"
#include "hal_extra.h"


#if MCUSH_OS != OS_NONE
void mcush_init(void);
void mcush_start(void);
extern os_task_handle_t task_mcush;
#endif

extern const char build_signature[];


#ifndef NULL
    #define NULL  (void*)0
#endif


#ifdef __cplusplus
}
#endif

#endif
