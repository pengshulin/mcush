////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_DBG.H
/// @author   PX Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE DBG
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
#ifndef __HAL_DBG_H
#define __HAL_DBG_H

// Files includes  -------------------------------------------------------------
#include "mm32.h"
#include "types.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup DBG_HAL
/// @brief DBG HAL modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup DBG_Exported_Types
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup DIV_Exported_Variables
/// @{
#ifdef _HAL_DBG_C_

#define GLOBAL
#else
#define GLOBAL extern
#endif

#undef GLOBAL
/// @}



////////////////////////////////////////////////////////////////////////////////
/// @defgroup DBG_Exported_Functions
/// @{
void DBGMCU_Configure(u32 periph, FunctionalState NewState);

/// @}


/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* __HAL_DBG_H */
////////////////////////////////////////////////////////////////////////////////
