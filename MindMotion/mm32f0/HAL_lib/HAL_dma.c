////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_DMA.C
/// @author   QR Wang
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE DMA FIRMWARE FUNCTIONS.
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
#define _HAL_DMA_C_

// Files includes  -------------------------------------------------------------
#include "types.h"
#include "HAL_dma.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup DMA_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup DMA_Exported_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the DMA Channeln registers to their default reset
///         values.
/// @param  channel can be 1 to 7 for DMA1 to select the DMA Channel.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DMA_DeInit(DMA_Channel_TypeDef* channel)
{
    channel->CCR &= ~DMA_CCR_EN;
    channel->CCR   = 0;
    channel->CNDTR = 0;
    channel->CPAR  = 0;
    channel->CMAR  = 0;

    DMA1->IFCR |= (u32)0x0F << (((*(u32*)&channel & (u32)0xff) - 8) / 5);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the DMA Channeln according to the specified
///         parameters in the pInitStruct.
/// @param  channel can be 1 to 7 for DMA1 to select the DMA Channel.
/// @param  pInitStruct: pointer to a DMA_InitTypeDef structure that
///         contains the configuration information for the specified DMA
///         Channel.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DMA_Init(DMA_Channel_TypeDef* channel, DMA_InitTypeDef* pInitStruct)
{
    MODIFY_REG(
        channel->CCR,
        (DMA_CCR_DIR | DMA_CCR_CIRC | DMA_CCR_PINC | DMA_CCR_MINC | DMA_CCR_PSIZE | DMA_CCR_MSIZE | DMA_CCR_PL | DMA_CCR_M2M),
        ((u32)pInitStruct->DIR | (u32)pInitStruct->Mode | (u32)pInitStruct->PeripheralInc |
         (u32)pInitStruct->MemoryInc | (u32)pInitStruct->PeripheralDataSize | (u32)pInitStruct->MemoryDataSize |
         (u32)pInitStruct->Priority | (u32)pInitStruct->M2M));

#if defined(__MM0P1) || defined(__MM0Q1)
    MODIFY_REG(channel->CCR, DMA_CCR_ARE, pInitStruct->AutoReload);
#endif
    channel->CNDTR = pInitStruct->BufferSize;
    channel->CPAR  = pInitStruct->PeripheralBaseAddr;
    channel->CMAR  = pInitStruct->MemoryBaseAddr;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each pInitStruct member with its default value.
/// @param  pInitStruct : pointer to a DMA_InitTypeDef structure which will
///         be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DMA_StructInit(DMA_InitTypeDef* pInitStruct)
{
    pInitStruct->PeripheralBaseAddr = 0;
    pInitStruct->MemoryBaseAddr     = 0;
    pInitStruct->DIR                = DMA_DIR_PeripheralSRC;
    pInitStruct->BufferSize         = 0;
    pInitStruct->PeripheralInc      = DMA_PeripheralInc_Disable;
    pInitStruct->MemoryInc          = DMA_MemoryInc_Disable;
    pInitStruct->PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    pInitStruct->MemoryDataSize     = DMA_MemoryDataSize_Byte;
    pInitStruct->Mode               = DMA_Mode_Normal;
    pInitStruct->Priority           = DMA_Priority_Low;
    pInitStruct->M2M                = DMA_M2M_Disable;

#if defined(__MM0P1) || defined(__MM0Q1)
    pInitStruct->AutoReload         = DMA_Auto_Reload_Disable;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified DMA Channeln.
/// @param  channel: where n can be 1 to 7 for DMA1 to select the DMA Channel.
/// @param  state: new state of the DMA Channeln.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DMA_Cmd(DMA_Channel_TypeDef* channel, FunctionalState state)
{
    MODIFY_REG(channel->CCR, DMA_CCR_EN, state << DMA_CCR_EN_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified DMA Channeln interrupts.
/// @param  channel: where n can be 1 to 7 for DMA1 to select the DMA Channel.
/// @param  it: specifies the DMA interrupts sources to be enabled
///         or disabled.
///         This parameter can be any combination of the following values:
///    @arg DMA_IT_TC:  Transfer complete interrupt mask
///    @arg DMA_IT_HT:  Half transfer interrupt mask
///    @arg DMA_IT_TE:  Transfer error interrupt mask
/// @param  state: new state of the specified DMA interrupts.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DMA_ITConfig(DMA_Channel_TypeDef* channel, u32 it, FunctionalState state)
{
    (state) ? (channel->CCR |= it) : (channel->CCR &= ~it);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the number of remaining data units in the current
///         DMA Channeln transfer.
/// @param  channel: where n can be 1 to 7 for DMA1 to select the DMA Channel.
/// @retval  The number of remaining data units in the current DMA Channeln
///         transfer.
////////////////////////////////////////////////////////////////////////////////
u16 DMA_GetCurrDataCounter(DMA_Channel_TypeDef* channel)
{
    return channel->CNDTR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified DMA Channeln flag is set or not.
/// @param  flag: specifies the flag to check.
///         This parameter can be one of the following values:
///    @arg DMA1_FLAG_GLn: DMA1 Channeln global flag(n = 1..7).
///    @arg DMA1_FLAG_TCn: DMA1 Channeln transfer complete flag(n = 1..7).
///    @arg DMA1_FLAG_HTn: DMA1 Channeln half transfer flag(n = 1..7).
///    @arg DMA1_FLAG_TEn: DMA1 Channeln transfer error flag(n = 1..7).
/// @retval  The new state of DMAy_FLAG (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
FlagStatus DMA_GetFlagStatus(u32 flag)
{
    return (DMA1->ISR & flag) ? SET : RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the DMA Channeln's pending flags.
/// @param  flag: specifies the flag to clear.
///         This parameter can be any combination (for the same DMA) of the
///         following values:
///    @arg DMA1_FLAG_GLn: DMA1 Channeln global flag(n = 1..7).
///    @arg DMA1_FLAG_TCn: DMA1 Channeln transfer complete flag(n = 1..7).
///    @arg DMA1_FLAG_HTn: DMA1 Channeln half transfer flag(n = 1..7).
///    @arg DMA1_FLAG_TEn: DMA1 Channeln transfer error flag(n = 1..7).
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DMA_ClearFlag(u32 flag)
{
    DMA1->IFCR = flag;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified DMA Channeln interrupt has occurred or
/// not.
/// @param  it: specifies the DMA interrupt source to check.
///         This parameter can be one of the following values:
///    @arg DMA1_IT_GLn: DMA1 Channeln global interrupt(n = 1..7).
///    @arg DMA1_IT_TCn: DMA1 Channeln transfer complete interrupt(n = 1..7).
///    @arg DMA1_IT_HTn: DMA1 Channeln half transfer interrupt(n = 1..7).
///    @arg DMA1_IT_TEn: DMA1 Channeln transfer error interrupt(n = 1..7).
/// @retval  The new state of DMAy_IT (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
ITStatus DMA_GetITStatus(u32 it)
{
    return (DMA1->ISR & it) ? SET : RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the DMA Channeln's interrupt pending bits.
/// @param  it: specifies the DMA interrupt pending bit to clear.
///         This parameter can be any combination (for the same DMA) of the
///         following values:
///    @arg DMA1_IT_GLn: DMA1 Channeln global interrupt(n = 1..7).
///    @arg DMA1_IT_TCn: DMA1 Channeln transfer complete interrupt(n = 1..7).
///    @arg DMA1_IT_HTn: DMA1 Channeln half transfer interrupt(n = 1..7).
///    @arg DMA1_IT_TEn: DMA1 Channeln transfer error interrupt(n = 1..7).
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DMA_ClearITPendingBit(u32 it)
{
    DMA1->IFCR = it;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the DMA Channeln's Peripheral address.
/// @param  channel : where n can be 1 to 7 for DMA1 to select the DMA Channel.
/// @param  address : DMA Peripheral address.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exDMA_SetPeripheralAddress(DMA_Channel_TypeDef* channel, u32 address)
{
    channel->CPAR = address;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the DMA Channeln's Peripheral address.
/// @param  channel : where n can be 1 to 7 for DMA1 to select the DMA Channel.
/// @param  length : Transmit lengths.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exDMA_SetTransmitLen(DMA_Channel_TypeDef* channel, u16 length)
{
    channel->CNDTR = length;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the DMA Channeln's Peripheral address.
/// @param  channel : where n can be 1 to 7 for DMA1 to select the DMA Channel.
/// @param  address : DMA memery address.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exDMA_SetMemoryAddress(DMA_Channel_TypeDef* channel, u32 address)
{
    channel->CMAR = address;
}

/// @}

/// @}

/// @}
