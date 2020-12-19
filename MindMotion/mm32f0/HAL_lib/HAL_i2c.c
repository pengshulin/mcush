////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_I2C.C
/// @author   S Yi
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE I2C FIRMWARE FUNCTIONS.
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
#define _HAL_I2C_C_

// Files includes  -------------------------------------------------------------
#include "HAL_gpio.h"
#include "HAL_i2c.h"
#include "HAL_rcc.h"
#include "HAL_nvic.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup I2C_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup I2C_Exported_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the I2Cn peripheral registers to their default
///   reset values.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_DeInit(I2C_TypeDef* I2Cn)
{
    switch (*(u32*)&I2Cn) {
        case (u32)I2C1:  // I2C1_BASE:
            exRCC_APB1PeriphReset(RCC_APB1ENR_I2C1);
            break;
#if defined(__MM3N1)
        case (u32)I2C2:  // I2C2_BASE:
            exRCC_APB1PeriphReset(RCC_APB1ENR_I2C2);
            break;
#endif
        default: break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the I2Cn peripheral according to the specified
///   parameters in the pInitStruct.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  pInitStruct: pointer to a I2C_InitTypeDef structure that
///   contains the configuration information for the specified
///   I2C peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_Init(I2C_TypeDef* I2Cn, I2C_InitTypeDef* pInitStruct)
{
    u32 pclk1         = 8000000;
    u32 minSclLowTime = 0;
    u32 i2cPeriod     = 0;
    u32 pclk1Period   = 0;

    RCC_ClocksTypeDef rcc_clocks;
    I2Cn->ENR &= ~I2C_ENR_ENABLE;
    RCC_GetClocksFreq(&rcc_clocks);

    pclk1       = rcc_clocks.PCLK1_Frequency;
    pclk1Period = 1000000000 / pclk1;
    i2cPeriod   = 1000000000 / pInitStruct->ClockSpeed;

    if (pInitStruct->ClockSpeed <= 100000) {
        minSclLowTime        = 4700;
        I2Cn->SSLR = minSclLowTime / pclk1Period;
        I2Cn->SSHR = (i2cPeriod - pclk1Period * I2Cn->SSLR) / pclk1Period;
    }
    else {
//      minSclLowTime        = 1300;
        minSclLowTime        = 1100;
        I2Cn->FSLR = minSclLowTime / pclk1Period;
        I2Cn->FSHR = (i2cPeriod - pclk1Period * I2Cn->FSLR) / pclk1Period;
    }

    I2Cn->CR &= ~(I2C_CR_EMPINT |
                I2C_CR_SLAVEDIS |
                I2C_CR_REPEN    |
                I2C_CR_MASTER10 |
                I2C_CR_SLAVE10  |
                I2C_CR_FAST     |
                I2C_CR_MASTER);

    I2Cn->CR =  I2C_CR_EMPINT   |
                I2C_CR_REPEN    |
                ((pInitStruct->Speed) ? I2C_CR_FAST : I2C_CR_STD) |
                ((pInitStruct->Mode)  ? I2C_CR_MASTER : 0x00);

    I2Cn->IMR &= INTR_MASK;

    I2Cn->RXTLR = 0x00;
    I2Cn->TXTLR = 0x01;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each pInitStruct member with its default value.
/// @param  pInitStruct: pointer to an I2C_InitTypeDef structure
///   which will be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_StructInit(I2C_InitTypeDef* pInitStruct)
{
    pInitStruct->Mode       = I2C_CR_MASTER;
    pInitStruct->OwnAddress = I2C_OWN_ADDRESS;
    pInitStruct->Speed      = I2C_CR_STD;
    pInitStruct->ClockSpeed = 100000;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified I2C peripheral.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  state: new state of the I2Cn peripheral. This parameter
///   can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_Cmd(I2C_TypeDef* I2Cn, FunctionalState state)
{
    (state) ? (I2Cn->ENR |= I2C_ENR_ENABLE) : (I2Cn->ENR &= ~I2C_ENR_ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified I2C DMA requests.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  state: new state of the I2C DMA transfer.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_DMACmd(I2C_TypeDef* I2Cn, FunctionalState state)
{
    if (state) {
        if (I2C_DMA_DIR == TDMAE_SET)
            I2Cn->DMA |= TDMAE_SET;

        else if (I2C_DMA_DIR == RDMAE_SET)
            I2Cn->DMA |= RDMAE_SET;
    }
    else
        I2Cn->DMA &= ~(I2C_DMA_RXEN | I2C_DMA_TXEN);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Generates I2Cn communication START condition.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  state: new state of the I2C START condition generation.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_GenerateSTART(I2C_TypeDef* I2Cn, FunctionalState state)
{
    (state) ? (I2Cn->CR |= I2C_CR_REPEN) : (I2Cn->CR &= ~I2C_CR_REPEN);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Generates I2Cn communication STOP condition.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  state: new state of the I2C STOP condition generation.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_GenerateSTOP(I2C_TypeDef* I2Cn, FunctionalState state)
{
    u16 overTime = 3000;

    (void)state;

    I2Cn->ENR |= I2C_ENR_ABORT;

    while (I2Cn->ENR & I2C_ENR_ABORT) {
        if (overTime-- == 0)
            break;
    }
    I2Cn->TX_ABRT;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the specified I2C own address2.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  addr: specifies the 7bit I2C own address2.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_OwnAddress2Config(I2C_TypeDef* I2Cn, u8 addr)
{
    MODIFY_REG(I2Cn->TAR, I2C_TAR_ADDR, addr >> 1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified I2C dual addressing mode.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  state: new state of the I2C dual addressing mode.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_DualAddressCmd(I2C_TypeDef* I2Cn, FunctionalState state)
{
    (state) ? (I2Cn->TAR |= IC_TAR_ENDUAL_Set) : (I2Cn->TAR &= IC_TAR_ENDUAL_Reset);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified I2C general call feature.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  state: new state of the I2C General call.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_GeneralCallCmd(I2C_TypeDef* I2Cn, FunctionalState state)
{
    (state) ? (I2Cn->TAR |= I2C_TAR_SPECIAL) : (I2Cn->TAR &= ~I2C_TAR_SPECIAL);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified I2C interrupts.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  it: specifies the I2C interrupts sources to be enabled
///   or disabled.
///   This parameter can be any combination of the following values:
/// @arg  I2C_IT_RX_UNDER   : Rx Buffer is empty interrupt mask
/// @arg  I2C_IT_RX_OVER    : RX  Buffer Overrun interrupt mask
/// @arg  I2C_IT_RX_FULL    : Rx buffer full interrupt mask
/// @arg  I2C_IT_TX_OVER    : TX  Buffer Overrun interrupt mask
/// @arg  I2C_IT_TX_EMPTY   : TX_FIFO empty interrupt mask
/// @arg  I2C_IT_RD_REQ     : I2C work as slave or master interrupt mask
/// @arg  I2C_IT_TX_ABRT    : TX error interrupt  mask(Master mode)
/// @arg  I2C_IT_RX_DONE    : Master not ack interrupt mask(slave mode)
/// @arg  I2C_IT_ACTIVITY   : I2C activity interrupt mask
/// @arg  I2C_IT_STOP_DET   : stop condition  interrupt mask
/// @arg  I2C_IT_START_DET  : start condition  interrupt mask
/// @arg  I2C_IT_GEN_CALL   : a general call address and ack interrupt mask
/// @param  state: new state of the specified I2C interrupts.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_ITConfig(I2C_TypeDef* I2Cn, u16 it, FunctionalState state)
{
    if (it == I2C_IT_RX_FULL)
        I2C_ReadCmd(I2Cn);
    (state) ? SET_BIT(I2Cn->IMR, it) : CLEAR_BIT(I2Cn->IMR, (u16)it);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sends a data byte through the I2Cn peripheral.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  dat: Byte to be transmitted..
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_SendData(I2C_TypeDef* I2Cn, u8 dat)
{
    I2Cn->DR = dat;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the most recent received data by the I2Cn peripheral.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @retval The value of the received data.
////////////////////////////////////////////////////////////////////////////////
void I2C_ReadCmd(I2C_TypeDef* I2Cn)
{
    I2Cn->DR = I2C_DR_CMD;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the most recent received data by the I2Cn peripheral.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @retval The value of the received data.
////////////////////////////////////////////////////////////////////////////////
u8 I2C_ReceiveData(I2C_TypeDef* I2Cn)
{
    I2C_CMD_DIR = 0;
    return (u8)I2Cn->DR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Transmits the address byte to select the slave device.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  addr: specifies the slave address which will be transmitted
/// @param  dir: specifies whether the I2C device will be a
///   Transmitter or a Receiver.
///   This parameter can be one of the following values
/// @arg  I2C_Direction_Transmitter: Transmitter mode
/// @arg  I2C_Direction_Receiver: Receiver mode
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_Send7bitAddress(I2C_TypeDef* I2Cn, u8 addr, u8 dir)
{
    (void)dir;

    I2Cn->TAR = addr >> 1;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reads the specified I2C register and returns its value.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  reg: specifies the register to read.
///         This parameter can be one of the following values:
/// @retval The value of the read register.
////////////////////////////////////////////////////////////////////////////////
u16 I2C_ReadRegister(I2C_TypeDef* I2Cn, u8 reg)
{
    return (*(u16*)(*((u32*)&I2Cn) + reg));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the last I2Cn Event.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @retval The last event
////////////////////////////////////////////////////////////////////////////////
u32 I2C_GetLastEvent(I2C_TypeDef* I2Cn)
{
    return I2Cn->RAWISR & FLAG_Mask;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the last I2Cn Event is equal to the one passed
///   as parameter.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  event: specifies the event to be checked.
///   This parameter can be one of the following values:
/// @arg  I2C_EVENT_RX_UNDER : Rx Buffer is empty event
/// @arg  I2C_EVENT_RX_OVER  : RX  Buffer Overrun event
/// @arg  I2C_EVENTT_RX_FULL : Rx buffer full event
/// @arg  I2C_EVENT_TX_OVER  : TX  Buffer Overrun event
/// @arg  I2C_EVENT_TX_EMPTY : TX_FIFO empty event
/// @arg  I2C_EVENT_RD_REQ   : I2C work as slave or master event
/// @arg  I2C_EVENT_TX_ABRT  : TX error event(Master mode)
/// @arg  I2C_EVENT_RX_DONE  : Master not ack event(slave mode)
/// @arg  I2C_EVENT_ACTIVITY : I2C activity event
/// @arg  I2C_EVENT_STOP_DET : stop condition  event
/// @arg  I2C_EVENT_START_DET: start condition  event
/// @arg  I2C_EVENT_GEN_CALL : a general call address and ack event
/// - SUCCESS: Last event is equal to the I2C_EVENT
/// - ERROR: Last event is different from the I2C_EVENT
////////////////////////////////////////////////////////////////////////////////
ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cn, u32 event)
{
    if ((event == I2C_EVENT_RX_FULL) && (I2C_CMD_DIR == 0)) {
        I2Cn->DR = I2C_DR_CMD;
        I2C_CMD_DIR       = 1;
    }

    return (ErrorStatus)((I2Cn->RAWISR & event) == event);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified I2C flag is set or not.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  flag: specifies the flag to check.
///   This parameter can be one of the following values:
/// @arg  I2C_FLAG_RX_UNDER : Rx Buffer is empty flag
/// @arg  I2C_FLAG_RX_OVER  : RX  Buffer Overrun flag
/// @arg  I2C_FLAG_RX_FULL  : Rx buffer full flag
/// @arg  I2C_FLAG_TX_OVER  : TX  Buffer Overrun flag
/// @arg  I2C_FLAG_TX_EMPTY : TX_FIFO empty flag
/// @arg  I2C_FLAG_RD_REQ   : I2C work as slave or master flag
/// @arg  I2C_FLAG_TX_ABRT  : TX error flag(Master mode)
/// @arg  I2C_FLAG_RX_DONE  : Master not ack flag(slave mode)
/// @arg  I2C_FLAG_ACTIVITY : I2C activity flag
/// @arg  I2C_FLAG_STOP_DET : stop condition  flag
/// @arg  I2C_FLAG_START_DET: start condition  flag
/// @arg  I2C_FLAG_GEN_CALL : a general call address and ack flag
/// @retval The new state of I2C_FLAG (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cn, u32 flag)
{
    if (flag & 0x8000)
        return ((I2Cn->SR & flag) ? SET : RESET);

    if ((flag == I2C_FLAG_RX_FULL) && (I2C_CMD_DIR == 0)) {
        I2Cn->DR = I2C_DR_CMD;
        I2C_CMD_DIR       = 1;
    }
    return (((I2Cn->RAWISR & flag)) ? SET : RESET);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the I2Cn's pending flags.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  flag: specifies the flag to clear.
///   This parameter can be any combination of the following values:
/// @arg  I2C_FLAG_RX_UNDER : Rx Buffer is empty flag
/// @arg  I2C_FLAG_RX_OVER  : RX  Buffer Overrun flag
/// @arg  I2C_FLAG_RX_FULL  : Rx buffer full flag
/// @arg  I2C_FLAG_TX_OVER  : TX  Buffer Overrun flag
/// @arg  I2C_FLAG_TX_EMPTY : TX_FIFO empty flag
/// @arg  I2C_FLAG_RD_REQ   : I2C work as slave or master flag
/// @arg  I2C_FLAG_TX_ABRT  : TX error flag(Master mode)
/// @arg  I2C_FLAG_RX_DONE  : Master not ack flag(slave mode)
/// @arg  I2C_FLAG_ACTIVITY : I2C activity flag
/// @arg  I2C_FLAG_STOP_DET : stop condition  flag
/// @arg  I2C_FLAG_START_DET: start condition  flag
/// @arg  I2C_FLAG_GEN_CALL : a general call address and ack flag
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_ClearFlag(I2C_TypeDef* I2Cn, u32 flag)
{
    if ((flag & I2C_FLAG_RX_UNDER) == I2C_FLAG_RX_UNDER)
        I2Cn->RX_UNDER;
    if ((flag & I2C_FLAG_RX_OVER) == I2C_FLAG_RX_OVER)
        I2Cn->RX_OVER;
    if ((flag & I2C_FLAG_TX_OVER) == I2C_FLAG_TX_OVER)
        I2Cn->TX_OVER;
    if ((flag & I2C_FLAG_RD_REQ) == I2C_FLAG_RD_REQ)
        I2Cn->RD_REQ;
    if ((flag & I2C_FLAG_TX_ABRT) == I2C_FLAG_TX_ABRT)
        I2Cn->TX_ABRT;
    if ((flag & I2C_FLAG_RX_DONE) == I2C_FLAG_RX_DONE)
        I2Cn->RX_DONE;
    if ((flag & I2C_FLAG_ACTIVITY) == I2C_FLAG_ACTIVITY)
        I2Cn->ACTIV;
    if ((flag & I2C_FLAG_STOP_DET) == I2C_FLAG_STOP_DET)
        I2Cn->STOP;
    if ((flag & I2C_FLAG_START_DET) == I2C_FLAG_START_DET)
        I2Cn->START;
    if ((flag & I2C_FLAG_GEN_CALL) == I2C_FLAG_GEN_CALL)
        I2Cn->GC;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified I2C interrupt has occurred or not.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  it: specifies the interrupt source to check.
///   This parameter can be one of the following values:
/// @arg  I2C_IT_RX_UNDER : Rx Buffer is empty interrupt
/// @arg  I2C_IT_RX_OVER  : RX  Buffer Overrun interrupt
/// @arg  I2C_IT_RX_FULL  : Rx buffer full interrupt
/// @arg  I2C_IT_TX_OVER  : TX  Buffer Overrun interrupt
/// @arg  I2C_IT_TX_EMPTY : TX_FIFO empty interrupt
/// @arg  I2C_IT_RD_REQ   : I2C work as slave or master interrupt
/// @arg  I2C_IT_TX_ABRT  : TX error interrupt  (Master mode)
/// @arg  I2C_IT_RX_DONE  : Master not ack interrupt (slave mode)
/// @arg  I2C_IT_ACTIVITY : I2C activity interrupt
/// @arg  I2C_IT_STOP_DET : stop condition  interrupt
/// @arg  I2C_IT_START_DET: start condition  interrupt
/// @arg  I2C_IT_GEN_CALL : a general call address and ack interrupt
/// @retval The new state of I2C_IT (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
ITStatus I2C_GetITStatus(I2C_TypeDef* I2Cn, u32 it)
{
    return ((I2Cn->RAWISR & it) ? SET : RESET);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the I2Cn interrupt pending bits.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  it: specifies the interrupt pending bit to clear.
///   This parameter can be any combination of the following values:
/// @arg  I2C_IT_RX_UNDER : Rx Buffer is empty interrupt
/// @arg  I2C_IT_RX_OVER  : RX  Buffer Overrun interrupt
/// @arg  I2C_IT_RX_FULL  : Rx buffer full interrupt
/// @arg  I2C_IT_TX_OVER  : TX  Buffer Overrun interrupt
/// @arg  I2C_IT_TX_EMPTY : TX_FIFO empty interrupt
/// @arg  I2C_IT_RD_REQ   : I2C work as slave or master interrupt
/// @arg  I2C_IT_TX_ABRT  : TX error interrupt  (Master mode)
/// @arg  I2C_IT_RX_DONE  : Master not ack interrupt (slave mode)
/// @arg  I2C_IT_ACTIVITY : I2C activity interrupt
/// @arg  I2C_IT_STOP_DET : stop condition  interrupt
/// @arg  I2C_IT_START_DET: start condition  interrupt
/// @arg  I2C_IT_GEN_CALL : a general call address and ack interrupt
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_ClearITPendingBit(I2C_TypeDef* I2Cn, u32 it)
{
    if ((it & I2C_IT_RX_UNDER) == I2C_FLAG_RX_UNDER)
        I2Cn->RX_UNDER;
    if ((it & I2C_IT_RX_OVER) == I2C_FLAG_RX_OVER)
        I2Cn->RX_OVER;
    if ((it & I2C_IT_TX_OVER) == I2C_FLAG_TX_OVER)
        I2Cn->TX_OVER;
    if ((it & I2C_IT_RD_REQ) == I2C_FLAG_RD_REQ)
        I2Cn->RD_REQ;
    if ((it & I2C_IT_TX_ABRT) == I2C_FLAG_TX_ABRT)
        I2Cn->TX_ABRT;
    if ((it & I2C_IT_RX_DONE) == I2C_FLAG_RX_DONE)
        I2Cn->RX_DONE;
    if ((it & I2C_IT_ACTIVITY) == I2C_FLAG_ACTIVITY)
        I2Cn->ACTIV;
    if ((it & I2C_IT_STOP_DET) == I2C_FLAG_STOP_DET)
        I2Cn->STOP;
    if ((it & I2C_IT_START_DET) == I2C_FLAG_START_DET)
        I2Cn->START;
    if ((it & I2C_IT_GEN_CALL) == I2C_FLAG_GEN_CALL)
        I2Cn->GC;
}

////////////////////////////////////////////////////////////////////////////////
//
//  New Function Interface
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures slave address.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  addr: specifies the slave address which will be transmitted
///   This parameter can be one of the following values
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_SendSlaveAddress(I2C_TypeDef* I2Cn, u8 addr)
{
    WRITE_REG(I2Cn->SAR, addr >> 1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the I2C slave mode.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  state: new state of the specified I2C interrupts.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_SlaveConfigure(I2C_TypeDef* I2Cn, FunctionalState state)
{
    (state) ? CLEAR_BIT(I2Cn->CR, I2C_CR_SLAVEDIS) : SET_BIT(I2Cn->CR, I2C_CR_SLAVEDIS);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the specified I2C DMA requests.
/// @param  I2Cn: where n can be 1 or 2 to select the I2C peripheral.
/// @param  dir : TDMAE_SET,RDMAE_SET
///   This parameter can be any combination of the following values:
/// @arg  TDMAE_SET: DMA TX set
/// @arg  RDMAE_SET: DMA RX set
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void I2C_DMAConfigure(I2C_TypeDef* I2Cn, u8 dir)
{
    WRITE_REG(I2Cn->DMA, (2 - dir));
}

/// @}

/// @}

/// @}
