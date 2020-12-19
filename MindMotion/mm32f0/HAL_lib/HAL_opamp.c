////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_OPAMP.C
/// @author   PX Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE OPAMP FIRMWARE FUNCTIONS.
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
#define _HAL_OPAMP_C

// Files includes  -------------------------------------------------------------
#include "types.h"
#include "HAL_opamp.h"



////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup OPAMP_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup OPAMP_Exported_Functions
/// @{

#if defined(__MM0P1)

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified OPAMP peripheral.
/// @param  Opx: the selected OPAMP.
///         This parameter can be OPx where x can be 1 to 4
/// @param  state: new state of the OPAMP peripheral.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void OPAMP_Configure(emOPAMP_OutEn Opx, FunctionalState state)
{
    (state != DISABLE) ? (OPAMP->CSR |= Opx) : (OPAMP->CSR &= ~Opx);
}
#endif
/// @}

/// @}

/// @}
