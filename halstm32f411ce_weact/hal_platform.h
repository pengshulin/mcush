#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

#define HAL_LED_REV
#define HAL_LED_NUM   1
#define HAL_LED_PORTS  { 2 } 
#define HAL_LED_PINS  { 13 }

#define HAL_RNG   0
void hal_rng_init(void);
uint32_t hal_rng_get(void);

#ifndef USE_CMD_SGPIO
#define USE_CMD_SGPIO  0
#endif

#include "mcush_vfs.h"

#ifndef MCUSH_SPIFFS
#define MCUSH_SPIFFS  1
#endif

#include "hal_platform_spiffs.h"

/* daq module */
int hal_daq_init( int freq, int sampling_length, int channel_num, int channel_mask, void *buf );
void hal_daq_deinit(void);
int hal_daq_start(void);
int hal_daq_stop(void);
int hal_daq_reset(void);
int hal_daq_done(void);



#endif
