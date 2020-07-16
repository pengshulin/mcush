/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __HAL_PLATFORM_SPIFFS_H__
#define __HAL_PLATFORM_SPIFFS_H__

    
#define sFLASH_SPI                           SPI1
#define sFLASH_SPI_CLK                       LL_APB2_GRP1_PERIPH_SPI1
#define sFLASH_SPI_CLK_INIT                  LL_APB2_GRP1_EnableClock
#define sFLASH_SPI_CLK_DEINIT                LL_APB2_GRP1_DisableClock

#define sFLASH_SPI_SCK_GPIO_PORT             GPIOA
#define sFLASH_SPI_SCK_PIN                   GPIO_PIN_5
#define sFLASH_SPI_SCK_AF                    GPIO_AF5_SPI1

#if WEACT_BOARD_VER == 20  /* PCB V2.0 */
    /* it's different that PB4 is used instead of PA6 */
    #define sFLASH_SPI_MISO_GPIO_PORT            GPIOB
    #define sFLASH_SPI_MISO_PIN                  GPIO_PIN_4
#else
    #define sFLASH_SPI_MISO_GPIO_PORT            GPIOA
    #define sFLASH_SPI_MISO_PIN                  GPIO_PIN_6
#endif
#define sFLASH_SPI_MISO_AF                   GPIO_AF5_SPI1

#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOA
#define sFLASH_SPI_MOSI_PIN                  GPIO_PIN_7
#define sFLASH_SPI_MOSI_AF                   GPIO_AF5_SPI1

#define sFLASH_CS_GPIO_PORT                  GPIOA
#define sFLASH_CS_PIN                        GPIO_PIN_4


#endif
