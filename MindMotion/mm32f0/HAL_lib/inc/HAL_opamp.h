////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_OPAMP.H
/// @author   PX Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE OPAMP
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
#ifndef __HAL_OPAMP_H
#define __HAL_OPAMP_H

// Files includes  -------------------------------------------------------------
#include "types.h"
#include "mm32.h"


////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup OPAMP_HAL
/// @brief OPAMP HAL modules
/// @{

#if defined(__MM0P1)
////////////////////////////////////////////////////////////////////////////////
/// @defgroup OPAMP_Exported_Types
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief OPAMP_OutputLevel
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    OPAMP1 = OPAMP_CSR_OP1, //  OPAMP1_EN,
    OPAMP2 = OPAMP_CSR_OP2,
    OPAMP3 = OPAMP_CSR_OP3,
    OPAMP4 = OPAMP_CSR_OP4
} emOPAMP_OutEn;
/// @}

////////////////////////////////////////////////////////////////////////////////
///@defgroup OPAMP_Exported_Variables
/// @{
#ifdef _HAL_OPAMP_C_

#define GLOBAL
#else
#define GLOBAL extern
#endif

#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup OPAMP_Exported_Functions
/// @{

void OPAMP_Configure(emOPAMP_OutEn Opx, FunctionalState state);

/// @}
#endif

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* __HAL_OPAMP_H */
////////////////////////////////////////////////////////////////////////////////
