////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_SPI.C
/// @author   Z Yan
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE SPI FIRMWARE FUNCTIONS.
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
#define _HAL_SPI_C_

// Files includes  -------------------------------------------------------------
#include "HAL_spi.h"
#include "HAL_rcc.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup SPI_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
///@addtogroup SPI_Exported_Functions
///@{

////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the SPIx peripheral registers to their
///   default reset values.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_DeInit(SPI_TypeDef* SPIx)
{
    switch (*(u32*)&SPIx) {
        case (u32)SPI2:  // SPI2_BASE:
            RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, DISABLE);
            break;
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0P1)
        case (u32)SPI1:  // SPI1_BASE:
            RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);
            break;
#endif
        default: break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the SPIx peripheral according to the specified
///   parameters in the pInitStruct .
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param pInitStruct: pointer to a SPI_InitTypeDef structure
///   that contains the configuration information for the
///   specified SPI peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* pInitStruct)
{
    //  MODIFY_REG(SPIx->GCTL, GCTL_Mask, ((u16)SPI_DataSize_8b
    //                                     | (u16)pInitStruct->SPI_NSS |
    //(u16)pInitStruct->SPI_Mode));
    if (pInitStruct->SPI_DataWidth == 32) {
        SET_BIT(SPIx->GCR, SPI_GCR_DWSEL);
        MODIFY_REG(SPIx->GCR, SPI_GCR_NSS, pInitStruct->SPI_NSS);
        MODIFY_REG(SPIx->GCR, SPI_GCR_MODE, pInitStruct->SPI_Mode);
//        MODIFY_REG(SPIx->GCR, GCR_Mask, (SPI_GCR_DWSEL | (u16)pInitStruct->SPI_NSS | (u16)pInitStruct->SPI_Mode));
    }
    else {
        MODIFY_REG(SPIx->GCR, SPI_GCR_NSS, pInitStruct->SPI_NSS);
        MODIFY_REG(SPIx->GCR, SPI_GCR_MODE, pInitStruct->SPI_Mode);

//        MODIFY_REG(SPIx->GCR, GCR_Mask, ((u16)pInitStruct->SPI_NSS | (u16)pInitStruct->SPI_Mode));
    }
    MODIFY_REG(SPIx->CCR, SPI_CCR_LSBFE, pInitStruct->SPI_FirstBit);
    MODIFY_REG(SPIx->CCR, SPI_CCR_CPOL, pInitStruct->SPI_CPOL);
    MODIFY_REG(SPIx->CCR, SPI_CCR_CPHA, pInitStruct->SPI_CPHA);
    SET_BIT(SPIx->CCR, SPI_CCR_SPILEN);


//    MODIFY_REG(SPIx->CCR, CCR_Mask,
//               ((u16)pInitStruct->SPI_FirstBit | (u16)pInitStruct->SPI_CPOL | (u16)pInitStruct->SPI_CPHA | SPI_CCR_SPILEN));

    //  if ((pInitStruct->SPI_DataWidth)!=SPI_DataWidth_8b)
    //      MODIFY_REG(SPIx->CCR,0x000C,((u16)SPI_FirstBit_LSB |
    // SPI_CCTL_SPILEN));

    MODIFY_REG(SPIx->BRR, BRR_Mask, pInitStruct->SPI_BaudRatePrescaler);

    MODIFY_REG(SPIx->ECR, ECR_Mask, pInitStruct->SPI_DataWidth);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each pInitStruct member with its default value.
/// @param pInitStruct: pointer to a SPI_InitTypeDef structure
///   which will be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_StructInit(SPI_InitTypeDef* pInitStruct)
{
    pInitStruct->SPI_Mode = SPI_Mode_Slave;
    // pInitStruct->SPI_DataSize = SPI_DataSize_8b;
    pInitStruct->SPI_DataWidth         = 8;
    pInitStruct->SPI_CPOL              = SPI_CPOL_Low;
    pInitStruct->SPI_CPHA              = SPI_CPHA_1Edge;
    pInitStruct->SPI_NSS               = SPI_NSS_Soft;
    pInitStruct->SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    pInitStruct->SPI_FirstBit          = SPI_FirstBit_MSB;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified SPI peripheral.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param newState: new state of the SPIx peripheral.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState newState)
{
    (newState) ? SET_BIT(SPIx->GCR, SPI_GCR_SPIEN) : CLEAR_BIT(SPIx->GCR, SPI_GCR_SPIEN);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified SPI interrupts.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param interrupt: specifies the SPI interrupt sources to be
///   enabled or disabled.
///   This parameter can be one of the following values:
/// @arg SPI_IT_TXEPT: Transmitter empty interrupt
/// @arg SPI_IT_RXFULL: RX FIFO full interrupt
/// @arg SPI_IT_RXMATCH: Receive data match the RXDNR number interrupt
/// @arg SPI_IT_RXOERR: Receive overrun error interrupt
/// @arg SPI_IT_UNDERRUN: underrun interrupt
/// @arg SPI_IT_RX: Receive data available interrupt
/// @arg SPI_IT_TX: Transmit FIFO available interrupt
/// @param newState: new state of the specified SPIx interrupts.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_ITConfig(SPI_TypeDef* SPIx, u8 interrupt, FunctionalState newState)
{
    if (newState) {
        SET_BIT(SPIx->GCR, (u32)SPI_GCR_IEN);
        SET_BIT(SPIx->IER, (u32)interrupt);
    }
    else {
        CLEAR_BIT(SPIx->IER, interrupt);
        CLEAR_BIT(SPIx->GCR, SPI_GCR_IEN);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the SPI DMA interface.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param newState: new state of the DMA Request sources.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_DMACmd(SPI_TypeDef* SPIx, FunctionalState newState)
{
    (newState) ? SET_BIT(SPIx->GCR, SPI_GCR_DMAEN) : CLEAR_BIT(SPIx->GCR, SPI_GCR_DMAEN);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  configure tn Fifo trigger level bit.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param fifoTriggerValue: specifies the Fifo trigger level
///   This parameter can be any combination of the following values:
/// SPI_TXTLF : SPI TX FIFO Trigger value set
/// SPI_RXTLF : SPI RX FIFO Trigger value set
/// @param newState: new state of the selected SPI DMA transfer
///   request.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_FifoTrigger(SPI_TypeDef* SPIx, SPI_TLF_TypeDef fifoTriggerValue, FunctionalState newState)
{
    (newState) ? SET_BIT(SPIx->GCR, (u32)fifoTriggerValue) : CLEAR_BIT(SPIx->GCR, (u32)fifoTriggerValue);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Transmits a Data through the SPIx peripheral.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param data : Data to be transmitted.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_SendData(SPI_TypeDef* SPIx, u32 data)
{
    __asm volatile("cpsid i");

    WRITE_REG(SPIx->TDR, data);

    u8 temp = READ_REG(SPIx->ECR);
    if (temp  != 32) {
    if (temp > 0x08)
        WRITE_REG(SPIx->TDR, data >> 8);
    if (temp > 0x10)
        WRITE_REG(SPIx->TDR, data >> 16);
    if (temp > 0x18)
        WRITE_REG(SPIx->TDR, data >> 24);
    }
    __asm volatile("cpsie i");
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the most recent received data by the SPIx peripheral.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @retval  The value of the received data.
////////////////////////////////////////////////////////////////////////////////
u32 SPI_ReceiveData(SPI_TypeDef* SPIx)
{
    __asm volatile("cpsid i");

    u32 temp = READ_REG(SPIx->RDR);

    //  if (SPIx->ECR > 8  || SPIx->ECR == 0)  temp
    //|=(u32)(READ_REG(SPIx->RDR) << 8);    if (SPIx->ECR > 16 || SPIx->ECR
    //== 0)  temp |=(u32)(READ_REG(SPIx->RDR) << 16);   if (SPIx->ECR > 24
    //|| SPIx->ECR == 0)    temp |=(u32)(READ_REG(SPIx->RDR) << 24);

    if (temp  != 32) {
    if (SPIx->ECR > 8)
        temp |= (u32)(READ_REG(SPIx->RDR) << 8);
    if (SPIx->ECR > 16)
        temp |= (u32)(READ_REG(SPIx->RDR) << 16);
    if (SPIx->ECR > 24)
        temp |= (u32)(READ_REG(SPIx->RDR) << 24);
        }
    __asm volatile("cpsie i");

    return temp;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Slave chip csn single by selected
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param newState: new state of the selected SPI CS pin
///   request.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_CSInternalSelected(SPI_TypeDef* SPIx, FunctionalState newState)
{
    (newState) ? CLEAR_BIT(SPIx->NSSR, SPI_NSSR_NSS) : SET_BIT(SPIx->NSSR, SPI_NSSR_NSS);  // illogical
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the NSS pin control mode for the selected
///   SPI.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param nss: specifies the SPI NSS internal state.
///   This parameter can be one of the following values:
/// @arg SPI_NSS_Soft: NSS pin control by software
/// @arg SPI_NSS_Hard: NSS pin control by hardware
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef* SPIx, SPI_NSS_TypeDef nss)
{
    (nss != SPI_NSS_Soft) ? SET_BIT(SPIx->GCR, SPI_NSS_Hard) : CLEAR_BIT(SPIx->GCR, SPI_NSS_Hard);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the data size for the selected SPI.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param dataSize: specifies the SPI data size.
///   This parameter can be one of the following values:
///        0 to 31, 0 = 32b, 1 = 1b, 2 = 2b
/// @arg DataSize : 0 to 31
/// @retval None.
/// @retval None.
bool SPI_DataSizeConfig(SPI_TypeDef* SPIx, u8 dataSize)
{
    if (dataSize > 32)
        return false;
    CLEAR_BIT(SPIx->GCR, SPI_DataSize_32b & 0x1F);
    SET_BIT(SPIx->GCR, (u32)dataSize);
    return true;
}

//////////////////////////////////////////////////////////////////////////////////
// void SPI_DataSizeConfig(SPI_TypeDef* SPIx, SPI_DataSize_TypeDef SPI_DataSize)
//{
//  CLEAR_BIT(SPIx->GCR,(u32)SPI_DataSize_32b);
//  SET_BIT(SPIx->GCR,(u32)SPI_DataSize);
//}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Selects the data transfer direction in bi-directional mode
///   for the specified SPI.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param direction: specifies the data transfer direction in
///   bi-directional mode.
///   This parameter can be one of the following values:
/// @arg SPI_Direction_Tx: Selects Tx transmission direction
/// @arg SPI_Direction_Rx: Selects Rx receive direction
/// @arg SPI_Disable_Tx: Selects Rx receive direction
/// @arg SPI_Disable_Rx: Selects Rx receive direction
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_BiDirectionalLineConfig(SPI_TypeDef* SPIx, SPI_Direction_TypeDef direction)
{
    switch (direction) {
        case SPI_Direction_Rx: SET_BIT(SPIx->GCR, SPI_GCR_RXEN); break;
        case SPI_Direction_Tx: SET_BIT(SPIx->GCR, SPI_GCR_TXEN); break;
        case SPI_Disable_Rx: CLEAR_BIT(SPIx->GCR, SPI_GCR_RXEN); break;
        case SPI_Disable_Tx: CLEAR_BIT(SPIx->GCR, SPI_GCR_TXEN); break;
        default: break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified SPI flag is set or not.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param flag: specifies the SPI flag to check.
///   This parameter can be one of the following values:
/// @arg SPI_FLAG_RXAVL: Rx buffer has bytes flag
/// @arg SPI_FLAG_TXEPT: Tx buffer and tx shifter empty flag
/// @arg SPI_FLAG_TXFULL: Tx buffer full flag
/// @arg SPI_FLAG_RXAVL_4BYTE: Receive available 4 byte data message flag.
/// @retval  The new state of SPI_FLAG (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
FlagStatus SPI_GetFlagStatus(SPI_TypeDef* SPIx, SPI_FLAG_TypeDef flag)
{
//    u8 number;
    return (SPIx->SR & flag) ? SET : RESET;
//  if (SPIx->ECR == 8 || SPIx->ECR == 0)
//      return (SPIx->SR & SPI_FLAG) ? SET : RESET;
//  else {
//      if ((SPIx->ECR > 0) && (SPIx->ECR <= 8))
//          number = 1;
//      else if ((SPIx->ECR) <= 16)
//          number = 2;
//      else if ((SPIx->ECR) <= 24)
//          number = 3;
//      else if (((SPIx->ECR) <= 31) || (SPIx->ECR == 0))
//          number = 4;
//      return (((SPIx->SR & 0xf00) >> 8) >= number) ? SET : RESET;
//  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified SPI interrupt has occurred or not.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param interrupt: specifies the SPI interrupt source to check.
///   This parameter can be one of the following values:
/// @arg SPI_IT_TX: Tx buffer empty interrupt
/// @arg SPI_IT_RX: Rx buffer  interrupt
/// @arg SPI_IT_UNDERRUN: under Error interrupt in slave mode
/// @arg SPI_IT_RXOVER: RX OVER Error interrupt
/// @arg SPI_IT_RXMATCH: spectials rx data numbers  interrupt
/// @arg SPI_IT_RXFULL: Rx buffer full interrupt
/// @arg SPI_IT_TXEPT: Tx buffer  and tx shifter empty interrupt
/// @retval  The new state of SPI_IT (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
ITStatus SPI_GetITStatus(SPI_TypeDef* SPIx, SPI_IT_TypeDef interrupt)
{
    return (SPIx->ISR & interrupt) ? SET : RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the SPIx  interrupt pending bit.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param interrupt: specifies the SPI interrupt pending bit to clear.
/// @arg SPI_IT_TX: Tx buffer empty interrupt
/// @arg SPI_IT_RX: Rx buffer  interrupt
/// @arg SPI_IT_UNDERRUN: under Error interrupt in slave mode
/// @arg SPI_IT_RXOVER: RX OVER Error interrupt
/// @arg SPI_IT_RXMATCH: spectials rx data numbers  interrupt
/// @arg SPI_IT_RXFULL: Rx buffer full interrupt
/// @arg SPI_IT_TXEPT: Tx buffer  and tx shifter empty interrupt
///   This function clears only ERR intetrrupt pending bit.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_ClearITPendingBit(SPI_TypeDef* SPIx, SPI_IT_TypeDef interrupt)
{
    SET_BIT(SPIx->ICR, interrupt);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI Hole a count Received bytes in next receive process.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param number: specifies the SPI receive Number.
///   This parament can be 1-65535.
///   This function can use only in SPI master single receive mode.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_RxBytes(SPI_TypeDef* SPIx, u16 number)
{
    WRITE_REG(SPIx->RDNR, number);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  slave mode tx data transmit phase adjust set.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param adjustValue: slave mode tx data transmit phase
/// adjust enum.
///   This parament can be :
///   SPI_SlaveAdjust_FAST:  fast speed use
///   SPI_SlaveAdjust_LOW:   low speed use
///   This function can use only in SPI master single receive mode.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_SlaveAdjust(SPI_TypeDef* SPIx, SPI_SlaveAdjust_TypeDef adjustValue)
{
    (adjustValue) ? SET_BIT(SPIx->CCR, SPI_CCR_RXEDGE) : CLEAR_BIT(SPIx->CCR, SPI_CCR_RXEDGE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables all SPI interrupts.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param newState: new state of all SPIx interrupts.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exSPI_ITCmd(SPI_TypeDef* SPIx, FunctionalState newState)
{
    (newState) ? SET_BIT(SPIx->IER, (u32)SPI_GCR_IEN) : CLEAR_BIT(SPIx->IER, (u32)SPI_GCR_IEN);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified SPI interrupts.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param interrupt: specifies the SPI interrupt sources to be
///   enabled or disabled.
///   This parameter can be one of the following values:
/// @arg SPI_IT_TXEPT: Transmitter empty interrupt
/// @arg SPI_IT_RXFULL: RX FIFO full interrupt
/// @arg SPI_IT_RXMATCH: Receive data match the RXDNR number interrupt
/// @arg SPI_IT_RXOERR: Receive overrun error interrupt
/// @arg SPI_IT_UNDERRUN: underrun interrupt
/// @arg SPI_IT_RX: Receive data available interrupt
/// @arg SPI_IT_TX: Transmit FIFO available interrupt
/// @param newState: new state of the specified SPIx interrupts.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exSPI_ITConfig(SPI_TypeDef* SPIx, SPI_IT_TypeDef interrupt, FunctionalState newState)
{
    (newState) ? SET_BIT(SPIx->IER, (u32)interrupt) : CLEAR_BIT(SPIx->IER, (u32)interrupt);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the SPI DMA request.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param newState: new state of the DMA Request.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exSPI_DMACmd(SPI_TypeDef* SPIx, FunctionalState newState)
{
    (newState) ? SET_BIT(SPIx->GCR, SPI_GCR_DMAEN) : CLEAR_BIT(SPIx->GCR, SPI_GCR_DMAEN);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Set or reset Slave chip csn signal output
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param newState: new state of Slave chip csn signal output.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exSPI_CSInternalSelected(SPI_TypeDef* SPIx, FunctionalState newState)
{
    (newState) ? CLEAR_BIT(SPIx->NSSR, SPI_NSSR_NSS) : SET_BIT(SPIx->NSSR, SPI_NSSR_NSS);  // illogical
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  tx data and rx data phase adjust.
/// @param SPIx: Select the SPI peripheral.
///   This parameter can be one of the following values:
///   SPI1, SPI2.
/// @param adjustValue: choose adjust mode.
///   This parament can be :
/// SPI_DataEdgeAdjust_LOW,
/// SPI_DataEdgeAdjust_FAST
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exSPI_DataEdgeAdjust(SPI_TypeDef* SPIx, SPI_DataEdgeAdjust_TypeDef adjustValue)
{
    // master mode
    if (SPIx->GCR & SPI_GCR_MODE) {
        adjustValue ? SET_BIT(SPIx->CCR, SPI_CCR_RXEDGE) : CLEAR_BIT(SPIx->CCR, SPI_CCR_RXEDGE);
    }
    // slave mode
    else {
        adjustValue ? SET_BIT(SPIx->CCR, SPI_CCR_TXEDGE) : CLEAR_BIT(SPIx->CCR, SPI_CCR_TXEDGE);
    }
}

/// @}

/// @}

/// @}
