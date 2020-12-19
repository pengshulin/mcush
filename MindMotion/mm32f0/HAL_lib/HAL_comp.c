////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_COMP.C
/// @author   PX Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE COMP FIRMWARE FUNCTIONS.
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
#define _HAL_COMP_C_

// Files includes  -------------------------------------------------------------
#include "HAL_comp.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup COMP_HAL
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @addtogroup COMP_Exported_Functions
/// @{

#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes COMP peripheral registers to their default reset
///         values.
/// @param  selection: the selected comparator.
///         This parameter can be COMP_Selection_COMPx where x can be 1 to 2
///         to select the COMP peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMP_DeInit(u32 selection)
{
    *(u32*)(COMP_BASE + selection) = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the COMP peripheral according to the specified
/// @param  selection: the selected comparator.
///         This parameter can be COMP_Selection_COMPx where x can be 1 to 2
///         to select the COMP peripheral.
/// @param  pInitStruct: pointer to an COMP_InitTypeDef structure that
///         contains the configuration information for the specified COMP
///         peripheral.
///         - COMP_InvertingInput specifies the inverting input of COMP
///         - COMP_NonInvertingInput specifies the non inverting input of COMP
///         - COMP_Output connect COMP output to selected timer
///           input (Input capture / Output Compare Reference Clear / Break
///           Input)
///         - COMP_BlankingSrce specifies the blanking source of COMP
///         - COMP_OutputPol select output polarity
///         - COMP_Hysteresis configures COMP hysteresis value
///         - COMP_Mode configures COMP power mode
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMP_Init(u32 selection, COMP_InitTypeDef* pInitStruct)
{
    *(u32*)(COMP_BASE + selection) =    pInitStruct->Invert     |
                                        pInitStruct->NonInvert  |
                                        pInitStruct->Output     |
                                        pInitStruct->OutputPol  |
                                        pInitStruct->BlankingSrce   |
                                        pInitStruct->Hysteresis |
                                        pInitStruct->Mode;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each COMP_InitStruct member with its default value.
/// @param  COMP_InitStruct: pointer to an COMP_InitTypeDef structure which will
///         be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMP_StructInit(COMP_InitTypeDef* COMP_InitStruct)
{
#if defined(__MM0N1)
    COMP_InitStruct->Invert         = emCOMP_InvertingInput_1_4VREFINT;
#endif
    COMP_InitStruct->NonInvert      = emCOMP_NonInvertingInput_IO1;
    COMP_InitStruct->Output         = emCOMP_Output_None;
    COMP_InitStruct->BlankingSrce   = COMP_BlankingSrce_None;
    COMP_InitStruct->OutputPol      = emCOMP_NonInverted;
    COMP_InitStruct->Hysteresis     = emCOMP_Hysteresis_No;
    COMP_InitStruct->Mode           = emCOMP_Mode_UltraLowPower;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enable or disable the COMP peripheral.
/// @param  selection: the selected comparator.
///         This parameter can be COMP_Selection_COMPx where x can be 1 to 2
///         to select the COMP peripheral.
/// @param  NewState: new state of the COMP peripheral.
///         This parameter can be: ENABLE or DISABLE.
///         When enabled, the comparator compares the non inverting input with
///         the inverting input and the comparison result is available on
///         comparator output.
///         When disabled, the comparator doesn't perform comparison and the
///         output level is low.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMP_Cmd(u32 selection, FunctionalState state)
{
    (state) ? (*(u32*)(COMP_BASE + selection) |=  COMP_CSR_EN) :
              (*(u32*)(COMP_BASE + selection) &= ~COMP_CSR_EN);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Close or Open the SW1 switch.
/// @param  selection: the selected comparator.
///         This parameter can be COMP_Selection_COMPx where x can be 1 to 2
///         to select the COMP peripheral.
/// @param  state: new state of the COMP peripheral.
///         This parameter can be: ENABLE or DISABLE.
///         When enabled, the comparator compares the non inverting input with
///         the inverting input and the comparison result is available on
///         comparator output.
///         When disabled, the comparator doesn't perform comparison and the
///         output level is low.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMP_SwitchCmd(u32 selection, FunctionalState state)
{
    (state) ?
        (*(u32*)(COMP_BASE + selection) |=  COMP_CSR_COMPSW1) :
        (*(u32*)(COMP_BASE + selection) &= ~COMP_CSR_COMPSW1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Return the output level (high or low) of the selected comparator.
///         The output level depends on the selected polarity.
///         If the polarity is not inverted:
///           - Comparator output is low when the non-inverting input is at a
///           lower voltage than the inverting input
///           - Comparator output is high when the non-inverting input is at a
///           higher voltage than the inverting input
///         If the polarity is inverted:
///           - Comparator output is high when the non-inverting input is at a
///           lower voltage than the inverting input
///           - Comparator output is low when the non-inverting input is at a
///           higher voltage than the inverting input
/// @param  comp: the selected comparator.
///         This parameter can be COMP_Selection_COMPx where x can be 1 to 2
///         to select the COMP peripheral.
/// @retval  The selected comparator output level: low or high.
////////////////////////////////////////////////////////////////////////////////
u32 COMP_GetOutputLevel(u32 comp)
{
    return (((*(u32*)(COMP_BASE + comp) & COMP_CSR_STA) != 0) ?
                emCOMP_OutputLevel_High :
                emCOMP_OutputLevel_Low );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Lock the selected comparator (COMP1/COMP2) configuration.
/// @param  selection: the selected comparator.
///         This parameter can be COMP_Selection_COMPx where x can be 1 to 2
///         to select the COMP peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void COMP_LockConfig(u32 selection)
{
    *(u32*)(COMP_BASE + selection) |= COMP_CSR_LOCK;
}

#if defined(__MM0P1) || defined(__MM0Q1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Enable or disable the COMP register.
/// @param  state: new state of the COMP peripheral.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exCOMP_CrvCmd(FunctionalState state)
{
    (state) ? (COMP->CRV |= COMP_CRV_EN_ENABLE) : (COMP->CRV &= ~COMP_CRV_EN_ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Select comparator external reference voltage.
/// @param  selection: the selected external reference voltage.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exCOMP_SwitchCrv(u32 crv)
{
    COMP->CRV |= crv;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Select comparator external reference voltage source.
/// @param  selection: the selected external reference voltage source.
///         This parameter can be: COMP_CRV_SRC_AVDD or COMP_CRV_SRC_VREF.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void exCOMP_CrvSrc(u32 src)
{
    COMP->CRV |= src;
}
#endif

#endif



/// @}

/// @}

/// @}
