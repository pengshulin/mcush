////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_UART.H
/// @author   QR Wang
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE UART
///           FIRMWARE LIBRARY.
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
#ifndef __HAL_UART_H
#define __HAL_UART_H

// Files includes  -------------------------------------------------------------
#include "mm32.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

/////////////////////////////////////1///////////////////////////////////////////
/// @defgroup UART_HAL
/// @brief UART HAL modules
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @defgroup UART_Exported_Types
/// @{
///

////////////////////////////////////////////////////////////////////////////////
/// @brief UART Word Length Enumerate definition
/// @anchor UART_Word_Length
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    UART_WordLength_5b = 0U,
    UART_WordLength_6b = 1U << UART_CCR_CHAR_Pos,
    UART_WordLength_7b = 2U << UART_CCR_CHAR_Pos,
    UART_WordLength_8b = 3U << UART_CCR_CHAR_Pos
} UART_WordLength_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief UART Stop Bits Enumerate definition
/// @anchor UART_Stop_Bits
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    UART_StopBits_1 = 0U,
    UART_StopBits_2 = UART_CCR_SPB,

#if defined(__MM0P1) || defined(__MM0Q1)
    UART_StopBits_0_5 = UART_CCR_SPB1,
    UART_StopBits_1_5 = UART_CCR_SPB1 | UART_CCR_SPB0,
#endif
} UART_Stop_Bits_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief UART Parity Enumerate definition
/// @anchor UART_Parity
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    UART_Parity_No   = 0U,
    UART_Parity_Even = UART_CCR_PEN | UART_CCR_PSEL,
    UART_Parity_Odd  = UART_CCR_PEN
} UART_Parity_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief UART Hardware Flow Control Enumerate definition
/// @anchor UART_Hardware_Flow_Control
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    UART_HWFlowControl_None    = 0U,
/*1*<
    UART_HWFlowControl_RTS     = UART_GCR_AUTOFLOW,
    UART_HWFlowControl_CTS     = UART_GCR_AUTOFLOW,
>*1*/
    UART_HWFlowControl_RTS_CTS = UART_GCR_AUTOFLOW
} UART_HW_FLOWCONTROL_TypeDef;

#if defined(__MM0P1) || defined(__MM0Q1)
typedef enum {
    UART_WakeUp_IdleLine    = 0U,  //
    UART_WakeUp_AddressMark = UART_CCR_WAKE
} UART_WakeUp_TypeDef;

typedef enum {
    UART_9bit_Polarity_Low  = 0U,  //
    UART_9bit_Polarity_High = UART_CCR_B8POL
} UART_9bit_Polarity_TypeDef;

#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief UART Init Structure definition
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    u32                         BaudRate;                           ///< This member configures the UART communication baud rate.
    UART_WordLength_TypeDef     WordLength;                         ///< Specifies the number of data bits transmitted or
                                                                    ///< received in a frame. This parameter can be a value of @ref UART_Word_Length
    UART_Stop_Bits_TypeDef      StopBits;                           ///< Specifies the number of stop bits transmitted.
                                                                    ///< This parameter can be a value of @ref UART_Stop_Bits
    UART_Parity_TypeDef         Parity;                             ///< Specifies the parity mode.
                                                                    ///< This parameter can be a value of @ref UART_Parity
    u16                         Mode;                               ///< Specifies wether the Receive or Transmit mode is
                                                                    ///< enabled or disabled. This parameter can be a value of UART_Mode
    UART_HW_FLOWCONTROL_TypeDef HWFlowControl;                      ///< Specifies wether the hardware flow control mode is enabled or disabled.
                                                                    ///< This parameter can be a value of UART_Hardware_Flow_Control
} UART_InitTypeDef;

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup UART_Exported_Constants
/// @{

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup UART_Exported_Variables
/// @{
#ifdef _HAL_UART_C_

#define GLOBAL
#else
#define GLOBAL extern
#endif

#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup UART_Exported_Functions
/// @{
void UART_DeInit(UART_TypeDef* UARTx);
void UART_Init(UART_TypeDef* UARTx, UART_InitTypeDef* UART_InitStruct);
void UART_StructInit(UART_InitTypeDef* UART_InitStruct);
void UART_Cmd(UART_TypeDef* UARTn, FunctionalState state);
void UART_ITConfig(UART_TypeDef* UARTn, u16 UART_IT, FunctionalState state);
void UART_DMACmd(UART_TypeDef* UARTn, u16 UART_DMAReq, FunctionalState state);
void UART_SendData(UART_TypeDef* UARTn, u16 Data);
void UART_ClearITPendingBit(UART_TypeDef* UARTn, u16 UART_IT);

u16        UART_ReceiveData(UART_TypeDef* UARTn);
FlagStatus UART_GetFlagStatus(UART_TypeDef* UARTn, u16 UART_FLAG);
ITStatus   UART_GetITStatus(UART_TypeDef* UARTn, u16 UART_IT);

#if defined(__MM0P1) || defined(__MM0Q1)
void UART_WakeUpConfig(UART_TypeDef* UARTx, UART_WakeUp_TypeDef mode);
void UART_ReceiverWakeUpCmd(UART_TypeDef* UARTx, FunctionalState state);
void UART_SetRXAddress(UART_TypeDef* UARTx, u8 address);
void UART_SetRXMASK(UART_TypeDef* UARTx, u8 address);
void UART_Enable9bit(UART_TypeDef* UARTx, FunctionalState state);
void UART_Set9bitLevel(UART_TypeDef* UARTx, FunctionalState state);
void UART_Set9bitPolarity(UART_TypeDef* UARTx, UART_9bit_Polarity_TypeDef polarity);
void UART_Set9bitAutomaticToggle(UART_TypeDef* UARTx, FunctionalState state);
void UART_HalfDuplexCmd(UART_TypeDef* UARTx, FunctionalState state);
void UART_SetGuardTime(UART_TypeDef* UARTx, u8 guardTime);
void UART_SmartCardCmd(UART_TypeDef* UARTx, FunctionalState state);
void UART_SmartCardNACKCmd(UART_TypeDef* UARTx, FunctionalState state);
void UART_SendBreak(UART_TypeDef* UARTx);

#endif

/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* __HAL_UART_H */
////////////////////////////////////////////////////////////////////////////////
