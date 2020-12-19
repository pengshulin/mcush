////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_NVIC.C
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE NVIC FIRMWARE FUNCTIONS.
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
#define _HAL_NVIC_C_

// Files includes  -------------------------------------------------------------
#include "HAL_nvic.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup NVIC_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup NVIC_Exported_Functions
/// @{

#if defined(__MM3N1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the NVIC interrupt vector table.
/// @param  NVIC_VectTab
///         This parameter can be any combination of the following values:
///    @arg NVIC_VectTab_RAM
///    @arg NVIC_VectTab_FLASH
/// @param  Offset
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void NVIC_SetVectorTable(u32 NVIC_VectTab, u32 offset)
{
    SCB->VTOR = NVIC_VectTab | (offset & (u32)0x1FFFFF80);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the NVIC interrupt priority group.
/// @param  NVIC_PriorityGroup
///         This parameter can be any combination of the following values:
///    @arg NVIC_PriorityGroup_0
///    @arg NVIC_PriorityGroup_1
///    @arg NVIC_PriorityGroup_2
///    @arg NVIC_PriorityGroup_3
///    @arg NVIC_PriorityGroup_4
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void NVIC_PriorityGroupConfig(u32 NVIC_PriorityGroup)
{
    SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup;
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief  NVIC initialization.
/// @param  pInitStruct
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void NVIC_Init(NVIC_InitTypeDef* pInitStruct)
{
#if defined(__MM3N1)
    if (pInitStruct->NVIC_IRQChannelCmd != DISABLE) {
        u32 pri = (SCB_AIRCR_PRIGROUP & ~(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk)) >> SCB_AIRCR_PRIGROUP_Pos;

        pri = (((u32)pInitStruct->NVIC_IRQChannelPreemptionPriority << (0x4 - pri)) |
               pInitStruct->NVIC_IRQChannelSubPriority & (0x0F >> pri))
              << 0x04;

        NVIC->IP[pInitStruct->NVIC_IRQChannel]           = pri;
        NVIC->ISER[pInitStruct->NVIC_IRQChannel >> 0x05] = 0x01 << (pInitStruct->NVIC_IRQChannel & 0x1F);
    }
    else {
        NVIC->ICER[pInitStruct->NVIC_IRQChannel >> 0x05] = 0x01 << (pInitStruct->NVIC_IRQChannel & 0x1F);
    }
#endif

#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    if (pInitStruct->NVIC_IRQChannelCmd != DISABLE) {
        NVIC->IP[pInitStruct->NVIC_IRQChannel >> 0x02] =
            (NVIC->IP[pInitStruct->NVIC_IRQChannel >> 0x02] &
                (~(((u32)0xFF) << ((pInitStruct->NVIC_IRQChannel & 0x03) * 8)))) |
            ((((u32)pInitStruct->NVIC_IRQChannelPriority << 6) & 0xFF) << ((pInitStruct->NVIC_IRQChannel & 0x03) * 8));

        NVIC->ISER[0] = 0x01 << (pInitStruct->NVIC_IRQChannel & 0x1F);
    }
    else {
        NVIC->ICER[0] = 0x01 << (pInitStruct->NVIC_IRQChannel & 0x1F);
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  NVIC initialized extension function.
/// @param  pInitStruct
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exNVIC_Init(exNVIC_Init_TypeDef* pInitStruct)
{
#if defined(__MM3N1)
    u32 pri;

    if (pInitStruct->NVIC_IRQChannelCmd != DISABLE) {
        pri = (SCB_AIRCR_PRIGROUP & ~(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk)) >> SCB_AIRCR_PRIGROUP_Pos;

        pri = (((u32)pInitStruct->NVIC_IRQChannelPreemptionPriority << (0x4 - pri)) |
                     pInitStruct->NVIC_IRQChannelSubPriority & (0x0F >> pri)) << 0x04;

        NVIC->IP[pInitStruct->NVIC_IRQChannel] = pri;
        NVIC->ISER[pInitStruct->NVIC_IRQChannel >> 0x05] = 0x01 << (pInitStruct->NVIC_IRQChannel & 0x1F);
    }
    else {
        NVIC->ICER[pInitStruct->NVIC_IRQChannel >> 0x05] = 0x01 << (pInitStruct->NVIC_IRQChannel & 0x1F);
    }
#endif

#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    if (pInitStruct->NVIC_IRQChannelCmd != DISABLE) {
        NVIC->IP[pInitStruct->NVIC_IRQChannel >> 0x02] =
            (NVIC->IP[pInitStruct->NVIC_IRQChannel >> 0x02] &
                (~(((u32)0xFF) << ((pInitStruct->NVIC_IRQChannel & 0x03) * 8)))) |
            //           ((((u32)pInitStruct->NVIC_IRQChannelPriority << 6)
            //& 0xFF)
            ((((u32)pInitStruct->NVIC_IRQChannelSubPriority << 6) & 0xFF) << ((pInitStruct->NVIC_IRQChannel & 0x03) * 8));

        NVIC->ISER[0] = 0x01 << (pInitStruct->NVIC_IRQChannel & 0x1F);
    }
    else {
        NVIC->ICER[0] = 0x01 << (pInitStruct->NVIC_IRQChannel & 0x1F);
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  System low power mode configuration.
/// @param  LowPowerMode
///         This parameter can be any combination of the following values:
///    @arg NVIC_LP_SEVONPEND
///    @arg NVIC_LP_SLEEPDEEP
///    @arg NVIC_LP_SLEEPONEXIT
/// @param  state: new state of the DMA Channeln.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void NVIC_SystemLPConfig(u8 LowPowerMode, FunctionalState state)
{
    (state) ? (SCB->SCR |= LowPowerMode) : (SCB->SCR &= ~(u32)LowPowerMode);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  SysTick clock source configuration.
/// @param  SysTick_CLKSource
///         This parameter can be any combination of the following values:
///    @arg SysTick_CLKSource_HCLK_Div8
///    @arg SysTick_CLKSource_HCLK
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SysTick_CLKSourceConfig(u32 SysTick_CLKSource)
{
    (SysTick_CLKSource == SysTick_CLKSource_HCLK) ? (SysTick->CTRL |= SysTick_CLKSource_HCLK)
                                                  : (SysTick->CTRL &= SysTick_CLKSource_HCLK_Div8);
}

/// @}

/// @}

/// @}
