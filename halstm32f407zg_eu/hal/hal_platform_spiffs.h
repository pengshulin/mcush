/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __HAL_PLATFORM_SPIFFS_H__
#define __HAL_PLATFORM_SPIFFS_H__

    
#define sFLASH_SPI                           SPI2
#define sFLASH_SPI_CLK                       LL_APB1_GRP1_PERIPH_SPI2
#define sFLASH_SPI_CLK_INIT                  LL_APB1_GRP1_EnableClock
#define sFLASH_SPI_CLK_DEINIT                LL_APB1_GRP1_DisableClock

/* CLK */
#define sFLASH_SPI_SCK_GPIO_PORT             GPIOB
#define sFLASH_SPI_SCK_PIN                   GPIO_PIN_13
#define sFLASH_SPI_SCK_AF                    GPIO_AF5_SPI2

/* MISO */
#define sFLASH_SPI_MISO_GPIO_PORT            GPIOB
#define sFLASH_SPI_MISO_PIN                  GPIO_PIN_14
#define sFLASH_SPI_MISO_AF                   GPIO_AF5_SPI2

/* MOSI */
#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOB
#define sFLASH_SPI_MOSI_PIN                  GPIO_PIN_15
#define sFLASH_SPI_MOSI_AF                   GPIO_AF5_SPI2

/* CS */
#define sFLASH_CS_GPIO_PORT                  GPIOB
#define sFLASH_CS_PIN                        GPIO_PIN_12


#endif
