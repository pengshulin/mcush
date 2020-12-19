////////////////////////////////////////////////////////////////////////////////
/// @file     COMMON.C
/// @author   AE Team
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES THE DRIVER LAYER FUNCTIONS.
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
#define _COMMON_C_

// Files includes  -------------------------------------------------------------
#include <string.h>
#include "HAL_exti.h"
#include "HAL_gpio.h"
#include "HAL_nvic.h"
#include "HAL_rcc.h"

//#include "drv.h"
//#include "drv_exti.h"
#include "common.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Driver_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup COMMON
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup COMMON_Exported_Functions
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @brief  EXTI Configure Common function
/// @param  EXTI line:
/// @param  Edge:
/// @param  Enable:
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMMON_EXTI_Enable(u32 line, EXTIMode_TypeDef mode, EXTITrigger_TypeDef edge )
{
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line    = line;
    EXTI_InitStructure.EXTI_Mode    = mode;
    EXTI_InitStructure.EXTI_Trigger = edge;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  EXTI Configure, Function: Event, Falling Edge,
///         EXTI line enable.
/// @param  EXTI line.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMMON_ExitEventFallingEnable(u32 line)
{
    COMMON_EXTI_Enable(line, EXTI_Mode_Event, EXTI_Trigger_Falling);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  EXTI Configure, Function: Event, Rising Edge,
///         EXTI line enable.
/// @param  EXTI line.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMMON_ExitEventRisingEnable(u32 line)
{
    COMMON_EXTI_Enable(line, EXTI_Mode_Event, EXTI_Trigger_Rising);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  EXTI Configure, Function: Interrupt, Falling Edge,
///         EXTI line enable.
/// @param  EXTI line.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMMON_ExitItFallingEnable(u32 line)
{
    COMMON_EXTI_Enable(line, EXTI_Mode_Interrupt, EXTI_Trigger_Falling);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  EXTI Configure, Function: Interrupt, Rising Edge,
///         EXTI line enable.
/// @param  EXTI line.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMMON_ExitItRisingEnable(u32 line)
{
    COMMON_EXTI_Enable(line, EXTI_Mode_Interrupt, EXTI_Trigger_Rising);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  EXTI Configure, Function: Event, Rising or Falling Edge,
///         EXTI line enable.
/// @param  EXTI line.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMMON_ExitEventRisingFallingEnable(u32 line)
{
    COMMON_EXTI_Enable(line, EXTI_Mode_Event, EXTI_Trigger_Rising_Falling);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  EXTI Configure, Function: Interrupt, Rising or Falling Edge,
///         EXTI line enable.
/// @param  EXTI line.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMMON_ExitItRisingFallingEnable(u32 line)
{
    COMMON_EXTI_Enable(line, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  NVIC Configure, Applicable to Cortex M0 or M3 processors.
/// @param  NVIC Channel, Priority or SubPriority.
/// @arg    ch: IRQChannel
/// @arg    pri: Priority, Use only Cortex-M3
/// @arg    sub: SubPriority
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMMON_NVIC_Configure(u8 ch, u8 pri, u8 sub)
{
#if defined(__MM3N1) || defined(__MM3O1)
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
#endif

    exNVIC_Init_TypeDef  NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = ch;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = pri;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = sub;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

    exNVIC_Init(&NVIC_InitStruct);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables the AHB, APB (APB1, APB2) peripheral clock.
/// @param  Periph Enumeration: emCLOCK_xxx.
/// @arg    emCLOCK_ADC1, emCLOCK_ADC2, emCLOCK_AES, emCLOCK_BKP, emCLOCK_CAN,
///         emCLOCK_COMP, emCLOCK_CRC, emCLOCK_CRS, emCLOCK_DAC, emCLOCK_DIV,
///         emCLOCK_DMA1, emCLOCK_EXTI, emCLOCK_GPIOA, emCLOCK_GPIOB,
///         emCLOCK_GPIOC, emCLOCK_GPIOD, emCLOCK_I2C1, emCLOCK_I2C2,
///         emCLOCK_PWR, emCLOCK_SPI1, emCLOCK_SPI2, emCLOCK_SQRT, emCLOCK_TIM1,
///         emCLOCK_TIM2, emCLOCK_TIM3, emCLOCK_TIM4, emCLOCK_TIM8,
///         emCLOCK_TIM14, emCLOCK_TIM16, emCLOCK_TIM17, emCLOCK_UART1,
///         emCLOCK_UART2, emCLOCK_UART3, emCLOCK_UART4, emCLOCK_UART5,
///         emCLOCK_UART6, emCLOCK_UART7, emCLOCK_UART8, emCLOCK_USB,
///         emCLOCK_WWDG
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMMON_EnableIpClock(EM_CLOCK val)
{
    u8 br = val >> 8;
    u8 pos = val & 0x1F;

    if      (br >  4)   return;
    else if (br == 0)   RCC_APB1PeriphClockCmd(1 << pos, ENABLE);
    else if (br == 1)   RCC_APB2PeriphClockCmd(1 << pos, ENABLE);
    else if (br >= 2)   RCC_AHBPeriphClockCmd( 1 << pos, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Disbles the AHB, APB (APB1, APB2) peripheral clock.
/// @param  Periph Enumeration: emCLOCK_xxx.
/// @arg    emCLOCK_ADC1, emCLOCK_ADC2, emCLOCK_AES, emCLOCK_BKP, emCLOCK_CAN,
///         emCLOCK_COMP, emCLOCK_CRC, emCLOCK_CRS, emCLOCK_DAC, emCLOCK_DIV,
///         emCLOCK_DMA1, emCLOCK_EXTI, emCLOCK_GPIOA, emCLOCK_GPIOB,
///         emCLOCK_GPIOC, emCLOCK_GPIOD, emCLOCK_I2C1, emCLOCK_I2C2,
///         emCLOCK_PWR, emCLOCK_SPI1, emCLOCK_SPI2, emCLOCK_SQRT, emCLOCK_TIM1,
///         emCLOCK_TIM2, emCLOCK_TIM3, emCLOCK_TIM4, emCLOCK_TIM8,
///         emCLOCK_TIM14, emCLOCK_TIM16, emCLOCK_TIM17, emCLOCK_UART1,
///         emCLOCK_UART2, emCLOCK_UART3, emCLOCK_UART4, emCLOCK_UART5,
///         emCLOCK_UART6, emCLOCK_UART7, emCLOCK_UART8, emCLOCK_USB,
///         emCLOCK_WWDG
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMMON_DisableIpClock(EM_CLOCK val)
{
    u8 br = val >> 8;
    u8 pos = val & 0x1F;

    if      (br >  4)   return;
    else if (br == 0)   RCC_APB1PeriphClockCmd(1 << pos, DISABLE);
    else if (br == 1)   RCC_APB2PeriphClockCmd(1 << pos, DISABLE);
    else if (br >= 2)   RCC_AHBPeriphClockCmd( 1 << pos, DISABLE);
}


/// @}


/// @}

/// @}

