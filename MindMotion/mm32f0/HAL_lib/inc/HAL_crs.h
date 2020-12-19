////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_CRS.H
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE CRS
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
#ifndef __HAL_CRS_H
#define __HAL_CRS_H


// Files includes  -------------------------------------------------------------
#include "types.h"
#include "mm32.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup CRS_HAL
/// @brief CRS HAL modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup GPIO_Exported_Types
/// @{


#if defined(__MM0N1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  CRS Init structure definition
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    u32 Prescaler;
    u32 Source;
    u32 Polarity;
    u32 ReloadValue;
    u32 ErrorLimitValue;
    u32 HSI48CalibrationValue;
} CRS_InitTypeDef;

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup CRS_Exported_Constants
/// @{

    #define CRS_SYNC_Div1   ((u32)0x00000000)                                   ///< Synchro Signal not divided
    #define CRS_SYNC_Div2   ((u32)0x01000000)                                   ///< Synchro Signal divided by 2
    #define CRS_SYNC_Div4   ((u32)0x02000000)                                   ///< Synchro Signal divided by 4
    #define CRS_SYNC_Div8   ((u32)0x03000000)                                   ///< Synchro Signal divided by 8
    #define CRS_SYNC_Div16  ((u32)0x04000000)                                   ///< Synchro Signal divided by 16
    #define CRS_SYNC_Div32  ((u32)0x05000000)                                   ///< Synchro Signal divided by 32
    #define CRS_SYNC_Div64  ((u32)0x06000000)                                   ///< Synchro Signal divided by 64
    #define CRS_SYNC_Div128 CRS_CFGR_DIV                                        ///< Synchro Signal divided by 128

    #define CRS_SYNCSource_GPIO     ((u32)0x00000000)                           ///< Synchro Signal soucre GPIO
    #define CRS_SYNCSource_LSE      ((u32)0x10000000)                           ///< Synchro Signal source LSE
    #define CRS_SYNCSource_USB      ((u32)0x20000000)                           ///< Synchro Signal source USB SOF
    #define CRS_SYNCPolarity_Rising  ((u32)0x00000000)                          ///< Synchro Active on rising edge
    #define CRS_SYNCPolarity_Falling CRS_CFGR_POL                               ///< Synchro Active on falling edge
    #define CRS_ERRORLIMIT_DEFAULT ((u32)0x00000022)                            ///< Default Frequency error limit
/// @}


////////////////////////////////////////////////////////////////////////////////
/// @defgroup CRS_Exported_Variables
/// @{
#ifdef _HAL_CRS_C_

#define GLOBAL
#else
#define GLOBAL extern
#endif

#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup CRS_Exported_Functions
/// @{

void       CRS_DeInit(void);
void       CRS_AdjustHSI48CalibrationValue(u8 Value);
void       CRS_FrequencyErrorCounterCmd(FunctionalState state);
void       CRS_AutomaticCalibrationCmd(FunctionalState state);
void       CRS_SoftwareSynchronizationGenerate(void);
void       CRS_FrequencyErrorCounterReload(u32 CRS_ReloadValue);
void       CRS_FrequencyErrorLimitConfig(u8 CRS_ErrorLimitValue);
void       CRS_SyncPrescalerConfig(u32 CRS_Prescaler);
void       CRS_SynSourceConfig(u32 CRS_Source);
void       CRS_SynchronizationPolarityConfig(u32 CRS_Polarity);
u32        CRS_GetReloadValue(void);
u32        CRS_GetHSI48CalibrationValue(void);
u32        CRS_GetFrequencyErrorValue(void);
u32        CRS_GetFrequencyErrorDirection(void);
void       CRS_ITConfig(u32 CRS_IT, FunctionalState state);
void       CRS_ClearFlag(u32 CRS_FLAG);
void       CRS_ClearITPendingBit(u32 CRS_IT);
void       exCRS_Init(CRS_InitTypeDef* CRS_InitStruct);

FlagStatus CRS_GetFlagStatus(u32 CRS_FLAG);
ITStatus   CRS_GetITStatus(u32 CRS_IT);
ITStatus   exCRS_GetITSource(u32 CRS_IT);

/// @}

/// @}

/// @}

#endif

////////////////////////////////////////////////////////////////////////////////
#endif /* __HAL_CRS_H */
////////////////////////////////////////////////////////////////////////////////

