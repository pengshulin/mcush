////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_RCC.C
/// @author   S Yi
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE RCC FIRMWARE FUNCTIONS.
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
#define _HAL_RCC_C_

// Files includes  -------------------------------------------------------------
#include "HAL_rcc.h"
#include "HAL_nvic.h"

u8 tbPresc[] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup RCC_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup RCC_Exported_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  Resets the RCC clock configuration to default state.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_DeInit()
{
    SET_BIT(RCC->CR, RCC_CR_HSION);
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_SW);
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON | RCC_CR_PLLDIV | RCC_CR_PLLMUL);
#endif
#if defined(__MM0Q1)
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
#endif
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
    CLEAR_REG(RCC->CFGR);
    CLEAR_REG(RCC->CIR);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the External High Speed oscillator (HSE).
/// @param  state: specifies the new state of HSE.
///   This parameter can be one of the following values:
/// @arg RCC_HSE_OFF: HSE oscillator OFF
/// @arg RCC_HSE_ON: HSE oscillator ON
/// @arg RCC_HSE_Bypass: HSE oscillator bypassed with external
///   clock
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_HSEConfig(RCCHSE_TypeDef state)
{
    RCC->CR &= ~(RCC_CR_HSEBYP | RCC_CR_HSEON);
    switch (state) {
        case RCC_HSE_Bypass: RCC->CR |= RCC_CR_HSEBYP;
        case RCC_HSE_ON: RCC->CR |= RCC_CR_HSEON;
        default: break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified RCC flag is set or not.
/// @param  flag: specifies the flag to check.
///   This parameter can be one of the following values:
/// @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready
/// @arg RCC_FLAG_HSERDY: HSE oscillator clock ready
/// @arg RCC_FLAG_PLLRDY: PLL clock ready
/// @arg RCC_FLAG_LSERDY: LSE oscillator clock ready
/// @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready
/// @arg RCC_FLAG_PINRST: Pin reset
/// @arg RCC_FLAG_PORRST: POR/PDR reset
/// @arg RCC_FLAG_SFTRST: Software reset
/// @arg RCC_FLAG_IWDGRST: Independent Watchdog reset
/// @arg RCC_FLAG_WWDGRST: Window Watchdog reset
/// @arg RCC_FLAG_LPWRRST: Low Power reset
/// @retval The new state of flag (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
FlagStatus RCC_GetFlagStatus(RCC_FLAG_TypeDef flag)
{
#if defined(__MM3N1) || defined(__MM3O1)
    return ((((flag >> 5) == CR_REG_INDEX) ? RCC->CR : (((flag >> 5) == BDCR_REG_INDEX) ? RCC->BDCR : RCC->CSR)) &
            (1 << (flag & 0x1F)))
               ? SET : RESET;
#endif
#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    return ((((flag >> 5) == CR_REG_INDEX) ? RCC->CR : (((flag >> 5) == BDCR_REG_INDEX) ? 0 : RCC->CSR)) &
            (1 << (flag & 0x1F)))
               ? SET : RESET;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Waits for HSE start-up.
/// @param  None.
/// @retval  An ErrorStatus enumuration value:
/// - SUCCESS: HSE oscillator is stable and ready to use
/// - ERROR: HSE oscillator not yet ready
////////////////////////////////////////////////////////////////////////////////
ErrorStatus RCC_WaitForHSEStartUp(void)
{
    u32 StartUpCounter = 0;

    FlagStatus HSEStatus;

    do {
        HSEStatus = RCC_GetFlagStatus(RCC_FLAG_HSERDY);
        StartUpCounter++;
    } while ((HSEStatus == RESET) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    return (ErrorStatus)(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET) ? SUCCESS : ERROR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Waits for flag start-up.
/// @param  flag: specifies the flag to check.
///   This parameter can be one of the following values:
/// @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready
/// @arg RCC_FLAG_HSERDY: HSE oscillator clock ready
/// @arg RCC_FLAG_PLLRDY: PLL clock ready
/// @arg RCC_FLAG_LSERDY: LSE oscillator clock ready
/// @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready
/// @arg RCC_FLAG_PINRST: Pin reset
/// @arg RCC_FLAG_PORRST: POR/PDR reset
/// @arg RCC_FLAG_SFTRST: Software reset
/// @arg RCC_FLAG_IWDGRST: Independent Watchdog reset
/// @arg RCC_FLAG_WWDGRST: Window Watchdog reset
/// @retval  An ErrorStatus enumuration value:
/// - SUCCESS: HSE oscillator is stable and ready to use
/// - ERROR: HSE oscillator not yet ready
////////////////////////////////////////////////////////////////////////////////
ErrorStatus RCC_WaitForFlagStartUp(RCC_FLAG_TypeDef flag)
{
    u32 StartUpCounter = 0;

    while (RCC_GetFlagStatus(flag) == RESET) {
        if (StartUpCounter++ > HSE_STARTUP_TIMEOUT) {
            return ERROR;
        }
    }
    return SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the Internal High Speed oscillator (HSI).
/// @param  state: new state of the HSI.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_HSICmd(FunctionalState state)
{
    MODIFY_REG(RCC->CR, RCC_CR_HSION, (state << RCC_CR_HSION_Pos));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the system clock (SYSCLK).
/// @param  sysClkSrc: specifies the clock source used as system
///   clock. This parameter can be one of the following values:
/// @arg RCC_HSI: specifies HSI as system clock
/// @arg RCC_HSE: specifies HSE as system clock
/// @arg RCC_PLL: specifies PLL as system clock
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_SYSCLKConfig(SYSCLK_TypeDef sysClkSrc)
{
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, (sysClkSrc << RCC_CFGR_SW_Pos));
}

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0P1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the PLL clock source and DM DN factor.
///   This function must be used only when the PLL is disabled.
/// @param  plldn: specifies the PLL multiplication factor.
///   This parameter can be RCC_PLLMul_x where x:[31:26]
/// @param  plldm: specifies the PLL Divsior factor.
///   This parameter can be RCC_Divsior_x where x:[22:20]
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_PLLDMDNConfig(u32 plldn, u32 plldm)
{
    MODIFY_REG(RCC->CR, (RCC_CR_PLLMUL | RCC_CR_PLLDIV), ((plldn << RCC_CR_PLLMUL_Pos) | (plldm << RCC_CR_PLLDIV_Pos)));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the PLL.
///   The PLL can not be disabled if it is used as system clock.
/// @param  state: new state of the PLL.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_PLLCmd(FunctionalState state)
{
    MODIFY_REG(RCC->CR, RCC_CR_PLLON, (state << RCC_CR_PLLON_Pos));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the PLL clock source and multiplication factor.
///   This function must be used only when the PLL is disabled.
/// @param  pllSrc: specifies the PLL entry clock source.
///   This parameter can be one of the following values:
/// @arg RCC_HSI_Div4: HSI oscillator clock divided
///   by 4 selected as PLL clock entry
/// @arg RCC_HSE_Div1: HSE oscillator clock selected
///   as PLL clock entry
/// @arg RCC_HSE_Div2: HSE oscillator clock divided
///   by 2 selected as PLL clock entry
/// @param  pllMul: specifies the PLL multiplication factor.
///   This parameter can be RCC_PLLMul_x where x:[31:26][22:20]
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_PLLConfig(RCC_PLLSource_TypeDef pllSrc, RCC_PLLMul_TypeDef pllMul)
{
    MODIFY_REG(RCC->CFGR, (RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC), pllSrc);

    u8 DNDM_Item[] = {0x07, 0x03, 0x05, 0x01, 0x07, 0x01, 0x09, 0x01,  // Frclk*8/4 ; Frclk*6/2 ; Frclk*8/2 ; Frclk*10/2;
                      0x0B, 0x01, 0x0D, 0x01, 0x0F, 0x01, 0x11, 0x01,  // Frclk*12/2; Frclk*14/2; Frclk*16/2; Frclk*18/2;
                      0x13, 0x01, 0x15, 0x01, 0x17, 0x01, 0x19, 0x01,  // Frclk*20/2; Frclk*22/2; Frclk*24/2; Frclk*26/2;
                      0x1B, 0x01, 0x1D, 0x01, 0x1F, 0x01};             // Frclk*28/2; Frclk*30/2;
                                                                       // Frclk*32/2;
    RCC_PLLDMDNConfig((u32)DNDM_Item[pllMul >> 17], (u32)DNDM_Item[(pllMul >> 17) + 1]);
}
#endif

#if defined(__MM3N1) || defined(__MM0N1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the USB clock (USBCLK).
/// @param  usbClkSrc: specifies the USB clock source.
///   This clock is derived from the PLL output.
///   This parameter can be one of the following values:
/// @arg RCC_USBCLKSource_PLLCLK_Div1: PLL clock selected as USB clock source
/// @arg RCC_USBCLKSource_PLLCLK_Div2: PLL clock divided by 2 selected as USB
/// clock source
/// @arg RCC_USBCLKSource_PLLCLK_Div3: PLL clock divided by 3 selected as USB
/// clock source
/// @arg RCC_USBCLKSource_PLLCLK_Div4: PLL clock divided by 4 selected as USB
/// clock source
/// @arg RCC_USBCLKSource_PLLCLK_Div5: PLL clock divided by 5 selected as USB
/// clock source
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_USBCLKConfig(RCC_USBCLKSOURCE_TypeDef usbClkSrc)
{
    MODIFY_REG(RCC->CFGR, RCC_CFGR_USBPRE, (usbClkSrc << RCC_CFGR_USBPRE_Pos));
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the clock source used as system clock.
/// @param  None.
/// @retval The clock source used as system clock. The returned value can
///   be one of the following:
/// - 0x00: HSI/6 used as system clock
/// - 0x04: HSE used as system clock
/// - 0x08: PLL used as system clock
////////////////////////////////////////////////////////////////////////////////
u8 RCC_GetSYSCLKSource(void)
{
    return ((u8)READ_BIT(RCC->CFGR, RCC_CFGR_SWS));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the AHB clock (HCLK).
/// @param  sysClk: defines the AHB clock divider. This clock is derived
///                    from the system clock (SYSCLK).
///   This parameter can be one of the following values:
/// @arg RCC_SYSCLK_Div1: AHB clock = SYSCLK
/// @arg RCC_SYSCLK_Div2: AHB clock = SYSCLK/2
/// @arg RCC_SYSCLK_Div4: AHB clock = SYSCLK/4
/// @arg RCC_SYSCLK_Div8: AHB clock = SYSCLK/8
/// @arg RCC_SYSCLK_Div16: AHB clock = SYSCLK/16
/// @arg RCC_SYSCLK_Div64: AHB clock = SYSCLK/64
/// @arg RCC_SYSCLK_Div128: AHB clock = SYSCLK/128
/// @arg RCC_SYSCLK_Div256: AHB clock = SYSCLK/256
/// @arg RCC_SYSCLK_Div512: AHB clock = SYSCLK/512
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_HCLKConfig(RCC_AHB_CLK_TypeDef sysClk)
{
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, sysClk);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the Low Speed APB clock (PCLK1).
/// @param  HCLK: defines the APB1 clock divider. This clock is derived from
///                  the AHB clock (HCLK).
///   This parameter can be one of the following values:
/// @arg RCC_HCLK_Div1: APB1 clock = HCLK
/// @arg RCC_HCLK_Div2: APB1 clock = HCLK/2
/// @arg RCC_HCLK_Div4: APB1 clock = HCLK/4
/// @arg RCC_HCLK_Div8: APB1 clock = HCLK/8
/// @arg RCC_HCLK_Div16: APB1 clock = HCLK/16
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_PCLK1Config(RCC_APB1_APB2_CLK_TypeDef HCLK)
{
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, HCLK);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the High Speed APB clock (PCLK2).
/// @param  HCLK: defines the APB2 clock divider. This clock is derived from
///                  the AHB clock (HCLK).
///   This parameter can be one of the following values:
/// @arg RCC_HCLK_Div1: APB2 clock = HCLK
/// @arg RCC_HCLK_Div2: APB2 clock = HCLK/2
/// @arg RCC_HCLK_Div4: APB2 clock = HCLK/4
/// @arg RCC_HCLK_Div8: APB2 clock = HCLK/8
/// @arg RCC_HCLK_Div16: APB2 clock = HCLK/16
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_PCLK2Config(RCC_APB1_APB2_CLK_TypeDef HCLK)
{
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, (HCLK << 3));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the ADC clock (ADCCLK).
/// @param  PCLK2: defines the ADC clock divider. This clock is derived from
///                   the APB2 clock (PCLK2).
///   This parameter can be one of the following values:
/// @arg RCC_PCLK2_Div2: ADC clock = PCLK2/2
/// @arg RCC_PCLK2_Div4: ADC clock = PCLK2/4
/// @arg RCC_PCLK2_Div6: ADC clock = PCLK2/6
/// @arg RCC_PCLK2_Div8: ADC clock = PCLK2/8
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_ADCCLKConfig(RCC_ADCCLKSOURCE_TypeDef PCLK2)
{
    MODIFY_REG(RCC->CFGR, ADC_CFGR_PRE, PCLK2);
}

#if defined(__MM3N1) || defined(__MM3O1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the External Low Speed oscillator (LSE).
/// @param  state: specifies the new state of the LSE.
///   This parameter can be one of the following values:
/// @arg RCC_LSE_OFF: LSE oscillator OFF
/// @arg RCC_LSE_ON: LSE oscillator ON
/// @arg RCC_LSE_Bypass: LSE oscillator bypassed with external
///   clock
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_LSEConfig(RCC_LSE_TypeDef state)
{
    RCC->BDCR &= ~(RCC_BDCR_LSEBYP | RCC_BDCR_LSEON);

    switch (state) {
        case RCC_LSE_Bypass: RCC->BDCR |= RCC_BDCR_LSEBYP;

        case RCC_LSE_ON: RCC->BDCR |= RCC_BDCR_LSEON;

        default: break;
    }
}
////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the RTC clock (RTCCLK).
///   Once the RTC clock is selected it can be changed unless the
///   Backup domain is reset.
/// @param  rtcClkSrc: specifies the RTC clock source.
///   This parameter can be one of the following values:
/// @arg RCC_RTCCLKSource_LSE: LSE selected as RTC clock
/// @arg RCC_RTCCLKSource_LSI: LSI selected as RTC clock
/// @arg RCC_RTCCLKSource_HSE_Div128: HSE clock divided by 128
///   selected as RTC clock
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_RTCCLKConfig(RCC_RTCCLKSOURCE_TypeDef rtcClkSrc)
{
    MODIFY_REG(RCC->BDCR, RCC_BDCR_RTCSEL, rtcClkSrc);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the RTC clock.
///   This function must be used only after the RTC clock was
///   selected using the RCC_RTCCLKConfig function.
/// @param  state: new state of the RTC clock.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_RTCCLKCmd(FunctionalState state)
{
    MODIFY_REG(RCC->BDCR, RCC_BDCR_RTCEN, (state << RCC_BDCR_RTCEN_Pos));
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the Internal Low Speed oscillator (LSI).
///   LSI can not be disabled if the IWDG is running.
/// @param  state: new state of the LSI.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_LSICmd(FunctionalState state)
{
    MODIFY_REG(RCC->CSR, RCC_CSR_LSION, (state << RCC_CSR_LSION_Pos));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the clock frequency of different on chip clocks.
/// @param  None.
/// @retval sysClk : System clock frequency
////////////////////////////////////////////////////////////////////////////////
u32 RCC_GetSysClockFreq(void)
{
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
    u32 clock, mul, div;
#endif
    switch (RCC->CFGR & RCC_CFGR_SWS) {
#if defined(__MM0P1) || defined(__MM0Q1)
        case RCC_CFGR_SWS_LSI:
            return LSI_VALUE;
#endif

        case RCC_CFGR_SWS_HSE:
            return HSE_VALUE;

#if defined(__MM0Q1)
        case RCC_CFGR_SWS_HSI:
            return (RCC->CR & RCC_CR_HSI_72M) ? HSI_72MHz : HSI_48MHz;
#endif

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
        case RCC_CFGR_SWS_PLL:
            clock = READ_BIT(RCC->CFGR, RCC_CFGR_PLLSRC) ? (READ_BIT(RCC->CFGR, RCC_CFGR_PLLXTPRE) ? (HSE_VALUE >> 1) : HSE_VALUE)
                : HSI_VALUE_PLL_ON;
            mul = ((RCC->CR & (u32)RCC_CR_PLLMUL) >> RCC_CR_PLLMUL_Pos) + 1;
            div = ((RCC->CR & RCC_CR_PLLDIV) >> RCC_CR_PLLDIV_Pos) + 1;

            return  clock * mul / div;
#endif

        default:
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
            return HSI_DIV6;
#endif
#if defined(__MM0Q1)
            return (RCC->CR & RCC_CR_HSI_72M) ? HSI_72MHz_DIV6 : HSI_48MHz_DIV6;
#endif
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the HCLK frequency of different on chip clocks.
/// @param  None.
/// @retval HCLK frequency
////////////////////////////////////////////////////////////////////////////////
u32 RCC_GetHCLKFreq(void)
{
    return (RCC_GetSysClockFreq() >> tbPresc[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos]);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the PCLK1 frequency of different on chip clocks.
/// @param  None.
/// @retval PCLK1 frequency
////////////////////////////////////////////////////////////////////////////////
u32 RCC_GetPCLK1Freq(void)
{
    return (RCC_GetHCLKFreq() >> tbPresc[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos]);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the PCLK2 frequency of different on chip clocks.
/// @param  None.
/// @retval PCLK2 frequency
////////////////////////////////////////////////////////////////////////////////
u32 RCC_GetPCLK2Freq(void)
{
    return (RCC_GetHCLKFreq() >> tbPresc[(RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos]);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the frequency of different on chip clocks.
/// @param  clk: pointer to a RCC_ClocksTypeDef structure which
///   will hold the clocks frequency.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_GetClocksFreq(RCC_ClocksTypeDef* clk)
{
    u8 tbADCPresc[] = {2, 4, 6, 8};

    clk->SYSCLK_Frequency = RCC_GetSysClockFreq();
    clk->HCLK_Frequency   = RCC_GetHCLKFreq();
    clk->PCLK1_Frequency  = RCC_GetPCLK1Freq();
    clk->PCLK2_Frequency  = RCC_GetPCLK2Freq();

    clk->ADCCLK_Frequency = clk->PCLK2_Frequency / tbADCPresc[(RCC->CFGR & ADC_CFGR_PRE) >> ADC_CFGR_PRE_Pos];
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the AHB peripheral clock.
/// @param  AHBPeriph: specifies the AHB peripheral to gates its clock.
///   This parameter can be any combination of the following values:
/// __MM3N1
/// @arg RCC_AHBENR_DMA1, RCC_AHBENR_SRAM, RCC_AHBENR_FLITF,
///      RCC_AHBENR_CRC, RCC_AHBENR_AES
/// __MM0N1
/// @arg RCC_AHBENR_DMA1, RCC_AHBENR_SRAM, RCC_AHBENR_FLITF,
///      RCC_AHBENR_CRC, RCC_AHBENR_AES, RCC_AHBENR_GPIOA,
///      RCC_AHBENR_GPIOB, RCC_AHBENR_GPIOC, RCC_AHBENR_GPIOD,
///      RCC_AHBENR_GPIOE
/// __MM0P1
/// @arg RCC_AHBENR_DMA1, RCC_AHBENR_SRAM, RCC_AHBENR_FLITF,
///      RCC_AHBENR_CRC, RCC_AHBENR_GPIOA, RCC_AHBENR_GPIOB,
///      RCC_AHBENR_GPIOC, RCC_AHBENR_GPIOD, RCC_AHBENR_HWSQRT,
///      RCC_AHBENR_HWDIV
/// __MM0Q1
/// @arg RCC_AHBENR_DMA1, RCC_AHBENR_SRAM, RCC_AHBENR_FLITF,
///      RCC_AHBENR_CRC, RCC_AHBENR_GPIOA, RCC_AHBENR_GPIOB,
///      RCC_AHBENR_GPIOC, RCC_AHBENR_GPIOD, RCC_AHBENR_HWDIV
/// @param  state: new state of the specified peripheral clock.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_AHBPeriphClockCmd(u32 AHBPeriph, FunctionalState state)
{
    (state) ? (RCC->AHBENR |= AHBPeriph) : (RCC->AHBENR &= ~AHBPeriph);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the High Speed APB (APB2) peripheral clock.
/// @param  APB2Periph: specifies the APB2 peripheral to gates its
///   clock.
///   This parameter can be any combination of the following values:
/// __MM3N1
/// @arg RCC_APB2ENR_EXTI, RCC_APB2ENR_GPIOA, RCC_APB2ENR_GPIOB,
///      RCC_APB2ENR_GPIOC, RCC_APB2ENR_GPIOD, RCC_APB2ENR_GPIOE,
///      RCC_APB2ENR_ADC1, RCC_APB2ENR_ADC2, RCC_APB2ENR_TIM1,
///      RCC_APB2ENR_SPI1, RCC_APB2ENR_UART1, RCC_APB2ENR_COMP
/// __MM0N1
/// @arg RCC_APB2ENR_EXTI, RCC_APB2ENR_ADC1, RCC_APB2ENR_TIM1,
///      RCC_APB2ENR_SPI1, RCC_APB2ENR_UART1, RCC_APB2ENR_COMP,
///      RCC_APB2ENR_TIM14, RCC_APB2ENR_TIM16, RCC_APB2ENR_TIM17,
///      RCC_APB2ENR_DBGMCU
/// __MM0P1
/// @arg RCC_APB2ENR_EXTI, RCC_APB2ENR_ADC1, RCC_APB2ENR_ADC2,
///      RCC_APB2ENR_TIM1, RCC_APB2ENR_SPI1, RCC_APB2ENR_TIM8,
///      RCC_APB2ENR_UART1, RCC_APB2ENR_COMP, RCC_APB2ENR_TIM14,
///      RCC_APB2ENR_TIM16, RCC_APB2ENR_TIM17, RCC_APB2ENR_DBGMCU,
///      RCC_APB2RSTR_PWM
/// __MM0Q1
/// @arg RCC_APB2ENR_EXTI, RCC_APB2ENR_ADC1, RCC_APB2ENR_TIM1,
///      RCC_APB2ENR_SPI1, RCC_APB2ENR_UART1, RCC_APB2ENR_COMP,
///      RCC_APB2ENR_TIM14, RCC_APB2ENR_TIM16, RCC_APB2ENR_TIM17,
///      RCC_APB2ENR_DBGMCU, RCC_APB2ENR_PWM
/// @param  state: new state of the specified peripheral clock.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_APB2PeriphClockCmd(u32 APB2Periph, FunctionalState state)
{
    (state) ? (RCC->APB2ENR |= APB2Periph) : (RCC->APB2ENR &= ~APB2Periph);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the Low Speed APB (APB1) peripheral clock.
/// @param  APB1Periph: specifies the APB1 peripheral to gates its
///   clock.
///   This parameter can be any combination of the following values:
/// __MM3N1
/// @arg RCC_APB1ENR_TIM2, RCC_APB1ENR_TIM3, RCC_APB1ENR_TIM4,
///      RCC_APB1ENR_WWDG, RCC_APB1ENR_SPI2, RCC_APB1ENR_UART2,
///      RCC_APB1ENR_UART3, RCC_APB1ENR_I2C1, RCC_APB1ENR_I2C2,
///      RCC_APB1ENR_USB, RCC_APB1ENR_CAN, RCC_APB1ENR_BKP,
///      RCC_APB1ENR_PWR, RCC_APB1ENR_DAC, RCC_APB1ENR_ALL
/// __MM0N1
/// @arg RCC_APB1ENR_TIM2, RCC_APB1ENR_TIM3, RCC_APB1ENR_WWDG,
///      RCC_APB1ENR_SPI2, RCC_APB1ENR_UART2, RCC_APB1ENR_I2C1,
///      RCC_APB1ENR_USB, RCC_APB1ENR_CAN, RCC_APB1ENR_CRS,
///      RCC_APB1ENR_PWR
/// __MM0P1
/// @arg RCC_APB1ENR_TIM2, RCC_APB1ENR_TIM3, RCC_APB1ENR_WWDG,
///      RCC_APB1ENR_SPI2, RCC_APB1ENR_UART2, RCC_APB1ENR_I2C1,
///      RCC_APB1ENR_PWR
/// __MM0Q1
/// @arg RCC_APB1ENR_TIM2, RCC_APB1ENR_TIM3, RCC_APB1ENR_WWDG,
///      RCC_APB1ENR_SPI2, RCC_APB1ENR_UART2, RCC_APB1ENR_I2C1,
///      RCC_APB1ENR_PWR
/// @param  state: new state of the specified peripheral clock.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_APB1PeriphClockCmd(u32 APB1Periph, FunctionalState state)
{
    (state) ? (RCC->APB1ENR |= APB1Periph) : (RCC->APB1ENR &= ~APB1Periph);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Forces or releases High Speed APB (APB2) peripheral reset.
/// @param  APB2Periph: specifies the APB2 peripheral to reset.
///   This parameter can be any combination of the following values:
/// __MM3N1
/// @arg RCC_APB2RSTR_EXTI, RCC_APB2RSTR_GPIOA, RCC_APB2RSTR_GPIOB,
///      RCC_APB2RSTR_GPIOC, RCC_APB2RSTR_GPIOD, RCC_APB2RSTR_GPIOE,
///      RCC_APB2RSTR_ADC1, RCC_APB2RSTR_ADC2, RCC_APB2RSTR_TIM1,
///      RCC_APB2RSTR_SPI1, RCC_APB2RSTR_UART1, RCC_APB2RSTR_COMP
/// __MM0N1
/// @arg RCC_APB2RSTR_EXTI, RCC_APB2RSTR_ADC1, RCC_APB2RSTR_TIM1,
///      RCC_APB2RSTR_SPI1, RCC_APB2RSTR_UART1, RCC_APB2RSTR_COMP,
///      RCC_APB2RSTR_TIM14, RCC_APB2RSTR_TIM16, RCC_APB2RSTR_TIM17,
///      RCC_APB2RSTR_DBGMCU
/// __MM0P1
/// @arg RCC_APB2RSTR_EXTI, RCC_APB2RSTR_ADC1, RCC_APB2RSTR_ADC2,
///      RCC_APB2RSTR_TIM1, RCC_APB2RSTR_SPI1, RCC_APB2RSTR_TIM8,
///      RCC_APB2RSTR_UART1, RCC_APB2RSTR_COMP, RCC_APB2RSTR_TIM14,
///      RCC_APB2RSTR_TIM16, RCC_APB2RSTR_TIM17, RCC_APB2RSTR_DBGMCU,
///      RCC_APB2RSTR_PWM
/// __MM0Q1
/// @arg RCC_APB2RSTR_EXTI, RCC_APB2RSTR_ADC1, RCC_APB2RSTR_TIM1,
///      RCC_APB2RSTR_SPI1, RCC_APB2RSTR_UART1, RCC_APB2RSTR_COMP,
///      RCC_APB2RSTR_TIM14, RCC_APB2RSTR_TIM16, RCC_APB2RSTR_TIM17,
///      RCC_APB2RSTR_DBGMCU, RCC_APB2RSTR_PWM
/// @param  state: new state of the specified peripheral reset.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_APB2PeriphResetCmd(u32 APB2Periph, FunctionalState state)
{
    (state) ? (RCC->APB2RSTR |= APB2Periph) : (RCC->APB2RSTR &= ~APB2Periph);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Forces or releases Low Speed APB (APB1) peripheral reset.
/// @param  APB1Periph: specifies the APB1 peripheral to reset.
///   This parameter can be any combination of the following values:
/// __MM3N1
/// @arg RCC_APB1RSTR_TIM2, RCC_APB1RSTR_TIM3, RCC_APB1RSTR_TIM4,
///      RCC_APB1RSTR_WWDG, RCC_APB1RSTR_SPI2, RCC_APB1RSTR_UART2,
///      RCC_APB1RSTR_UART3, RCC_APB1RSTR_I2C1, RCC_APB1RSTR_I2C2,
///      RCC_APB1RSTR_USB, RCC_APB1RSTR_CAN, RCC_APB1RSTR_BKP,
///      RCC_APB1RSTR_PWR, RCC_APB1RSTR_DAC, RCC_APB1RSTR_ALL
/// __MM0N1
/// @arg RCC_APB1RSTR_TIM2, RCC_APB1RSTR_TIM3, RCC_APB1RSTR_WWDG,
///      RCC_APB1RSTR_SPI2, RCC_APB1RSTR_UART2, RCC_APB1RSTR_I2C1,
///      RCC_APB1RSTR_USB, RCC_APB1RSTR_CAN, RCC_APB1RSTR_CRS,
///      RCC_APB1RSTR_PWR
/// __MM0P1
/// @arg RCC_APB1RSTR_TIM2, RCC_APB1RSTR_TIM3, RCC_APB1RSTR_WWDG,
///      RCC_APB1RSTR_SPI2, RCC_APB1RSTR_UART2, RCC_APB1RSTR_I2C1,
///      RCC_APB1RSTR_PWR
/// __MM0Q1
/// @arg RCC_APB1RSTR_TIM2, RCC_APB1RSTR_TIM3, RCC_APB1RSTR_WWDG,
///      RCC_APB1RSTR_SPI2, RCC_APB1RSTR_UART2, RCC_APB1RSTR_I2C1,
///      RCC_APB1RSTR_PWR
/// @param  state: new state of the specified peripheral clock.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_APB1PeriphResetCmd(u32 APB1Periph, FunctionalState state)
{
    (state) ? (RCC->APB1RSTR |= APB1Periph) : (RCC->APB1RSTR &= ~APB1Periph);
}

#if defined(__MM0P1) || defined(__MM0Q1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Forces or releases Low Speed AHB peripheral reset.
/// @param  AHBPeriph: specifies the AHB peripheral to reset.
///   This parameter can be any combination of the following values:
/// __MM0P1
/// @arg RCC_AHBRSTR_DMA1, RCC_AHBRSTR_SRAM, RCC_AHBRSTR_FLITF,
///      RCC_AHBRSTR_CRC, RCC_AHBRSTR_GPIOA, RCC_AHBRSTR_GPIOB,
///      RCC_AHBRSTR_GPIOC, RCC_AHBRSTR_GPIOD, RCC_AHBRSTR_HWSQRT,
///      RCC_AHBRSTR_HWDIV
/// __MM0Q1
/// @arg RCC_AHBRSTR_DMA1, RCC_AHBRSTR_SRAM, RCC_AHBRSTR_FLITF,
///      RCC_AHBRSTR_CRC, RCC_AHBRSTR_GPIOA, RCC_AHBRSTR_GPIOB,
///      RCC_AHBRSTR_GPIOC, RCC_AHBRSTR_GPIOD, RCC_AHBRSTR_HWDIV
/// @param  state: new state of the specified peripheral clock.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_AHBPeriphResetCmd(u32 AHBPeriph, FunctionalState state)
{
    (state) ? (RCC->AHBRSTR |= AHBPeriph) : (RCC->AHBRSTR &= ~AHBPeriph);
}
#endif

#if defined(__MM3N1) || defined(__MM3O1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Forces or releases the Backup domain reset.
/// @param  state: new state of the Backup domain reset.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_BackupResetCmd(FunctionalState state)
{
    MODIFY_REG(RCC->BDCR, RCC_BDCR_BDRST, (state << RCC_BDCR_BDRST_Pos));
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the Clock Security System.
/// @param  state: new state of the Clock Security System..
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_ClockSecuritySystemCmd(FunctionalState state)
{
    MODIFY_REG(RCC->CR, RCC_CR_CSSON, (state << RCC_CR_CSSON_Pos));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Selects the clock source to output on MCO pin.
/// @param  mcoSrc: specifies the clock source to output.
///   This parameter can be one of the following values:
/// @arg RCC_MCO_NoClock: No clock selected
/// @arg RCC_MCO_LSI: LSI oscillator clock selected
/// @arg RCC_MCO_LSE: LSE oscillator clock selected
/// @arg RCC_MCO_SYSCLK: System clock selected
/// @arg RCC_MCO_HSI: HSI oscillator clock selected
/// @arg RCC_MCO_HSE: HSE oscillator clock selected
/// @arg RCC_MCO_PLLCLK_Div2: PLL clock divided by 2 selected
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_MCOConfig(RCC_MCO_TypeDef mcoSrc)
{
    MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO, mcoSrc);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the RCC reset flags.
///   The reset flags are: RCC_FLAG_PINRST, RCC_FLAG_PORRST,
///   RCC_FLAG_SFTRST, RCC_FLAG_IWDGRST, RCC_FLAG_WWDGRST,
///   RCC_FLAG_LPWRRST
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_ClearFlag(void)
{
    SET_BIT(RCC->CSR, RCC_CSR_RMVF);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified RCC interrupts.
/// @param  it: specifies the RCC interrupt sources to be enabled or
/// disabled.
///   This parameter can be any combination of the following values:
/// @arg RCC_IT_LSIRDY: LSI ready interrupt
/// @arg RCC_IT_LSERDY: LSE ready interrupt
/// @arg RCC_IT_HSIRDY: HSI ready interrupt
/// @arg RCC_IT_HSERDY: HSE ready interrupt
/// @arg RCC_IT_PLLRDY: PLL ready interrupt
/// @param  state: new state of the specified RCC interrupts.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_ITConfig(RCC_IT_TypeDef it, FunctionalState state)
{
    (state) ? SET_BIT(RCC->CIR, it << RCC_CIR_LSIRDYIE_Pos) : CLEAR_BIT(RCC->CIR, it << RCC_CIR_LSIRDYIE_Pos);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified RCC interrupt has occurred or not.
/// @param  it: specifies the RCC interrupt source to check.
///   This parameter can be one of the following values:
/// @arg RCC_IT_LSIRDY: LSI ready interrupt
/// @arg RCC_IT_LSERDY: LSE ready interrupt
/// @arg RCC_IT_HSIRDY: HSI ready interrupt
/// @arg RCC_IT_HSERDY: HSE ready interrupt
/// @arg RCC_IT_PLLRDY: PLL ready interrupt
/// @arg RCC_IT_CSS: Clock Security System interrupt
/// @retval The new state of it (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
ITStatus RCC_GetITStatus(RCC_IT_TypeDef it)
{
    return (ITStatus)READ_BIT(RCC->CIR, (it << RCC_CIR_LSIRDYF_Pos));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the RCC�?interrupt pending bits.
/// @param  it: specifies the interrupt pending bit to clear.
///   This parameter can be any combination of the following values:
/// @arg RCC_IT_LSIRDY: LSI ready interrupt
/// @arg RCC_IT_LSERDY: LSE ready interrupt
/// @arg RCC_IT_HSIRDY: HSI ready interrupt
/// @arg RCC_IT_HSERDY: HSE ready interrupt
/// @arg RCC_IT_PLLRDY: PLL ready interrupt
/// @arg RCC_IT_CSS: Clock Security System interrupt
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void RCC_ClearITPendingBit(u8 it)
{
    SET_BIT(RCC->CIR, (it << RCC_CIR_LSIRDYC_Pos));
}

////////////////////////////////////////////////////////////////////////////////
//
//  New Function Interface
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief  Forces or releases Low Speed APB (APB1) peripheral reset.
/// @param  RCC_APB1Periph: specifies the APB1 peripheral to reset.
///   This parameter can be any combination of the following values:
/// __MM3N1
/// @arg RCC_APB1RSTR_TIM2, RCC_APB1RSTR_TIM3, RCC_APB1RSTR_TIM4,
///      RCC_APB1RSTR_WWDG, RCC_APB1RSTR_SPI2, RCC_APB1RSTR_UART2,
///      RCC_APB1RSTR_UART3, RCC_APB1RSTR_I2C1, RCC_APB1RSTR_I2C2,
///      RCC_APB1RSTR_USB, RCC_APB1RSTR_CAN, RCC_APB1RSTR_BKP,
///      RCC_APB1RSTR_PWR, RCC_APB1RSTR_DAC, RCC_APB1RSTR_ALL
/// __MM0N1
/// @arg RCC_APB1RSTR_TIM2, RCC_APB1RSTR_TIM3, RCC_APB1RSTR_WWDG,
///      RCC_APB1RSTR_SPI2, RCC_APB1RSTR_UART2, RCC_APB1RSTR_I2C1,
///      RCC_APB1RSTR_USB, RCC_APB1RSTR_CAN, RCC_APB1RSTR_CRS,
///      RCC_APB1RSTR_PWR
/// __MM0P1
/// @arg RCC_APB1RSTR_TIM2, RCC_APB1RSTR_TIM3, RCC_APB1RSTR_WWDG,
///      RCC_APB1RSTR_SPI2, RCC_APB1RSTR_UART2, RCC_APB1RSTR_I2C1,
///      RCC_APB1RSTR_PWR
/// __MM0Q1
/// @arg RCC_APB1RSTR_TIM2, RCC_APB1RSTR_TIM3, RCC_APB1RSTR_WWDG,
///      RCC_APB1RSTR_SPI2, RCC_APB1RSTR_UART2, RCC_APB1RSTR_I2C1,
///      RCC_APB1RSTR_PWR
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exRCC_APB1PeriphReset(u32 RCC_APB1Periph)
{
    RCC->APB1RSTR |= RCC_APB1Periph;
    RCC->APB1RSTR &= ~RCC_APB1Periph;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exRCC_BackupReset()
{
#if defined(__MM3N1)
    RCC->BDCR |=   RCC_BDCR_BDRST;
    RCC->BDCR &=  ~RCC_BDCR_BDRST;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Forces or releases High Speed APB (APB2) peripheral reset.
/// @param  RCC_APB2Periph: specifies the APB2 peripheral to reset.
///   This parameter can be any combination of the following values:
/// __MM3N1
/// @arg RCC_APB2RSTR_EXTI, RCC_APB2RSTR_GPIOA, RCC_APB2RSTR_GPIOB,
///      RCC_APB2RSTR_GPIOC, RCC_APB2RSTR_GPIOD, RCC_APB2RSTR_GPIOE,
///      RCC_APB2RSTR_ADC1, RCC_APB2RSTR_ADC2, RCC_APB2RSTR_TIM1,
///      RCC_APB2RSTR_SPI1, RCC_APB2RSTR_UART1, RCC_APB2RSTR_COMP
/// __MM0N1
/// @arg RCC_APB2RSTR_EXTI, RCC_APB2RSTR_ADC1, RCC_APB2RSTR_TIM1,
///      RCC_APB2RSTR_SPI1, RCC_APB2RSTR_UART1, RCC_APB2RSTR_COMP,
///      RCC_APB2RSTR_TIM14, RCC_APB2RSTR_TIM16, RCC_APB2RSTR_TIM17,
///      RCC_APB2RSTR_DBGMCU
/// __MM0P1
/// @arg RCC_APB2RSTR_EXTI, RCC_APB2RSTR_ADC1, RCC_APB2RSTR_ADC2,
///      RCC_APB2RSTR_TIM1, RCC_APB2RSTR_SPI1, RCC_APB2RSTR_TIM8,
///      RCC_APB2RSTR_UART1, RCC_APB2RSTR_COMP, RCC_APB2RSTR_TIM14,
///      RCC_APB2RSTR_TIM16, RCC_APB2RSTR_TIM17, RCC_APB2RSTR_DBGMCU,
///      RCC_APB2RSTR_PWM
/// __MM0Q1
/// @arg RCC_APB2RSTR_EXTI, RCC_APB2RSTR_ADC1, RCC_APB2RSTR_TIM1,
///      RCC_APB2RSTR_SPI1, RCC_APB2RSTR_UART1, RCC_APB2RSTR_COMP,
///      RCC_APB2RSTR_TIM14, RCC_APB2RSTR_TIM16, RCC_APB2RSTR_TIM17,
///      RCC_APB2RSTR_DBGMCU, RCC_APB2RSTR_PWM
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exRCC_APB2PeriphReset(u32 RCC_APB2Periph)
{
    RCC->APB2RSTR |= RCC_APB2Periph;
    RCC->APB2RSTR &= ~RCC_APB2Periph;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Disable systick
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exRCC_SystickDisable()
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enable systick
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exRCC_SystickEnable(u32 sysTickPeriod)
{
    SysTick_Config(RCC_GetHCLKFreq() / 1000000 * sysTickPeriod);
}


#define RCC_LATENCY_TB      0
#define RCC_MUL_TB          1
#define RCC_DIV_TB          2
#define RCC_SW_TB           3
#define RCC_SRC_TB          4
#define RCC_ROW_TB          5


////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the External High Speed oscillator (HSE).
/// @param  para
/// @retval An ErrorStatus enumuration value:
/// - SUCCESS: HSE oscillator is stable and ready to use
/// - ERROR: HSE oscillator not yet ready
////////////////////////////////////////////////////////////////////////////////
ErrorStatus exRCC_Init(RCCInitStruct_TypeDef* para)
{
//------------------------------------------------------------------------------
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
    u8 Latency_Mul_Div_Sw_Src_tb[] = {
        0, 0,  0, 0, 0,  // HSI_6d,
        0, 0,  0, 2, 0,  // HSI_12MHz,          //  HSI_12MHz,
        0, 1,  0, 2, 0,  // HSI_24MHz,          //  HSI_24MHz,
        1, 3,  0, 2, 0,  // HSI_48MHz,          //  HSI_48MHz,
        2, 5,  0, 2, 0,  // HSI_72MHz,          //  HSI_72MHz,
        3, 7,  0, 2, 0,  // HSI_96MHz           //  HSI_96MHz,
        0, 0,  0, 1, 1,  // HSE_1x,             //  HSE_8MHz,
        0, 2,  0, 2, 1,  // HSE_3x,             //  HSE_24MHz,
        1, 3,  0, 2, 1,  // HSE_4x,             //  HSE_32MHz,
        1, 5,  0, 2, 1,  // HSE_6x,             //  HSE_48MHz,
        2, 6,  0, 2, 1,  // HSE_7x,             //  HSE_56MHz,
        2, 8,  0, 2, 1,  // HSE_9x,             //  HSE_72MHz,
        3, 11, 0, 2, 1   // HSE_12x,            //  HSE_96MHz,
    };
#endif
#if defined(__MM0Q1)
    u8 Latency_Mul_Div_Sw_Src_tb[] = {
        0, 0, 0, 0, 0,  //  HSI_6d,
        0, 0, 0, 1, 1,  //  HSE_1x,             //  HSE_8MHz-24MHz,
        1, 0, 0, 2, 0,  //  HSI_48MHz,
        2, 0, 0, 2, 0,  //  HSI_72MHz,          //  HSI_72MHz,
        0, 0, 0, 3, 2   //  LSI_40kHz
    };
#endif

    //------------------------------------------------------------------------------
    RCC_DeInit();

    // Protection
    if (para->RCC_SystemClock >= (sizeof(Latency_Mul_Div_Sw_Src_tb) / RCC_ROW_TB)) {
        para->RCC_SystemClock = SYSCLK_HSI_6d;                                  // default clock
        RCC->CR |= RCC_CR_HSION;
        if (!RCC_WaitForFlagStartUp(RCC_FLAG_HSIRDY))  return ERROR;
    }
    else {
        // Flash
        FLASH->ACR |= FLASH_ACR_PRFTBE;
        FLASH->ACR &= ~FLASH_ACR_LATENCY;
        FLASH->ACR |= Latency_Mul_Div_Sw_Src_tb[para->RCC_SystemClock * RCC_ROW_TB + RCC_LATENCY_TB];

//------------------------------------------------------------------------------
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
        // Set PLL
        if (Latency_Mul_Div_Sw_Src_tb[para->RCC_SystemClock * RCC_ROW_TB + RCC_SW_TB] == 2) {
            RCC->CR |= RCC_CR_PLLON;
            if (!RCC_WaitForFlagStartUp(RCC_FLAG_PLLRDY)) return ERROR;
        }
        RCC->CR |= (Latency_Mul_Div_Sw_Src_tb[para->RCC_SystemClock * RCC_ROW_TB + RCC_MUL_TB] << RCC_CR_PLLMUL_Pos) & RCC_CR_PLLMUL;   // RCC_CR_PLLMUL
        RCC->CR |= (Latency_Mul_Div_Sw_Src_tb[para->RCC_SystemClock * RCC_ROW_TB + RCC_DIV_TB] << RCC_CR_PLLDIV_Pos) & RCC_CR_PLLDIV;   // RCC_CR_PLLDIV
#endif

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1)
        RCC->CFGR |= (para->RCC_PrescaleUSB << RCC_CFGR_USBPRE_Pos) & RCC_CFGR_USBPRE;                                                  // RCC_CR_USB
#endif

#if defined(__MM0Q1)
    // Null
#endif

//------------------------------------------------------------------------------
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
        // Set Oscillator
        if (Latency_Mul_Div_Sw_Src_tb[para->RCC_SystemClock * RCC_ROW_TB + RCC_SRC_TB] == 0) {
            RCC->CR |= RCC_CR_HSION;
            if (!RCC_WaitForFlagStartUp(RCC_FLAG_HSIRDY)) return ERROR;
            RCC->CFGR &= ~RCC_CFGR_PLLSRC;
        }
        else {
            RCC->CR |= RCC_CR_HSEON;
            if (!RCC_WaitForFlagStartUp(RCC_FLAG_HSERDY)) return ERROR;
            RCC->CFGR |= RCC_CFGR_PLLSRC;
        }
#endif

#if defined(__MM0Q1)
        if (Latency_Mul_Div_Sw_Src_tb[para->RCC_SystemClock * RCC_ROW_TB + RCC_SRC_TB] == 1) {
            RCC->CR |= RCC_CR_HSEON;
            if (!RCC_WaitForFlagStartUp(RCC_FLAG_HSERDY)) return ERROR;
        }
        else if (Latency_Mul_Div_Sw_Src_tb[para->RCC_SystemClock * RCC_ROW_TB + RCC_SRC_TB] == 2) {
            RCC->CSR |= RCC_CSR_LSION;
            if (!RCC_WaitForFlagStartUp(RCC_FLAG_LSIRDY)) return ERROR;
        }
        else {
            (Latency_Mul_Div_Sw_Src_tb[para->RCC_SystemClock * RCC_ROW_TB + RCC_LATENCY_TB] == 1) ?
                (RCC->CR &= ~RCC_CR_HSI_72M) : (RCC->CR |=  RCC_CR_HSI_72M);

            RCC->CR |= RCC_CR_HSION;
            if (!RCC_WaitForFlagStartUp(RCC_FLAG_HSIRDY)) return ERROR;
        }
#endif
    }
    //------------------------------------------------------------------------------
    // AHB, APB1, APB2
    RCC->CFGR |= (para->RCC_PrescaleAHB  << RCC_CFGR_HPRE_Pos)  & RCC_CFGR_HPRE;
    RCC->CFGR |= (para->RCC_PrescaleAPB1 << RCC_CFGR_PPRE1_Pos) & RCC_CFGR_PPRE1;
    RCC->CFGR |= (para->RCC_PrescaleAPB2 << RCC_CFGR_PPRE2_Pos) & RCC_CFGR_PPRE2;

    // Clock Switch to
    RCC->CFGR |= (Latency_Mul_Div_Sw_Src_tb[para->RCC_SystemClock * 5 + RCC_SW_TB] << RCC_CFGR_SW_Pos) & RCC_CFGR_SW;

    while (((RCC->CFGR & RCC_CFGR_SWS) >> 2) != Latency_Mul_Div_Sw_Src_tb[para->RCC_SystemClock * RCC_ROW_TB + RCC_SW_TB]);

    if (para->SysTickEN == ENABLE)
        SysTick_Config(RCC_GetHCLKFreq() / 1000000 * para->SysTickPeriod);
    else
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    return SUCCESS;
}

#if defined(__MM3O1) || defined(__MM0P1) || defined(__MM0Q1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  External crystal drive current calibration value.If the crystal oscillator is abnormal,
///   the drive current can be adjusted to match the crystal oscillator.
/// @param  RCC_Config: external crystal drive current calibration value.
///   This parameter can be any combination of the following values:
/// @arg RCC_CONFIG_OSC_ITRIM_2mA, RCC_CONFIG_OSC_ITRIM_4mA,
///      RCC_CONFIG_OSC_ITRIM_6mA, RCC_CONFIG_OSC_ITRIM_8mA.
////////////////////////////////////////////////////////////////////////////////
void exRCC_Set_OSC_ITRIM_Config(u32 val)
{

  RCC->CONFIG &= ~RCC_CONFIG_OSC_ITRIM;
  RCC->CONFIG |= val;
}
#endif
/// @}

/// @}

/// @}
