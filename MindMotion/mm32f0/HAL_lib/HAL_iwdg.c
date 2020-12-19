////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_IWDG.C
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE IWDG FIRMWARE FUNCTIONS.
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
#define _HAL_IWDG_C_

// Files includes  -------------------------------------------------------------
#include "HAL_iwdg.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup IWDG_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup IWDG_Exported_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables write access to IWDG_PR and IWDG_RLR
///   registers.
/// @param  IWDG_WriteAccess: new state of write access to IWDG_PR and
///   IWDG_RLR registers.
///   This parameter can be one of the following values:
/// @arg IWDG_WriteAccess_Enable: Enable write access to
///   IWDG_PR and IWDG_RLR registers
/// @arg IWDG_WriteAccess_Disable: Disable write access to
///   IWDG_PR and IWDG_RLR registers
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void IWDG_WriteAccessCmd(u16 IWDG_WriteAccess)
{
    IWDG->KR = IWDG_WriteAccess;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets IWDG Prescaler value.
/// @param  IWDG_Prescaler: specifies the IWDG Prescaler value.
///   This parameter can be one of the following values:
/// @arg IWDG_Prescaler_4: IWDG prescaler set to 4
/// @arg IWDG_Prescaler_8: IWDG prescaler set to 8
/// @arg IWDG_Prescaler_16: IWDG prescaler set to 16
/// @arg IWDG_Prescaler_32: IWDG prescaler set to 32
/// @arg IWDG_Prescaler_64: IWDG prescaler set to 64
/// @arg IWDG_Prescaler_128: IWDG prescaler set to 128
/// @arg IWDG_Prescaler_256: IWDG prescaler set to 256
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void IWDG_SetPrescaler(u8 IWDG_Prescaler)
{
    IWDG->PR = IWDG_Prescaler;
    PVU_CheckStatus();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set IWDG Reload value.
/// @param  Reload: specifies the IWDG Reload value.
///   This parameter must be a number between 0 and 0x0FFF.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void IWDG_SetReload(u16 Reload)
{
    IWDG->RLR = Reload;
    RVU_CheckStatus();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Get IWDG Reload value.
/// @param  None.
/// @retval Reload: specifies the IWDG Reload value.
////////////////////////////////////////////////////////////////////////////////
u32 IWDG_GetReload(void)
{
    return IWDG->RLR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reloads IWDG counter with value defined in the reload register
///   (write access to IWDG_PR and IWDG_RLR registers disabled).
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void IWDG_ReloadCounter(void)
{
    IWDG->KR = KR_KEY_Reload;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables IWDG (write access to IWDG_PR and IWDG_RLR registers
///   disabled).
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void IWDG_Enable(void)
{
    IWDG->KR = KR_KEY_Enable;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified IWDG flag is set or not.
/// @param  IWDG_FLAG: specifies the flag to check.
///   This parameter can be one of the following values:
/// @arg IWDG_FLAG_PVU: Prescaler Value Update on going
/// @arg IWDG_FLAG_RVU: Reload Value Update on going
/// @retval  The new state of IWDG_FLAG (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
FlagStatus IWDG_GetFlagStatus(u16 IWDG_FLAG)
{
    return ((IWDG->SR & IWDG_FLAG) != (u32)RESET) ? SET : RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks prescaler value has been updated.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void PVU_CheckStatus(void)
{
    while (IWDG_GetFlagStatus(IWDG_FLAG_PVU) == SET);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks count relead value has been updated.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RVU_CheckStatus(void)
{
    while (IWDG_GetFlagStatus(IWDG_FLAG_RVU) == SET);
}

#if defined(__MM3O1) || defined(__MM0P1) || defined(__MM0Q1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  IWDG overflow configuration.
/// @param  IWDG_Overflow_Config
/// @arg    IWDG_Overflow_Interrupt: Interrupt after overflow.
/// @arg    IWDG_Overflow_Reset:     Reset after overflow.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void IWDG_OverflowConfig(IWDGOverflowConfig_TypeDef IWDG_Overflow_Config)
{
    IWDG->CR &= ~IWDG_CR_IRQSEL;
    IWDG->CR |= IWDG_Overflow_Config;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clear interrupt flag
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void IWDG_ClearITPendingBit()
{
    IWDG->CR &= ~IWDG_CR_IRQCLR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clear interrupt flag
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void IWDG_CheckIRQ()
{
    IWDG->CR &= ~IWDG_CR_IRQCLR;
}



#endif

/// @}

/// @}

/// @}
