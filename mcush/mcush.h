/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _MCUSH_H_
#define _MCUSH_H_
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "hal.h"
#include "shell.h"
#include "mcush_opt.h"
#include "mcush_freertos_api.h"


#define MCUSH_VERSION_MAJOR    1
#define MCUSH_VERSION_MINOR    0
#define MCUSH_VERSION_STRING   "1.0"


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
    #define MCUSH_STACK_SIZE  (2*1024 / sizeof(portSTACK_TYPE))
#endif

#ifndef MCUSH_QUEUE_SIZE
    #define MCUSH_QUEUE_SIZE  (64)
#endif



#ifdef DEBUG
    extern void halt(const char *message);
#else
    #define halt(message)  while(1)
#endif



typedef struct {
    uint8_t type;
    uint8_t val;
} mcush_message_t;

#define MCUSH_MESSAGE_TYPE_CHAR   0
#define MCUSH_MESSAGE_TYPE_RESET  1
#define MCUSH_MESSAGE_TYPE_ECHO   2
#define MCUSH_MESSAGE_TYPE_BAUD   3


extern void mcush_init(void);
extern void mcush_start(void);
extern TaskHandle_t task_mcush;

#endif



