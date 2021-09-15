#ifndef __HAL_CHIP_H__
#define __HAL_CHIP_H__

#include "stm32g0xx.h"
#include "stm32g0xx_hal.h"

#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_iwdg.h"
#include "stm32g0xx_ll_crc.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_rng.h"
#include "stm32g0xx_ll_spi.h"
#include "stm32g0xx_ll_rtc.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_usart.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_adc.h"


/* NOTE: stm32g030 has no 96-bit Unique ID, use internal flash instead,
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
