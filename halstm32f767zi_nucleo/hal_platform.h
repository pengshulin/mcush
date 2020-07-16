#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

#define HAL_LED_NUM   3
#define HAL_LED_PORTS  { 1, 1, 1 } 
#define HAL_LED_PINS  { 0, 7, 14 }

#define HAL_UART_DEFINE 
#define HAL_UART_RCC_GPIO_ENABLE_CMD    LL_AHB1_GRP1_EnableClock
#define HAL_UART_RCC_GPIO_ENABLE_BIT    LL_AHB1_GRP1_PERIPH_GPIOD
#define HAL_UART_RCC_USART_ENABLE_CMD   LL_APB1_GRP1_EnableClock
#define HAL_UART_RCC_USART_ENABLE_BIT   LL_APB1_GRP1_PERIPH_USART3
#define HAL_UARTx                       USART3
#define HAL_UARTx_TX_PORT               GPIOD
#define HAL_UARTx_TX_PIN                LL_GPIO_PIN_8
#define HAL_UARTx_TX_AF                 LL_GPIO_AF_7
#define HAL_UARTx_RX_PORT               GPIOD
#define HAL_UARTx_RX_PIN                LL_GPIO_PIN_9
#define HAL_UARTx_RX_AF                 LL_GPIO_AF_7
#define HAL_UARTx_IRQn                  USART3_IRQn
#define HAL_UARTx_IRQHandler            USART3_IRQHandler
#define HAL_UARTx_BAUDRATE              9600
#define HAL_UART_QUEUE_RX_LEN           128
#define HAL_UART_QUEUE_TX_LEN           128
#define HAL_UART_QUEUE_ADD_TO_REG       1


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

#include "mcush_vfs.h"

#ifndef MCUSH_SPIFFS
  #define MCUSH_SPIFFS  1
#endif

#define HAL_SPIFFS_CHIPID  0xEF4017  // W25P64
#define sFLASH_SPI                           SPI2
#define sFLASH_SPI_CLK                       RCC_APB1Periph_SPI2
#define sFLASH_SPI_CLK_INIT                  RCC_APB1PeriphClockCmd
#define sFLASH_SPI_SCK_PIN                   GPIO_Pin_13
#define sFLASH_SPI_SCK_GPIO_PORT             GPIOB
#define sFLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_SCK_SOURCE                GPIO_PinSource13
#define sFLASH_SPI_SCK_AF                    GPIO_AF_SPI2
#define sFLASH_SPI_MISO_PIN                  GPIO_Pin_14
#define sFLASH_SPI_MISO_GPIO_PORT            GPIOB
#define sFLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MISO_SOURCE               GPIO_PinSource14
#define sFLASH_SPI_MISO_AF                   GPIO_AF_SPI2
#define sFLASH_SPI_MOSI_PIN                  GPIO_Pin_15
#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOB
#define sFLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MOSI_SOURCE               GPIO_PinSource15
#define sFLASH_SPI_MOSI_AF                   GPIO_AF_SPI2
#define sFLASH_CS_PIN                        GPIO_Pin_12
#define sFLASH_CS_GPIO_PORT                  GPIOB
#define sFLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOB

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


#endif
