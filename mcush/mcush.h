/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_H__
#define __MCUSH_H__
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "mcush_freertos_api.h"
#include "shell.h"
#include "shell_str.h"
#include "mcush_ver.h"
#include "mcush_opt.h"
#include "mcush_vfs.h"
#include "mcush_lib.h"
#include "mcush_base64.h"



/* task priority
 * 6 -- highest (timer task)
 * 5 -- reserved
 * 4 -- reserved
 * 3 -- shell task default
 * 2 -- reserved
 * 1 -- reserved
 * 0 -- lowest (idle task)
 */
#ifndef MCUSH_PRIORITY
    #define MCUSH_PRIORITY  (3)  
#endif

#ifndef MCUSH_STACK_SIZE
    #define MCUSH_STACK_SIZE  (2*1024)
#endif

#ifndef MCUSH_QUEUE_SIZE
    #define MCUSH_QUEUE_SIZE  (64)
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

#ifndef MCUSH_SPIFFS
    #define MCUSH_SPIFFS  0
#endif

#ifndef MCUSH_FATFS
    #define MCUSH_FATFS  0
#endif


#if !MCUSH_VFS
    #ifdef MCUSH_ROMFS
        #undef MCUSH_ROMFS
        #define MCUSH_ROMFS  0
    #endif
    #ifdef MCUSH_SPIFFS
        #undef MCUSH_SPIFFS
        #define MCUSH_SPIFFS  0
    #endif
    #ifdef MCUSH_FATFS
        #undef MCUSH_FATFS
        #define MCUSH_FATFS  0
    #endif
#endif




extern void mcush_init(void);
extern void mcush_start(void);
extern TaskHandle_t task_mcush;


#ifndef NULL
    #define NULL  (void*)0
#endif

    
    
extern void _halt_with_message(const char *message);
extern void _halt(void);
#if DEBUG
    #define halt(message)  _halt_with_message(message)
#else
    #define halt(message)  _halt() 
#endif




#endif



