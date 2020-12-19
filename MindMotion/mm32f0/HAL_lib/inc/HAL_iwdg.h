////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_IWDG.H
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE IWDG
///           FIRMWARE LIBRARY.
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
#ifndef __HAL_IWDG_H
#define __HAL_IWDG_H

// Files includes  -------------------------------------------------------------
#include "types.h"
#include "mm32.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup IWDG_HAL
/// @brief IWDG HAL modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup IWDG_Exported_Constants
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  IWDG prescaler
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    IWDG_Prescaler_4   = IWDG_PR_PR_DIV4,
    IWDG_Prescaler_8   = IWDG_PR_PR_DIV8,
    IWDG_Prescaler_16  = IWDG_PR_PR_DIV16,
    IWDG_Prescaler_32  = IWDG_PR_PR_DIV32,
    IWDG_Prescaler_64  = IWDG_PR_PR_DIV64,
    IWDG_Prescaler_128 = IWDG_PR_PR_DIV128,
    IWDG_Prescaler_256 = IWDG_PR_PR_DIV256
} IWDGPrescaler_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  IWDG flag
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    IWDG_FLAG_PVU = 0x0001,  // IWDG prescaler value update flag
    IWDG_FLAG_RVU = 0x0002   // IWDG counter reload value update flag
} IWDGFlag_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  Write access to IWDG_PR and IWDG_RLR registers
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    IWDG_WriteAccess_Enable  = 0x5555,  // Enable write
    IWDG_WriteAccess_Disable = 0x0000   // Disable write
} IWDGWriteAccess_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  IWDG Key Reload
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    KR_KEY_Reload = 0xAAAA,  // Reload value
    KR_KEY_Enable = 0xCCCC   // Start IWDG
} IWDGKey_TypeDef;

#if defined(__MM3O1) || defined(__MM0P1) || defined(__MM0Q1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  IWDG Overflow Configration
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    IWDG_Overflow_Reset     = 0,               //
    IWDG_Overflow_Interrupt = IWDG_CR_IRQSEL   //
} IWDGOverflowConfig_TypeDef;
#endif

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup IWDG_Exported_Variables
/// @{
#ifdef _HAL_IWDG_C_
#define GLOBAL

#else
#define GLOBAL extern
#endif


#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup IWDG_Exported_Functions
/// @{
FlagStatus IWDG_GetFlagStatus(u16 IWDG_FLAG);

void IWDG_WriteAccessCmd(u16 IWDG_WriteAccess);
void IWDG_SetPrescaler(u8 IWDG_Prescaler);
void IWDG_SetReload(u16 Reload);
u32 IWDG_GetReload(void);
void IWDG_ReloadCounter(void);
void IWDG_Enable(void);
void PVU_CheckStatus(void);
void RVU_CheckStatus(void);

#if defined(__MM3O1) || defined(__MM0P1) || defined(__MM0Q1)
void IWDG_OverflowConfig(IWDGOverflowConfig_TypeDef IWDG_Overflow_Config);
void IWDG_ClearITPendingBit(void);
#endif

/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* __HAL_IWDG_H */
////////////////////////////////////////////////////////////////////////////////
