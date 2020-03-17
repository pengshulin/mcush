/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __HAL_PLATFORM_SPIFFS_H__
#define __HAL_PLATFORM_SPIFFS_H__

    
#define sFLASH_SPI                           SPI5
#define sFLASH_SPI_CLK                       RCC_APB2Periph_SPI5
#define sFLASH_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

/* CLK */
#define sFLASH_SPI_SCK_PIN                   GPIO_Pin_7
#define sFLASH_SPI_SCK_GPIO_PORT             GPIOF
#define sFLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOF
#define sFLASH_SPI_SCK_SOURCE                GPIO_PinSource7
#define sFLASH_SPI_SCK_AF                    GPIO_AF_SPI2

/* MISO */
#define sFLASH_SPI_MISO_PIN                  GPIO_Pin_8
#define sFLASH_SPI_MISO_GPIO_PORT            GPIOF
#define sFLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOF
#define sFLASH_SPI_MISO_SOURCE               GPIO_PinSource8
#define sFLASH_SPI_MISO_AF                   GPIO_AF_SPI2

/* MOSI */
#define sFLASH_SPI_MOSI_PIN                  GPIO_Pin_9
#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOF
#define sFLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOF
#define sFLASH_SPI_MOSI_SOURCE               GPIO_PinSource9
#define sFLASH_SPI_MOSI_AF                   GPIO_AF_SPI2

/* CS */
#define sFLASH_CS_PIN                        GPIO_Pin_6
#define sFLASH_CS_GPIO_PORT                  GPIOF
#define sFLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOF


#endif
