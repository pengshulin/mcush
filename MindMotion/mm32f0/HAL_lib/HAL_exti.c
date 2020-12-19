////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_EXTI.C
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE EXTI FIRMWARE FUNCTIONS.
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
#define _HAL_EXTI_C_

// Files includes  -------------------------------------------------------------
#include "HAL_exti.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup EXTI_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup EXTI_Exported_Functions
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the EXTI peripheral registers to their default reset
///         values.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the EXTI registers to their default reset values.
/// @param  None.
/// @retval None.
/// @note   MEM_MODE bits are not affected by APB reset.
/// @note   MEM_MODE bits took the value from the user option bytes.
/// @note   CFGR2 register is not affected by APB reset.
/// @note   CLABBB configuration bits are locked when set.
/// @note   To unlock the configuration, perform a system reset.
////////////////////////////////////////////////////////////////////////////////
void EXTI_DeInit()
{
#if defined(__MM3N1)
    // Clear all
    exEXTI_LineDisable(~0x00000000);

    // rc_w1
    EXTI->PR = EXTI->PR;
#endif

#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)

    // Set EXTI_CFGR1 register to reset value without affecting MEM_MODE bits
    EXTI->CFGR &= EXTI_CFGR_MEMMODE;

    // Set EXTICRx registers to reset value
    for (u8 i = 0; i < 4; i++) {
        EXTI->CR[i] = 0;
    }
#endif
}


#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the memory mapping at address 0x00000000.
/// @param  EXTI_MemoryRemap: selects the memory remapping.
///          This parameter can be one of the following values:
///            @arg EXTI_MemoryRemap_Flash: Main Flash memory mapped at 0x00000000
///            @arg EXTI_MemoryRemap_SystemMemory: System Flash memory mapped at 0x00000000
///            @arg EXTI_MemoryRemap_SRAM: Embedded SRAM mapped at 0x00000000
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void EXTI_MemoryRemapConfig(u32 EXTI_MemoryRemap)
{
    EXTI->CFGR = (EXTI->CFGR & ~EXTI_CFGR_MEMMODE) | EXTI_MemoryRemap;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configure the DMA channels remapping.
/// @param  EXTI_DMARemap: selects the DMA channels remap. This parameter can be one of the following values:
///            @arg EXTI_DMARemap_TIM17: Remap TIM17 DMA requests from channel1 to channel2
///            @arg EXTI_DMARemap_TIM16: Remap TIM16 DMA requests from channel3 to channel4
///            @arg EXTI_DMARemap_UART1Rx: Remap UART1 Rx DMA requests from channel3 to channel5
///            @arg EXTI_DMARemap_UART1Tx: Remap UART1 Tx DMA requests from channel2 to channel4
///            @arg EXTI_DMARemap_ADC1: Remap ADC1 DMA requests from channel1 to channel2
/// @param  state: new state of the DMA channel remapping.
///         This parameter can be: ENABLE or DISABLE.
/// @note   When enabled, DMA channel of the selected peripheral is remapped
/// @note   When disabled, Default DMA channel is mapped to the selected peripheral
/// @note   By default TIM17 DMA requests is mapped to channel 1, use EXTI_DMAChannelRemapConfig(EXTI_DMARemap_TIM17, Enable)
/// to remap TIM17 DMA requests to channel 2 and use EXTI_DMAChannelRemapConfig(EXTI_DMARemap_TIM17, Disable) to map TIM17 DMA
/// requests to channel 1 (default mapping)
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void EXTI_DMAChannelRemapConfig(u32 EXTI_DMARemap, FunctionalState state)
{
    (state) ? (EXTI->CFGR |= EXTI_DMARemap) : (EXTI->CFGR &= ~EXTI_DMARemap);
}

#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief  Selects the GPIO pin used as EXTI Line.
/// @param  EXTI_PortSourceGPIOx: selects the GPIO port to be used as source for EXTI lines where x can be (A, B, C, D, E or F).
/// @param  EXTI_PinSourcex: specifies the EXTI line to be configured.
/// @note   This parameter can be EXTI_PinSourcex where x can be:
///         For MCU: (0..15) for GPIOA, GPIOB, (13..15) for GPIOC and (0..1, 6..7) for GPIOD.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void EXTI_LineConfig(u8 EXTI_PortSourceGPIOx, u8 EXTI_PinSourcex)
{
    EXTI->CR[EXTI_PinSourcex >> 0x02] &= ~(0x0F << (0x04 * (EXTI_PinSourcex & 0x03)));
    EXTI->CR[EXTI_PinSourcex >> 0x02] |= ((EXTI_PortSourceGPIOx) << (0x04 * (EXTI_PinSourcex & 0x03)));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the EXTI peripheral according to the specified
///         parameters in the pInitStruct.
/// @param  pInitStruct: pointer to a EXTI_InitTypeDef structure that
///         contains the configuration information for the EXTI peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void EXTI_Init(EXTI_InitTypeDef* pInitStruct)
{
    exEXTI_LineDisable(pInitStruct->EXTI_Line);

    if (pInitStruct->EXTI_LineCmd) {

        if (pInitStruct->EXTI_Mode == EXTI_Mode_Interrupt)
            EXTI->IMR |= pInitStruct->EXTI_Line;
        else
            EXTI->EMR |= pInitStruct->EXTI_Line;

        if (pInitStruct->EXTI_Trigger == EXTI_Trigger_Rising_Falling) {
            EXTI->RTSR |= pInitStruct->EXTI_Line;
            EXTI->FTSR |= pInitStruct->EXTI_Line;                               // Rising and Faling    afio
        }
        else if (pInitStruct->EXTI_Trigger == EXTI_Trigger_Rising)
            EXTI->RTSR |= pInitStruct->EXTI_Line;
        else
            EXTI->FTSR |= pInitStruct->EXTI_Line;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each pInitStruct member with its reset value.
/// @param  pInitStruct: pointer to a EXTI_InitTypeDef structure which will
///         be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void EXTI_StructInit(EXTI_InitTypeDef* pInitStruct)
{
    pInitStruct->EXTI_Line    = EXTI_LineNone;
    pInitStruct->EXTI_Mode    = EXTI_Mode_Interrupt;
    pInitStruct->EXTI_Trigger = EXTI_Trigger_Falling;
    pInitStruct->EXTI_LineCmd = DISABLE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Generates a Software interrupt on selected EXTI line.
/// @param  EXTI_Line: specifies the EXTI line on which the software interrupt
///         will be generated.
///         This parameter can be any combination of EXTI_Linex where x can be
///         (0..27).
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void EXTI_GenerateSWInterrupt(u32 EXTI_Line)
{
    EXTI->SWIER |= EXTI_Line;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified EXTI line flag is set or not.
/// @param  EXTI_Line: specifies the EXTI line flag to check.
///         This parameter can be EXTI_Linex where x can be (0..27).
/// @retval The new state of EXTI_Line (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
FlagStatus EXTI_GetFlagStatus(u32 EXTI_Line)
{
    return (EXTI->PR & EXTI_Line) ? SET : RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the EXTI's line pending flags.
/// @param  EXTI_Line: specifies the EXTI lines flags to clear.
///         This parameter can be any combination of EXTI_Linex where x can be
///         (0..27).
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void EXTI_ClearFlag(u32 EXTI_Line)
{
    EXTI->PR = EXTI_Line;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified EXTI line is asserted or not.
/// @param  EXTI_Line: specifies the EXTI line to check.
///         This parameter can be EXTI_Linex where x can be (0..27).
/// @retval The new state of EXTI_Line (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
ITStatus EXTI_GetITStatus(u32 EXTI_Line)
{
    return ((EXTI->PR & EXTI_Line) && (EXTI->IMR & EXTI_Line)) ? SET : RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the EXTI's line pending bits.
/// @param  EXTI_Line: specifies the EXTI lines to clear.
///         This parameter can be any combination of EXTI_Linex where x can be
///         (0..27).
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void EXTI_ClearITPendingBit(u32 EXTI_Line)
{
    EXTI->PR = EXTI_Line;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  EXTI Line Disable
/// @param  EXTI_Line: specifies the EXTI lines to clear.
///         This parameter can be any combination of EXTI_Linex where x can be
///         (0..27).
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exEXTI_LineDisable(u32 EXTI_Line)
{
    EXTI->IMR  &= ~EXTI_Line;
    EXTI->EMR  &= ~EXTI_Line;
    EXTI->RTSR &= ~EXTI_Line;
    EXTI->FTSR &= ~EXTI_Line;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the EXTI's line all pending bits.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
u32 exEXTI_GetAllFlagStatus()
{
    return EXTI->PR;
}

/// @}

/// @}

/// @}
