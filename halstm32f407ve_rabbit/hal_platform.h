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

//#define HAL_SPIFFS_CHIPID  0xEF4017  // W25P64
#define sFLASH_SPI                           SPI1
#define sFLASH_SPI_CLK                       RCC_APB2Periph_SPI1
#define sFLASH_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define sFLASH_SPI_SCK_PIN                   GPIO_Pin_5
#define sFLASH_SPI_SCK_GPIO_PORT             GPIOA
#define sFLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define sFLASH_SPI_SCK_SOURCE                GPIO_PinSource5
#define sFLASH_SPI_SCK_AF                    GPIO_AF_SPI1

#define sFLASH_SPI_MISO_PIN                  GPIO_Pin_6
#define sFLASH_SPI_MISO_GPIO_PORT            GPIOA
#define sFLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define sFLASH_SPI_MISO_SOURCE               GPIO_PinSource6
#define sFLASH_SPI_MISO_AF                   GPIO_AF_SPI1

#define sFLASH_SPI_MOSI_PIN                  GPIO_Pin_5
#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOB
#define sFLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MOSI_SOURCE               GPIO_PinSource5
#define sFLASH_SPI_MOSI_AF                   GPIO_AF_SPI1

#define sFLASH_CS_PIN                        GPIO_Pin_4
#define sFLASH_CS_GPIO_PORT                  GPIOA
#define sFLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOA


#endif
