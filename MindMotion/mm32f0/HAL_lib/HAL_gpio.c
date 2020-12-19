////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_GPIO.C
/// @author   C Yuan
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE GPIO FIRMWARE FUNCTIONS.
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
#define _HAL_GPIO_C_

// Files includes  -------------------------------------------------------------
#include "HAL_gpio.h"
#include "HAL_rcc.h"
#include "HAL_bkp.h"


#include "common.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup GPIO_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup GPIO_Exported_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the GPIOx peripheral registers to their default reset
///         values.
/// @param  GPIOx: where x can be (A..D) to select the GPIO peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void GPIO_DeInit(GPIO_TypeDef* GPIOx)
{
    switch (*(u32*)&GPIOx) {
#if defined(__MM3N1)
        case (u32)GPIOA: exRCC_APB2PeriphReset(RCC_APB2RSTR_GPIOA); break;
        case (u32)GPIOB: exRCC_APB2PeriphReset(RCC_APB2RSTR_GPIOB); break;
        case (u32)GPIOC: exRCC_APB2PeriphReset(RCC_APB2RSTR_GPIOC); break;
        case (u32)GPIOD: exRCC_APB2PeriphReset(RCC_APB2RSTR_GPIOD); break;
#endif
#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
        case (u32)GPIOA: COMMON_DisableIpClock(emCLOCK_GPIOA);  break;
        case (u32)GPIOB: COMMON_DisableIpClock(emCLOCK_GPIOB);  break;
        case (u32)GPIOC: COMMON_DisableIpClock(emCLOCK_GPIOC);  break;
        case (u32)GPIOD: COMMON_DisableIpClock(emCLOCK_GPIOD);  break;
#endif
        default: break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the Alternate Functions (remap, event control
///         and EXTI configuration) registers to their default reset values.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void GPIO_AFIODeInit()
{
#if defined(__MM3N1)
    exRCC_APB2PeriphReset(RCC_APB2ENR_EXTI);
#endif
#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    GPIOA->AFRL = 0xFFFFFFFF;
    GPIOA->AFRH = 0xF00FFFFF;       //  PA14:SWCLK, PA13:PSWDIO
    GPIOB->AFRL = 0xFFFFFFFF;
    GPIOB->AFRH = 0xFFFFFFFF;
    GPIOC->AFRL = 0xFFFFFFFF;
    GPIOC->AFRH = 0xFFFFFFFF;
    GPIOD->AFRL = 0xFFFFFFFF;
    GPIOD->AFRH = 0xFFFFFFFF;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reads all data from the Data Backup Registers,then clear the
///         registers, transfer the data to the registers at last.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void GPIO_checkBKP(void)
{
#if defined(__MM3N1) || defined(__MM3O1) || defined(__MM0N1) || defined(__MM0P1)
    u16 BKPBuff[BKP_NUMBER];
    exBKP_Init();
    for (u8 i = 0;i < BKP_NUMBER;i++) {
        BKPBuff[i] = exBKP_ImmRead((BKPDR_Typedef)(BKP_DR1 + 0x04 * i));
    }
#if defined(__MM3N1) || defined(__MM3O1)
    BKP_DeInit();
#endif
#if defined(__MM0N1) || defined(__MM0P1)
    for (u8 i = 0;i < BKP_NUMBER;i++) {
        exBKP_ImmWrite((BKPDR_Typedef)(BKP_DR1 + 0x04 * i), 0x0000);
    }
#endif
    for (u8 i = 0;i < BKP_NUMBER;i++) {
        exBKP_ImmWrite((BKPDR_Typedef)(BKP_DR1 + 0x04 * i), BKPBuff[i]);
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the GPIOx peripheral according to the specified
///         parameters in the pInitStruct.
/// @param  GPIOx: where x can be (A..D) to select the GPIO peripheral.
/// @param  pInitStruct: pointer to a GPIO_InitTypeDef structure that
///         contains the configuration information for the specified GPIO
///         peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* pInitStruct)
{
    switch (*(u32*)&GPIOx) {
        case (u32)GPIOC:
            GPIO_checkBKP();
            break;
        default: break;
    }

    u8   idx;
    __IO u32* pReg;

    // 1x
    u32 dat = pInitStruct->GPIO_Mode & 0x0F;
    if (pInitStruct->GPIO_Mode & 0x10)
        dat |= pInitStruct->GPIO_Speed;

    // 0x
    pReg = &GPIOx->CRL;
    for (u8 i = 0; i < 8; i++) {
        idx = i * 4;
        if ((pInitStruct->GPIO_Pin) & (1 << i)) {
            *pReg = (*pReg & ~(0xF << idx)) | (dat << idx);
        }
    }

    pReg    = &GPIOx->CRH;
    u32 tmp = pInitStruct->GPIO_Pin >> 8;
    for (u8 i = 0; i < 8; i++) {
        idx = i * 4;
        if (tmp & (1 << i)) {
            *pReg = (*pReg & ~(0xF << idx)) | (dat << idx);
        }
    }

    // 2x,4x
    if (pInitStruct->GPIO_Mode == GPIO_Mode_IPD)
        GPIOx->BRR |= pInitStruct->GPIO_Pin;
    else if (pInitStruct->GPIO_Mode == GPIO_Mode_IPU)
        GPIOx->BSRR |= pInitStruct->GPIO_Pin;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reads the input data of specified GPIO port pin.
/// @param  GPIOx: where x can be (A..D) to select the GPIO peripheral.
/// @param  pin:  specifies the port pin to be read.
///         This parameter can be GPIO_Pin_x where x can be (0..15).
/// @retval The input port pin value.
////////////////////////////////////////////////////////////////////////////////
bool GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, u16 pin)
{
    return ((GPIOx->IDR & pin)) ? Bit_SET : Bit_RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reads all GPIO port pins input data.
/// @param  GPIOx: where x can be (A..D) to select the GPIO peripheral.
/// @retval GPIO port input data value.
////////////////////////////////////////////////////////////////////////////////
u16 GPIO_ReadInputData(GPIO_TypeDef* GPIOx)
{
    return GPIOx->IDR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reads the output data of specified GPIO port pin.
/// @param  GPIOx: where x can be (A..D) to select the GPIO peripheral.
/// @param  pin:  specifies the port bit to be read.
///         This parameter can be GPIO_Pin_x where x can be (0..15).
/// @retval The output port pin value.
////////////////////////////////////////////////////////////////////////////////
bool GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, u16 pin)
{
    return (GPIOx->ODR & pin) ? Bit_SET : Bit_RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reads all GPIO port pins output data.
/// @param  GPIOx: where x can be (A..D) to select the GPIO peripheral.
/// @retval GPIO port output data value.
////////////////////////////////////////////////////////////////////////////////
u16 GPIO_ReadOutputData(GPIO_TypeDef* GPIOx)
{
    return GPIOx->ODR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the selected GPIO port pin.
/// @param  GPIOx: where x can be (A..D) to select the GPIO peripheral.
/// @param  pin: specifies the port pins to be written.
///         This parameter can be any combination of GPIO_Pin_x where x can be
///         (0..15).
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void GPIO_SetBits(GPIO_TypeDef* GPIOx, u16 pin)
{
    GPIOx->BSRR = pin;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the selected GPIO port bit.
/// @param  GPIOx: where x can be (A..D) to select the GPIO peripheral.
/// @param  pin: specifies the port pins to be written.
///         This parameter can be any combination of GPIO_Pin_x where x can be
///         (0..15).
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, u16 pin)
{
    GPIOx->BRR = pin;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets or clears the selected GPIO port pin.
/// @param  GPIOx: where x can be (A..D) to select the GPIO peripheral.
/// @param  pin: specifies the port bit to be written.
///         This parameter can be one of GPIO_Pin_x where x can be (0..15).
/// @param  value: specifies the value to be written to the selected bit.
///         This parameter can be one of the BitAction enum values:
/// @arg    Bit_RESET: to clear the port pin
/// @arg    Bit_SET: to set the port pin
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, u16 pin, BitAction value)
{
    (value) ? (GPIOx->BSRR = pin) : (GPIOx->BRR = pin);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Writes data to all GPIO port pins.
/// @param  GPIOx: where x can be (A..D) to select the GPIO peripheral.
/// @param  value: specifies the value to be written to the port output data
///         register.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void GPIO_Write(GPIO_TypeDef* GPIOx, u16 value)
{
    GPIOx->ODR = value;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Locks GPIO Pins configuration.
/// @param  GPIOx: where x can be (A..D) to select the GPIO peripheral.
/// @param  pin: specifies the port bit to be written.
///         This parameter can be any combination of GPIO_Pin_x where x can be
///         (0..15).
/// @param  newState: new lock state of the port pin.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void GPIO_PinLock(GPIO_TypeDef* GPIOx, u16 pin, FunctionalState newState)
{
    (newState) ? (GPIOx->LCKR |= pin) : (GPIOx->LCKR &= ~pin);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Locks GPIO Pins configuration registers until next system reset.
/// @param  GPIOx: where x can be (A..D) to select the GPIO peripheral.
/// @param  pin: specifies the port bit to be written.
///         This parameter can be any combination of GPIO_Pin_x where x can be
///         (0..15).
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void GPIO_PinLockConfig(GPIO_TypeDef* GPIOx, u16 pin)
{
    GPIOx->LCKR = GPIO_LCKR_LCKK | pin;
    GPIOx->LCKR = pin;
    GPIOx->LCKR = GPIO_LCKR_LCKK | pin;
    GPIOx->LCKR;
    GPIOx->LCKR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the port pin remapping.
/// @param  remap: selects the pin to remap.
/// @param  mask: the corresponding remapping mask of the remapping pin.
/// @param  state: new state of the port pin remapping.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
#if defined(__MM3N1)
void GPIO_PinRemapConfigSub(u32 remap, u32 mask, bool state)
{
    EXTI->MAPR &= ~mask;
    if (state) EXTI->MAPR |= remap & mask;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Changes the mapping of the specified pin.
/// @param  remap: selects the pin to remap.
///         This parameter can be one of the following values:
/// @arg    EXTI_MAPR_PD01              : SPI1 Alternate Function mapping
/// @arg    EXTI_MAPR_I2C1              : I2C1 Alternate Function mapping
/// @arg    EXTI_MAPR_UART1             : UART1 Alternate Function mapping
/// @arg    EXTI_MAPR_UART3             : UART3 Full Alternate Function
/// mapping
/// @arg    EXTI_MAPR_TIM1_PARTIAL      : TIM1 Partial Alternate Function
/// mapping
/// @arg    EXTI_MAPR_TIM1              : TIM1 Full Alternate Function
/// mapping
/// @arg    EXTI_MAPR_TIM2_PARTIAL1     : TIM2 Partial1 Alternate Function
/// mapping
/// @arg    EXTI_MAPR_TIM2_PARTIAL2     : TIM2 Partial2 Alternate Function
/// mapping
/// @arg    EXTI_MAPR_TIM2              : TIM2 Full Alternate Function
/// mapping
/// @arg    EXTI_MAPR_TIM3_PARTIAL      : TIM3 Partial Alternate Function
/// mapping
/// @arg    EXTI_MAPR_TIM3              : TIM3 Full Alternate Function
/// mapping
/// @arg    EXTI_MAPR_CAN               : CAN1 Alternate Function mapping
/// @arg    EXTI_MAPR_PD01              : PD01 Alternate Function mapping
/// @arg    EXTI_MAPR_SWJ_NOJNTRST      : Full SWJ Enabled (JTAG-DP + SW-DP)
/// but without JTRST
/// @arg    EXTI_MAPR_SWJ_JTAGDISABLE   : JTAG-DP Disabled and SW-DP Enabled
/// @arg    EXTI_MAPR_SWJ_DISABLE       : Full SWJ Disabled (JTAG-DP +
/// SW-DP)
/// @param  newState: new state of the port pin remapping.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void GPIO_PinRemapConfig(u32 remap, FunctionalState newState)
{
    if (remap >> 16)                        GPIO_PinRemapConfigSub(remap, EXTI_MAPR_SWJ_MASK, newState);    // bit 15, 2, 1, 0
    else if (remap & EXTI_MAPR_PD01)        GPIO_PinRemapConfigSub(remap, EXTI_MAPR_PD01, newState);
    else if (remap & EXTI_MAPR_UART1)       GPIO_PinRemapConfigSub(remap, EXTI_MAPR_UART1, newState);
    else if (remap & EXTI_MAPR_I2C1)        GPIO_PinRemapConfigSub(remap, EXTI_MAPR_I2C1, newState);
    else if (remap & EXTI_MAPR_SPI1)        GPIO_PinRemapConfigSub(remap, EXTI_MAPR_SPI1, newState);        // bit 5,4
    else if (remap & EXTI_MAPR_UART3_MASK)  GPIO_PinRemapConfigSub(remap, EXTI_MAPR_UART3_MASK, newState);  // bit 7, 6
    else if (remap & EXTI_MAPR_TIM1_MASK)   GPIO_PinRemapConfigSub(remap, EXTI_MAPR_TIM1_MASK, newState);   // bit 9, 8
    else if (remap & EXTI_MAPR_TIM2_MASK)   GPIO_PinRemapConfigSub(remap, EXTI_MAPR_TIM2_MASK, newState);   // bit 11, 10
    else if (remap & EXTI_MAPR_TIM3_MASK)   GPIO_PinRemapConfigSub(remap, EXTI_MAPR_TIM3_MASK, newState);   // bit 14, 13
    else if (remap & EXTI_MAPR_CAN_MASK)    GPIO_PinRemapConfigSub(remap, EXTI_MAPR_CAN_MASK, newState);
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief  Writes data to the specified GPIO data port.
/// @param  GPIOx: where x can be (A, B, C, D ) to select the GPIO peripheral.
/// @param  pin: specifies the pin for the Alternate function.
///         This parameter can be GPIO_PinSourcex where x can be (0..15) for
///         GPIOA, GPIOB, GPIOD and (0..12) for GPIOC .
/// @param  funcAF: selects the pin to used as Alternate function.
///         This parameter can be the GPIO_AF_x where x can be (0..7).
/// @note   The pin should be used for Digital IP.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, u8 pin, u8 funcAF)
{
    u8 shift = (pin & 0x07) * 4;
    u32* ptr = (pin < 8) ? (u32*)&GPIOx->AFRL : (u32*)&GPIOx->AFRH;
    *ptr = (*ptr & ~(0x0F << shift)) | (funcAF << shift);
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the remap function and AF function of the GPIO pin.
/// @param  GPIOx: where x can be (A, B, C, D ) to select the GPIO peripheral.
/// @param  pin: specifies the pin for the Alternate function.
///         This parameter can be GPIO_Pin_x where x can be (0..15) for
///         GPIOA, GPIOB, GPIOD and (0..12) for GPIOC .
/// @param  remap: selects the pin to remap.
/// @param  funcAF: selects the pin to used as Alternate function.
///         This parameter can be the GPIO_AF_x where x can be (0..7).
/// @note   The pin should be used for Digital IP.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exGPIO_PinAFConfig(GPIO_TypeDef* GPIOx, u16 pin, s32 remap, s8 funcAF)
{
    (void)remap;

#if defined(__MM3N1)
    if (remap > 0) {
        COMMON_EnableIpClock(emCLOCK_EXTI);
        GPIO_PinRemapConfig(remap, ENABLE);
    }
#endif

#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    if (funcAF >= 0) {
        for (u8 i = 0; i < 32; i++) {
            if (pin & 0x01) {
                pin = i;
                break;
            }
            pin >>= 1;
        }

        u8 shift = (pin & 0x07) * 4;
        u32* ptr = (pin < 8) ? (u32*)&GPIOx->AFRL : (u32*)&GPIOx->AFRH;
        *ptr = (*ptr & ~(0x0F << shift)) | (funcAF << shift);
    }
#endif
}

/// @}

/// @}

/// @}

