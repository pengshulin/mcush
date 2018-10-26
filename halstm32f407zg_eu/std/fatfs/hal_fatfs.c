#if MCUSH_FATFS
#include <stdarg.h>
#include "hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "mcush.h"
#include "mcush_vfs.h"

/* Hardware connection:
   ----------------------------- 
   (MCU)          (CARD)
   PC10 D2 -----> PIN 1
   PC11 D3 <----> PIN 2
   PD2 CMD <----> PIN 3
   VDD <--------> PIN 4
   PC12 CLK <---> PIN 5
   GND <--------> PIN 6
   PC8 D0 <-----> PIN 7
   PC9 D1 <-----> PIN 8
   ----------------------------- 
 */

#ifndef HAL_SDIO_DEFINE 
    #define HAL_SDIO_RCC_GPIO_ENABLE_CMD    RCC_AHB1PeriphClockCmd
    #define HAL_SDIO_RCC_GPIO_ENABLE_BIT    (RCC_AHB1Periph_GPIOC + RCC_AHB1Periph_GPIOD)
    #define HAL_SDIO_RCC_SDIO_ENABLE_CMD    RCC_APB2PeriphClockCmd
    #define HAL_SDIO_RCC_SDIO_ENABLE_BIT    RCC_APB2Periph_SDIO
    #define HAL_SDIO_RCC_SDIO_RESET_CMD     RCC_APB2PeriphResetCmd
    #define HAL_SDIO_RCC_SDIO_RESET_BIT     RCC_APB2Periph_SDIO
    #define HAL_SDIO_D0_PORT                GPIOC
    #define HAL_SDIO_D0_PIN                 GPIO_Pin_8
    #define HAL_SDIO_D0_PINSRC              GPIO_PinSource8
    #define HAL_SDIO_D1_PORT                GPIOC
    #define HAL_SDIO_D1_PIN                 GPIO_Pin_9
    #define HAL_SDIO_D1_PINSRC              GPIO_PinSource9
    #define HAL_SDIO_D2_PORT                GPIOC
    #define HAL_SDIO_D2_PIN                 GPIO_Pin_10
    #define HAL_SDIO_D2_PINSRC              GPIO_PinSource10
    #define HAL_SDIO_D3_PORT                GPIOC
    #define HAL_SDIO_D3_PIN                 GPIO_Pin_11
    #define HAL_SDIO_D3_PINSRC              GPIO_PinSource11
    #define HAL_SDIO_CLK_PORT               GPIOC
    #define HAL_SDIO_CLK_PIN                GPIO_Pin_12
    #define HAL_SDIO_CLK_PINSRC             GPIO_PinSource12
    #define HAL_SDIO_CMD_PORT               GPIOD
    #define HAL_SDIO_CMD_PIN                GPIO_Pin_2
    #define HAL_SDIO_CMD_PINSRC             GPIO_PinSource2
    #define HAL_SDIO_AF                     GPIO_AF_SDIO
    #define HAL_SDIO_IRQn                   SDIO_IRQn
    #define HAL_SDIO_IRQHandler             SDIO_IRQHandler
#endif

SemaphoreHandle_t semaphore_sdio;

void hal_fatfs_init(void)
{
    GPIO_InitTypeDef gpio_init;
    NVIC_InitTypeDef nvic_init;

    semaphore_sdio = xSemaphoreCreateMutex();
    if( !semaphore_sdio )
        halt("fatfs semphr create"); 

    /* clock enable */
    HAL_SDIO_RCC_GPIO_ENABLE_CMD(HAL_SDIO_RCC_GPIO_ENABLE_BIT, ENABLE);
    HAL_SDIO_RCC_SDIO_ENABLE_CMD(HAL_SDIO_RCC_SDIO_ENABLE_BIT, ENABLE);
    HAL_SDIO_RCC_SDIO_RESET_CMD(HAL_SDIO_RCC_SDIO_RESET_BIT, ENABLE);
    /* pin config */
    gpio_init.GPIO_Pin = HAL_SDIO_D0_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( HAL_SDIO_D0_PORT, &gpio_init );
    gpio_init.GPIO_Pin = HAL_SDIO_D1_PIN;
    GPIO_Init( HAL_SDIO_D1_PORT, &gpio_init );
    gpio_init.GPIO_Pin = HAL_SDIO_D2_PIN;
    GPIO_Init( HAL_SDIO_D2_PORT, &gpio_init );
    gpio_init.GPIO_Pin = HAL_SDIO_D3_PIN;
    GPIO_Init( HAL_SDIO_D3_PORT, &gpio_init );
    gpio_init.GPIO_Pin = HAL_SDIO_CLK_PIN;
    GPIO_Init( HAL_SDIO_CLK_PORT, &gpio_init );
    gpio_init.GPIO_Pin = HAL_SDIO_CMD_PIN;
    GPIO_Init( HAL_SDIO_CMD_PORT, &gpio_init );
    /* Connect AF */
    GPIO_PinAFConfig( HAL_SDIO_D0_PORT, HAL_SDIO_D0_PINSRC, HAL_SDIO_AF );
    GPIO_PinAFConfig( HAL_SDIO_D1_PORT, HAL_SDIO_D1_PINSRC, HAL_SDIO_AF );
    GPIO_PinAFConfig( HAL_SDIO_D2_PORT, HAL_SDIO_D2_PINSRC, HAL_SDIO_AF );
    GPIO_PinAFConfig( HAL_SDIO_D3_PORT, HAL_SDIO_D3_PINSRC, HAL_SDIO_AF );
    GPIO_PinAFConfig( HAL_SDIO_CLK_PORT, HAL_SDIO_CLK_PINSRC, HAL_SDIO_AF );
    GPIO_PinAFConfig( HAL_SDIO_CMD_PORT, HAL_SDIO_CMD_PINSRC, HAL_SDIO_AF );
    HAL_SDIO_RCC_SDIO_RESET_CMD(HAL_SDIO_RCC_SDIO_RESET_BIT, DISABLE);

    /* SDIO config */

    /* NVIC config */
    nvic_init.NVIC_IRQChannel = SDIO_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority=0;
    nvic_init.NVIC_IRQChannelSubPriority =0;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);
}

//int hal_fatfs_flash_read_id(void)
//{
//    //return (int)sFLASH_ReadID();
//    return 1;
//}


//s32_t *hal_fatfs_read(u32_t addr, u32_t size, u8_t *dst)
//{
//    xSemaphoreTake( semaphore_sdio, portMAX_DELAY );
//    sFLASH_ReadBuffer(dst, addr, size);
//    xSemaphoreGive( semaphore_sdio );
//    return SPIFFS_OK;
//}
//
//
//s32_t *hal_fatfs_write(u32_t addr, u32_t size, u8_t *src)
//{
//    xSemaphoreTake( semaphore_sdio, portMAX_DELAY );
//    sFLASH_WriteBuffer(src, addr, size);
//    xSemaphoreGive( semaphore_sdio );
//    return SPIFFS_OK;
//}
//
//
//s32_t *hal_fatfs_erase(u32_t addr, u32_t size)
//{
//    xSemaphoreTake( semaphore_sdio, portMAX_DELAY );
//    sFLASH_EraseSector(addr);
//    xSemaphoreGive( semaphore_sdio );
//    return SPIFFS_OK;
//}


#endif
