////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_SQRT.C
/// @author   Z Yan
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE SQRT FIRMWARE FUNCTIONS.
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
#define _HAL_SQRT_C_

// Files includes  -------------------------------------------------------------
#include "HAL_sqrt.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup SQRT_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup SQRT_Exported_Functions
/// @{

#if defined(__MM0P1)

////////////////////////////////////////////////////////////////////////////////
/// @brief  Write data to square data register
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void HWRoot_Set(u32 x)
{
    SQRT->SQR = x;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Get square result
/// @retval  HWROOT->ROOT
////////////////////////////////////////////////////////////////////////////////
u32 HWRoot_Get()
{
    return SQRT->RESULT;
}

///@}

///@}

///@}

#endif
