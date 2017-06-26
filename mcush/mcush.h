/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_H__
#define __MCUSH_H__
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if defined(MCUSH_NON_OS)
  #include "mcush_event.h"
#else
  #include "FreeRTOS.h"
  #include "task.h"
  #include "queue.h"
  #include "mcush_freertos_api.h"
#endif
#include "hal.h"
#include "shell.h"
#include "mcush_ver.h"
#include "mcush_opt.h"
#include "mcush_vfs.h"
#include "mcush_lib.h"


#if defined(MCUSH_NON_OS)

#ifndef configTICK_RATE_HZ
    #define configTICK_RATE_HZ  250
#endif

void task_mcush_entry(void);
unsigned int get_sys_tick_count(void);

#else

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
#else
    #define mcushTaskAddToRegistered( a ) 
    #ifdef USE_CMD_SYSTEM
        #undef USE_CMD_SYSTEM
    #endif
    #define USE_CMD_SYSTEM  0
#endif

#ifndef MCUSH_VFS
    #define MCUSH_VFS  0
#endif




extern void mcush_init(void);
extern void mcush_start(void);
extern TaskHandle_t task_mcush;
#endif



#ifdef DEBUG
    extern void halt(const char *message);
#else
    #define halt(message)  while(1)
#endif





#endif



