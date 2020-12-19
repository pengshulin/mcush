////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_CRC.C
/// @author   Y Shi
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE CRC FIRMWARE FUNCTIONS.
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
#define _HAL_CRC_C_

// Files includes  -------------------------------------------------------------
#include "HAL_crc.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CRC_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CRC_Exported_Functions
/// @{

#if defined(__MM3N1) || defined(__MM3O1) || defined(__MM0P1) || defined(__MM0Q1)

////////////////////////////////////////////////////////////////////////////////
/// @brief  Resets the CRC Data register (DR).
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRC_ResetDR()
{
    CRC->CR = CRC_CR_RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Computes the 32-bit CRC of a given data word(32-bit).
/// @param Data: data word(32-bit) to compute its CRC
/// @retval  32-bit CRC
////////////////////////////////////////////////////////////////////////////////
u32 CRC_CalcCRC(u32 data)
{
    CRC->DR = data;
    return (CRC->DR);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Computes the 32-bit CRC of a given buffer of data word(32-bit).
/// @param  pBuffer: pointer to the buffer containing the data to be computed
/// @param  BufferLength: length of the buffer to be computed
/// @retval  32-bit CRC
////////////////////////////////////////////////////////////////////////////////
u32 CRC_CalcBlockCRC(u32* pBuffer, u32 length)
{
    for (u32 i = 0; i < length; i++) {
        CRC->DR = pBuffer[i];
    }
    return (CRC->DR);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the current CRC value.
/// @param  None.
/// @retval  32-bit CRC
////////////////////////////////////////////////////////////////////////////////
u32 CRC_GetCRC()
{
    return (CRC->DR);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Stores a 8-bit data in the Independent Data(ID) register.
/// @param  IDValue: 8-bit value to be stored in the ID register
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CRC_SetIDRegister(u8 IDValue)
{
    CRC->IDR = IDValue;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the 8-bit data stored in the Independent Data(ID) register
/// @param  None.
/// @retval  8-bit value of the ID register
////////////////////////////////////////////////////////////////////////////////
u8 CRC_GetIDRegister()
{
    return (CRC->IDR);
}

/// @}

/// @}

/// @}

#endif
