////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_DIV.H
/// @author   Z Yan
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE DIV
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
#ifndef __HAL_DIV_H
#define __HAL_DIV_H

// Files includes  -------------------------------------------------------------
#include "types.h"
#include "mm32.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup DIV_HAL
/// @brief DIV HAL modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup DIV_Exported_Types
/// @{

#if defined(__MM0P1) || defined(__MM0Q1)

////////////////////////////////////////////////////////////////////////////////
/// @defgroup CRS_Exported_Constants
/// @{

#define SET_HWDivider(x, y)     HWDIV->DVD = x;   HWDIV->DVS = y;
#define GET_HWDivider(void)     HWDIV->QUOT;
#define GET_Divider(dvd, dvs)   (dvd / dvs)

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup DIV_Exported_Variables
/// @{

#ifdef _HAL_DIV_C_

#define GLOBAL
#else
#define GLOBAL extern
#endif

#undef GLOBAL

/// @}


void HWDivider_Init(bool usign, bool zero);

u32 Divider_Calc(u32 dvd, u32 dvs);
s32 HWDivider_Calc(u32 dvd, u32 dvs);

// HWDivider_Init

#endif

/// @}

/// @}

/// @}


////////////////////////////////////////////////////////////////////////////////
#endif /* __HAL_DIV_H */
////////////////////////////////////////////////////////////////////////////////
