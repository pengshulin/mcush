/**
  ******************************************************************************
  * @file    QSPI/QSPI_MemoryMapped/main.h
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below according to QSPI FLASH Memory connected on your
   evaluation board (U3)
  */
#if !defined(QSPI_FLASH_MICRON) && !defined(QSPI_FLASH_SPANSION)
  /* #define QSPI_FLASH_SPANSION */  /*!< QSPI FLASH Spansion */
   #define QSPI_FLASH_MICRON     /*!<  QSPI FLASH Micron */
#endif

/*GPIO Pins and Clocks Definition*/
#define QSPIx_CLK                     RCC_AHB3Periph_QSPI
#define QSPIx_CLK_INIT                RCC_AHB3PeriphClockCmd
#define QSPIx_IRQn                    QUADSPI_IRQn
#define QSPIx_IRQHANDLER              QUADSPI_IRQHandler

#if defined(STM32F446xx)
#define QSPIx_CLK_PIN                 GPIO_Pin_3
#define QSPIx_CLK_GPIO_PORT           GPIOD
#define QSPIx_CLK_GPIO_CLK            RCC_AHB1Periph_GPIOD
#define QSPIx_CLK_SOURCE              GPIO_PinSource3
#define QSPIx_CLK_AF                  GPIO_AF9_QUADSPI
#elif defined(STM32F412xG) || defined(STM32F413_423xx)
#define QSPIx_CLK_PIN                 GPIO_Pin_2
#define QSPIx_CLK_GPIO_PORT           GPIOB
#define QSPIx_CLK_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define QSPIx_CLK_SOURCE              GPIO_PinSource2
#define QSPIx_CLK_AF                  GPIO_AF9_QUADSPI
#else /* STM32F469_479xx */
#define QSPIx_CLK_PIN                 GPIO_Pin_10
#define QSPIx_CLK_GPIO_PORT           GPIOF
#define QSPIx_CLK_GPIO_CLK            RCC_AHB1Periph_GPIOF
#define QSPIx_CLK_SOURCE              GPIO_PinSource10
#define QSPIx_CLK_AF                  GPIO_AF9_QUADSPI
#endif /* STM32F446xx */

#define QSPIx_D0_PIN                  GPIO_Pin_8
#define QSPIx_D0_GPIO_PORT            GPIOF
#define QSPIx_D0_GPIO_CLK             RCC_AHB1Periph_GPIOF
#define QSPIx_D0_SOURCE               GPIO_PinSource8
#define QSPIx_D0_AF                   GPIO_AF10_QUADSPI

#define QSPIx_D1_PIN                  GPIO_Pin_9
#define QSPIx_D1_GPIO_PORT            GPIOF
#define QSPIx_D1_GPIO_CLK             RCC_AHB1Periph_GPIOF
#define QSPIx_D1_SOURCE               GPIO_PinSource9
#define QSPIx_D1_AF                   GPIO_AF10_QUADSPI

#if defined(STM32F413_423xx)
#define QSPIx_D2_PIN                  GPIO_Pin_2
#define QSPIx_D2_GPIO_PORT            GPIOE
#define QSPIx_D2_GPIO_CLK             RCC_AHB1Periph_GPIOE
#define QSPIx_D2_SOURCE               GPIO_PinSource2
#define QSPIx_D2_AF                   GPIO_AF9_QUADSPI
#else
#define QSPIx_D2_PIN                  GPIO_Pin_7
#define QSPIx_D2_GPIO_PORT            GPIOF
#define QSPIx_D2_GPIO_CLK             RCC_AHB1Periph_GPIOF
#define QSPIx_D2_SOURCE               GPIO_PinSource7
#define QSPIx_D2_AF                   GPIO_AF9_QUADSPI
#endif /* STM32F413_423xx*/

#if defined(STM32F413_423xx)
#define QSPIx_D3_PIN                  GPIO_Pin_13
#define QSPIx_D3_GPIO_PORT            GPIOD
#define QSPIx_D3_GPIO_CLK             RCC_AHB1Periph_GPIOD
#define QSPIx_D3_SOURCE               GPIO_PinSource13
#define QSPIx_D3_AF                   GPIO_AF9_QUADSPI
#else
#define QSPIx_D3_PIN                  GPIO_Pin_6
#define QSPIx_D3_GPIO_PORT            GPIOF
#define QSPIx_D3_GPIO_CLK             RCC_AHB1Periph_GPIOF
#define QSPIx_D3_SOURCE               GPIO_PinSource6
#define QSPIx_D3_AF                   GPIO_AF9_QUADSPI
#endif /* STM32F413_423xx */

#if defined(STM32F446xx) || defined(STM32F412xG) || defined(STM32F413_423xx)
#define QSPIx_CS_PIN                  GPIO_Pin_6
#define QSPIx_CS_GPIO_PORT            GPIOG
#define QSPIx_CS_GPIO_CLK             RCC_AHB1Periph_GPIOG
#define QSPIx_CS_SOURCE               GPIO_PinSource6
#define QSPIx_CS_AF                   GPIO_AF10_QUADSPI
#else /* STM32F469_479xx */
#define QSPIx_CS_PIN                  GPIO_Pin_6
#define QSPIx_CS_GPIO_PORT            GPIOB
#define QSPIx_CS_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define QSPIx_CS_SOURCE               GPIO_PinSource6
#define QSPIx_CS_AF                   GPIO_AF10_QUADSPI
#endif /* STM32F446xx || STM32F412xG || STM32F413_423xx */

/* DMA Definitions */
#define QSPI_DMA_CHANNEL                DMA_Channel_3
#define QSPI_DMA_STREAM                 DMA2_Stream7
#define QSPI_DMA_FLAG_TC                DMA_FLAG_TCIF7

/* S25FL512S SPANSION FLASH Definitions*/
/* Size of the flash */
#define QSPI_FLASH_SIZE                      24
#define QSPI_PAGE_SIZE                       256

/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

/* Identification Operations */
#define READ_ID_CMD                          0x9E
#define READ_ID_CMD2                         0x9F
#define MULTIPLE_IO_READ_ID_CMD              0xAF
#define READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5A

/* Read Operations */
#define READ_CMD                             0x03
#define READ_4_BYTE_ADDR_CMD                 0x13

#define FAST_READ_CMD                        0x0B
#define FAST_READ_DTR_CMD                    0x0D
#define FAST_READ_4_BYTE_ADDR_CMD            0x0C

#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_OUT_FAST_READ_DTR_CMD           0x3D
#define DUAL_OUT_FAST_READ_4_BYTE_ADDR_CMD   0x3C

#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define DUAL_INOUT_FAST_READ_DTR_CMD         0xBD
#define DUAL_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xBC

#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_OUT_FAST_READ_DTR_CMD           0x6D
#define QUAD_OUT_FAST_READ_4_BYTE_ADDR_CMD   0x6C

#define QUAD_INOUT_FAST_READ_CMD             0xEB
#define QUAD_INOUT_FAST_READ_DTR_CMD         0xED
#define QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xEC

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG_CMD                  0x05
#define WRITE_STATUS_REG_CMD                 0x01

#define READ_LOCK_REG_CMD                    0xE8
#define WRITE_LOCK_REG_CMD                   0xE5

#define READ_FLAG_STATUS_REG_CMD             0x70
#define CLEAR_FLAG_STATUS_REG_CMD            0x50

#define READ_NONVOL_CFG_REG_CMD              0xB5
#define WRITE_NONVOL_CFG_REG_CMD             0xB1

#define READ_VOL_CFG_REG_CMD                 0x85
#define WRITE_VOL_CFG_REG_CMD                0x81

#define READ_ENHANCED_VOL_CFG_REG_CMD        0x65
#define WRITE_ENHANCED_VOL_CFG_REG_CMD       0x61

#define READ_EXT_ADDR_REG_CMD                0xC8
#define WRITE_EXT_ADDR_REG_CMD               0xC5

/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define PAGE_PROG_4_BYTE_ADDR_CMD            0x12

#define DUAL_IN_FAST_PROG_CMD                0xA2
#define EXT_DUAL_IN_FAST_PROG_CMD            0xD2

#define QUAD_IN_FAST_PROG_CMD                0x32
#define EXT_QUAD_IN_FAST_PROG_CMD            0x12
#define QUAD_IN_FAST_PROG_4_BYTE_ADDR_CMD    0x34

/* Erase Operations */
#define SUBSECTOR_ERASE_CMD                  0x20
#define SUBSECTOR_ERASE_4_BYTE_ADDR_CMD      0x21

#define SECTOR_ERASE_CMD                     0xD8
#define SECTOR_ERASE_4_BYTE_ADDR_CMD         0xDC

#define BULK_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75

/* One-Time Programmable Operations */
#define READ_OTP_ARRAY_CMD                   0x4B
#define PROG_OTP_ARRAY_CMD                   0x42

/* 4-byte Address Mode Operations */
#define ENTER_4_BYTE_ADDR_MODE_CMD           0xB7
#define EXIT_4_BYTE_ADDR_MODE_CMD            0xE9

/* Quad Operations */
#define ENTER_QUAD_CMD                       0x35
#define EXIT_QUAD_CMD                        0xF5

/* Default dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_READ              8

#if defined(QSPI_FLASH_SPANSION)
#define DUMMY_CLOCK_CYCLES_READ_QUAD         0x0C
#elif defined(QSPI_FLASH_MICRON)
#define DUMMY_CLOCK_CYCLES_READ_QUAD         0x10
#endif /* QSPI_FLASH_SPANSION */

#define DUMMY_CLOCK_CYCLES_READ_DTR          6
#define DUMMY_CLOCK_CYCLES_READ_QUAD_DTR     8

/* End address of the QSPI memory */
#define QSPI_END_ADDR              (1 << QSPI_FLASH_SIZE)
#define QSPI_END_EXT_ADDR          0x93FFFFFF
#define QSPI_START_EXT_ADDR        0x90000000 
/* Size of buffers */
#define BUFFERSIZE                 (COUNTOF(aTxBuffer) - 1)

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)        (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
