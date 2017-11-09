#ifndef __PARMS_TEST_H__
#define __PARMS_TEST_H__
#include <stdint.h>
typedef int8_t s8_t;
typedef uint8_t u8_t;
typedef int16_t s16_t;
typedef uint16_t u16_t;
typedef int32_t s32_t;
typedef uint32_t u32_t;
#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"

//#define SPIFFS_DBG(...) {if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED) shell_printf(__VA_ARGS__);}

#define SPIFFS_TEST_VISUALISATION    0
//#define SPIFFS_TEST_VISUALISATION    1
//#define spiffs_printf(...) {if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED) shell_printf(__VA_ARGS__);}

#endif
