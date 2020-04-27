#ifndef __HAL_CHIP_H__
#define __HAL_CHIP_H__

#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"

/* NOTE: stm32f030 has no 96-bit Unique ID, use internal flash instead,
   so the base address must be assigned correctly */
#ifndef UNIQUE_ID0
#define UNIQUE_ID0  0x0800FFF4  /* 12 bytes at the bottom of 64K flash */
#endif
#ifndef UNIQUE_ID1
#define UNIQUE_ID1  ((UNIQUE_ID0)+4)
#endif
#ifndef UNIQUE_ID2
#define UNIQUE_ID2  ((UNIQUE_ID0)+8)
#endif


#endif
