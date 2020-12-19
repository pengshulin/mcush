////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_CAN.C
/// @author   ---
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE PROVIDES ALL THE CAN FIRMWARE FUNCTIONS.
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
#define __HAL_CAN_C

// Files includes  -------------------------------------------------------------
#include "HAL_can.h"
#include "HAL_rcc.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CAN_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CAN_Exported_Functions
/// @{

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1)

////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the CAN peripheral registers to their default reset
/// values.
/// @param  CANx: where x can be 1 to select the CAN peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_DeInit(CAN_TypeDef* CANx)
{
    (void)CANx;

    exRCC_APB1PeriphReset(RCC_APB1ENR_CAN);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief   Initializes the CAN peripheral according to the specified
///          parameters in the CAN_InitStruct.
/// @param  CANx: where x can be 1 to select the CAN peripheral.
/// @param  CAN_InitStruct: pointer to a CAN_InitTypeDef structure that
///          contains the configuration information for the CAN peripheral.
/// @retval  Constant indicates initialization succeed which will be
///          CANINITFAILED or CANINITOK.
////////////////////////////////////////////////////////////////////////////////
u8 CAN_Init(CAN_TypeDef* CANx, CAN_Basic_InitTypeDef* pInitStruct)
{
    u8 InitStatus = CANINITFAILED;

    CANx->BTR0 = ((u32)(pInitStruct->SJW) << 6) | ((u32)(pInitStruct->BRP));
    CANx->BTR1 = ((u32)(pInitStruct->SAM) << 7) | ((u32)(pInitStruct->TESG2) << 4) | ((u32)(pInitStruct->TESG1));

    return InitStatus;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the CAN_Basic reception filter according to the specified
///         parameters in the CAN_Basic_FilterInitStruct.
/// @param  CAN_Basic_FilterInitStruct: pointer to a CAN_Basic_FilterInitTypeDef
/// structure that contains the configuration information.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_FilterInit(CAN_Basic_FilterInitTypeDef* CAN_Basic_FilterInitStruct)
{
    // Filter Mode
    CAN1->ACR = CAN_Basic_FilterInitStruct->CAN_FilterId;
    CAN1->AMR = CAN_Basic_FilterInitStruct->CAN_FilterMaskId;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each pInitStruct member with its default value.
/// @param  pInitStruct : pointer to a CAN_Basic_InitTypeDef structure which will be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_StructInit(CAN_Basic_InitTypeDef* pInitStruct)
{
    // Reset CAN_Basic init structure parameters values

    // initialize the BRP member(where can be set with (0..63))
    pInitStruct->BRP = 0x0;
    // initialize the SJW member(where can be set with (0..3))
    pInitStruct->SJW = 0x0;
    // Initialize the TESG1 member(where can be set with (0..15))
    pInitStruct->TESG1 = 0x0;
    // Initialize the TESG2 member(where can be set with(0..7))
    pInitStruct->TESG2 = 0x0;
    // Initialize the SAM member(where can be set (SET or RESET))
    pInitStruct->SAM = RESET;

    //  CDCLK, CLOSE_OPEN_CLK, RXINTEN and CBP bits are not exist in CDR register;

    // Initialize the external pin CLKOUT frequence
    // pInitStruct->CDCLK = 0x0;
    // Initialize the external clk is open or close
    // pInitStruct->CLOSE_OPEN_CLK = 0x0;
    // Initialize the TX1 pin work as rx interrupt output
    // pInitStruct->RXINTEN = 0x0;
    // Initialize the CBP of CDR register
    // pInitStruct->CBP = 0x0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified CAN interrupts.
/// @param  CANx: where x can be 1 to select the CAN peripheral.
/// @param  CAN_IT: specifies the CAN interrupt sources to be enabled or
///         disabled.
///         This parameter can be: CAN_IT_OIE, CAN_IT_EIE, CAN_IT_TIE,
///         CAN_IT_RIE.
/// @param  Newstate: new state of the CAN interrupts.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_ITConfig(CAN_TypeDef* CANx, u32 CAN_IT, FunctionalState Newstate)
{
    (Newstate) ? (CANx->CR |= CAN_IT) : (CANx->CR &= ~CAN_IT);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initiates and transmits a CAN frame message.
/// @param  CANx: where x can be 1 to select the CAN peripheral.
/// @param  TxMessage: pointer to a structure which contains CAN Id, CAN DLC and
///         CAN data.
/// @retval CANTXOK if the CAN driver transmits the message
////////////////////////////////////////////////////////////////////////////////
u8 CAN_Transmit(CAN_TypeDef* CANx, CanBasicTxMsg* BasicTxMessage)
{
    CANx->TXID0 = (BasicTxMessage->IDH);
    CANx->TXID1 = (BasicTxMessage->IDL << 5) | (BasicTxMessage->RTR << 4) | (BasicTxMessage->DLC);
    if ((FunctionalState)(BasicTxMessage->RTR) != ENABLE) {
        CANx->TXDR0 = BasicTxMessage->Data[0];
        CANx->TXDR1 = BasicTxMessage->Data[1];
        CANx->TXDR2 = BasicTxMessage->Data[2];
        CANx->TXDR3 = BasicTxMessage->Data[3];
        CANx->TXDR4 = BasicTxMessage->Data[4];
        CANx->TXDR5 = BasicTxMessage->Data[5];
        CANx->TXDR6 = BasicTxMessage->Data[6];
        CANx->TXDR7 = BasicTxMessage->Data[7];
    }

    CANx->CMR = CAN_CMR_TR;

    return (CANx->SR & 0x01);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Cancels a transmit request.
/// @param  CANx: where x can be 1 to select the CAN peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_CancelTransmit(CAN_TypeDef* CANx)
{
    // abort transmission
    CANx->CMR = CAN_AT;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Releases the specified receive FIFO.
/// @param  CANx: where x can be 1 to select the CAN peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_FIFORelease(CAN_TypeDef* CANx)
{
    // Release FIFO
    CANx->CMR |= (u32)CAN_RRB;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Receives a correct CAN frame.
/// @param  CANx: where x can be 1 to select the CAN peripheral.
/// @param  RxMessage: pointer to a structure receive frame which contains CAN
///         Id,CAN DLC, CAN data and FMI number.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_Receive(CAN_TypeDef* CANx, CanBasicRxMsg* BasicRxMessage)
{
    u16 tempid;

    BasicRxMessage->RTR = (u8)((CANx->RXID1) >> 4) & 0x1;
    BasicRxMessage->DLC = (u8)((CANx->RXID1) & 0xf);
    tempid              = (u16)(((CANx->RXID1) & 0xe0) >> 5);
    tempid |= (u16)(CANx->RXID0 << 3);
    BasicRxMessage->ID      = tempid;
    BasicRxMessage->Data[0] = CAN1->RXDR0;
    BasicRxMessage->Data[1] = CAN1->RXDR1;
    BasicRxMessage->Data[2] = CAN1->RXDR2;
    BasicRxMessage->Data[3] = CAN1->RXDR3;
    BasicRxMessage->Data[4] = CAN1->RXDR4;
    BasicRxMessage->Data[5] = CAN1->RXDR5;
    BasicRxMessage->Data[6] = CAN1->RXDR6;
    BasicRxMessage->Data[7] = CAN1->RXDR7;
    CAN_FIFORelease(CANx);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified CAN flag is set or not.
/// @param  CANx: where x can be 1 or 2 to to select the CAN peripheral.
/// @param  flag: specifies the flag to check.
///         This parameter can be one of the following values:
///            @arg CAN_STATUS_RBS: Receive buffer status
///            @arg CAN_STATUS_DOS: Data overflow status
///            @arg CAN_STATUS_TBS: Transmit buffer status
///            @arg CAN_STATUS_TCS: Transmit complete status
///            @arg CAN_STATUS_RS: Receiving status
///            @arg CAN_STATUS_TS: Transmiting status
///            @arg CAN_STATUS_ES: Error status
///            @arg CAN_STATUS_BS: bus status, close or open
/// @retval The new state of CAN_FLAG (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
FlagStatus CAN_GetFlagStatus(CAN_TypeDef* CANx, u32 flag)
{
    return (FlagStatus)(((CANx->SR & flag) == flag) ? SET : RESET);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified CAN interrupt has occurred or not.
/// @param  CANx: where x can be 1 to select the CAN peripheral.
/// @param  CAN_IT: specifies the CAN interrupt source to check.
///         This parameter can be one of the following values:
///            @arg CAN_IT_RI: Receive FIFO not empty Interrupt
///            @arg CAN_IT_TI: Transmit Interrupt
///            @arg CAN_IT_EI: ERROR Interrupt
///            @arg CAN_IT_DOI: Data voerflow Interrupt
///            @arg CAN_IT_WUI: Wakeup Interrupt
///            @arg CAN_IT_ALL: use it can enble all Interrupt
/// @retval The current state of CAN_IT (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
ITStatus CAN_GetITStatus(CAN_TypeDef* CANx, u32 CAN_IT)
{
    return (ITStatus)((CANx->IR & CAN_IT) != CAN_IT) ? RESET : SET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Select the can work as peli mode or basic mode
/// @param  CANx: where x can be 1 or 2 to to select the CAN peripheral.
/// @param  CAN_MODE: specifies the work mode:CAN_BASICMode,CAN_PELIMode
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_Mode_Cmd(CAN_TypeDef* CANx, u32 CAN_Mode)
{
    CANx->CDR |= CAN_Mode;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Select the Reset mode or not
/// @param  CANx: where x can be 1 or 2 to to select the CAN peripheral.
/// @param  state to go into the Reset mode or go out
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_ResetMode_Cmd(CAN_TypeDef* CANx, FunctionalState state)
{
    (state == ENABLE) ? (CANx->CR |= CAN_ResetMode) : (CANx->CR &= ~CAN_ResetMode);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clear the data overflow.
/// @param  CANx: where x can be 1 or 2 to to select the CAN peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_ClearDataOverflow(CAN_TypeDef* CANx)
{
    CANx->CMR |= (u32)CAN_CDO;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the CAN's IT pending.
/// @param  CANx: where x can be 1 or 2 to to select the CAN peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_ClearITPendingBit(CAN_TypeDef* CANx)
{
    u32 temp = 0;
    temp     = temp;
    temp     = CANx->IR;  // read this register clear all interrupt
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the CAN's IT pending.
/// @param  CANx: where x can be 1 or 2 to to select the CAN peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
u32 exCAN_ClearITPendingBit(CAN_Peli_TypeDef* CANx)
{
    (void)CANx;

    return CAN1_PELI->IR;  // read this register clear all interrupt
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each CAN1_PELI_InitStruct member with its default value.
/// @param  pInitStruct : pointer to a CAN_Peli_InitTypeDef structure
///         which will be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_Peli_StructInit(CAN_Peli_InitTypeDef* pInitStruct)
{
    //--------------- Reset CAN_Peli init structure parameters values
    //---------------
    pInitStruct->BRP   = 0x0;      // initialize the BRP member(where can be set with (0..63))
    pInitStruct->SJW   = 0x0;      // initialize the SJW member(where can be set with (0..3))
    pInitStruct->TESG1 = 0x0;      // Initialize the TESG1 member(where can be set with (0..15))
    pInitStruct->TESG2 = 0x0;      // Initialize the TESG2 member(where can be set with(0..7))
    pInitStruct->SAM   = RESET;    // Initialize the SAM member(where can be set (SET or RESET))
    pInitStruct->LOM   = DISABLE;  // Initialize the LOM member
    pInitStruct->STM   = DISABLE;  // Initialize the STM member
    pInitStruct->SM    = DISABLE;  // Initialize the SM member
    pInitStruct->SRR   = DISABLE;
    pInitStruct->EWLR  = 0x96;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes the CAN_Peli peripheral according to the specified
///         parameters in the pInitStruct.
/// @param  pInitStruct: pointer to a CAN_Peli_InitTypeDef structure that
///         contains the configuration information for the CAN peripheral in the peli workmode.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_Peli_Init(CAN_Peli_InitTypeDef* pInitStruct)
{
    CAN1_PELI->BTR0 = ((u32)pInitStruct->SJW << 6) | ((u32)pInitStruct->BRP);
    CAN1_PELI->BTR1 = ((u32)pInitStruct->SAM << 7) | ((u32)pInitStruct->TESG2 << 4) | ((u32)pInitStruct->TESG1);
    if (pInitStruct->LOM == ENABLE)
        CAN1_PELI->MOD |= (u32)CAN_ListenOnlyMode;
    else
        CAN1_PELI->MOD &= ~(u32)CAN_ListenOnlyMode;
    if (pInitStruct->STM == ENABLE)
        CAN1_PELI->MOD |= (u32)CAN_SeftTestMode;
    else
        CAN1_PELI->MOD &= ~(u32)CAN_SeftTestMode;
    CAN1_PELI->EWLR = (u32)pInitStruct->EWLR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Configures the CAN_Peli reception filter according to the specified
///         parameters in the CAN_Peli_FilterInitStruct.
/// @param  CAN_Peli_FilterInitStruct: pointer to a CAN_Peli_FilterInitTypeDef
///         structure that contains the configuration information.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_Peli_FilterInit(CAN_Peli_FilterInitTypeDef* CAN_Peli_FilterInitStruct)
{
    (CAN_Peli_FilterInitStruct->AFM == CAN_FilterMode_Singal) ? (CAN1_PELI->MOD |= (u32)CAN_FilterMode_Singal)
                                                              : (CAN1_PELI->MOD &= (u32)CAN_FilterMode_Double);

    CAN1_PELI->FF    = CAN_Peli_FilterInitStruct->CAN_FilterId0;
    CAN1_PELI->ID0   = CAN_Peli_FilterInitStruct->CAN_FilterId1;
    CAN1_PELI->ID1   = CAN_Peli_FilterInitStruct->CAN_FilterId2;
    CAN1_PELI->DATA0 = CAN_Peli_FilterInitStruct->CAN_FilterId3;

    CAN1_PELI->DATA1 = CAN_Peli_FilterInitStruct->CAN_FilterMaskId0;
    CAN1_PELI->DATA2 = CAN_Peli_FilterInitStruct->CAN_FilterMaskId1;
    CAN1_PELI->DATA3 = CAN_Peli_FilterInitStruct->CAN_FilterMaskId2;
    CAN1_PELI->DATA4 = CAN_Peli_FilterInitStruct->CAN_FilterMaskId3;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each CAN_Peli_FilterInitStruct member with its default value.
/// @param  CAN_Peli_FilterInitStruct: pointer to a CAN_InitTypeDef structure
///         which ill be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_Peli_FilterStructInit(CAN_Peli_FilterInitTypeDef* CAN_Peli_FilterInitStruct)
{
    CAN_Peli_FilterInitStruct->CAN_FilterId0 = 0;
    CAN_Peli_FilterInitStruct->CAN_FilterId1 = 0;
    CAN_Peli_FilterInitStruct->CAN_FilterId2 = 0;
    CAN_Peli_FilterInitStruct->CAN_FilterId3 = 0;

    CAN_Peli_FilterInitStruct->CAN_FilterMaskId0 = 0;
    CAN_Peli_FilterInitStruct->CAN_FilterMaskId1 = 0;
    CAN_Peli_FilterInitStruct->CAN_FilterMaskId2 = 0;
    CAN_Peli_FilterInitStruct->CAN_FilterMaskId3 = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initiates and transmits a CAN frame message.
/// @param  TxMessage: pointer to a structure which contains CAN Id, CAN DLC and
///         CAN data.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_Peli_Transmit(CanPeliTxMsg* PeliTxMessage)
{
    CAN1_PELI->FF = (PeliTxMessage->FF << 7) | (PeliTxMessage->RTR << 6) | (PeliTxMessage->DLC);
    if (((FunctionalState)PeliTxMessage->FF) != ENABLE) {
        CAN1_PELI->ID0 = (PeliTxMessage->IDHH);
        // CAN1_PELI->ID1 = ((PeliTxMessage->IDHL)<<5);
        CAN1_PELI->ID1 = (PeliTxMessage->IDHL & 0xE0);
        if ((FunctionalState)(PeliTxMessage->RTR) != ENABLE) {
            CAN1_PELI->DATA0 = PeliTxMessage->Data[0];
            CAN1_PELI->DATA1 = PeliTxMessage->Data[1];
            CAN1_PELI->DATA2 = PeliTxMessage->Data[2];
            CAN1_PELI->DATA3 = PeliTxMessage->Data[3];
            CAN1_PELI->DATA4 = PeliTxMessage->Data[4];
            CAN1_PELI->DATA5 = PeliTxMessage->Data[5];
            CAN1_PELI->DATA6 = PeliTxMessage->Data[6];
            CAN1_PELI->DATA7 = PeliTxMessage->Data[7];
        }
    }
    else {
        CAN1_PELI->ID0   = PeliTxMessage->IDHH;
        CAN1_PELI->ID1   = PeliTxMessage->IDHL;
        CAN1_PELI->DATA0 = PeliTxMessage->IDLH;
        CAN1_PELI->DATA1 = PeliTxMessage->IDLL;
        if ((FunctionalState)(PeliTxMessage->RTR) != ENABLE) {
            CAN1_PELI->DATA2 = PeliTxMessage->Data[0];
            CAN1_PELI->DATA3 = PeliTxMessage->Data[1];
            CAN1_PELI->DATA4 = PeliTxMessage->Data[2];
            CAN1_PELI->DATA5 = PeliTxMessage->Data[3];
            CAN1_PELI->DATA6 = PeliTxMessage->Data[4];
            CAN1_PELI->DATA7 = PeliTxMessage->Data[5];
            CAN1_PELI->DATA8 = PeliTxMessage->Data[6];
            CAN1_PELI->DATA9 = PeliTxMessage->Data[7];
        }
    }

    (CAN1_PELI->MOD & CAN_MOD_STM) ? (CAN1->CMR = CAN_CMR_SRR | CAN_CMR_AT) : (CAN1->CMR = CAN_CMR_TR | CAN_CMR_AT);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initiates and transmits a CAN frame message.
/// @param  TxMessage: pointer to a structure which contains CAN Id, CAN DLC and
///         CAN data.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_Peli_TransmitRepeat(CanPeliTxMsg* PeliTxMessage)
{
    CAN1_PELI->FF = (PeliTxMessage->FF << 7) | (PeliTxMessage->RTR << 6) | (PeliTxMessage->DLC);
    if (((FunctionalState)PeliTxMessage->FF) != ENABLE) {
        CAN1_PELI->ID0 = (PeliTxMessage->IDHH);
        CAN1_PELI->ID1 = ((PeliTxMessage->IDHL) << 5);
        if ((FunctionalState)(PeliTxMessage->RTR) != ENABLE) {
            CAN1_PELI->DATA0 = PeliTxMessage->Data[0];
            CAN1_PELI->DATA1 = PeliTxMessage->Data[1];
            CAN1_PELI->DATA2 = PeliTxMessage->Data[2];
            CAN1_PELI->DATA3 = PeliTxMessage->Data[3];
            CAN1_PELI->DATA4 = PeliTxMessage->Data[4];
            CAN1_PELI->DATA5 = PeliTxMessage->Data[5];
            CAN1_PELI->DATA6 = PeliTxMessage->Data[6];
            CAN1_PELI->DATA7 = PeliTxMessage->Data[7];
        }
    }
    else {
        CAN1_PELI->ID0   = PeliTxMessage->IDHH;
        CAN1_PELI->ID1   = PeliTxMessage->IDHL;
        CAN1_PELI->DATA0 = PeliTxMessage->IDLH;
        CAN1_PELI->DATA1 = PeliTxMessage->IDLL;
        if ((FunctionalState)(PeliTxMessage->RTR) != ENABLE) {
            CAN1_PELI->DATA2 = PeliTxMessage->Data[0];
            CAN1_PELI->DATA3 = PeliTxMessage->Data[1];
            CAN1_PELI->DATA4 = PeliTxMessage->Data[2];
            CAN1_PELI->DATA5 = PeliTxMessage->Data[3];
            CAN1_PELI->DATA6 = PeliTxMessage->Data[4];
            CAN1_PELI->DATA7 = PeliTxMessage->Data[5];
            CAN1_PELI->DATA8 = PeliTxMessage->Data[6];
            CAN1_PELI->DATA9 = PeliTxMessage->Data[7];
        }
    }

    (CAN1_PELI->MOD & CAN_MOD_STM) ? (CAN1->CMR =  CAN_CMR_AT) : (CAN1->CMR = CAN_CMR_TR);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Receives a correct CAN frame.
/// @param  RxMessage: pointer to a structure receive frame which contains CAN
///         Id,CAN DLC, CAN data and FMI number.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_Peli_Receive(CanPeliRxMsg* PeliRxMessage)
{
    u32 tempid;
    PeliRxMessage->FF  = (CAN1_PELI->FF) >> 7;
    PeliRxMessage->RTR = ((CAN1_PELI->FF) >> 6) & 0x1;
    PeliRxMessage->DLC = (CAN1_PELI->FF) & 0xf;

    if (((FunctionalState)PeliRxMessage->FF) != ENABLE) {
        tempid = (u32)(CAN1_PELI->ID1 >> 5);
        tempid |= (u32)(CAN1_PELI->ID0 << 3);
        PeliRxMessage->ID      = tempid;
        PeliRxMessage->Data[0] = CAN1_PELI->DATA0;
        PeliRxMessage->Data[1] = CAN1_PELI->DATA1;
        PeliRxMessage->Data[2] = CAN1_PELI->DATA2;
        PeliRxMessage->Data[3] = CAN1_PELI->DATA3;
        PeliRxMessage->Data[4] = CAN1_PELI->DATA4;
        PeliRxMessage->Data[5] = CAN1_PELI->DATA5;
        PeliRxMessage->Data[6] = CAN1_PELI->DATA6;
        PeliRxMessage->Data[7] = CAN1_PELI->DATA7;
    }
    else {
        tempid = (u32)((CAN1_PELI->DATA1 & 0xf8) >> 3);
        tempid |= (u32)(CAN1_PELI->DATA0 << 5);
        tempid |= (u32)(CAN1_PELI->ID1 << 13);
        tempid |= (u32)(CAN1_PELI->ID0 << 21);
        PeliRxMessage->ID      = tempid;
        PeliRxMessage->Data[0] = CAN1_PELI->DATA2;
        PeliRxMessage->Data[1] = CAN1_PELI->DATA3;
        PeliRxMessage->Data[2] = CAN1_PELI->DATA4;
        PeliRxMessage->Data[3] = CAN1_PELI->DATA5;
        PeliRxMessage->Data[4] = CAN1_PELI->DATA6;
        PeliRxMessage->Data[5] = CAN1_PELI->DATA7;
        PeliRxMessage->Data[6] = CAN1_PELI->DATA8;
        PeliRxMessage->Data[7] = CAN1_PELI->DATA9;
    }
    CAN_FIFORelease(CAN1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Get available current informatoin in receive FIFO only in Peli
///         workmode.
/// @retval The value in reg RMC
////////////////////////////////////////////////////////////////////////////////
u32 CAN_Peli_GetRxFIFOInfo(void)
{
    return CAN1_PELI->RMC;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the CAN's last error code (LEC).
/// @retval Error code:
///          - CAN_ERRORCODE_NoErr: No Error
///          - CAN_ERRORCODE_StuffErr: Stuff Error
///          - CAN_ERRORCODE_FormErr: Form Error
///          - CAN_ERRORCODE_ACKErr : Acknowledgment Error
///          - CAN_ERRORCODE_BitRecessiveErr: Bit Recessive Error
///          - CAN_ERRORCODE_BitDominantErr: Bit Dominant Error
///          - CAN_ERRORCODE_CRCErr: CRC Error
///          - CAN_ERRORCODE_SoftwareSetErr: Software Set Error
////////////////////////////////////////////////////////////////////////////////
u8 CAN_Peli_GetLastErrorCode(void)
{
    // Return the error code
    return (u8)CAN1_PELI->ECC;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the CAN Receive Error Counter (REC).
/// @note   In case of an error during reception, this counter is incremented
///         by 1 or by 8 depending on the error condition as defined by the CAN
///         standard. After every successful reception, the counter is
///         decremented by 1 or reset to 120 if its value was higher than 128.
///         When the counter value exceeds 127, the CAN controller enters the
///         error passive state.
/// @retval CAN Receive Error Counter.
////////////////////////////////////////////////////////////////////////////////
u8 CAN_Peli_GetReceiveErrorCounter(void)
{
    // Return the Receive Error Counter
    return (u8)(CAN1_PELI->RXERR);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Returns the LSB of the 9-bit CANx Transmit Error Counter(TEC).
/// @retval LSB of the 8-bit CAN Transmit Error Counter.
////////////////////////////////////////////////////////////////////////////////
u8 CAN_Peli_GetLSBTransmitErrorCounter(void)
{
    // Return the LSB of the 8-bit CAN Transmit Error Counter(TEC)
    return (u8)(CAN1_PELI->TXERR);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified CAN interrupts in peli workmode.
/// @param  CAN_IT: specifies the CAN interrupt sources to be enabled or
///         disabled.
///          This parameter can be:
///            @arg CAN_IT_RI: Receive FIFO not empty Interrupt
///            @arg CAN_IT_TI: Transmit Interrupt
///            @arg CAN_IT_EI: ERROR Interrupt
///            @arg CAN_IT_DOI: Data voerflow Interrupt
///            @arg CAN_IT_WUI: Wakeup Interrupt
///            @arg CAN_IT_EPI(only Peli): passive error Interrupt
///            @arg CAN_IT_ALI(only Peli): arbiter lose Interrupt
///            @arg CAN_IT_BEI(only Peli): bus error Interrupt
/// @arg    CAN_IT_ALL: use it can enble all Interrupt
/// @param  state: new state of the CAN interrupts.
///         This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void CAN_Peli_ITConfig(u32 CAN_IT, FunctionalState state)
{
    (state) ? (CAN1_PELI->IER |= CAN_IT) : (CAN1_PELI->IER &= ~CAN_IT);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified CAN interrupt has occurred or not.
/// @param  CAN_IT: specifies the CAN interrupt source to check.
///         This parameter can be one of the following values:
///            @arg CAN_IT_RI: Receive FIFO not empty Interrupt
///            @arg CAN_IT_TI: Transmit Interrupt
///            @arg CAN_IT_EI: ERROR Interrupt
///            @arg CAN_IT_DOI: Data voerflow Interrupt
///            @arg CAN_IT_WUI: Wakeup Interrupt
///            @arg CAN_IT_EPI(only Peli): passive error Interrupt
///            @arg CAN_IT_ALI(only Peli): arbiter lose Interrupt
///            @arg CAN_IT_BEI(only Peli): bus error Interrupt
/// @arg    CAN_IT_ALL: use it can enble all Interrupt
/// @retval The current state of CAN_IT (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
ITStatus CAN_Peli_GetITStatus(u32 CAN_IT)
{
    return (ITStatus)(((CAN1_PELI->IR & CAN_IT) != CAN_IT) ? RESET : SET);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Config CAN_Peli_InitTypeDef baud parameter.
/// @param  CAN_Peli_InitTypeDef: CAN struct.
/// @param  SrcClk: CAN module clock.
/// @param  baud: specified baud.
/// @retval The current state of CAN_IT (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
void CAN_AutoCfg_BaudParam(CAN_Peli_InitTypeDef* pInitStruct, u32 SrcClk, u32 baud)
{
    u32 i, value = baud, record = 1;
    u32 remain = 0, sumPrescaler = 0;
    while ((baud == 0) || (SrcClk == 0))
        ;
    sumPrescaler = SrcClk / baud;
    sumPrescaler = sumPrescaler / 2;
    for (i = 25; i > 3; i--) {
        remain = sumPrescaler - ((sumPrescaler / i) * i);
        if (remain == 0) {
            record = i;
            break;
        }
        else {
            if (remain < value) {
                value  = remain;
                record = i;
            }
        }
    }
    pInitStruct->SJW   = 0;
    pInitStruct->BRP   = (sumPrescaler / record) - 1;
    pInitStruct->TESG2 = (record - 3) / 3;
    pInitStruct->TESG1 = (record - 3) - pInitStruct->TESG2;
}

/// @}

/// @}

/// @}

#endif
