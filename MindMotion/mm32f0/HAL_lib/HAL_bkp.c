////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_BKP.C
/// @author   D Chen
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE BKP FIRMWARE FUNCTIONS.
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
#define _HAL_BKP_C_

// Files includes  -------------------------------------------------------------
#include "types.h"
#include "HAL_pwr.h"
#include "HAL_rcc.h"
#include "HAL_bkp.h"

#include "common.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup BKP_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup BKP_Exported_Functions
/// @{

#if defined(__MM3N1) || defined(__MM3O1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the BKP peripheral registers to their default reset
///         values.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void BKP_DeInit(void)
{
    RCC_BackupResetCmd(ENABLE);
    RCC_BackupResetCmd(DISABLE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the Tamper Pin active level.
/// @param  BKP_TamperPinLevel: specifies the Tamper Pin active level.
///   This parameter can be one of the following values:
///     @arg BKP_TamperPinLevel_High: Tamper pin active on high level
///     @arg BKP_TamperPinLevel_Low: Tamper pin active on low level
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void BKP_TamperPinLevelConfig(BKPTPAL_Typedef BKP_TamperPinLevel)
{
    BKP->CR = BKP_TamperPinLevel;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the Tamper Pin activation.
/// @param  state: new state of the Tamper Pin activation.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void BKP_TamperPinCmd(FunctionalState state)
{
    (state) ? SET_BIT(BKP->CR, BKP_CR_TPE) : CLEAR_BIT(BKP->CR, BKP_CR_TPE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the Tamper Pin Interrupt.
/// @param  state: new state of the Tamper Pin Interrupt.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void BKP_ITConfig(FunctionalState state)
{
    (state) ? SET_BIT(BKP->CSR, BKP_CSR_TPIE) : CLEAR_BIT(BKP->CSR, BKP_CSR_TPIE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Select the RTC output source to output on the Tamper pin.
/// @param  BKP_RTCOutputSource: specifies the RTC output source.
///   This parameter can be one of the following values:
///     @arg BKP_RTCOutputSource_None: no RTC output on the Tamper pin.
///     @arg BKP_RTCOutputSource_CalibClock: output the RTC clock with frequency
///                                          divided by 64 on the Tamper pin.
///     @arg BKP_RTCOutputSource_Alarm: output the RTC Alarm pulse signal on
///                                     the Tamper pin.
///     @arg BKP_RTCOutputSource_Second: output the RTC Second pulse signal on
///                                      the Tamper pin.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void BKP_RTCOutputConfig(BKPRTCOUTPUTSRC_Typedef BKP_RTCOutputSource)
{
    MODIFY_REG(BKP->RTCCR, BKP_RTCCR_CCO | BKP_RTCCR_ASOE | BKP_RTCCR_ASOS, BKP_RTCOutputSource);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets RTC Clock Calibration value.
/// @param  CalibrationValue: specifies the RTC Clock Calibration value.
///   This parameter must be a number between 0 and 0x7F.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void BKP_SetRTCCalibrationValue(u8 CalibrationValue)
{
    MODIFY_REG(BKP->RTCCR, BKP_RTCCR_CAL, CalibrationValue);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the Tamper Pin Event flag is set or not.
/// @param  None.
/// @retval State: The new state of the Tamper Pin Event flag (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
FlagStatus BKP_GetFlagStatus(void)
{
    return ((BKP->CSR & BKP_CSR_TEF) ? SET : RESET);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears Tamper Pin Event pending flag.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void BKP_ClearFlag(void)
{
    SET_BIT(BKP->CSR, BKP_CSR_CTE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the Tamper Pin Interrupt has occurred or not.
/// @param  None.
/// @retval State: The new state of the Tamper Pin Interrupt (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
ITStatus BKP_GetITStatus(void)
{
    return ((BKP->CSR & BKP_CSR_TIF) ? SET : RESET);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears Tamper Pin Interrupt pending bit.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void BKP_ClearITPendingBit(void)
{
    SET_BIT(BKP->CSR, BKP_CSR_CTI);
}
#endif

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Writes user data to the specified Data Backup Register.
/// @param  BKP_DR: specifies the Data Backup Register.
///   This parameter can be BKP_DRx where x:[1, 10]
/// @param  Data: data to write
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void BKP_WriteBackupRegister(BKPDR_Typedef BKP_DR, u16 Data)
{
    *(u16*)(BKP_BASE + BKP_DR) = Data;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reads data from the specified Data Backup Register.
/// @param  BKP_DR: specifies the Data Backup Register.
///   This parameter can be BKP_DRx where x:[1, 10]
/// @retval Data: The content of the specified Data Backup Register
////////////////////////////////////////////////////////////////////////////////
u16 BKP_ReadBackupRegister(BKPDR_Typedef BKP_DR)
{
    return (*(u16*)(BKP_BASE + BKP_DR));
}

////////////////////////////////////////////////////////////////////////////////
//          Extended function interface
////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the BKP peripheral, enable access to the backup
/// registers.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exBKP_Init(void)
{
#if defined(__MM3N1) || defined(__MM3O1)
    COMMON_EnableIpClock(emCLOCK_PWR);
    COMMON_EnableIpClock(emCLOCK_BKP);
#endif

#if defined(__MM0N1) || defined(__MM0P1)
    COMMON_EnableIpClock(emCLOCK_PWR);

#endif
    PWR->CR |= PWR_CR_DBP;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Writes user data to the specified Data Backup Register immediately.
/// @param  BKP_DR: specifies the Data Backup Register.
///   This parameter can be BKP_DRx where x:[1, 10]
/// @param  Data: data to write
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exBKP_ImmWrite(BKPDR_Typedef BKP_DR, u16 dat)
{
    PWR->CR |= PWR_CR_DBP;
    *(u16*)(BKP_BASE + BKP_DR) = dat;
    PWR->CR &= ~PWR_CR_DBP;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reads data from the specified Data Backup Register immediately.
/// @param  BKP_DR: specifies the Data Backup Register.
///   This parameter can be BKP_DRx where x:[1, 10]
/// @retval Data: The content of the specified Data Backup Register
////////////////////////////////////////////////////////////////////////////////
u16 exBKP_ImmRead(BKPDR_Typedef BKP_DR)
{
    PWR->CR |= PWR_CR_DBP;
    u16 dat = (*(u16*)(BKP_BASE + BKP_DR));
    PWR->CR &= ~PWR_CR_DBP;
    return dat;
}
#endif

/// @}

/// @}

/// @}
