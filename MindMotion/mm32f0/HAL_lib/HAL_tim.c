////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_TIM.C
/// @author   D Chen
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE TIM FIRMWARE FUNCTIONS.
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
#define _HAL_TIM_C_

// Files includes  -------------------------------------------------------------
#include "HAL_rcc.h"
#include "HAL_tim.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup TIM_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup TIM_Exported_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the TIMx peripheral registers to their default reset values.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_DeInit(TIM_TypeDef* TIMx)
{
    switch (*(u32*)&TIMx) {
        case (u32)TIM1: exRCC_APB2PeriphReset(RCC_APB2ENR_TIM1); break;
        case (u32)TIM2: exRCC_APB1PeriphReset(RCC_APB1ENR_TIM2); break;
        case (u32)TIM3: exRCC_APB1PeriphReset(RCC_APB1ENR_TIM3); break;
#if defined(__MM3N1) || defined(__MM3O1)
        case (u32)TIM4: exRCC_APB1PeriphReset(RCC_APB1ENR_TIM4); break;
#endif

#if defined(__MM3O1)
        case (u32)TIM5: exRCC_APB1PeriphReset(RCC_APB1ENR_TIM5); break;
        case (u32)TIM6: exRCC_APB1PeriphReset(RCC_APB1ENR_TIM6); break;
        case (u32)TIM7: exRCC_APB1PeriphReset(RCC_APB1ENR_TIM7); break;
#endif

#if defined(__MM3O1) || defined(__MM0P1)
        case (u32)TIM8: exRCC_APB2PeriphReset(RCC_APB2ENR_TIM8); break;
#endif

#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
        case (u32)TIM14: exRCC_APB2PeriphReset(RCC_APB2ENR_TIM14); break;
        case (u32)TIM16: exRCC_APB2PeriphReset(RCC_APB2ENR_TIM16); break;
        case (u32)TIM17: exRCC_APB2PeriphReset(RCC_APB2ENR_TIM17); break;
#endif
        default: break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the TIMx Time Base Unit peripheral according to
///         the specified parameters in the pInitStruct.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 6, 7, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  pInitStruct: pointer to a TIM_TimeBaseInitTypeDef
///         structure that contains the configuration information for the
///         specified TIM peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* pInitStruct)
{
    MODIFY_REG(TIMx->CR1, TIM_CR1_CKD, pInitStruct->TIM_ClockDivision);

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0Q1)
    if ((TIMx == TIM1) || (TIMx == TIM2) || (TIMx == TIM3))
#endif
#if defined(__MM3O1)
    if ((TIMx == TIM1) || (TIMx == TIM2) || (TIMx == TIM3) || (TIMx == TIM4) || (TIMx == TIM5) || (TIMx == TIM8))
#endif
#if defined(__MM0P1)
    if ((TIMx == TIM1) || (TIMx == TIM2) || (TIMx == TIM3) || (TIMx == TIM8))
#endif
        MODIFY_REG(TIMx->CR1, TIM_CR1_CMS | TIM_CR1_DIR, pInitStruct->TIM_CounterMode);

#if defined(__MM3N1)
    if (TIMx == TIM1)
#endif
#if defined(__MM0N1) || defined(__MM0Q1)
    if ((TIMx == TIM1) || (TIMx == TIM16) || (TIMx == TIM17))
#endif
#if defined(__MM3O1) || defined(__MM0P1)
    if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM16) || (TIMx == TIM17))
#endif
        SET_BIT(TIMx->RCR, pInitStruct->TIM_RepetitionCounter);

    SET_BIT(TIMx->ARR, pInitStruct->TIM_Period);
    SET_BIT(TIMx->PSC, pInitStruct->TIM_Prescaler);
    SET_BIT(TIMx->EGR, TIM_PSCReloadMode_Immediate);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the TIMx Channel1 according to the specified
///         parameters in the pInitStruct.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  pInitStruct: pointer to a TIM_OCInitTypeDef structure that
///         contains the configuration information for the specified TIM peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* pInitStruct)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC1M, pInitStruct->TIM_OCMode);
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC1P, pInitStruct->TIM_OCPolarity);
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC1E, pInitStruct->TIM_OutputState);
    SET_BIT(TIMx->CCR1, pInitStruct->TIM_Pulse);

#if defined(__MM3N1)
    if (TIMx == TIM1)
#endif
#if defined(__MM0N1) || defined(__MM0Q1)
    if ((TIMx == TIM1) || (TIMx == TIM16) || (TIMx == TIM17))
#endif
#if defined(__MM3O1)
    if ((TIMx == TIM1) || (TIMx == TIM8)
#endif
#if defined(__MM0P1)
    if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM16) || (TIMx == TIM17))
#endif
    {
        MODIFY_REG(TIMx->CCER, TIM_CCER_CC1NP, pInitStruct->TIM_OCNPolarity);
        MODIFY_REG(TIMx->CCER, TIM_CCER_CC1NE, pInitStruct->TIM_OutputNState);
        MODIFY_REG(TIMx->CR2, TIM_CR2_OIS1, pInitStruct->TIM_OCIdleState);
        MODIFY_REG(TIMx->CR2, TIM_CR2_OIS1N, pInitStruct->TIM_OCNIdleState);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the TIMx Channel2 according to the specified
///         parameters in the pInitStruct.
/// @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  pInitStruct: pointer to a TIM_OCInitTypeDef structure that
///         contains the configuration information for the specified TIM peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* pInitStruct)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC2M, pInitStruct->TIM_OCMode << 8);
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC2E | TIM_CCER_CC2P,
               (pInitStruct->TIM_OCPolarity << 4) | (pInitStruct->TIM_OutputState << 4));
    SET_BIT(TIMx->CCR2, pInitStruct->TIM_Pulse);

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0Q1)
    if (TIMx == TIM1)
#endif
#if defined(__MM3O1) || defined(__MM0P1)
    if ((TIMx == TIM1) || (TIMx == TIM8))
#endif
    {
        MODIFY_REG(TIMx->CCER, TIM_CCER_CC2NP | TIM_CCER_CC2NE,
                   (pInitStruct->TIM_OCNPolarity << 4) | (pInitStruct->TIM_OutputNState << 4));
        MODIFY_REG(TIMx->CR2, TIM_CR2_OIS2 | TIM_CR2_OIS2N,
                   (pInitStruct->TIM_OCIdleState << 2) | (pInitStruct->TIM_OCNIdleState << 2));
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the TIMx Channel3 according to the specified
///         parameters in the pInitStruct.
/// @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  pInitStruct: pointer to a TIM_OCInitTypeDef structure that
///         contains the configuration information for the specified TIM peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* pInitStruct)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC3M, pInitStruct->TIM_OCMode);
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC3E | TIM_CCER_CC3P,
               (pInitStruct->TIM_OCPolarity << 8) | (pInitStruct->TIM_OutputState << 8));
    SET_BIT(TIMx->CCR3, pInitStruct->TIM_Pulse);

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0Q1)
    if (TIMx == TIM1)
#endif
#if defined(__MM3O1) || defined(__MM0P1)
    if ((TIMx == TIM1) || (TIMx == TIM8))
#endif
    {
        MODIFY_REG(TIMx->CCER, TIM_CCER_CC3NP | TIM_CCER_CC3NE,
                   (pInitStruct->TIM_OCNPolarity << 8) | (pInitStruct->TIM_OutputNState << 8));
        MODIFY_REG(TIMx->CR2, TIM_CR2_OIS3 | TIM_CR2_OIS3N,
                   (pInitStruct->TIM_OCIdleState << 4) | (pInitStruct->TIM_OCNIdleState << 4));
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the TIMx Channel4 according to the specified
///         parameters in the pInitStruct.
/// @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  pInitStruct: pointer to a TIM_OCInitTypeDef structure that
///         contains the configuration information for the specified TIM peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* pInitStruct)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC4M, (pInitStruct->TIM_OCMode) << 8);
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC4E | TIM_CCER_CC4P,
               (pInitStruct->TIM_OCPolarity << 12) | (pInitStruct->TIM_OutputState << 12));
    SET_BIT(TIMx->CCR4, pInitStruct->TIM_Pulse);

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0Q1)
    if (TIMx == TIM1)
#endif
#if defined(__MM3O1) || defined(__MM0P1)
    if ((TIMx == TIM1) || (TIMx == TIM8))
#endif
        MODIFY_REG(TIMx->CR2, TIM_CR2_OIS4, pInitStruct->TIM_OCIdleState << 6);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the TIM peripheral according to the specified
///         parameters in the pInitStruct.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  pInitStruct: pointer to a TIM_ICInitTypeDef structure that
///         contains the configuration information for the specified TIM peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ICInit(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* pInitStruct)
{
    switch (pInitStruct->TIM_Channel) {
        case TIM_Channel_1:
            TI1_Configure(TIMx, pInitStruct->TIM_ICPolarity, pInitStruct->TIM_ICSelection, pInitStruct->TIM_ICFilter);
            TIM_SetIC1Prescaler(TIMx, pInitStruct->TIM_ICPrescaler);
            break;
        case TIM_Channel_2:
            TI2_Configure(TIMx, pInitStruct->TIM_ICPolarity, pInitStruct->TIM_ICSelection, pInitStruct->TIM_ICFilter);
            TIM_SetIC2Prescaler(TIMx, pInitStruct->TIM_ICPrescaler);
            break;
        case TIM_Channel_3:
            TI3_Configure(TIMx, pInitStruct->TIM_ICPolarity, pInitStruct->TIM_ICSelection, pInitStruct->TIM_ICFilter);
            TIM_SetIC3Prescaler(TIMx, pInitStruct->TIM_ICPrescaler);
            break;
        case TIM_Channel_4:
            TI4_Configure(TIMx, pInitStruct->TIM_ICPolarity, pInitStruct->TIM_ICSelection, pInitStruct->TIM_ICFilter);
            TIM_SetIC4Prescaler(TIMx, pInitStruct->TIM_ICPrescaler);
            break;
        default:
            /* TODO: some condition not handled */
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIM peripheral according to the specified
///         parameters in the pInitStruct to measure an external PWM signal.
/// @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  pInitStruct: pointer to a TIM_ICInitTypeDef structure that
///         contains the configuration information for the specified TIM peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_PWMIConfig(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* pInitStruct)
{
    u16 icoppositepolarity  = TIM_ICPolarity_Rising;
    u16 icoppositeselection = TIM_ICSelection_DirectTI;
    icoppositepolarity = (pInitStruct->TIM_ICPolarity == TIM_ICPolarity_Rising) ? TIM_ICPolarity_Falling : TIM_ICPolarity_Rising;
    icoppositeselection =
        (pInitStruct->TIM_ICSelection == TIM_ICSelection_DirectTI) ? TIM_ICSelection_IndirectTI : TIM_ICSelection_DirectTI;
    if (pInitStruct->TIM_Channel == TIM_Channel_1) {
        TI1_Configure(TIMx, pInitStruct->TIM_ICPolarity, pInitStruct->TIM_ICSelection, pInitStruct->TIM_ICFilter);
        TIM_SetIC1Prescaler(TIMx, pInitStruct->TIM_ICPrescaler);
        TI2_Configure(TIMx, icoppositepolarity, icoppositeselection, pInitStruct->TIM_ICFilter);
        TIM_SetIC2Prescaler(TIMx, pInitStruct->TIM_ICPrescaler);
    }
    else {
        TI2_Configure(TIMx, pInitStruct->TIM_ICPolarity, pInitStruct->TIM_ICSelection, pInitStruct->TIM_ICFilter);
        TIM_SetIC2Prescaler(TIMx, pInitStruct->TIM_ICPrescaler);
        TI1_Configure(TIMx, icoppositepolarity, icoppositeselection, pInitStruct->TIM_ICFilter);
        TIM_SetIC1Prescaler(TIMx, pInitStruct->TIM_ICPrescaler);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the: Break feature, dead time, Lock level, the OSSI,
///         the OSSR State and the AOE(automatic output enable).
/// @param  TIMx: where x can be 1 or 8 to select the TIM
/// @param  pInitStruct: pointer to a TIM_BDTRInitTypeDef structure that
///         contains the BDTR Register configuration  information for the TIM peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_BDTRConfig(TIM_TypeDef* TIMx, TIM_BDTRInitTypeDef* pInitStruct)
{
    TIMx->BDTR = (u32)pInitStruct->TIM_OSSRState | pInitStruct->TIM_OSSIState | pInitStruct->TIM_LOCKLevel |
                 (pInitStruct->TIM_DeadTime & 0xFF) | pInitStruct->TIM_Break | pInitStruct->TIM_BreakPolarity |
                 pInitStruct->TIM_AutomaticOutput;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each pInitStruct member with its default value.
/// @param  pInitStruct : pointer to a TIM_TimeBaseInitTypeDef
///         structure which will be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef* pInitStruct)
{
    pInitStruct->TIM_Period            = 0xFFFFFFFF;
    pInitStruct->TIM_Prescaler         = 0x0000;
    pInitStruct->TIM_ClockDivision     = TIM_CKD_DIV1;
    pInitStruct->TIM_CounterMode       = TIM_CounterMode_Up;
    pInitStruct->TIM_RepetitionCounter = 0x00;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each pInitStruct member with its default value.
/// @param  pInitStruct : pointer to a TIM_OCInitTypeDef structure which will
///         be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OCStructInit(TIM_OCInitTypeDef* pInitStruct)
{
    pInitStruct->TIM_OCMode       = TIM_OCMode_Timing;
    pInitStruct->TIM_OutputState  = TIM_OutputState_Disable;
    pInitStruct->TIM_OutputNState = TIM_OutputNState_Disable;
    pInitStruct->TIM_Pulse        = 0x00000000;
    pInitStruct->TIM_OCPolarity   = TIM_OCPolarity_High;
    pInitStruct->TIM_OCNPolarity  = TIM_OCNPolarity_High;
    pInitStruct->TIM_OCIdleState  = TIM_OCIdleState_Reset;
    pInitStruct->TIM_OCNIdleState = TIM_OCNIdleState_Reset;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each pInitStruct member with its default value.
/// @param  pInitStruct: pointer to a TIM_ICInitTypeDef structure which will
///         be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ICStructInit(TIM_ICInitTypeDef* pInitStruct)
{
    pInitStruct->TIM_Channel     = TIM_Channel_1;
    pInitStruct->TIM_ICPolarity  = TIM_ICPolarity_Rising;
    pInitStruct->TIM_ICSelection = TIM_ICSelection_DirectTI;
    pInitStruct->TIM_ICPrescaler = TIM_ICPSC_DIV1;
    pInitStruct->TIM_ICFilter    = 0x00;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each pInitStruct member with its default value.
/// @param  pInitStruct: pointer to a TIM_BDTRInitTypeDef structure which
///         will be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_BDTRStructInit(TIM_BDTRInitTypeDef* pInitStruct)
{
    pInitStruct->TIM_OSSRState       = TIM_OSSRState_Disable;
    pInitStruct->TIM_OSSIState       = TIM_OSSIState_Disable;
    pInitStruct->TIM_LOCKLevel       = TIM_LOCKLevel_OFF;
    pInitStruct->TIM_DeadTime        = 0x00;
    pInitStruct->TIM_Break           = TIM_Break_Disable;
    pInitStruct->TIM_BreakPolarity   = TIM_BreakPolarity_Low;
    pInitStruct->TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified TIM peripheral.
/// @param  TIMx: where x can be 1 to 17 to select the TIMx peripheral.
/// @param  state: new state of the TIMx peripheral.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState state)
{
    (state) ? SET_BIT(TIMx->CR1, TIM_CR1_CEN) : CLEAR_BIT(TIMx->CR1, TIM_CR1_CEN);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the TIM peripheral Main Outputs.
/// @param  TIMx: where x can be 1, 8, 16 or 17 to select the TIMx peripheral.
/// @param  state: new state of the TIM peripheral Main Outputs.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState state)
{
    (state) ? SET_BIT(TIMx->BDTR, TIM_BDTR_MOE) : CLEAR_BIT(TIMx->BDTR, TIM_BDTR_MOE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified TIM interrupts.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIMx peripheral.
/// @param  it: specifies the TIM interrupts sources to be enabled or disabled.
///   This parameter can be any combination of the following values:
///     @arg TIM_IT_Update: TIM update Interrupt source
///     @arg TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
///     @arg TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
///     @arg TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
///     @arg TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
///     @arg TIM_IT_COM: TIM Commutation Interrupt source
///     @arg TIM_IT_Trigger: TIM Trigger Interrupt source
///     @arg TIM_IT_Break: TIM Break Interrupt source
/// @note
///   - TIM14, TIM16 and TIM17 can have TIM_IT_Update or TIM_IT_CC1.
///   - TIM_IT_Break is used only with TIM1 and TIM8.
///   - TIM_IT_COM is used only with TIM1, TIM8, TIM16 and TIM17.
/// @param  state: new state of the TIM interrupts.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ITConfig(TIM_TypeDef* TIMx, TIMIT_TypeDef it, FunctionalState state)
{
    (state) ? SET_BIT(TIMx->DIER, it) : CLEAR_BIT(TIMx->DIER, it);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx event to be generate by software.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  source: specifies the event source.
///   This parameter can be one or more of the following values:
///     @arg TIM_EventSource_Update: Timer update Event source
///     @arg TIM_EventSource_CC1: Timer Capture Compare 1 Event source
///     @arg TIM_EventSource_CC2: Timer Capture Compare 2 Event source
///     @arg TIM_EventSource_CC3: Timer Capture Compare 3 Event source
///     @arg TIM_EventSource_CC4: Timer Capture Compare 4 Event source
///     @arg TIM_EventSource_COM: Timer COM event source
///     @arg TIM_EventSource_Trigger: Timer Trigger Event source
///     @arg TIM_EventSource_Break: Timer Break event source
/// @note
///   - TIM_EventSource_COM and TIM_EventSource_Break are used only with TIM1 and TIM8.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_GenerateEvent(TIM_TypeDef* TIMx, TIMEGR_Typedef source)
{
    SET_BIT(TIMx->EGR, source);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx's DMA interface.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 16 or 17 to select the TIM peripheral.
/// @param  DMABase: DMA Base address.
///   This parameter can be one of the following values:
///     @arg TIM_DMABase_CR, TIM_DMABase_CR2, TIM_DMABase_SMCR,
///          TIM_DMABase_DIER, TIM1_DMABase_SR, TIM_DMABase_EGR,
///          TIM_DMABase_CCMR1, TIM_DMABase_CCMR2, TIM_DMABase_CCER,
///          TIM_DMABase_CNT, TIM_DMABase_PSC, TIM_DMABase_ARR,
///          TIM_DMABase_RCR, TIM_DMABase_CCR1, TIM_DMABase_CCR2,
///          TIM_DMABase_CCR3, TIM_DMABase_CCR4, TIM_DMABase_BDTR,
///          TIM_DMABase_DCR.
/// @param  length: DMA Burst length.
///   This parameter can be one value between:
///   TIM_DMABurstLength_1Transfer and TIM_DMABurstLength_18Transfers.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_DMAConfig(TIM_TypeDef* TIMx, TIMDMABASE_Typedef DMABase, TIMDMABURSTLENGTH_Typedef length)
{
    SET_BIT(TIMx->DCR, DMABase);
    SET_BIT(TIMx->DCR, length);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the TIMx's DMA Requests.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 16 or 17 to select the TIM peripheral.
/// @param  source: specifies the DMA Request sources.
///   This parameter can be any combination of the following values:
///     @arg TIM_DMA_Update: TIM update Interrupt source
///     @arg TIM_DMA_CC1: TIM Capture Compare 1 DMA source
///     @arg TIM_DMA_CC2: TIM Capture Compare 2 DMA source
///     @arg TIM_DMA_CC3: TIM Capture Compare 3 DMA source
///     @arg TIM_DMA_CC4: TIM Capture Compare 4 DMA source
///     @arg TIM_DMA_COM: TIM Commutation DMA source
///     @arg TIM_DMA_Trigger: TIM Trigger DMA source
/// @param  state: new state of the DMA Request sources.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_DMACmd(TIM_TypeDef* TIMx, TIMDMASRC_Typedef source, FunctionalState state)
{
    (state) ? SET_BIT(TIMx->DIER, source) : CLEAR_BIT(TIMx->DIER, source);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx internal Clock
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_InternalClockConfig(TIM_TypeDef* TIMx)
{
    CLEAR_BIT(TIMx->SMCR, TIM_SMCR_SMS);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Internal Trigger as External Clock
/// @param  TIMx: where x can be 1, 2, 3, 4 or 5 to select the TIM peripheral.
/// @param  source: Trigger source.
///   This parameter can be one of the following values:
///     @arg  TIM_TS_ITR0: Internal Trigger 0
///     @arg  TIM_TS_ITR1: Internal Trigger 1
///     @arg  TIM_TS_ITR2: Internal Trigger 2
///     @arg  TIM_TS_ITR3: Internal Trigger 3
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ITRxExternalClockConfig(TIM_TypeDef* TIMx, TIMTS_TypeDef source)
{
    TIM_SelectInputTrigger(TIMx, source);
    SET_BIT(TIMx->SMCR, TIM_SlaveMode_External1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Trigger as External Clock
/// @param  TIMx: where x can be 1, 2, 3, 4 or 5 to select the TIM peripheral.
/// @param  source: Trigger source.
///   This parameter can be one of the following values:
///     @arg TIM_TIxExternalCLK1Source_TI1ED: TI1 Edge Detector
///     @arg TIM_TIxExternalCLK1Source_TI1: Filtered Timer Input 1
///     @arg TIM_TIxExternalCLK1Source_TI2: Filtered Timer Input 2
/// @param  polarity: specifies the TIx Polarity.
///   This parameter can be one of the following values:
///     @arg TIM_ICPolarity_Rising
///     @arg TIM_ICPolarity_Falling
/// @param  filter : specifies the filter value.
///   This parameter must be a value between 0x0 and 0xF.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_TIxExternalClockConfig(TIM_TypeDef* TIMx, TIM_TIEXTCLKSRC_Typedef source, TIMICP_Typedef polarity, u16 filter)
{
    (source == TIM_TIxExternalCLK1Source_TI2) ? (TI2_Configure(TIMx, polarity, TIM_ICSelection_DirectTI, filter))
                                              : (TI1_Configure(TIMx, polarity, TIM_ICSelection_DirectTI, filter));
    TIM_SelectInputTrigger(TIMx, (TIMTS_TypeDef)source);
    SET_BIT(TIMx->SMCR, TIM_SlaveMode_External1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx External Trigger (ETR).
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  psc: The external Trigger Prescaler.
///   This parameter can be one of the following values:
///     @arg TIM_ExtTRGPSC_OFF: ETRP Prescaler OFF.
///     @arg TIM_ExtTRGPSC_DIV2: ETRP frequency divided by 2.
///     @arg TIM_ExtTRGPSC_DIV4: ETRP frequency divided by 4.
///     @arg TIM_ExtTRGPSC_DIV8: ETRP frequency divided by 8.
/// @param  polarity: The external Trigger Polarity.
///   This parameter can be one of the following values:
///     @arg TIM_ExtTRGPolarity_Inverted: active low or falling edge active.
///     @arg TIM_ExtTRGPolarity_NonInverted: active high or rising edge active.
/// @param  filter: External Trigger Filter.
///   This parameter must be a value between 0x00 and 0x0F
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ETRConfig(TIM_TypeDef* TIMx, TIMEXTTRGPSC_Typedef psc, TIMETP_Typedef polarity, u16 filter)
{
    CLEAR_BIT(TIMx->SMCR, TIM_SMCR_ECE);
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_ETP, polarity);
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_ETPS, psc);
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_ETF, filter << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the External clock Mode1
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  psc: The external Trigger Prescaler.
///   This parameter can be one of the following values:
///     @arg TIM_ExtTRGPSC_OFF: ETRP Prescaler OFF.
///     @arg TIM_ExtTRGPSC_DIV2: ETRP frequency divided by 2.
///     @arg TIM_ExtTRGPSC_DIV4: ETRP frequency divided by 4.
///     @arg TIM_ExtTRGPSC_DIV8: ETRP frequency divided by 8.
/// @param  polarity: The external Trigger Polarity.
///   This parameter can be one of the following values:
///     @arg TIM_ExtTRGPolarity_Inverted: active low or falling edge active.
///     @arg TIM_ExtTRGPolarity_NonInverted: active high or rising edge active.
/// @param  filter: External Trigger Filter.
///   This parameter must be a value between 0x00 and 0x0F
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ETRClockMode1Config(TIM_TypeDef* TIMx, TIMEXTTRGPSC_Typedef psc, TIMETP_Typedef polarity, u16 filter)
{
    TIM_ETRConfig(TIMx, psc, polarity, filter);
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_TS, TIM_TS_ETRF);
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_SMS, TIM_SlaveMode_External1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the External clock Mode2
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  psc: The external Trigger Prescaler.
///   This parameter can be one of the following values:
///     @arg TIM_ExtTRGPSC_OFF: ETRP Prescaler OFF.
///     @arg TIM_ExtTRGPSC_DIV2: ETRP frequency divided by 2.
///     @arg TIM_ExtTRGPSC_DIV4: ETRP frequency divided by 4.
///     @arg TIM_ExtTRGPSC_DIV8: ETRP frequency divided by 8.
/// @param  polarity: The external Trigger Polarity.
///   This parameter can be one of the following values:
///     @arg TIM_ExtTRGPolarity_Inverted: active low or falling edge active.
///     @arg TIM_ExtTRGPolarity_NonInverted: active high or rising edge active.
/// @param  filter: External Trigger Filter.
///   This parameter must be a value between 0x00 and 0x0F
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ETRClockMode2Config(TIM_TypeDef* TIMx, TIMEXTTRGPSC_Typedef psc, TIMETP_Typedef polarity, u16 filter)
{
    TIM_ETRConfig(TIMx, psc, polarity, filter);
    SET_BIT(TIMx->SMCR, TIM_SMCR_ECE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Prescaler.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  prescaler: specifies the Prescaler Register value
/// @param  reloadMode: specifies the TIM Prescaler Reload mode
///   This parameter can be one of the following values:
///     @arg TIM_PSCReloadMode_Update: The Prescaler is loaded at the update event.
///     @arg TIM_PSCReloadMode_Immediate: The Prescaler is loaded immediately.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_PrescalerConfig(TIM_TypeDef* TIMx, u16 prescaler, TIMUG_Typedef reloadMode)
{
    WRITE_REG(TIMx->PSC, prescaler);
    SET_BIT(TIMx->EGR, reloadMode);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Specifies the TIMx Counter Mode to be used.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  counterMode: specifies the Counter Mode to be used
///   This parameter can be one of the following values:
///     @arg TIM_CounterMode_Up: TIM Up Counting Mode
///     @arg TIM_CounterMode_Down: TIM Down Counting Mode
///     @arg TIM_CounterMode_CenterAligned1: TIM Center Aligned Mode1
///     @arg TIM_CounterMode_CenterAligned2: TIM Center Aligned Mode2
///     @arg TIM_CounterMode_CenterAligned3: TIM Center Aligned Mode3
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_CounterModeConfig(TIM_TypeDef* TIMx, TIMCOUNTMODE_Typedef counterMode)
{
    MODIFY_REG(TIMx->CR1, TIM_CR1_CMS | TIM_CR1_DIR, counterMode);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Selects the Input Trigger source
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  source: The Input Trigger source.
///   This parameter can be one of the following values:
///     @arg TIM_TS_ITR0: Internal Trigger 0
///     @arg TIM_TS_ITR1: Internal Trigger 1
///     @arg TIM_TS_ITR2: Internal Trigger 2
///     @arg TIM_TS_ITR3: Internal Trigger 3
///     @arg TIM_TS_TI1F_ED: TI1 Edge Detector
///     @arg TIM_TS_TI1FP1: Filtered Timer Input 1
///     @arg TIM_TS_TI2FP2: Filtered Timer Input 2
///     @arg TIM_TS_ETRF: External Trigger input
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SelectInputTrigger(TIM_TypeDef* TIMx, TIMTS_TypeDef source)
{
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_TS, source);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Encoder Interface.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  encoderMode: specifies the TIMx Encoder Mode.
///   This parameter can be one of the following values:
///     @arg TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
///     @arg TIM_EncoderMode_TI2: Counter counts on TI2FP2 edge depending on TI1FP1 level.
///     @arg TIM_EncoderMode_TI12: Counter counts on both TI1FP1 and TI2FP2 edges depending
///                                on the level of the other input.
/// @param  IC1Polarity: specifies the IC1 Polarity
///   This parameter can be one of the following values:
///     @arg TIM_ICPolarity_Falling: IC Falling edge.
///     @arg TIM_ICPolarity_Rising: IC Rising edge.
/// @param  IC2Polarity: specifies the IC2 Polarity
///   This parameter can be one of the following values:
///     @arg TIM_ICPolarity_Falling: IC Falling edge.
///     @arg TIM_ICPolarity_Rising: IC Rising edge.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_EncoderInterfaceConfig(TIM_TypeDef*          TIMx,
                                TIMSMSENCODER_Typedef encoderMode,
                                TIMICP_Typedef        IC1Polarity,
                                TIMICP_Typedef        IC2Polarity)
{
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_SMS, encoderMode);
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_CC1S | TIM_CCMR1_CC2S, TIM_CCMR1_CC1S_DIRECTTI | TIM_CCMR1_CC2S_DIRECTTI);
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC1P | TIM_CCER_CC2P, IC1Polarity | (IC2Polarity << 4));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Forces the TIMx output 1 waveform to active or inactive level.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  forcedAction: specifies the forced Action to be set to the output waveform.
///   This parameter can be one of the following values:
///     @arg TIM_ForcedAction_Active: Force active level on OC1REF
///     @arg TIM_ForcedAction_InActive: Force inactive level on OC1REF.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ForcedOC1Config(TIM_TypeDef* TIMx, TIMOCMODE_Typedef forcedAction)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC1M, forcedAction);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Forces the TIMx output 2 waveform to active or inactive level.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  forcedAction: specifies the forced Action to be set to the output waveform.
///   This parameter can be one of the following values:
///     @arg TIM_ForcedAction_Active: Force active level on OC2REF
///     @arg TIM_ForcedAction_InActive: Force inactive level on OC2REF.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ForcedOC2Config(TIM_TypeDef* TIMx, TIMOCMODE_Typedef forcedAction)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC2M, forcedAction << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Forces the TIMx output 3 waveform to active or inactive level.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  forcedAction: specifies the forced Action to be set to the output waveform.
///   This parameter can be one of the following values:
///     @arg TIM_ForcedAction_Active: Force active level on OC3REF
///     @arg TIM_ForcedAction_InActive: Force inactive level on OC3REF.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ForcedOC3Config(TIM_TypeDef* TIMx, TIMOCMODE_Typedef forcedAction)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC3M, forcedAction);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Forces the TIMx output 4 waveform to active or inactive level.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  forcedAction: specifies the forced Action to be set to the output waveform.
///   This parameter can be one of the following values:
///     @arg TIM_ForcedAction_Active: Force active level on OC4REF
///     @arg TIM_ForcedAction_InActive: Force inactive level on OC4REF.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ForcedOC4Config(TIM_TypeDef* TIMx, TIMOCMODE_Typedef forcedAction)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC4M, forcedAction << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables TIMx peripheral Preload register on ARR.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  state: new state of the TIMx peripheral Preload register
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ARRPreloadConfig(TIM_TypeDef* TIMx, FunctionalState state)
{
    (state) ? SET_BIT(TIMx->CR1, TIM_CR1_ARPE) : CLEAR_BIT(TIMx->CR1, TIM_CR1_ARPE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Selects the TIM peripheral Commutation event.
/// @param  TIMx: where x can be 1, 8, 16 or 17 to select the TIMx peripheral.
/// @param  state: new state of the Commutation event.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SelectCOM(TIM_TypeDef* TIMx, FunctionalState state)
{
    (state) ? SET_BIT(TIMx->CR2, TIM_CR2_CCUS) : CLEAR_BIT(TIMx->CR2, TIM_CR2_CCUS);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Selects the TIMx peripheral Capture Compare DMA source.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 16 or 17 to select the TIM peripheral.
/// @param  state: new state of the Capture Compare DMA source
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SelectCCDMA(TIM_TypeDef* TIMx, FunctionalState state)
{
    (state) ? SET_BIT(TIMx->CR2, TIM_CR2_CCDS) : CLEAR_BIT(TIMx->CR2, TIM_CR2_CCDS);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets or Resets the TIM peripheral Capture Compare Preload Control bit.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIMx peripheral.
/// @param  state: new state of the Capture Compare Preload Control bit
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_CCPreloadControl(TIM_TypeDef* TIMx, FunctionalState state)
{
    (state) ? SET_BIT(TIMx->CR2, TIM_CR2_CCPC) : CLEAR_BIT(TIMx->CR2, TIM_CR2_CCPC);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the TIMx peripheral Preload register on CCR1.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  preload: new state of the TIMx peripheral Preload register
///   This parameter can be one of the following values:
///     @arg TIM_OCPreload_Enable
///     @arg TIM_OCPreload_Disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC1PreloadConfig(TIM_TypeDef* TIMx, TIMOCPE_Typedef preload)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC1PE, preload);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the TIMx peripheral Preload register on CCR2.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  preload: new state of the TIMx peripheral Preload register
///   This parameter can be one of the following values:
///     @arg TIM_OCPreload_Enable
///     @arg TIM_OCPreload_Disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC2PreloadConfig(TIM_TypeDef* TIMx, TIMOCPE_Typedef preload)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC2PE, preload << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the TIMx peripheral Preload register on CCR3.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  preload: new state of the TIMx peripheral Preload register
///   This parameter can be one of the following values:
///     @arg TIM_OCPreload_Enable
///     @arg TIM_OCPreload_Disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC3PreloadConfig(TIM_TypeDef* TIMx, TIMOCPE_Typedef preload)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC3PE, preload);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the TIMx peripheral Preload register on CCR4.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  preload: new state of the TIMx peripheral Preload register
///   This parameter can be one of the following values:
///     @arg TIM_OCPreload_Enable
///     @arg TIM_OCPreload_Disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC4PreloadConfig(TIM_TypeDef* TIMx, TIMOCPE_Typedef preload)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR2_OC4PE, preload << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Output Compare 1 Fast feature.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 16 or 17 to select the TIM peripheral.
/// @param  fast: new state of the Output Compare Fast Enable Bit.
///   This parameter can be one of the following values:
///     @arg TIM_OCFast_Enable: TIM output compare fast enable
///     @arg TIM_OCFast_Disable: TIM output compare fast disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC1FastConfig(TIM_TypeDef* TIMx, TIMOCFE_Typedef fast)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC1FE, fast);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Output Compare 2 Fast feature.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  fast: new state of the Output Compare Fast Enable Bit.
///   This parameter can be one of the following values:
///     @arg TIM_OCFast_Enable: TIM output compare fast enable
///     @arg TIM_OCFast_Disable: TIM output compare fast disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC2FastConfig(TIM_TypeDef* TIMx, TIMOCFE_Typedef fast)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC2FE, fast << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Output Compare 3 Fast feature.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  fast: new state of the Output Compare Fast Enable Bit.
///   This parameter can be one of the following values:
///     @arg TIM_OCFast_Enable: TIM output compare fast enable
///     @arg TIM_OCFast_Disable: TIM output compare fast disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC3FastConfig(TIM_TypeDef* TIMx, TIMOCFE_Typedef fast)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC3FE, fast);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Output Compare 4 Fast feature.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  fast: new state of the Output Compare Fast Enable Bit.
///   This parameter can be one of the following values:
///     @arg TIM_OCFast_Enable: TIM output compare fast enable
///     @arg TIM_OCFast_Disable: TIM output compare fast disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC4FastConfig(TIM_TypeDef* TIMx, TIMOCFE_Typedef fast)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC4FE, fast << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears or safeguards the OCREF1 signal on an external event
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  clear: new state of the Output Compare Clear Enable Bit.
///   This parameter can be one of the following values:
///     @arg TIM_OCClear_Enable: TIM Output clear enable
///     @arg TIM_OCClear_Disable: TIM Output clear disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ClearOC1Ref(TIM_TypeDef* TIMx, TIMOCCE_Typedef clear)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC1CE, clear);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears or safeguards the OCREF2 signal on an external event
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  clear: new state of the Output Compare Clear Enable Bit.
///   This parameter can be one of the following values:
///     @arg TIM_OCClear_Enable: TIM Output clear enable
///     @arg TIM_OCClear_Disable: TIM Output clear disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ClearOC2Ref(TIM_TypeDef* TIMx, TIMOCCE_Typedef clear)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC2CE, clear << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears or safeguards the OCREF3 signal on an external event
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  clear: new state of the Output Compare Clear Enable Bit.
///   This parameter can be one of the following values:
///     @arg TIM_OCClear_Enable: TIM Output clear enable
///     @arg TIM_OCClear_Disable: TIM Output clear disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ClearOC3Ref(TIM_TypeDef* TIMx, TIMOCCE_Typedef clear)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC3CE, clear);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears or safeguards the OCREF4 signal on an external event
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  clear: new state of the Output Compare Clear Enable Bit.
///   This parameter can be one of the following values:
///     @arg TIM_OCClear_Enable: TIM Output clear enable
///     @arg TIM_OCClear_Disable: TIM Output clear disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ClearOC4Ref(TIM_TypeDef* TIMx, TIMOCCE_Typedef clear)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC4CE, clear << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx channel 1 polarity.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 16 or 17 to select the TIM peripheral.
/// @param  polarity: specifies the OC1 Polarity
///   This parameter can be one of the following values:
///     @arg TIM_OCPolarity_High: Output Compare active high
///     @arg TIM_OCPolarity_Low: Output Compare active low
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC1PolarityConfig(TIM_TypeDef* TIMx, TIMCCxP_Typedef polarity)
{
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC1P, polarity);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Channel 1N polarity.
/// @param  TIMx: where x can be 1, 8, 16 or 17 to select the TIM peripheral.
/// @param  polarity: specifies the OC1N Polarity
///   This parameter can be one of the following values:
///     @arg TIM_OCNPolarity_High: Output Compare active high
///     @arg TIM_OCNPolarity_Low: Output Compare active low
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC1NPolarityConfig(TIM_TypeDef* TIMx, TIMCCxNP_Typedef polarity)
{
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC1NP, polarity);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx channel 2 polarity.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  polarity: specifies the OC2 Polarity
///   This parameter can be one of the following values:
///     @arg TIM_OCPolarity_High: Output Compare active high
///     @arg TIM_OCPolarity_Low: Output Compare active low
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC2PolarityConfig(TIM_TypeDef* TIMx, TIMCCxP_Typedef polarity)
{
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC2P, polarity << 4);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Channel 2N polarity.
/// @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
/// @param  polarity: specifies the OC2N Polarity
///   This parameter can be one of the following values:
///     @arg TIM_OCNPolarity_High: Output Compare active high
///     @arg TIM_OCNPolarity_Low: Output Compare active low
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC2NPolarityConfig(TIM_TypeDef* TIMx, TIMCCxNP_Typedef polarity)
{
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC2NP, polarity << 4);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx channel 3 polarity.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  polarity: specifies the OC3 Polarity
///   This parameter can be one of the following values:
///     @arg TIM_OCPolarity_High: Output Compare active high
///     @arg TIM_OCPolarity_Low: Output Compare active low
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC3PolarityConfig(TIM_TypeDef* TIMx, TIMCCxP_Typedef polarity)
{
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC3P, polarity << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Channel 3N polarity.
/// @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
/// @param  polarity: specifies the OC3N Polarity
///   This parameter can be one of the following values:
///     @arg TIM_OCNPolarity_High: Output Compare active high
///     @arg TIM_OCNPolarity_Low: Output Compare active low
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC3NPolarityConfig(TIM_TypeDef* TIMx, TIMCCxNP_Typedef polarity)
{
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC3NP, polarity << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx channel 4 polarity.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  polarity: specifies the OC4 Polarity
///   This parameter can be one of the following values:
///     @arg TIM_OCPolarity_High: Output Compare active high
///     @arg TIM_OCPolarity_Low: Output Compare active low
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_OC4PolarityConfig(TIM_TypeDef* TIMx, TIMCCxP_Typedef polarity)
{
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC4P, polarity << 12);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the TIM Capture Compare Channel x.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  channel: specifies the TIM Channel
///   This parameter can be one of the following values:
///     @arg TIM_Channel_1: TIM Channel 1
///     @arg TIM_Channel_2: TIM Channel 2
///     @arg TIM_Channel_3: TIM Channel 3
///     @arg TIM_Channel_4: TIM Channel 4
///     @arg TIM_Channel_5: TIM Channel 5(Only for some MM32 TIM1/8)
/// @param  TIM_CCx: specifies the TIM Channel CCxE bit new state.
///   This parameter can be: TIM_CCx_Enable or TIM_CCx_Disable.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_CCxCmd(TIM_TypeDef* TIMx, TIMCHx_Typedef channel, TIMCCxE_Typedef ccxEn)
{
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC1E << channel, ccxEn << channel);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the TIM Capture Compare Channel xN.
/// @param  TIMx: where x can be 1, 8, 16 or 17 to select the TIM peripheral.
/// @param  channel: specifies the TIM Channel
///   This parameter can be one of the following values:
///     @arg TIM_Channel_1: TIM Channel 1
///     @arg TIM_Channel_2: TIM Channel 2
///     @arg TIM_Channel_3: TIM Channel 3
/// @param  TIM_CCxN: specifies the TIM Channel CCxNE bit new state.
///   This parameter can be: TIM_CCxN_Enable or TIM_CCxN_Disable.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_CCxNCmd(TIM_TypeDef* TIMx, TIMCHx_Typedef channel, TIMCCxNE_Typedef ccxnEn)
{
    if (channel != TIM_Channel_4)
        MODIFY_REG(TIMx->CCER, TIM_CCER_CC1NE << channel, ccxnEn << channel);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Selects the TIM Output Compare Mode.
/// @note   This function disables the selected channel before changing the Output
///         Compare Mode.
///         User has to enable this channel using TIM_CCxCmd and TIM_CCxNCmd functions.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  channel: specifies the TIM Channel
///   This parameter can be one of the following values:
///     @arg TIM_Channel_1: TIM Channel 1
///     @arg TIM_Channel_2: TIM Channel 2
///     @arg TIM_Channel_3: TIM Channel 3
///     @arg TIM_Channel_4: TIM Channel 4
/// @param  mode: specifies the TIM Output Compare Mode.
///   This parameter can be one of the following values:
///     @arg TIM_OCMode_Timing
///     @arg TIM_OCMode_Active
///     @arg TIM_OCMode_Toggle
///     @arg TIM_OCMode_PWM1
///     @arg TIM_OCMode_PWM2
///     @arg TIM_ForcedAction_Active
///     @arg TIM_ForcedAction_InActive
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SelectOCxM(TIM_TypeDef* TIMx, TIMCHx_Typedef channel, TIMOCMODE_Typedef mode)
{
    CLEAR_BIT(TIMx->CCER, TIM_CCER_CC1E << channel);
    switch (channel) {
        case TIM_Channel_1: MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC1M, mode); break;
        case TIM_Channel_2: MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC2M, mode << 8); break;
        case TIM_Channel_3: MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC3M, mode); break;
        case TIM_Channel_4: MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC4M, mode << 8); break;
        default:
            /* TODO: some condition not handled */
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or Disables the TIMx Update event.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  state: new state of the TIMx UDIS bit
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_UpdateDisableConfig(TIM_TypeDef* TIMx, FunctionalState state)
{
    (state) ? SET_BIT(TIMx->CR1, TIM_CR1_UDIS) : CLEAR_BIT(TIMx->CR1, TIM_CR1_UDIS);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Update Request Interrupt source.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  source: specifies the Update source.
///   This parameter can be one of the following values:
///     @arg TIM_UpdateSource_Regular: Source of update is the counter overflow/underflow
///                                    or the setting of UG bit, or an update generation
///                                    through the slave mode controller.
///     @arg TIM_UpdateSource_Global: Source of update is counter overflow/underflow.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_UpdateRequestConfig(TIM_TypeDef* TIMx, TIMURS_Typedef source)
{
    MODIFY_REG(TIMx->CR1, TIM_CR1_URS, source);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the TIMx's Hall sensor interface.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  state: new state of the TIMx Hall sensor interface.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SelectHallSensor(TIM_TypeDef* TIMx, FunctionalState state)
{
    (state) ? SET_BIT(TIMx->CR2, TIM_CR2_TI1S) : CLEAR_BIT(TIMx->CR2, TIM_CR2_TI1S);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Selects the TIMx's One Pulse Mode.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  mode: specifies the OPM Mode to be used.
///   This parameter can be one of the following values:
///     @arg TIM_OPMode_Single
///     @arg TIM_OPMode_Repetitive
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SelectOnePulseMode(TIM_TypeDef* TIMx, TIMOPMODE_Typedef mode)
{
    MODIFY_REG(TIMx->CR1, TIM_CR1_OPM, mode);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Selects the TIMx Trigger Output Mode.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 16 or 17 to select the TIM peripheral.
/// @param  source: specifies the Trigger Output source.
///   This paramter can be one of the following values:
///  - For all TIMx
///     @arg TIM_TRIGSource_Reset:  The UG bit in the TIM_EGR register is used as the trigger output (TRIG).
///     @arg TIM_TRIGSource_Enable: The Counter Enable CEN is used as the trigger output (TRIG).
///     @arg TIM_TRIGSource_Update: The update event is selected as the trigger output (TRIG).
///     @arg TIM_TRIGSource_OC1: The trigger output sends a positive pulse when the CC1IF flag
///                              is to be set, as soon as a capture or compare match occurs (TRIG).
///     @arg TIM_TRIGSource_OC1Ref: OC1REF signal is used as the trigger output (TRIG).
///     @arg TIM_TRIGSource_OC2Ref: OC2REF signal is used as the trigger output (TRIG).
///     @arg TIM_TRIGSource_OC3Ref: OC3REF signal is used as the trigger output (TRIG).
///     @arg TIM_TRIGSource_OC4Ref: OC4REF signal is used as the trigger output (TRIG).
///
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SelectOutputTrigger(TIM_TypeDef* TIMx, TIMMMS_Typedef source)
{
    MODIFY_REG(TIMx->CR2, TIM_CR2_MMS, source);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Selects the TIMx Slave Mode.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  mode: specifies the Timer Slave Mode.
///   This parameter can be one of the following values:
///     @arg TIM_SlaveMode_Reset: Rising edge of the selected trigger signal (TRGI) re-initializes
///                               the counter and triggers an update of the registers.
///     @arg TIM_SlaveMode_Gated:     The counter clock is enabled when the trigger signal (TRGI) is high.
///     @arg TIM_SlaveMode_Trigger:   The counter starts at a rising edge of the trigger TRGI.
///     @arg TIM_SlaveMode_External1: Rising edges of the selected trigger (TRGI) clock the counter.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SelectSlaveMode(TIM_TypeDef* TIMx, TIMSMSMODE_Typedef mode)
{
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_SMS, mode);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets or Resets the TIMx Master/Slave Mode.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  mode: specifies the Timer Master Slave Mode.
///   This parameter can be one of the following values:
///     @arg TIM_MasterSlaveMode_Enable: synchronization between the current timer
///                                      and its slaves (through TRIG).
///     @arg TIM_MasterSlaveMode_Disable: No action
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SelectMasterSlaveMode(TIM_TypeDef* TIMx, TIMMSM_Typedef mode)
{
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_MSM, mode);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the TIMx Counter Register value
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  autoReload: specifies the Counter register new value.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SetAutoreload(TIM_TypeDef* TIMx, u32 autoReload)
{
    //WRITE_REG(TIMx->ARR, autoReload); //???chend:190514
#if defined(__MM3O1)
    if ((TIMx == TIM2) || (TIMx == TIM5))
        WRITE_REG(TIMx->ARR, (u32)autoReload);
    else
#endif
#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    if (TIMx == TIM2)
        WRITE_REG(TIMx->ARR, (u32)autoReload);
    else
#endif
        WRITE_REG(TIMx->ARR, (u16)autoReload);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the TIMx Input Capture 1 prescaler.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  psc: specifies the Input Capture1 prescaler new value.
///   This parameter can be one of the following values:
///     @arg TIM_ICPSC_DIV1: no prescaler
///     @arg TIM_ICPSC_DIV2: capture is done once every 2 events
///     @arg TIM_ICPSC_DIV4: capture is done once every 4 events
///     @arg TIM_ICPSC_DIV8: capture is done once every 8 events
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SetIC1Prescaler(TIM_TypeDef* TIMx, TIMICPSC_Typedef psc)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_IC1PSC, psc);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the TIMx Input Capture 2 prescaler.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  psc: specifies the Input Capture2 prescaler new value.
///   This parameter can be one of the following values:
///     @arg TIM_ICPSC_DIV1: no prescaler
///     @arg TIM_ICPSC_DIV2: capture is done once every 2 events
///     @arg TIM_ICPSC_DIV4: capture is done once every 4 events
///     @arg TIM_ICPSC_DIV8: capture is done once every 8 events
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SetIC2Prescaler(TIM_TypeDef* TIMx, TIMICPSC_Typedef psc)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_IC2PSC, psc << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the TIMx Input Capture 3 prescaler.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  psc: specifies the Input Capture3 prescaler new value.
///   This parameter can be one of the following values:
///     @arg TIM_ICPSC_DIV1: no prescaler
///     @arg TIM_ICPSC_DIV2: capture is done once every 2 events
///     @arg TIM_ICPSC_DIV4: capture is done once every 4 events
///     @arg TIM_ICPSC_DIV8: capture is done once every 8 events
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SetIC3Prescaler(TIM_TypeDef* TIMx, TIMICPSC_Typedef psc)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_IC3PSC, psc);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the TIMx Input Capture 4 prescaler.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  psc: specifies the Input Capture4 prescaler new value.
///   This parameter can be one of the following values:
///     @arg TIM_ICPSC_DIV1: no prescaler
///     @arg TIM_ICPSC_DIV2: capture is done once every 2 events
///     @arg TIM_ICPSC_DIV4: capture is done once every 4 events
///     @arg TIM_ICPSC_DIV8: capture is done once every 8 events
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SetIC4Prescaler(TIM_TypeDef* TIMx, TIMICPSC_Typedef psc)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_IC4PSC, psc << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the TIMx Clock Division value.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select
///   the TIM peripheral.
/// @param  clockDiv: specifies the clock division value.
///   This parameter can be one of the following value:
///     @arg TIM_CKD_DIV1: TDTS = Tck_tim
///     @arg TIM_CKD_DIV2: TDTS = 2 * Tck_tim
///     @arg TIM_CKD_DIV4: TDTS = 4 * Tck_tim
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SetClockDivision(TIM_TypeDef* TIMx, TIMCKD_TypeDef clockDiv)
{
    MODIFY_REG(TIMx->CR1, TIM_CR1_CKD, clockDiv);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the TIMx Counter Register value
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  counter: specifies the Counter register new value.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SetCounter(TIM_TypeDef* TIMx, u32 counter)
{
#if defined(__MM3O1)
    if ((TIMx == TIM2) || (TIMx == TIM5))
        WRITE_REG(TIMx->CNT, (u32)counter);
    else
#endif
#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    if (TIMx == TIM2)
        WRITE_REG(TIMx->CNT, (u32)counter);
    else
#endif
        WRITE_REG(TIMx->CNT, (u16)counter);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the TIMx Capture Compare1 Register value
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  compare: specifies the Capture Compare1 register new value.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SetCompare1(TIM_TypeDef* TIMx, u32 compare)
{
#if defined(__MM3O1)
    if ((TIMx == TIM2) || (TIMx == TIM5))
        WRITE_REG(TIMx->CCR1, (u32)compare);
    else
#endif
#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    if (TIMx == TIM2)
        WRITE_REG(TIMx->CCR1, (u32)compare);
    else
#endif
        WRITE_REG(TIMx->CCR1, (u16)compare);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the TIMx Capture Compare2 Register value
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  compare: specifies the Capture Compare2 register new value.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SetCompare2(TIM_TypeDef* TIMx, u32 compare)
{
#if defined(__MM3O1)
    if ((TIMx == TIM2) || (TIMx == TIM5))
        WRITE_REG(TIMx->CCR2, (u32)compare);
    else
#endif
#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    if (TIMx == TIM2)
        WRITE_REG(TIMx->CCR2, (u32)compare);
    else
#endif
        WRITE_REG(TIMx->CCR2, (u16)compare);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the TIMx Capture Compare3 Register value
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  compare: specifies the Capture Compare3 register new value.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SetCompare3(TIM_TypeDef* TIMx, u32 compare)
{
#if defined(__MM3O1)
    if ((TIMx == TIM2) || (TIMx == TIM5))
        WRITE_REG(TIMx->CCR3, (u32)compare);
    else
#endif
#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    if (TIMx == TIM2)
        WRITE_REG(TIMx->CCR3, (u32)compare);
    else
#endif
        WRITE_REG(TIMx->CCR3, (u16)compare);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the TIMx Capture Compare4 Register value
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  compare: specifies the Capture Compare4 register new value.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_SetCompare4(TIM_TypeDef* TIMx, u32 compare)
{
#if defined(__MM3O1)
    if ((TIMx == TIM2) || (TIMx == TIM5))
        WRITE_REG(TIMx->CCR4, (u32)compare);
    else
#endif
#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    if (TIMx == TIM2)
        WRITE_REG(TIMx->CCR4, (u32)compare);
    else
#endif
        WRITE_REG(TIMx->CCR4, (u16)compare);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Gets the TIMx Input Capture 1 value.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @retval Value: Capture Compare 1 Register value.
////////////////////////////////////////////////////////////////////////////////
u32 TIM_GetCapture1(TIM_TypeDef* TIMx)
{
    return TIMx->CCR1;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Gets the TIMx Input Capture 2 value.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @retval Value: Capture Compare 2 Register value.
////////////////////////////////////////////////////////////////////////////////
u32 TIM_GetCapture2(TIM_TypeDef* TIMx)
{
    return TIMx->CCR2;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Gets the TIMx Input Capture 3 value.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @retval Value: Capture Compare 3 Register value.
////////////////////////////////////////////////////////////////////////////////
u32 TIM_GetCapture3(TIM_TypeDef* TIMx)
{
    return TIMx->CCR3;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Gets the TIMx Input Capture 4 value.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @retval Value: Capture Compare 4 Register value.
////////////////////////////////////////////////////////////////////////////////
u32 TIM_GetCapture4(TIM_TypeDef* TIMx)
{
    return TIMx->CCR4;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Gets the TIMx Counter value.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @retval Value: Counter Register value.
////////////////////////////////////////////////////////////////////////////////
u32 TIM_GetCounter(TIM_TypeDef* TIMx)
{
    return TIMx->CNT;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Gets the TIMx Prescaler value.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @retval Value: Prescaler Register value.
////////////////////////////////////////////////////////////////////////////////
u16 TIM_GetPrescaler(TIM_TypeDef* TIMx)
{
    return TIMx->PSC;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified TIM flag is set or not.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  flag: specifies the flag to check.
///   This parameter can be one of the following values:
///     @arg TIM_FLAG_Update: TIM update Flag
///     @arg TIM_FLAG_CC1: TIM Capture Compare 1 Flag
///     @arg TIM_FLAG_CC2: TIM Capture Compare 2 Flag
///     @arg TIM_FLAG_CC3: TIM Capture Compare 3 Flag
///     @arg TIM_FLAG_CC4: TIM Capture Compare 4 Flag
///     @arg TIM_FLAG_COM: TIM Commutation Flag
///     @arg TIM_FLAG_Trigger: TIM Trigger Flag
///     @arg TIM_FLAG_Break: TIM Break Flag
///     @arg TIM_FLAG_CC1OF: TIM Capture Compare 1 overcapture Flag
///     @arg TIM_FLAG_CC2OF: TIM Capture Compare 2 overcapture Flag
///     @arg TIM_FLAG_CC3OF: TIM Capture Compare 3 overcapture Flag
///     @arg TIM_FLAG_CC4OF: TIM Capture Compare 4 overcapture Flag
/// @note
///   - TIM14, TIM16 and TIM17 can have TIM_FLAG_Update or TIM_FLAG_CC1.
///   - TIM_FLAG_Break is used only with TIM1 and TIM8.
///   - TIM_FLAG_COM is used only with TIM1, TIM8, TIM16 and TIM17.
/// @retval State: The new state of TIM_FLAG (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
FlagStatus TIM_GetFlagStatus(TIM_TypeDef* TIMx, TIMFLAG_Typedef flag)
{
    return ((TIMx->SR & flag) ? SET : RESET);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the TIMx's pending flags.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  flag: specifies the flag bit to clear.
///   This parameter can be any combination of the following values:
///     @arg TIM_FLAG_Update: TIM update Flag
///     @arg TIM_FLAG_CC1: TIM Capture Compare 1 Flag
///     @arg TIM_FLAG_CC2: TIM Capture Compare 2 Flag
///     @arg TIM_FLAG_CC3: TIM Capture Compare 3 Flag
///     @arg TIM_FLAG_CC4: TIM Capture Compare 4 Flag
///     @arg TIM_FLAG_COM: TIM Commutation Flag
///     @arg TIM_FLAG_Trigger: TIM Trigger Flag
///     @arg TIM_FLAG_Break: TIM Break Flag
///     @arg TIM_FLAG_CC1OF: TIM Capture Compare 1 overcapture Flag
///     @arg TIM_FLAG_CC2OF: TIM Capture Compare 2 overcapture Flag
///     @arg TIM_FLAG_CC3OF: TIM Capture Compare 3 overcapture Flag
///     @arg TIM_FLAG_CC4OF: TIM Capture Compare 4 overcapture Flag
/// @note
///   - TIM14, TIM16 and TIM17 can have TIM_FLAG_Update or TIM_FLAG_CC1.
///   - TIM_FLAG_Break is used only with TIM1 and TIM8.
///   - TIM_FLAG_COM is used only with TIM1, TIM8, TIM16 and TIM17.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ClearFlag(TIM_TypeDef* TIMx, TIMFLAG_Typedef flag)
{
    CLEAR_BIT(TIMx->SR, flag);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the TIM interrupt has occurred or not.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  it: specifies the TIM interrupt source to check.
///   This parameter can be one of the following values:
///     @arg TIM_IT_Update: TIM update Interrupt source
///     @arg TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
///     @arg TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
///     @arg TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
///     @arg TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
///     @arg TIM_IT_COM: TIM Commutation Interrupt source
///     @arg TIM_IT_Trigger: TIM Trigger Interrupt source
///     @arg TIM_IT_Break: TIM Break Interrupt source
/// @note
///   - TIM14, TIM16 and TIM17 can have TIM_IT_Update or TIM_IT_CC1.
///   - TIM_IT_Break is used only with TIM1 and TIM8.
///   - TIM_IT_COM is used only with TIM1, TIM8, TIM16 and TIM17.
/// @retval State: The new state of the TIM_IT(SET or RESET).
////////////////////////////////////////////////////////////////////////////////
ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, TIMIT_TypeDef it)
{
    return (((TIMx->SR & it) && (TIMx->DIER & it)) ? SET : RESET);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the TIMx's interrupt pending bits.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  it: specifies the pending bit to clear.
///   This parameter can be any combination of the following values:
///     @arg TIM_IT_Update: TIM1 update Interrupt source
///     @arg TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
///     @arg TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
///     @arg TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
///     @arg TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
///     @arg TIM_IT_COM: TIM Commutation Interrupt source
///     @arg TIM_IT_Trigger: TIM Trigger Interrupt source
///     @arg TIM_IT_Break: TIM Break Interrupt source
/// @note
///   - TIM14, TIM16 and TIM17 can have TIM_IT_Update or TIM_IT_CC1.
///   - TIM_IT_Break is used only with TIM1 and TIM8.
///   - TIM_IT_COM is used only with TIM1, TIM8, TIM16 and TIM17.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, TIMIT_TypeDef it)
{
    CLEAR_BIT(TIMx->SR, it);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx channel 1 polarity.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 16 or 17 to select the TIM peripheral.
/// @param  polarity: specifies the IC1 Polarity
///   This parameter can be one of the following values:
///     @arg TIM_ICPolarity_Rising
///     @arg TIM_ICPolarity_Falling
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exTIM_SetIC1Plority(TIM_TypeDef* TIMx, TIMICP_Typedef pol)
{
    (pol) ? SET_BIT(TIMx->CCER, TIM_CCER_CC1P) : CLEAR_BIT(TIMx->CCER, TIM_CCER_CC1P);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx channel 2 polarity.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  polarity: specifies the IC2 Polarity
///   This parameter can be one of the following values:
///     @arg TIM_ICPolarity_Rising
///     @arg TIM_ICPolarity_Falling
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exTIM_SetIC2Plority(TIM_TypeDef* TIMx, TIMICP_Typedef pol)
{
    (pol) ? SET_BIT(TIMx->CCER, TIM_CCER_CC2P) : CLEAR_BIT(TIMx->CCER, TIM_CCER_CC2P);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx channel 3 polarity.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  polarity: specifies the IC3 Polarity
///   This parameter can be one of the following values:
///     @arg TIM_ICPolarity_Rising
///     @arg TIM_ICPolarity_Falling
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exTIM_SetIC3Plority(TIM_TypeDef* TIMx, TIMICP_Typedef pol)
{
    (pol) ? SET_BIT(TIMx->CCER, TIM_CCER_CC3P) : CLEAR_BIT(TIMx->CCER, TIM_CCER_CC3P);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx channel 4 polarity.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  polarity: specifies the IC4 Polarity
///   This parameter can be one of the following values:
///     @arg TIM_ICPolarity_Rising
///     @arg TIM_ICPolarity_Falling
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exTIM_SetIC4Plority(TIM_TypeDef* TIMx, TIMICP_Typedef pol)
{
    (pol) ? SET_BIT(TIMx->CCER, TIM_CCER_CC4P) : CLEAR_BIT(TIMx->CCER, TIM_CCER_CC4P);
}


#if defined(__MM0P1) || defined(__MM0Q1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Sets the TIMx Capture Compare 5 Register value
/// @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
/// @param  compare: specifies the Capture Compare5 register new value.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exTIM_SetCompare5(TIM_TypeDef* TIMx, u32 compare)
{
    WRITE_REG(TIMx->CCR5, (u16)compare);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Gets the TIMx Input Capture 5 value.
/// @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
/// @retval Value: Capture Compare 5 Register value.
////////////////////////////////////////////////////////////////////////////////
u32 exTIM_GetCapture5(TIM_TypeDef* TIMx)
{
    return TIMx->CCR5;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the TIMx Channel5 according to the specified
///         parameters in the pInitStruct.
/// @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
/// @param  pInitStruct: pointer to a TIM_OCInitTypeDef structure that
///         contains the configuration information for the specified TIM peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exTIM_OC5Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* pInitStruct)
{
    MODIFY_REG(TIMx->CCMR3, TIM_CCMR3_OC5M, (pInitStruct->TIM_OCMode) << 4);
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC5E | TIM_CCER_CC5P,
               (pInitStruct->TIM_OCPolarity << 16) | (pInitStruct->TIM_OutputState << 16));
    SET_BIT(TIMx->CCR4, pInitStruct->TIM_Pulse);

#if defined(__MM0Q1)
    if (TIMx == TIM1)
#endif
#if defined(__MM0P1)
    if ((TIMx == TIM1) || (TIMx == TIM8))
#endif
        MODIFY_REG(TIMx->CR2, TIM_CR2_OIS5, pInitStruct->TIM_OCIdleState << 8);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the TIMx peripheral Preload register on CCR5.
/// @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
/// @param  preload: new state of the TIMx peripheral Preload register
///   This parameter can be one of the following values:
///     @arg TIM_OCPreload_Enable
///     @arg TIM_OCPreload_Disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exTIM_OC5PreloadConfig(TIM_TypeDef* TIMx, TIMOCPE_Typedef preload)
{
    MODIFY_REG(TIMx->CCMR3, TIM_CCMR3_OC5PE, preload);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx channel 5 polarity.
/// @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
/// @param  polarity: specifies the OC5 Polarity
///   This parameter can be one of the following values:
///     @arg TIM_OCPolarity_High: Output Compare active high
///     @arg TIM_OCPolarity_Low: Output Compare active low
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exTIM_OC5PolarityConfig(TIM_TypeDef* TIMx, TIMCCxP_Typedef polarity)
{
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC5P, polarity << 16);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the TIMx Output Compare 5 Fast feature.
/// @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
/// @param  fast: new state of the Output Compare Fast Enable Bit.
///   This parameter can be one of the following values:
///     @arg TIM_OCFast_Enable: TIM output compare fast enable
///     @arg TIM_OCFast_Disable: TIM output compare fast disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exTIM_OC5FastConfig(TIM_TypeDef* TIMx, TIMOCFE_Typedef fast)
{
    MODIFY_REG(TIMx->CCMR3, TIM_CCMR3_OC5FE, fast);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears or safeguards the OCREF4 signal on an external event
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  clear: new state of the Output Compare Clear Enable Bit.
///   This parameter can be one of the following values:
///     @arg TIM_OCClear_Enable: TIM Output clear enable
///     @arg TIM_OCClear_Disable: TIM Output clear disable
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exTIM_ClearOC5Ref(TIM_TypeDef* TIMx, TIMOCCE_Typedef clear)
{
    MODIFY_REG(TIMx->CCMR3, TIM_CCMR3_OC5CE, clear);
}

#endif

#if defined(__MM3O1) || defined(__MM0P1) || defined(__MM0Q1)

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the TIMx complementary PWM output Status after Break.
/// @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
/// @param  state: new state of the TIMx complementary PWM output.
///   This parameter can be: ENABLE or DISABLE.
///     @arg ENABLE: Direct output enable, no longer waiting for output after dead time.
///     @arg DISABLE: Direct output disable, output waiting for dead time.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exTIM_DirectOutput(TIM_TypeDef* TIMx, FunctionalState state)
{
    (state) ? SET_BIT(TIMx->BDTR, TIM_BDTR_DOE) : CLEAR_BIT(TIMx->BDTR, TIM_BDTR_DOE);
}
#endif

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup TIM_Private_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configure the TI1 as Input.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 14, 16 or 17 to select the TIM peripheral.
/// @param  polarity : The Input Polarity.
///   This parameter can be one of the following values:
///     @arg TIM_ICPolarity_Rising
///     @arg TIM_ICPolarity_Falling
/// @param  selection: specifies the input to be used.
///   This parameter can be one of the following values:
///     @arg TIM_ICSelection_DirectTI: TIM Input 1 is selected to be connected to IC1.
///     @arg TIM_ICSelection_IndirectTI: TIM Input 1 is selected to be connected to IC2.
///     @arg TIM_ICSelection_TRC: TIM Input 1 is selected to be connected to TRC.
/// @param  filter: Specifies the Input Capture Filter.
///   This parameter must be a value between 0x00 and 0x0F.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
static void TI1_Configure(TIM_TypeDef* TIMx, u16 polarity, u16 selection, u16 filter)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_CC1S | TIM_CCMR1_IC1F, (filter << 4) | selection);
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC1E | TIM_CCER_CC1P, polarity | TIM_CCER_CC1E);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configure the TI2 as Input.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  polarity : The Input Polarity.
///   This parameter can be one of the following values:
///     @arg TIM_ICPolarity_Rising
///     @arg TIM_ICPolarity_Falling
/// @param  selection: specifies the input to be used.
///   This parameter can be one of the following values:
///     @arg TIM_ICSelection_DirectTI: TIM Input 2 is selected to be connected to IC2.
///     @arg TIM_ICSelection_IndirectTI: TIM Input 2 is selected to be connected to IC1.
///     @arg TIM_ICSelection_TRC: TIM Input 2 is selected to be connected to TRC.
/// @param  filter: Specifies the Input Capture Filter.
///   This parameter must be a value between 0x00 and 0x0F.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
static void TI2_Configure(TIM_TypeDef* TIMx, u16 polarity, u16 selection, u16 filter)
{
    MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_CC2S | TIM_CCMR1_IC2F, (filter << 12) | (selection << 8));
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC2E | TIM_CCER_CC2P, (polarity << 4) | TIM_CCER_CC2E);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configure the TI3 as Input.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  polarity : The Input Polarity.
///   This parameter can be one of the following values:
///     @arg TIM_ICPolarity_Rising
///     @arg TIM_ICPolarity_Falling
/// @param  selection: specifies the input to be used.
///   This parameter can be one of the following values:
///     @arg TIM_ICSelection_DirectTI: TIM Input 3 is selected to be connected to IC3.
///     @arg TIM_ICSelection_IndirectTI: TIM Input 3 is selected to be connected to IC4.
///     @arg TIM_ICSelection_TRC: TIM Input 3 is selected to be connected to TRC.
/// @param  filter: Specifies the Input Capture Filter.
///   This parameter must be a value between 0x00 and 0x0F.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
static void TI3_Configure(TIM_TypeDef* TIMx, u16 polarity, u16 selection, u16 filter)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_CC3S | TIM_CCMR2_IC3F, (filter << 4) | selection);
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC3E | TIM_CCER_CC3P, (polarity << 8) | TIM_CCER_CC3E);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configure the TI4 as Input.
/// @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
/// @param  polarity : The Input Polarity.
///   This parameter can be one of the following values:
///     @arg TIM_ICPolarity_Rising
///     @arg TIM_ICPolarity_Falling
/// @param  selection: specifies the input to be used.
///   This parameter can be one of the following values:
///     @arg TIM_ICSelection_DirectTI: TIM Input 4 is selected to be connected to IC4.
///     @arg TIM_ICSelection_IndirectTI: TIM Input 4 is selected to be connected to IC3.
///     @arg TIM_ICSelection_TRC: TIM Input 4 is selected to be connected to TRC.
/// @param  filter: Specifies the Input Capture Filter.
///   This parameter must be a value between 0x00 and 0x0F.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
static void TI4_Configure(TIM_TypeDef* TIMx, u16 polarity, u16 selection, u16 filter)
{
    MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_CC4S | TIM_CCMR2_IC4F, (filter << 12) | (selection << 8));
    MODIFY_REG(TIMx->CCER, TIM_CCER_CC4E | TIM_CCER_CC4P, (polarity << 12) | TIM_CCER_CC4E);
}

/// @}

/// @}

/// @}
