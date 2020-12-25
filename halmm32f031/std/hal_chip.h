#ifndef __HAL_CHIP_H__
#define __HAL_CHIP_H__

#include "HAL_rcc.h"
#include "HAL_bkp.h"
#include "HAL_gpio.h"
#include "HAL_nvic.h"
#include "HAL_iwdg.h"
#include "HAL_uart.h"
#include "HAL_flash.h"
#include "HAL_tim.h"

#ifdef USE_FCFS_UID
    #define UNIQUE_ID0  0x0800FC04
#else
    #define UNIQUE_ID0  0x1FFFF7E8
#endif

#endif
