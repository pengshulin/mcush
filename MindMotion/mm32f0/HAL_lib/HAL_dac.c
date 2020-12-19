////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_DAC.C
/// @author   Y Shi
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE DAC FIRMWARE FUNCTIONS.
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
#define _HAL_DAC_C_

// Files includes  -------------------------------------------------------------
#include "HAL_dac.h"
#include "HAL_rcc.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup DAC_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup DAC_Exported_Functions
/// @{

#if defined(__MM3N1)

////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the DAC peripheral registers to their default reset values.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DAC_DeInit(void)
{
    exRCC_APB1PeriphReset(RCC_APB1Periph_DAC);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the DAC peripheral according to the specified parameters in the DAC_InitStruct.
/// @param DAC_Channel: the selected DAC channel.
/// @param DAC_InitStruct: pointer to a DAC_InitTypeDef structure that contains the configuration information for the specified
/// DAC channel.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DAC_Init(emDACCH_TypeDef DAC_Channel, DAC_InitTypeDef* pInitStruct)
{
    DAC->CR &= ~((DAC_CR_BOFF1 | DAC_CR_TEN1 | DAC_CR_TSEL1 | DAC_CR_WAVE1 | DAC_CR_MAMP1) << DAC_Channel);
    DAC->CR |= (((u32)(pInitStruct->DAC_Trigger) | (u32)(pInitStruct->DAC_WaveGeneration) |
                 (u32)(pInitStruct->DAC_LFSRUnmask_TriangleAmplitude) | (u32)(pInitStruct->DAC_OutputBuffer))
                << DAC_Channel);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each DAC_InitStruct member with its default value.
/// @param DAC_InitStruct : pointer to a DAC_InitTypeDef structure which will be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DAC_StructInit(DAC_InitTypeDef* pInitStruct)
{
    pInitStruct->DAC_Trigger                      = DAC_Trigger_None;
    pInitStruct->DAC_WaveGeneration               = DAC_WaveGeneration_None;
    pInitStruct->DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1;
    pInitStruct->DAC_OutputBuffer                 = DAC_OutputBuffer_Enable;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified DAC channel.
/// @param DAC_Channel: the selected DAC channel.
/// @param state: new state of the DAC channel.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DAC_Cmd(emDACCH_TypeDef DAC_Channel, FunctionalState state)
{
    (state) ? (DAC->CR |= DAC_CR_EN1 << DAC_Channel) : (DAC->CR &= ~(DAC_CR_EN1 << DAC_Channel));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified DAC channel DMA request.
/// @param DAC_Channel: the selected DAC channel.
/// @param state: new state of the selected DAC channel DMA request.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DAC_DMACmd(emDACCH_TypeDef DAC_Channel, FunctionalState state)
{
    (state) ? (DAC->CR |= DAC_CR_DMAEN1 << DAC_Channel) : (DAC->CR &= ~(DAC_CR_DMAEN1 << DAC_Channel));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the selected DAC channel software trigger.
/// @param DAC_Channel: the selected DAC channel.
/// @param state: new state of the selected DAC channel software trigger.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DAC_SoftwareTriggerCmd(emDACCH_TypeDef DAC_Channel, FunctionalState state)
{
    (state) ? (DAC->SWTRIGR |= (DAC_SWTRIGR_SWTRIG1 << (DAC_Channel >> 4)))
               : (DAC->SWTRIGR &= ~(DAC_SWTRIGR_SWTRIG1 << (DAC_Channel >> 4)));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables simultaneously the two DAC channels software triggers.
/// @param state: new state of the DAC channels software triggers.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DAC_DualSoftwareTriggerCmd(FunctionalState state)
{
    (state) ? (DAC->SWTRIGR |= (DAC_SWTRIGR_SWTRIG1 | DAC_SWTRIGR_SWTRIG2))
               : (DAC->SWTRIGR &= ~(DAC_SWTRIGR_SWTRIG1 | DAC_SWTRIGR_SWTRIG2));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the selected DAC channel wave generation.
/// @param DAC_Channel: the selected DAC channel.
/// @param DAC_Wave: Specifies the wave type to enable or disable.
/// @param state: new state of the selected DAC channel wave generation.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DAC_WaveGenerationCmd(emDACCH_TypeDef DAC_Channel, emDACWAVE_TypeDef DAC_Wave, FunctionalState state)
{
    (state) ? (DAC->CR |= DAC_Wave << DAC_Channel) : (DAC->CR &= ~(DAC_Wave << DAC_Channel));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the specified data holding register value for DAC channel1.
/// @param DAC_Align: Specifies the data alignement for DAC channel1.
/// @param Data : Data to be loaded in the selected data holding register.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DAC_SetChannel1Data(emDACALIGN_TypeDef DAC_Align, u16 Data)
{
    *((u32*)(DAC_BASE + DHR12R1_Offset + DAC_Align)) = Data;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the specified data holding register value for DAC channel2.
/// @param DAC_Align: Specifies the data alignement for DAC channel2.
/// @param Data : Data to be loaded in the selected data holding
///   register.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DAC_SetChannel2Data(emDACALIGN_TypeDef DAC_Align, u16 Data)
{
    *((u32*)(DAC_BASE + DHR12R2_Offset + DAC_Align)) = Data;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the specified data holding register value for dual channel DAC.
/// @param DAC_Align: Specifies the data alignement for dual channel DAC.
/// @param Data2: Data for DAC Channel2 to be loaded in the selected data holding register.
/// @param Data1: Data for DAC Channel1 to be loaded in the selected data holding register.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DAC_SetDualChannelData(emDACALIGN_TypeDef DAC_Align, u16 Data2, u16 Data1)
{
    u32 data = ((DAC_Align == DAC_Align_8b_R) ? ((Data2 << 8) | Data1) : ((Data2 << 16) | Data1));
    *((u32*)(DAC_BASE + DHR12RD_Offset + DAC_Align)) = data;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the last data output value of the selected DAC cahnnel.
/// @param DAC_Channel: the selected DAC channel.
/// @retval  The selected DAC channel data output value.
////////////////////////////////////////////////////////////////////////////////
u16 DAC_GetDataOutputValue(emDACCH_TypeDef DAC_Channel)
{
    return (*(u32*)(DAC_BASE + DOR_Offset + (DAC_Channel >> 2)));
}

/// @}

/// @}

/// @}

#endif
