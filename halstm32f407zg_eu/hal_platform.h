#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

#define HAL_LED_REV
#define HAL_LED_NUM   2
#define HAL_LED_PORTS  { 2, 3 } 
#define HAL_LED_PINS  { 0, 3 }

#ifndef HAL_RTC
  #define HAL_RTC  1
  #define USE_CMD_RTC  1
#endif

#include <time.h>
int  hal_rtc_get( struct tm *t );
int  hal_rtc_set( struct tm *t );
int  hal_rtc_get_tick( uint64_t *tick );
void hal_rtc_init(void);

#ifndef HAL_RNG
  #define HAL_RNG   1
#endif
void hal_rng_init(void);
uint32_t hal_rng_get(void);

#ifndef USE_CMD_SGPIO
  #define USE_CMD_SGPIO  1
#endif


#include "mcush_vfs.h"


#ifndef MCUSH_SPIFFS
  #define MCUSH_SPIFFS  1
#endif

#include "hal_platform_spiffs.h"


#define  LCD_ROW_NUM    320
#define  LCD_COL_NUM    240


void hal_lcd_init(void);
void hal_lcd_backlight(int on);
void hal_lcd_clrscr(uint16_t backcolor);
void hal_lcd_pixel(uint16_t x0, uint16_t y0, uint16_t color);
void hal_lcd_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void hal_lcd_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void hal_lcd_hline(uint8_t y, uint8_t x0, uint8_t x1, uint16_t color);
void hal_lcd_vline(uint8_t x, uint8_t y0, uint8_t y1, uint16_t color);
uint16_t hal_lcd_get_pixel(uint16_t x0, uint16_t y0);

int hal_upgrade_prepare_swap( const char *filename, int debug_mode );
void hal_upgrade_run_stage2(void);

void hal_can_init( void );
void hal_can_deinit( void );
int hal_can_transmit( const char *buf );



#endif
