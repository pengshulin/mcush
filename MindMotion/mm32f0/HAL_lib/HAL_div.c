////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_DIV.C
/// @author   Z Yan
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE DIV FIRMWARE FUNCTIONS.
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
#define _HAL_DIV_C_

// Files includes  -------------------------------------------------------------
#include "HAL_div.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup DIV_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup DIV_Exported_Functions
/// @{

#if defined(__MM0P1) || defined(__MM0Q1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Hardware divider unsigned mode initial.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void HWDivider_Init(bool usign, bool zero)
{
    DIV->CR = (usign ? DIV_CR_USIGN : 0) | (zero ? DIV_CR_OVFE : 0);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Calculate by hardware
/// @param  DVD: Dividend data
/// @param  DVS: Divisor data
/// @retval  HWDIV->QUOTR
////////////////////////////////////////////////////////////////////////////////
s32 HWDivider_Calc(u32 dvd, u32 dvs)
{
    DIV->DVDR = dvd;
    DIV->DVSR = dvs;

    // overflow
    if (DIV->SR & DIV_SR_OVF) {
        return 0xffffffff;
    }
    return DIV->QUOTR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Calculate by software
/// @param  DVD: Dividend data
/// @param  DVS: Divisor data
/// @retval  Divider result
////////////////////////////////////////////////////////////////////////////////
u32 Divider_Calc(u32 dvd, u32 dvs)
{
    return dvd / dvs;
}

///@}

///@}

///@}

#endif
