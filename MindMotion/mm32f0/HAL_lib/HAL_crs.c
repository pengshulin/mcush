////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_CRS.C
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE CRS FIRMWARE FUNCTIONS.
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
#define _HAL_CRS_C_

// Files includes  -------------------------------------------------------------
#include "HAL_rcc.h"
#include "HAL_crs.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CRS_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CRS_Exported_Functions
/// @{

#if defined(__MM0N1)

////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes CRS peripheral registers to their default reset
/// values.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_DeInit()
{
    exRCC_APB1PeriphReset(RCC_APB1ENR_CRS);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Adjusts the Internal High Speed 48 oscillator (HSI 48) calibration
/// value.
/// @note   The calibration is used to compensate for the variations in voltage
///         and temperature that influence the frequency of the internal HSI48
///         RC.
/// @note   This function can be called only when the AUTOTRIMEN bit is reset.
/// @param  CRS_HSI48CalibrationValue:
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_AdjustHSI48CalibrationValue(u8 value)
{
    CRS->CR = (CRS->CR & ~CRS_CR_TRIM) | ((u32)value << CRS_CR_TRIM_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the oscillator clock for frequency error
/// counter.
/// @note   when the CEN bit is set the CRS_CFGR register becomes
/// write-protected.
/// @param  state: new state of the frequency error counter.
///          This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_FrequencyErrorCounterCmd(FunctionalState state)
{
    (state) ? (CRS->CR |= CRS_CR_CNTEN) : (CRS->CR &= ~CRS_CR_CNTEN);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the automatic hardware adjustement of TRIM bits.
/// @note   When the AUTOTRIMEN bit is set the CRS_CFGR register becomes
/// write-protected.
/// @param  state: new state of the automatic trimming.
///          This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_AutomaticCalibrationCmd(FunctionalState state)
{
    (state) ? (CRS->CR |= CRS_CR_AUTOTRIMEN) : (CRS->CR &= ~CRS_CR_AUTOTRIMEN);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Generate the software synchronization event
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_SoftwareSynchronizationGenerate()
{
    CRS->CR |= CRS_CR_SWSYNC;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Adjusts the Internal High Speed 48 oscillator (HSI 48) calibration
/// value.
/// @note   The calibration is used to compensate for the variations in voltage
///         and temperature that influence the frequency of the internal HSI48
///         RC.
/// @note   This function can be called only when the CEN bit is reset.
/// @param  CRS_ReloadValue: specifies the HSI calibration trimming value.
///          This parameter must be a number between 0 and .
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_FrequencyErrorCounterReload(u32 CRS_ReloadValue)
{
    CRS->CFGR = (CRS->CFGR & ~CRS_CFGR_RELOAD) | CRS_ReloadValue;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief
/// @note   This function can be called only when the CEN bit is reset.
/// @param  CRS_ErrorLimitValue: specifies the HSI calibration trimming value.
///          This parameter must be a number between 0 and .
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_FrequencyErrorLimitConfig(u8 CRS_ErrorLimitValue)
{
    CRS->CFGR = (CRS->CFGR & ~CRS_CFGR_FELIM) | CRS_ErrorLimitValue;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief
/// @note   This function can be called only when the CEN bit is reset.
/// @param  CRS_Prescaler: specifies the HSI calibration trimming value.
///          This parameter can be one of the following values:
///            @arg CRS_SYNC_Div1:
///            @arg CRS_SYNC_Div2:
///            @arg CRS_SYNC_Div4:
///            @arg CRS_SYNC_Div8:
///            @arg CRS_SYNC_Div16:
///            @arg CRS_SYNC_Div32:
///            @arg CRS_SYNC_Div64:
///            @arg CRS_SYNC_Div128:
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_SyncPrescalerConfig(u32 CRS_Prescaler)
{
    CRS->CFGR = (CRS->CFGR & ~CRS_CFGR_DIV) | CRS_Prescaler;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief
/// @note   This function can be called only when the CEN bit is reset.
/// @param  CRS_Source: .
///          This parameter can be one of the following values:
///            @arg CRS_SYNCSource_GPIO:
///            @arg CRS_SYNCSource_LSE:
///            @arg CRS_SYNCSource_USB:
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_SynSourceConfig(u32 CRS_Source)
{
    CRS->CFGR = (CRS->CFGR & ~CRS_CFGR_SRC) | CRS_Source;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief
/// @note   This function can be called only when the CEN bit is reset.
/// @param  CRS_Polarity: .
///          This parameter can be one of the following values:
///            @arg CRS_SYNCPolarity_Rising:
///            @arg CRS_SYNCPolarity_Falling:
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_SynchronizationPolarityConfig(u32 CRS_Polarity)
{
    CRS->CFGR = (CRS->CFGR & ~CRS_CFGR_POL) | CRS_Polarity;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the Relaod value.
/// @param  None.
/// @retval The reload value
////////////////////////////////////////////////////////////////////////////////
u32 CRS_GetReloadValue()
{
    return (CRS->CFGR & CRS_CFGR_RELOAD);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the HSI48 Calibration value.
/// @param  None.
/// @retval The reload value
////////////////////////////////////////////////////////////////////////////////
u32 CRS_GetHSI48CalibrationValue()
{
    return ((CRS->CR & CRS_CR_TRIM) >> CRS_CR_TRIM_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the frequency error capture.
/// @param  None.
/// @retval The frequency error capture value
////////////////////////////////////////////////////////////////////////////////
u32 CRS_GetFrequencyErrorValue()
{
    return (CRS->ISR & CRS_ISR_FECAP);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the frequency error direction.
/// @param  None.
/// @retval The frequency error direction. The returned value can be one
///         of the following values:
///           - 0x00: Up counting
///           - 0x8000: Down counting
////////////////////////////////////////////////////////////////////////////////
u32 CRS_GetFrequencyErrorDirection()
{
    return (CRS->ISR & CRS_ISR_FEDIR);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified CRS interrupts.
/// @param  CRS_IT: specifies the RCC interrupt sources to be enabled or
/// disabled.
///          This parameter can be any combination of the following values:
///              @arg CRS_IT_SYNCOK:
///              @arg CRS_IT_SYNCWARN:
///              @arg CRS_IT_ERR:
///              @arg CRS_IT_ESYNC:
/// @param  state: new state of the specified CRS interrupts.
///          This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_ITConfig(u32 CRS_IT, FunctionalState state)
{
    (state) ? (CRS->CR |= CRS_IT) : (CRS->CR &= ~CRS_IT);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified CRS flag is set or not.
/// @param  CRS_FLAG: specifies the flag to check.
///          This parameter can be one of the following values:
///              @arg CRS_FLAG_SYNCOK:
///              @arg CRS_FLAG_SYNCWARN:
///              @arg CRS_FLAG_ERR:
///              @arg CRS_FLAG_ESYNC:
///              @arg CRS_FLAG_TRIMOVF:
///              @arg CRS_FLAG_SYNCERR:
///              @arg CRS_FLAG_SYNCMISS:
/// @retval The new state of CRS_FLAG (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
FlagStatus CRS_GetFlagStatus(u32 CRS_FLAG)
{
    return (CRS->ISR & CRS_FLAG) ? SET : RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the CRS specified FLAG.
/// @param  CRS_FLAG: specifies the flag to check.
///          This parameter can be one of the following values:
///              @arg CRS_FLAG_SYNCOK:
///              @arg CRS_FLAG_SYNCWARN:
///              @arg CRS_FLAG_ERR:
///              @arg CRS_FLAG_ESYNC:
///              @arg CRS_FLAG_TRIMOVF:
///              @arg CRS_FLAG_SYNCERR:
///              @arg CRS_FLAG_SYNCMISS:
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_ClearFlag(u32 CRS_FLAG)
{
    CRS->ICR |= (CRS_FLAG & 0x1F) ? CRS_ICR_ERR : CRS_FLAG;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified CRS IT pending bit is set or not.
/// @param  CRS_IT: specifies the IT pending bit to check.
///          This parameter can be one of the following values:
///              @arg CRS_IT_SYNCOK:
///              @arg CRS_IT_SYNCWARN:
///              @arg CRS_IT_ERR:
///              @arg CRS_IT_ESYNC:
///              @arg CRS_IT_TRIMOVF:
///              @arg CRS_IT_SYNCERR:
///              @arg CRS_IT_SYNCMISS:
/// @retval The new state of CRS_IT (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
ITStatus CRS_GetITStatus(u32 CRS_IT)
{
    return (CRS->ISR & CRS_IT) ? SET : RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the CRS specified IT pending bit.
/// @param  CRS_FLAG: specifies the IT pending bit to clear.
///          This parameter can be one of the following values:
///              @arg CRS_IT_SYNCOK:
///              @arg CRS_IT_SYNCWARN:
///              @arg CRS_IT_ERR:
///              @arg CRS_IT_ESYNC:
///              @arg CRS_IT_TRIMOVF:
///              @arg CRS_IT_SYNCERR:
///              @arg CRS_IT_SYNCMISS:
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRS_ClearITPendingBit(u32 CRS_IT)
{
    CRS->ICR |= (CRS_IT & 0x1F) ? CRS_IT : CRS_ICR_ERR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Get the CRS specified IT Source bit.
/// @param  CRS_IT: specifies the IT Source.
///          This parameter can be one of the following values:
///              @arg CRS_IT_SYNCOK:
///              @arg CRS_IT_SYNCWARN:
///              @arg CRS_IT_ERR:
///              @arg CRS_IT_ESYNC:
/// @retval ITStatus: SET or RESET
////////////////////////////////////////////////////////////////////////////////
ITStatus exCRS_GetITSource(u32 CRS_IT)
{
    return (CRS->CR & CRS_IT) ? SET : RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  CRS initialization function.
/// @param  pInitStruct: pointer to a CRS_InitTypeDef structure that
///         contains the configuration information for the specified CRS
///         peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exCRS_Init(CRS_InitTypeDef* pInitStruct)
{
    exRCC_APB1PeriphReset(RCC_APB1ENR_CRS);

    CRS->CFGR &= ~(CRS_CFGR_DIV | CRS_CFGR_SRC | CRS_CFGR_POL);
    CRS->CFGR |= pInitStruct->Prescaler;
    CRS->CFGR |= pInitStruct->Source;
    CRS->CFGR |= pInitStruct->Polarity;

    CRS->CFGR &= ~(CRS_CFGR_RELOAD | CRS_CFGR_FELIM);
    CRS->CFGR |= pInitStruct->ReloadValue;
    CRS->CFGR |= (pInitStruct->ErrorLimitValue << CRS_CFGR_FELIM_Pos);

    CRS->CR &= ~CRS_CR_TRIM;
    CRS->CR |= (pInitStruct->HSI48CalibrationValue << CRS_CR_TRIM_Pos);
}

/// @}

/// @}

/// @}

#endif
