////////////////////////////////////////////////////////////////////////////////
/// @file     COMMON.H
/// @author   AE Team
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE
///           DRIVER LAYER.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT 2018-2019 MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

// Define to prevent recursive inclusion  --------------------------------------
#ifndef __DRV_COMMON_H
#define __DRV_COMMON_H

// Files includes  -------------------------------------------------------------
#include "HAL_exti.h"


////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Driver_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup COMMON
/// @brief EXTI driver modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup COMMON_Exported_Constants
/// @{

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup COMMON_Exported_Types
/// @{

#if defined(__MM3N1)
typedef enum {
    emCLOCK_ADC1  = 0x109,
    emCLOCK_ADC2  = 0x10A,
    emCLOCK_AES   = 0x207,
    emCLOCK_BKP   = 0x01B,
    emCLOCK_CAN   = 0x019,
    emCLOCK_COMP  = 0x10F,
    emCLOCK_CRC   = 0x206,
    emCLOCK_CRS   = 0x01B,
    emCLOCK_DAC   = 0x01D,
    emCLOCK_DIV   = 0xFFF,
    emCLOCK_DMA1  = 0x200,
    emCLOCK_EXTI  = 0x100,
    emCLOCK_GPIOA = 0x102,
    emCLOCK_GPIOB = 0x103,
    emCLOCK_GPIOC = 0x104,
    emCLOCK_GPIOD = 0x105,
    emCLOCK_I2C1  = 0x015,
    emCLOCK_I2C2  = 0x016,
    emCLOCK_PWR   = 0x01C,
    emCLOCK_SPI1  = 0x10C,
    emCLOCK_SPI2  = 0x00E,
    emCLOCK_SQRT  = 0xFFF,
    emCLOCK_TIM1  = 0x10B,
    emCLOCK_TIM2  = 0x000,
    emCLOCK_TIM3  = 0x001,
    emCLOCK_TIM4  = 0x002,
    emCLOCK_TIM8  = 0xFFF,
    emCLOCK_TIM14 = 0xFFF,
    emCLOCK_TIM16 = 0xFFF,
    emCLOCK_TIM17 = 0xFFF,
    emCLOCK_UART1 = 0x10E,
    emCLOCK_UART2 = 0x011,
    emCLOCK_UART3 = 0x012,
    emCLOCK_UART4 = 0x012,
    emCLOCK_UART5 = 0x012,
    emCLOCK_UART6 = 0x012,
    emCLOCK_UART7 = 0x012,
    emCLOCK_UART8 = 0x012,
    emCLOCK_USB   = 0x017,
    emCLOCK_WWDG  = 0x00B
} EM_CLOCK;
#endif

#if defined(__MM0N1)
typedef enum {
    emCLOCK_ADC1  = 0x109,
    emCLOCK_ADC2  = 0xFFF,
    emCLOCK_AES   = 0x207,
    emCLOCK_BKP   = 0x01C,
    emCLOCK_CAN   = 0x019,
    emCLOCK_COMP  = 0x10F,
    emCLOCK_CRC   = 0x206,
    emCLOCK_CRS   = 0x01B,
    emCLOCK_DAC   = 0xFFF,
    emCLOCK_DIV   = 0xFFF,
    emCLOCK_DMA1  = 0x200,
    emCLOCK_EXTI  = 0x100,
    emCLOCK_GPIOA = 0x411,
    emCLOCK_GPIOB = 0x412,
    emCLOCK_GPIOC = 0x413,
    emCLOCK_GPIOD = 0x414,
    emCLOCK_I2C1  = 0x015,
    emCLOCK_I2C2  = 0xFFF,
    emCLOCK_PWR   = 0x01C,
    emCLOCK_SPI1  = 0x10C,
    emCLOCK_SPI2  = 0x00E,
    emCLOCK_SQRT  = 0xFFF,
    emCLOCK_TIM1  = 0x10B,
    emCLOCK_TIM2  = 0x000,
    emCLOCK_TIM3  = 0x001,
    emCLOCK_TIM4  = 0xFFF,
    emCLOCK_TIM8  = 0xFFF,
    emCLOCK_TIM14 = 0x110,
    emCLOCK_TIM16 = 0x111,
    emCLOCK_TIM17 = 0x112,
    emCLOCK_UART1 = 0x10E,
    emCLOCK_UART2 = 0x011,
    emCLOCK_UART3 = 0xFFF,
    emCLOCK_UART4 = 0xFFF,
    emCLOCK_UART5 = 0xFFF,
    emCLOCK_UART6 = 0xFFF,
    emCLOCK_UART7 = 0xFFF,
    emCLOCK_UART8 = 0xFFF,
    emCLOCK_USB   = 0x017,
    emCLOCK_WWDG  = 0x00B
} EM_CLOCK;
#endif

#if defined(__MM0P1)
typedef enum {
    emCLOCK_ADC1  = 0x109,
    emCLOCK_ADC2  = 0x10A,
    emCLOCK_AES   = 0xFFF,
    emCLOCK_BKP   = 0xFFF,
    emCLOCK_CAN   = 0xFFF,
    emCLOCK_COMP  = 0x10F,
    emCLOCK_CRC   = 0x206,
    emCLOCK_CRS   = 0xFFF,
    emCLOCK_DAC   = 0xFFF,
    emCLOCK_DIV   = 0x31A,
    emCLOCK_DMA1  = 0x200,
    emCLOCK_EXTI  = 0x100,
    emCLOCK_GPIOA = 0x411,
    emCLOCK_GPIOB = 0x412,
    emCLOCK_GPIOC = 0x413,
    emCLOCK_GPIOD = 0x414,
    emCLOCK_I2C1  = 0x015,
    emCLOCK_I2C2  = 0xFFF,
    emCLOCK_PWR   = 0x01C,
    emCLOCK_SPI1  = 0x10C,
    emCLOCK_SPI2  = 0x00E,
    emCLOCK_SQRT  = 0x319,
    emCLOCK_TIM1  = 0x10B,
    emCLOCK_TIM2  = 0x000,
    emCLOCK_TIM3  = 0x001,
    emCLOCK_TIM4  = 0xFFF,
    emCLOCK_TIM8  = 0x10D,
    emCLOCK_TIM14 = 0x110,
    emCLOCK_TIM16 = 0x111,
    emCLOCK_TIM17 = 0x112,
    emCLOCK_UART1 = 0x10E,
    emCLOCK_UART2 = 0x011,
    emCLOCK_UART3 = 0xFFF,
    emCLOCK_UART4 = 0xFFF,
    emCLOCK_UART5 = 0xFFF,
    emCLOCK_UART6 = 0xFFF,
    emCLOCK_UART7 = 0xFFF,
    emCLOCK_UART8 = 0xFFF,
    emCLOCK_USB   = 0xFFF,
    emCLOCK_WWDG  = 0x00B
} EM_CLOCK;
#endif

#if defined(__MM0Q1)
typedef enum {
    emCLOCK_ADC1  = 0x109,
    emCLOCK_ADC2  = 0xFFF,
    emCLOCK_AES   = 0xFFF,
    emCLOCK_BKP   = 0xFFF,
    emCLOCK_CAN   = 0xFFF,
    emCLOCK_COMP  = 0x10F,
    emCLOCK_CRC   = 0x206,
    emCLOCK_CRS   = 0xFFF,
    emCLOCK_DAC   = 0xFFF,
    emCLOCK_DIV   = 0x31A,
    emCLOCK_DMA1  = 0x200,
    emCLOCK_EXTI  = 0x100,
    emCLOCK_GPIOA = 0x411,
    emCLOCK_GPIOB = 0x412,
    emCLOCK_GPIOC = 0x413,
    emCLOCK_GPIOD = 0x414,
    emCLOCK_I2C1  = 0x015,
    emCLOCK_I2C2  = 0xFFF,
    emCLOCK_PWR   = 0x01C,
    emCLOCK_SPI1  = 0x10C,
    emCLOCK_SPI2  = 0x00E,
    emCLOCK_SQRT  = 0xFFF,
    emCLOCK_TIM1  = 0x10B,
    emCLOCK_TIM2  = 0x000,
    emCLOCK_TIM3  = 0x001,
    emCLOCK_TIM4  = 0xFFF,
    emCLOCK_TIM8  = 0xFFF,
    emCLOCK_TIM14 = 0x110,
    emCLOCK_TIM16 = 0x111,
    emCLOCK_TIM17 = 0x112,
    emCLOCK_UART1 = 0x10E,
    emCLOCK_UART2 = 0x011,
    emCLOCK_UART3 = 0xFFF,
    emCLOCK_UART4 = 0xFFF,
    emCLOCK_UART5 = 0xFFF,
    emCLOCK_UART6 = 0xFFF,
    emCLOCK_UART7 = 0xFFF,
    emCLOCK_USB   = 0xFFF,
    emCLOCK_WWDG  = 0x00B
} EM_CLOCK;
#endif

////////////////////////////////////////////////////////////////////////////////
/// @defgroup COMMON_Exported_Variables
/// @{
#ifdef _COMMON_C_
#define GLOBAL


#else
#define GLOBAL extern
#endif


#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup COMMON_Exported_Functions
/// @{


void COMMON_EXTI_Enable(u32 line, EXTIMode_TypeDef mode, EXTITrigger_TypeDef edge);
void COMMON_ExitEventFallingEnable(u32 line);
void COMMON_ExitEventRisingEnable(u32 line);
void COMMON_ExitItFallingEnable(u32 line);
void COMMON_ExitItRisingEnable(u32 line);

void COMMON_ExitEventRisingFallingEnable(u32 line);
void COMMON_ExitItRisingFallingEnable(u32 line);

void COMMON_NVIC_Configure(u8 ch, u8 pri, u8 sub);

void COMMON_EnableIpClock(EM_CLOCK val);
void COMMON_DisableIpClock(EM_CLOCK val);

/// @}


/// @}

/// @}


////////////////////////////////////////////////////////////////////////////////
#endif /* __COMMON_H */
////////////////////////////////////////////////////////////////////////////////



