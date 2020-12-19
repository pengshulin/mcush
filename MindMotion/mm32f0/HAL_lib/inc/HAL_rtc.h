////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_RTC.H
/// @author   C Yuan
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE RTC
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
#ifndef __HAL_RTC_H
#define __HAL_RTC_H

// Includes  -------------------------------------------------------------------
#include "types.h"
#include "mm32.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup RTC_HAL
/// @brief RTC HAL modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup RTC_Exported_Types
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief RTC_interrupts_define
////////////////////////////////////////////////////////////////////////////////
#if defined(__MM3N1)
typedef enum {
    RTC_IT_OW  = RTC_CR_OWIE,   ///< Overflow interrupt
    RTC_IT_ALR = RTC_CR_ALRIE,  ///< Alarm interrupt
    RTC_IT_SEC = RTC_CR_SECIE   ///< Second interrupt
} RTC_IT_TypeDef;
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief RTC_interrupts_flags
////////////////////////////////////////////////////////////////////////////////
#if defined(__MM3N1)
typedef enum {
    RTC_FLAG_RTOFF = RTC_CSR_RTOFF,  ///< RTC Operation OFF flag
    RTC_FLAG_RSF   = RTC_CSR_RSF,    ///< Registers Synchronized flag
    RTC_FLAG_OW    = RTC_CSR_OWF,    ///< Overflow flag
    RTC_FLAG_ALR   = RTC_CSR_ALRF,   ///< Alarm flag
    RTC_FLAG_SEC   = RTC_CSR_SECF    ///< Second flag
} RTC_FLAG_TypeDef;
#endif
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup RTC_Exported_Constants
/// @{

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup RTC_Exported_Variables
/// @{

#ifdef _HAL_RTC_C_

#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL  bool accessRTC;


#undef GLOBAL

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup RTC_Exported_Functions
/// @{
#if defined(__MM3N1)
void RTC_ITConfig(RTC_IT_TypeDef it, FunctionalState state);
void RTC_ClearFlag(RTC_FLAG_TypeDef flag);
void RTC_ClearITPendingBit(RTC_IT_TypeDef it);
void RTC_EnterConfigMode(void);
void RTC_SetCounter(u32 count);
void RTC_SetPrescaler(u32 prescaler);
void RTC_SetAlarm(u32 alarm);
void RTC_ExitConfigMode(void);
void RTC_WaitForLastTask(void);
void RTC_WaitForSynchro(void);

u32 RTC_GetCounter(void);
u32 RTC_GetDivider(void);

FlagStatus RTC_GetFlagStatus(RTC_FLAG_TypeDef flag);
ITStatus   RTC_GetITStatus(RTC_IT_TypeDef it);
#endif

/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* __HAL_RTC_H */
////////////////////////////////////////////////////////////////////////////////
