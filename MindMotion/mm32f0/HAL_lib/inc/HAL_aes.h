////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_AES.H
/// @author   Z Yan
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE AES
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
#ifndef __HAL_AES_H
#define __HAL_AES_H

// Files includes  -------------------------------------------------------------
#include "types.h"
#include "mm32.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup AES_HAL
/// @brief AES HAL modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup AES_Exported_Types
/// @{

#if defined(__MM3N1) || defined(__MM0N1)

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES Init structure definition
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    u32 AES_Operation;                                                          ///< AES operation mode
    u32 AES_Chaining;                                                           ///< AES chaining mode
    u32 AES_DataType;                                                           ///< AES data type select

} AES_InitTypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES key structure definition
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    u32 AES_Key0;                                                               ///< AES key part 0
    u32 AES_Key1;                                                               ///< AES key part 1
    u32 AES_Key2;                                                               ///< AES key part 2
    u32 AES_Key3;                                                               ///< AES key part 3
    u32 AES_Key4;                                                               ///< AES key part 4
    u32 AES_Key5;                                                               ///< AES key part 5
    u32 AES_Key6;                                                               ///< AES key part 6
    u32 AES_Key7;                                                               ///< AES key part 7
} AES_KeyInitTypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES initialization vector structure definition
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    u32 AES_IV0;                                                                ///< AES initialization vector part 0
    u32 AES_IV1;                                                                ///< AES initialization vector part 1
    u32 AES_IV2;                                                                ///< AES initialization vector part 2
    u32 AES_IV3;                                                                ///< AES initialization vector part 3
} AES_IVInitTypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES operation enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    AES_Operation_Encryp            = AES_CR_ENCRYPT,                           ///< Encrypt operation
    AES_Operation_KeyDeriv          = AES_CR_KEYEXT,                            ///< Key extand operation
    AES_Operation_Decryp            = AES_CR_DECRYPT,                           ///< Decrypt operation
    AES_Operation_KeyDerivAndDecryp = AES_CR_EXTDECRYPT                         ///< Key extand and decrypt operation
} AES_Operation_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES chaining mode enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    AES_Chaining_ECB = AES_CR_EBC,                                              ///< AES EBC chainging mode
    AES_Chaining_CBC = AES_CR_CBC,                                              ///< AES CBC chainging mode
    AES_Chaining_CTR = AES_CR_CTR,                                              ///< AES CTR chainging mode
    AES_Chaining_CFB = AES_CR_CFB,                                              ///< AES CFB chainging mode
    AES_Chaining_OFB = AES_CR_OFB                                               ///< AES OFB chainging mode
} AES_ChainingMode_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES data type enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    AES_DataType_32b = AES_CR_UNREVERSE,                                        ///< Input data unreverse
    AES_DataType_16b = AES_CR_HALFREVERSE,                                      ///< Input data half word reverse
    AES_DataType_8b  = AES_CR_BYTEREVERSE,                                      ///< Input data byte reverse
    AES_DataType_1b  = AES_CR_BITREVERSE                                        ///< Input data bit reverse
} AES_DataType_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES flag enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    AES_FLAG_CCF   = AES_SR_CCF,                                                ///< AES calculate complete flag
    AES_FLAG_RDERR = AES_SR_RDERR,                                              ///< AES read error flag
    AES_FLAG_WRERR = AES_SR_WRERR                                               ///< AES write error flag
} AES_Flag_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES interrupt enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    AES_IT_CC  = AES_CR_CCIE,                                                   ///< AES calculate complete interrut enable
    AES_IT_ERR = AES_CR_ERRIE                                                   ///< AES error interrupt enable
} AES_Interrupt_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES DMA transfer enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    AES_DMATransfer_In    = AES_CR_DMAINEN,                                     ///< AES DMA input enable
    AES_DMATransfer_Out   = AES_CR_DMAOUTEN,                                    ///< AES DMA output enable
    AES_DMATransfer_InOut = (AES_CR_DMAINEN | AES_CR_DMAOUTEN)                  ///< AES DMA input and output enable
} AES_DMATransfer_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES key size enum definition
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    AES_Key_128 = AES_CR_KSIZE128,                                              ///< 128 bits key size
    AES_Key_192 = AES_CR_KSIZE192,                                              ///< 192 bits key size
    AES_Key_256 = AES_CR_KSIZE256                                               ///< 256 bits key size
} AES_KeySize_TypeDef;

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup AES_Exported_Constants
/// @{

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup AES_Exported_Variables
/// @{

#ifdef _HAL_AES_C_
#define GLOBAL
#else
#define GLOBAL extern
#endif

#undef GLOBAL

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup AES_Exported_Functions
/// @{

/* Subfunction for encrypt and decrypt ***************************************/
void AES_SetMode(AES_Operation_TypeDef operation, AES_ChainingMode_TypeDef chainingMode);
void LET_AES_KEY(u8* key);
void LET_AES_IVR(u32* initVectors);

bool AES_GetStatus(u8 sta, u8* buffer);

/* Initialization and configuration functions *********************************/
void AES_DeInit(void);
void AES_Init(AES_InitTypeDef* pInitStruct);
void AES_KeyInit(AES_KeyInitTypeDef* pKeyInitStruct);
void AES_IVInit(AES_IVInitTypeDef* pIVInitStruct);
void AES_Cmd(FunctionalState newState);
void AES_KeySize(AES_KeySize_TypeDef keySize);

/* Structures initialization functions ****************************************/
void AES_StructInit(AES_InitTypeDef* pInitStruct);
void AES_KeyStructInit(AES_KeyInitTypeDef* pKeyInitStruct);
void AES_IVStructInit(AES_IVInitTypeDef* pIVInitStruct);

/* AES Read and Write functions **********************************************/
void AES_WriteSubData(u32 data);
void AES_ReadKey(AES_KeyInitTypeDef* pKeyInitStruct);
void AES_ReadIV(AES_IVInitTypeDef* pIVInitStruct);

u32 AES_ReadSubData(void);

/* DMA transfers management function ******************************************/
void AES_DMAConfig(AES_DMATransfer_TypeDef dmaTransfer, FunctionalState newState);

/* Interrupts and flags management functions **********************************/
void AES_ITConfig(AES_Interrupt_TypeDef interruput, FunctionalState newState);
void AES_ClearFlag(AES_Flag_TypeDef flag);
void AES_ClearITPendingBit(AES_Interrupt_TypeDef interruput);

FlagStatus AES_GetFlagStatus(AES_Flag_TypeDef flag);

ITStatus AES_GetITStatus(AES_Interrupt_TypeDef interruput);

bool AES_ECB_Encrypt(u8* pKey, u8* pBuffer);
bool AES_ECB_Decrypt(u8* pKey, u8* pBuffer);

bool AES_CBC_Encrypt(u8* pKey, u8* pInitVectors, u8* pBuffer);
bool AES_CBC_Decrypt(u8* pKey, u8* pInitVectors, u8* pBuffer);
bool AES_CTR_Encrypt(u8* pKey, u8* pInitVectors, u8* pBuffer);
bool AES_CTR_Decrypt(u8* pKey, u8* pInitVectors, u8* pBuffer);
bool AES_CFB_Encrypt(u8* pKey, u8* pInitVectors, u8* pBuffer);
bool AES_CFB_Decrypt(u8* pKey, u8* pInitVectors, u8* pBuffer);
bool AES_OFB_Encrypt(u8* pKey, u8* pInitVectors, u8* pBuffer);
bool AES_OFB_Decrypt(u8* pKey, u8* pInitVectors, u8* pBuffer);

#endif


/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* __HAL_AES_H */
////////////////////////////////////////////////////////////////////////////////
