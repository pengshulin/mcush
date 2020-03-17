/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __HAL_PLATFORM_SPIFFS_H__
#define __HAL_PLATFORM_SPIFFS_H__

    
#define sFLASH_SPI                           SPI5
#define sFLASH_SPI_CLK                       LL_APB2_GRP1_PERIPH_SPI5
#define sFLASH_SPI_CLK_INIT                  LL_APB2_GRP1_EnableClock
#define sFLASH_SPI_CLK_DEINIT                LL_APB2_GRP1_DisableClock

/* CLK */
#define sFLASH_SPI_SCK_GPIO_PORT             GPIOF
#define sFLASH_SPI_SCK_PIN                   GPIO_PIN_7
#define sFLASH_SPI_SCK_AF                    GPIO_AF5_SPI5

/* MISO */
#define sFLASH_SPI_MISO_GPIO_PORT            GPIOF
#define sFLASH_SPI_MISO_PIN                  GPIO_PIN_8
#define sFLASH_SPI_MISO_AF                   GPIO_AF5_SPI5

/* MOSI */
#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOF
#define sFLASH_SPI_MOSI_PIN                  GPIO_PIN_9
#define sFLASH_SPI_MOSI_AF                   GPIO_AF5_SPI5

/* CS */
#define sFLASH_CS_GPIO_PORT                  GPIOF
#define sFLASH_CS_PIN                        GPIO_PIN_6


#endif
