#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

#define HAL_LED_REV
#define HAL_LED_NUM   3
#define HAL_LED_PORTS  { 4, 4, 4 } 
#define HAL_LED_PINS  { 5, 4, 3 }

#ifndef HAL_RTC
  #define HAL_RTC  1
  #define USE_CMD_RTC  1
#endif

#include <time.h>
int  hal_rtc_get( struct tm *t );
int  hal_rtc_set( struct tm *t );
int  hal_rtc_get_tick( uint64_t *tick );
void hal_rtc_init(void);

#define HAL_RNG   1
void hal_rng_init(void);
uint32_t hal_rng_get(void);

#ifndef USE_CMD_SGPIO
#define USE_CMD_SGPIO  0
#endif

#include "mcush_vfs.h"

#ifndef MCUSH_SPIFFS
#define MCUSH_SPIFFS  0
#endif

#include "hal_platform_spiffs.h"

//#ifdef HAL_ETH
//#include "hal_eth.h"
//#endif

#ifdef HAL_MOTION
#include "hal_motion.h"
#endif

#endif
