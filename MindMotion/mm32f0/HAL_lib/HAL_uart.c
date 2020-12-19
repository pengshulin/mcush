////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_UART.C
/// @author   QR Wang
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE UART FIRMWARE FUNCTIONS.
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
#define _HAL_UART_C_

// Files includes  -------------------------------------------------------------
#include "HAL_rcc.h"
#include "HAL_uart.h"
#include "HAL_gpio.h"
#include "HAL_dma.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
///@addtogroup UART_HAL
///@{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup UART_Exported_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the UARTx peripheral registers to their
///         default reset values.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_DeInit(UART_TypeDef* UARTx)
{
    switch (*(u32*)&UARTx) {
        case (u32)UART1:  // UART1_BASE:
            exRCC_APB2PeriphReset(RCC_APB2Periph_UART1);
            break;
        case (u32)UART2:  // UART2_BASE:
            exRCC_APB1PeriphReset(RCC_APB1Periph_UART2);
            break;
#if defined(__MM3N1)
        case (u32)UART3:  // UART3_BASE:
            exRCC_APB1PeriphReset(RCC_APB1Periph_UART3);
            break;
#endif
        default: break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the UARTx peripheral according to the specified
///         parameters in the UART_InitStruct .
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  pInitStruct: pointer to a UART_InitTypeDef structure
///         that contains the configuration information for the
///         specified UART peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_Init(UART_TypeDef* UARTx, UART_InitTypeDef* pInitStruct)
{
    u32 apbclock = 0x00;
    /*---------------------------- UART CCR Configuration
     * -----------------------*/
    MODIFY_REG(UARTx->CCR, UART_CCR_CHAR, pInitStruct->WordLength);

#if defined(__MM3N1) || defined(__MM0N1)
    MODIFY_REG(UARTx->CCR, UART_CCR_SPB, pInitStruct->StopBits);
#endif

#if defined(__MM0P1) || defined(__MM0Q1)
    MODIFY_REG(UARTx->CCR, (UART_CCR_SPB0 | UART_CCR_SPB1), pInitStruct->StopBits);
#endif

    MODIFY_REG(UARTx->CCR, (UART_CCR_PEN | UART_CCR_PSEL), pInitStruct->Parity);

    /*---------------------------- UART GCR Configuration
     * -----------------------*/
    MODIFY_REG(UARTx->GCR, (UART_GCR_TX | UART_GCR_RX), pInitStruct->Mode);
    MODIFY_REG(UARTx->GCR, UART_GCR_AUTOFLOW, pInitStruct->HWFlowControl);

    /*---------------------------- UART BRR Configuration
     * -----------------------*/
    /* Configure the UART Baud Rate
     * -------------------------------------------*/
    if (UARTx == UART1) {
        apbclock = RCC_GetPCLK2Freq();
    }
    else {
        apbclock = RCC_GetPCLK1Freq();
    }
    /* Determine the UART_baud*/
    UARTx->BRR = (apbclock / pInitStruct->BaudRate) / 16;
    UARTx->FRA = (apbclock / pInitStruct->BaudRate) % 16;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each UART_InitStruct member with its default value.
/// @param  pInitStruct: pointer to a UART_InitTypeDef structure
///         which will be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_StructInit(UART_InitTypeDef* pInitStruct)
{
    /* UART_InitStruct members default value */
    pInitStruct->BaudRate       = 9600;
    pInitStruct->WordLength     = UART_WordLength_8b;
    pInitStruct->StopBits       = UART_StopBits_1;
    pInitStruct->Parity         = UART_Parity_No;
    pInitStruct->Mode           = UART_Mode_Rx | UART_Mode_Tx;
    pInitStruct->HWFlowControl  = UART_HWFlowControl_None;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified UART peripheral.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  state: new state of the UARTx peripheral.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_Cmd(UART_TypeDef* UARTx, FunctionalState state)
{
    MODIFY_REG(UARTx->GCR, UART_GCR_UART, state << UART_GCR_UART_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified UART interrupts.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  it: specifies the UART interrupt sources to be
///         enabled or disabled.
///         This parameter can be one of the following values:
/// @arg    UART_IT_ERR:  Error interrupt(Frame error,)
/// @arg    UART_IT_PE:   Parity Error interrupt
/// @arg    UART_OVER_ERR:  overrun Error interrupt
/// @arg    UART_IT_RXIEN: Receive Data register interrupt
/// @arg    UART_IT_TXIEN:  Tansmit Data Register empty interrupt
///
/// @param  state: new state of the specified UARTx interrupts.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_ITConfig(UART_TypeDef* UARTx, u16 it, FunctionalState state)
{
    (state) ? (UARTx->IER |= it) : (UARTx->IER &= ~it);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the UART DMA interface.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  DMAReq: specifies the DMA request.
///         This parameter can be any combination of the following values:
/// @arg    UART_DMAReq_EN: UART DMA transmit request
///
/// @param  state: new state of the DMA Request sources.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_DMACmd(UART_TypeDef* UARTx, u16 DMAReq, FunctionalState state)
{
    (void)DMAReq;
    MODIFY_REG(UARTx->GCR, UART_GCR_DMA, state << UART_GCR_DMA_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Transmits single data through the UARTx peripheral.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  Data: the data to transmit.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_SendData(UART_TypeDef* UARTx, u16 value)
{
    /* Transmit Data */
    WRITE_REG(UARTx->TDR, (value & 0xFFU));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the most recent received data by the UARTx peripheral.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @retval  The received data.
////////////////////////////////////////////////////////////////////////////////
u16 UART_ReceiveData(UART_TypeDef* UARTx)
{
    /* Receive Data */
    return (u16)(UARTx->RDR & 0xFFU);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified UART flag is set or not.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  flag: specifies the flag to check.
///         This parameter can be one of the following values:
/// @arg    UART_FLAG_TXEMPTY: Transmit data register empty flag
/// @arg    UART_FLAG_TXFULL: Transmit data buffer full
/// @arg    UART_FLAG_RXAVL: RX Buffer has a byte flag
/// @arg    UART_FLAG_OVER: OverRun Error flag
/// @arg    UART_FLAG_TXEPT: tx and shifter are emptys flag
/// @retval  The new state of UART_FLAG (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
FlagStatus UART_GetFlagStatus(UART_TypeDef* UARTx, u16 flag)
{
    return (UARTx->CSR & flag) ? SET : RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified UART interrupt has occurred or not.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  it: specifies the UART interrupt source to check.
///         This parameter can be one of the following values:
/// @arg    UART_IT_ERR:  Error interrupt(Frame error,)
/// @arg    UART_IT_PE:   Parity Error interrupt
/// @arg    UART_OVER_ERR:  overrun Error interrupt
/// @arg    UART_IT_RXIEN: Receive Data register interrupt
/// @arg    UART_IT_TXIEN:  Tansmit Data Register empty interrupt
/// @retval  The new state of UART_IT (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
ITStatus UART_GetITStatus(UART_TypeDef* UARTx, u16 it)
{
    return (UARTx->ISR & it) ? SET : RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the UARTx interrupt pending bits.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  it: specifies the interrupt pending bit to clear.
///         This parameter can be one of the following values:
/// @arg    UART_IT_ERR:  Error interrupt(Frame error,)
/// @arg    UART_IT_PE:   Parity Error interrupt
/// @arg    UART_OVER_ERR:  overrun Error interrupt
/// @arg    UART_IT_RXIEN: Receive Data register interrupt
/// @arg    UART_IT_TXIEN:  Tansmit Data Register empty interrupt
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_ClearITPendingBit(UART_TypeDef* UARTx, u16 it)
{
    /*clear UART_IT pendings bit*/
    UARTx->ICR = it;
}

#if defined(__MM0P1) || defined(__MM0Q1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Selects the UART WakeUp method.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  mode: specifies the UART wakeup method.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_WakeUpConfig(UART_TypeDef* UARTx, UART_WakeUp_TypeDef mode)
{
    MODIFY_REG(UARTx->CCR, UART_CCR_WAKE, mode);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Determines if the UART is in mute mode or not.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  state: new state of the UART mute mode.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_ReceiverWakeUpCmd(UART_TypeDef* UARTx, FunctionalState state)
{
    MODIFY_REG(UARTx->CCR, UART_CCR_RWU, state << UART_CCR_RWU_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the address of the UART Rx Address.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  address: Indicates the address of the UART Rx Address.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_SetRXAddress(UART_TypeDef* UARTx, u8 address)
{
    MODIFY_REG(UARTx->RXAR, UART_RXAR_ADDR, address);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the address of the UART Rx MASK.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  address: Indicates the address of the UART Rx MASK.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_SetRXMASK(UART_TypeDef* UARTx, u8 address)
{
    MODIFY_REG(UARTx->RXMR, UART_RXMR_MASK, address);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  ENBALE or DISABLE the UART's 9bit.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  state: new state of the UART 9 bit.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_Enable9bit(UART_TypeDef* UARTx, FunctionalState state)
{
    MODIFY_REG(UARTx->CCR, UART_CCR_B8EN, state << UART_CCR_B8EN_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the UART's 9bit Level.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  state: new state of the UART 9 bit.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_Set9bitLevel(UART_TypeDef* UARTx, FunctionalState state)
{
    MODIFY_REG(UARTx->CCR, UART_CCR_B8TXD, state << UART_CCR_B8TXD_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the UART's 9bit Polarity.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  polarity: new state of the UART 9 bit Polarity.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_Set9bitPolarity(UART_TypeDef* UARTx, UART_9bit_Polarity_TypeDef polarity)
{
    MODIFY_REG(UARTx->CCR, UART_CCR_B8POL, polarity);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the UART's 9bit Automatic Toggle.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  state: new state of the UART 9 bit Automatic Toggle.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_Set9bitAutomaticToggle(UART_TypeDef* UARTx, FunctionalState state)
{
    MODIFY_REG(UARTx->CCR, UART_CCR_B8TOG, state << UART_CCR_B8TOG_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the UART Half Duplex communication.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  state: new state of the UART Communication.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_HalfDuplexCmd(UART_TypeDef* UARTx, FunctionalState state)
{
    MODIFY_REG(UARTx->SCR, UART_SCR_HDSEL, state << UART_SCR_HDSEL_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the specified UART guard time.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  guardTime: specifies the guard time.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_SetGuardTime(UART_TypeDef* UARTx, u8 guardTime)
{
    MODIFY_REG(UARTx->SCR, UART_SCR_SCFCNT, guardTime << UART_SCR_SCFCNT_Pos);
    // /* Clear the UART Guard time */
    // UARTx->SCR &= SCR_SCFCNT_Mask;
    // /* Set the UART guard time */
    // UARTx->SCR |= (uint16_t)((uint16_t)guardTime << 0x04);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the UART's Smart Card mode.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  state: new state of the Smart Card mode.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_SmartCardCmd(UART_TypeDef* UARTx, FunctionalState state)
{
    MODIFY_REG(UARTx->SCR, UART_SCR_SCEN, state << UART_SCR_SCEN_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables NACK transmission.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @param  state: new state of the NACK transmission.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_SmartCardNACKCmd(UART_TypeDef* UARTx, FunctionalState state)
{
    MODIFY_REG(UARTx->SCR, UART_SCR_SCARB, state << UART_SCR_SCARB_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Transmits break characters.
/// @param  UARTx: Select the UART or the UART peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UART_SendBreak(UART_TypeDef* UARTx)
{
    SET_BIT(UARTx->CCR, UART_CCR_BRK);
}

#endif
/// @}

/// @}

/// @}
