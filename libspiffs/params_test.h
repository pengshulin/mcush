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
#define SPIFFS_DBG(...) shell_printf(__VA_ARGS__)

#endif
