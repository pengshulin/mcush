////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_SPI.H
/// @author   Z Yan
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE SPI
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
#ifndef __HAL_SPI_H
#define __HAL_SPI_H

// Files includes  -------------------------------------------------------------
#include "types.h"
#include "mm32.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup SPI_HAL
/// @brief SPI HAL modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup SPI_Exported_Types
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI mode enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_Mode_Slave  = 0x0000,        ///< SPI slave mode
    SPI_Mode_Master = SPI_GCR_MODE   ///< SPI master mode
} SPI_Mode_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI data size enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_DataSize_8b  = 0x0000,         ///< 8 bits valid data
    SPI_DataSize_32b = SPI_GCR_DWSEL   ///< 32 bits valid data
} SPI_DataSize_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI clock polarity enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_CPOL_Low  = 0x0000,                                                     ///< The clock is low in idle state.
    SPI_CPOL_High = SPI_CCR_CPOL                                                ///< The clock is high in idle state.
} SPI_CPOL_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI clock phase enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_CPHA_2Edge = 0x0000,                                                    ///< Data sampling starts from the second clock edge.
    SPI_CPHA_1Edge = SPI_CCR_CPHA                                               ///< Data sampling starts from the first clock edge.
} SPI_CPHA_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI nss control mode enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_NSS_Soft = 0x0000,
    SPI_NSS_Hard = SPI_GCR_NSS
} SPI_NSS_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI baud rate prescaler enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_BaudRatePrescaler_2   = 0x0002,                                         ///< SCK clock devide by 2
    SPI_BaudRatePrescaler_4   = 0x0004,                                         ///< SCK clock devide by 4
    SPI_BaudRatePrescaler_8   = 0x0008,                                         ///< SCK clock devide by 7
    SPI_BaudRatePrescaler_16  = 0x0010,                                         ///< SCK clock devide by 16
    SPI_BaudRatePrescaler_32  = 0x0020,                                         ///< SCK clock devide by 32
    SPI_BaudRatePrescaler_64  = 0x0040,                                         ///< SCK clock devide by 64
    SPI_BaudRatePrescaler_128 = 0x0080,                                         ///< SCK clock devide by 128
    SPI_BaudRatePrescaler_256 = 0x0100                                          ///< SCK clock devide by 256
} SPI_BaudRatePrescaler_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI first bit enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_FirstBit_MSB = 0x0000,                                                  ///< Data transfers start from MSB
    SPI_FirstBit_LSB = SPI_CCR_LSBFE                                            ///< Data transfers start from LSB
} SPI_FirstBit_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI FIFO trigger level enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_RXTLF = SPI_GCR_RXTLF_Half,                                             ///< RX FIFO trigger level
    SPI_TXTLF = SPI_GCR_TXTLF_Half                                              ///< TX FIFO trigger level
} SPI_TLF_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI bit derection enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_Direction_Rx,                                                           ///< Receive enable
    SPI_Direction_Tx,                                                           ///< Transmit enable
    SPI_Disable_Rx,                                                             ///< Receive disable
    SPI_Disable_Tx                                                              ///< Transmit disable
} SPI_Direction_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI flag enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_FLAG_RXAVL       = SPI_SR_RXAVL,                                        ///< Receive 1 byte available data flag
    SPI_FLAG_TXEPT       = SPI_SR_TXEPT,                                        ///< Transmitter empty flag
    SPI_FLAG_TXFULL      = SPI_SR_TXFULL,                                       ///< Transmitter FIFO full status flag
    SPI_FLAG_RXAVL_4BYTE = SPI_SR_RXAVL_4BYTE                                   ///< Receive 4 bytes available data flag
} SPI_FLAG_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI slave mode data edge adjust enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_SlaveAdjust_LOW,                                                        ///< SPI slave mode data edge adjust in low speed mode
    SPI_SlaveAdjust_FAST                                                        ///< SPI slave mode data edge adjust in fast speed mode
} SPI_SlaveAdjust_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI data edge adjust enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_DataEdgeAdjust_LOW,                                                     ///< SPI data edge adjust in low speed mode
    SPI_DataEdgeAdjust_FAST                                                     ///< SPI data edge adjust in fast speed mode
} SPI_DataEdgeAdjust_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI interruput enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPI_IT_TXEPT    = 0x40,                                                     ///< Transmitter empty interrupt
    SPI_IT_RXFULL   = 0x20,                                                     ///< RX FIFO full interrupt
    SPI_IT_RXMATCH  = 0x10,                                                     ///< Receive data match the RXDNR number interrut
    SPI_IT_RXOERR   = 0x08,                                                     ///< Receive overrun error interrupt
    SPI_IT_UNDERRUN = 0x04,                                                     ///< Underrun interrupt
    SPI_IT_RX       = 0x02,                                                     ///< Receive available data interrupt
    SPI_IT_TX       = 0x01                                                      ///< Transmit FIFO available interrupt
} SPI_IT_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI Init structure definition
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    SPI_Mode_TypeDef                SPI_Mode;                                   ///< Specifies the SPI operating mode
    SPI_DataSize_TypeDef            SPI_DataSize;                               ///< Specifies the SPI available data size
    u8                              SPI_DataWidth;                              ///< SPI data length
    SPI_CPOL_TypeDef                SPI_CPOL;                                   ///< Specifies the serial clock steady state
    SPI_CPHA_TypeDef                SPI_CPHA;                                   ///< Specifies the clock active edge for the bit capture
    SPI_NSS_TypeDef                 SPI_NSS;                                    ///< Specifies whether the NSS signal is managed by hardware or by software
    SPI_BaudRatePrescaler_TypeDef   SPI_BaudRatePrescaler;                      ///< Specifies the Baud Rate prescaler value which will be
                                                                                ///< used to configure the transmit and receive SCK clock
    SPI_FirstBit_TypeDef            SPI_FirstBit;                               ///< Specifies whether data transfers start from MSB or LSB bit
    //  u16     SPI_length;
} SPI_InitTypeDef;

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup SPI_Exported_Constants
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup SPI_Register_Mask
/// @{

#define GCR_Mask ((u32)0x0FFF)
#define CCR_Mask ((u32)0x003F)
#define BRR_Mask ((u32)0xFFFF)
#define ECR_Mask ((u32)0x001F)

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup SPI_Exported_Variables
/// @{

#ifdef _HAL_SPI_C_
#define GLOBAL
#else
#define GLOBAL extern
#endif

#undef GLOBAL

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup SPI_Exported_Functions
/// @{

void SPI_DeInit(SPI_TypeDef* SPIx);
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* pInitStruct);
void SPI_StructInit(SPI_InitTypeDef* pInitStruct);
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState newState);
void SPI_ITConfig(SPI_TypeDef* SPIx, u8 interrupt, FunctionalState newState);
void SPI_DMACmd(SPI_TypeDef* SPIx, FunctionalState newState);
void SPI_FifoTrigger(SPI_TypeDef* SPIx, SPI_TLF_TypeDef fifoTriggerValue, FunctionalState newState);
void SPI_SendData(SPI_TypeDef* SPIx, u32 data);
void SPI_CSInternalSelected(SPI_TypeDef* SPIx, FunctionalState newState);
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef* SPIx, SPI_NSS_TypeDef nss);

void SPI_BiDirectionalLineConfig(SPI_TypeDef* SPIx, SPI_Direction_TypeDef direction);
void SPI_ClearITPendingBit(SPI_TypeDef* SPIx, SPI_IT_TypeDef interrupt);
void SPI_RxBytes(SPI_TypeDef* SPIx, u16 number);
void SPI_SlaveAdjust(SPI_TypeDef* SPIx, SPI_SlaveAdjust_TypeDef adjustValue);

bool SPI_DataSizeConfig(SPI_TypeDef* SPIx, u8 dataSize);

u32 SPI_ReceiveData(SPI_TypeDef* SPIx);

FlagStatus SPI_GetFlagStatus(SPI_TypeDef* SPIx, SPI_FLAG_TypeDef flag);

ITStatus SPI_GetITStatus(SPI_TypeDef* SPIx, SPI_IT_TypeDef interrupt);

////////////////////////////////////////////////////////////////////////////////
//          Extended function interface
////////////////////////////////////////////////////////////////////////////////
void exSPI_ITCmd(SPI_TypeDef* SPIx, FunctionalState newState);
void exSPI_ITConfig(SPI_TypeDef* SPIx, SPI_IT_TypeDef interrput, FunctionalState newState);
void exSPI_DMACmd(SPI_TypeDef* SPIx, FunctionalState newState);
void exSPI_CSInternalSelected(SPI_TypeDef* SPIx, FunctionalState newState);
void exSPI_DataEdgeAdjust(SPI_TypeDef* SPIx, SPI_DataEdgeAdjust_TypeDef adjustValue);


/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /*__HAL_SPI_H */
////////////////////////////////////////////////////////////////////////////////
