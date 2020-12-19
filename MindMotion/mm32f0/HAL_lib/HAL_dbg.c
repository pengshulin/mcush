////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_DBG.C
/// @author   PX Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE DBG FIRMWARE FUNCTIONS.
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
#define _HAL_DBG_C

// Files includes  -------------------------------------------------------------
#include "types.h"
#include "HAL_dbg.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup DBG_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup DBG_Exported_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified DBG peripheral.
/// @param  periph: DBG peripheral.
/// @param  state: new state of the specified DBG peripheral.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DBGMCU_Configure(u32 periph, FunctionalState state)
{
    (state) ? (DBGMCU->CR |= periph) : (DBGMCU->CR &= ~periph);
}

/// @}

/// @}

/// @}
