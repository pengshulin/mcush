////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_WWDG.H
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE WWDG
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
#ifndef __HAL_WWDG_H
#define __HAL_WWDG_H

// Files includes  -------------------------------------------------------------
#include "types.h"
#include "mm32.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup WWDG_HAL
/// @brief WWDG HAL modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup WWDG_Exported_Types
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  WWDG_Prescaler
/// @anchor WWDG_Prescaler

typedef enum {
    WWDG_Prescaler_1 = WWDG_CFGR_WDGTB_1,
    WWDG_Prescaler_2 = WWDG_CFGR_WDGTB_2,
    WWDG_Prescaler_4 = WWDG_CFGR_WDGTB_4,
    WWDG_Prescaler_8 = WWDG_CFGR_WDGTB_8
} WWDG_Prescaler_Typedef;

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup WWDG_Exported_Variables
/// @{

#ifdef _HAL_WWDG_C_

#define GLOBAL
#else
#define GLOBAL extern
#endif

#undef GLOBAL

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup WWDG_Exported_Functions
/// @{

void       WWDG_DeInit(void);
void       WWDG_SetPrescaler(u32 prescaler);
void       WWDG_SetWindowValue(u8 windowValue);
void       WWDG_EnableIT(void);
void       WWDG_SetCounter(u8 count);
u32        WWDG_GetCounter(void);
void       WWDG_Enable(u8 count);
FlagStatus WWDG_GetFlagStatus(void);
void       WWDG_ClearFlag(void);

/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* __HAL_WWDG_H */
////////////////////////////////////////////////////////////////////////////////
