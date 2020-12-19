////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_AES.C
/// @author   Z Yan
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE AES FIRMWARE FUNCTIONS.
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
#define _HAL_AES_C_

// Files includes  -------------------------------------------------------------
#include <string.h>
#include "HAL_aes.h"
#include "HAL_rcc.h"

#include "common.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AES_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AES_Exported_Functions
/// @{

#if defined(__MM3N1) || defined(__MM0N1)

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set AES operation mode.
/// @param operation: Select AES operation mode.
///   This parameter can be:
/// @AES_Operation_Encryp
/// @AES_Operation_KeyDeriv
/// @AES_Operation_Decryp
/// @AES_Operation_KeyDerivAndDecryp
/// @param chainingMode: Select AES chaining mode.
///   This parameter can be:
/// @AES_Chaining_ECB
/// @AES_Chaining_CBC
/// @AES_Chaining_CTR
/// @AES_Chaining_CFB
/// @AES_Chaining_OFB
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_SetMode(AES_Operation_TypeDef operation, AES_ChainingMode_TypeDef chainingMode)
{
    AES->CR = 0;
    AES->CR = (AES->CR & (~(u32)AES_CR_MODE)) | operation;
    AES->CR = (AES->CR & (~(u32)(AES_CR_CHMODL | AES_CR_CHMODH))) | chainingMode;
}

u32 inin;

u32 swap(u32 data)
{
//  return (data & 0xff000000) >> 24 | (data & 0x00ff0000) >> 8 | (data & 0x0000ff00) << 8 | (data & 0xff) << 24;
//  return data;

    u32 dat = 0;
    for (u8 i = 0; i < 32; i++) {
        dat <<= 1;
        if ((data & 0x00000001) == 1) {
            dat++;
        }
        data >>= 1;
    }
    return dat;
//  return (data & 0xffff0000) >> 16 | (data & 0x00ffff) << 16;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Subfunction used in encrypt and decrypt progress.
/// @param sta: Decide whether to write and read data.
/// @param pBuffer: input and oupt data pointer.
/// @retval  Process result
/// This parameter can be ERROR or SUCCESS.
////////////////////////////////////////////////////////////////////////////////
bool AES_GetStatus(u8 sta, u8* pBuffer)
{
    bool status = SUCCESS;
    u16         i;
    u32* pdata = (u32*)pBuffer;

    AES_Cmd(ENABLE);


    if (sta) {
        for (i = 0; i < 4; i++) {
//            AES_WriteSubData(swap(pdata[i]));
            AES_WriteSubData(swap(pdata[3 - i]));
//            AES_WriteSubData(((u32*)pBuffer)[3 - i]);
            inin = AES->DINR;
        }
    }

    do {
        if (AES_GetFlagStatus(AES_FLAG_CCF)) {
            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR)) {
                AES_ClearFlag(AES_FLAG_WRERR);
                status = ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000) {
            status = ERROR;
        }
    } while (i++ < 1000);

    if (sta) {
        if (status != ERROR) {
            for (i = 0; i < 4; i++) {
                ((u32*)pBuffer)[3 - i] = swap(AES->DOUTR);
//                ((u32*)pBuffer)[i] = swap(AES->DOUTR);
            }
        }
    }

    AES_Cmd(DISABLE);

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Write key to key register.
/// @param pKey: Key pointer.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void LET_AES_KEY(u8* pKey)
{
    u8 KeySize = (AES->CR & AES_CR_KSIZE) >> AES_CR_KSIZE_Pos;

    AES->KEYR0 = *(u32*)(pKey + 0);
    AES->KEYR1 = *(u32*)(pKey + 4);
    AES->KEYR2 = *(u32*)(pKey + 8);
    AES->KEYR3 = *(u32*)(pKey + 12);

    if (KeySize) {
        AES->KEYR4 = *(u32*)(pKey + 16);
        AES->KEYR5 = *(u32*)(pKey + 20);
    }
    if (KeySize > 1) {
        AES->KEYR6 = *(u32*)(pKey + 24);
        AES->KEYR7 = *(u32*)(pKey + 28);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Write initialization vector to
///   initialization vector register.
/// @param pInitVectors: Initialization vector pointer.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void LET_AES_IVR(u32* pInitVectors)
{
    AES->IVR0 = *(u32*)(pInitVectors++);
    AES->IVR1 = *(u32*)(pInitVectors++);
    AES->IVR2 = *(u32*)(pInitVectors++);
    AES->IVR3 = *(u32*)(pInitVectors++);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Deinitializes the AES peripheral registers to their
///   default reset values.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_DeInit(void)
{
    COMMON_DisableIpClock(emCLOCK_AES);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initializes AES according to the specified
///   parameters in the AES_InitStruct .
/// @param pInitStruct: Pointer to a AES_InitTypeDef structure
///   that contains the configuration information for AES.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_Init(AES_InitTypeDef* pInitStruct)
{
    MODIFY_REG(AES->CR, ~(AES_CR_MODE | AES_CR_CHMODL | AES_CR_CHMODH | AES_CR_DATATYPE),
               (pInitStruct->AES_Operation | pInitStruct->AES_Chaining | pInitStruct->AES_DataType));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Write key to key register.
/// @param pKeyInitStruct: Pointer to a AES_KeyInitTypeDef structure
///   that contains the key.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_KeyInit(AES_KeyInitTypeDef* pKeyInitStruct)
{
    WRITE_REG(AES->KEYR0, pKeyInitStruct->AES_Key0);
    WRITE_REG(AES->KEYR1, pKeyInitStruct->AES_Key1);
    WRITE_REG(AES->KEYR2, pKeyInitStruct->AES_Key2);
    WRITE_REG(AES->KEYR3, pKeyInitStruct->AES_Key3);
    WRITE_REG(AES->KEYR4, pKeyInitStruct->AES_Key4);
    WRITE_REG(AES->KEYR5, pKeyInitStruct->AES_Key5);
    WRITE_REG(AES->KEYR6, pKeyInitStruct->AES_Key6);
    WRITE_REG(AES->KEYR7, pKeyInitStruct->AES_Key7);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Write initialization vector to initialization vector register.
/// @param pIVInitStruct: Pointer to a AES_IVInitTypeDef structure
///   that contains the initialization vector.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_IVInit(AES_IVInitTypeDef* pIVInitStruct)
{
    WRITE_REG(AES->IVR0, pIVInitStruct->AES_IV0);
    WRITE_REG(AES->IVR1, pIVInitStruct->AES_IV1);
    WRITE_REG(AES->IVR2, pIVInitStruct->AES_IV2);
    WRITE_REG(AES->IVR3, pIVInitStruct->AES_IV3);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables AES.
/// @param newState: new state of AES.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_Cmd(FunctionalState newState)
{
    (newState != DISABLE) ? SET_BIT(AES->CR, (u32)AES_CR_EN) : CLEAR_BIT(AES->CR, (u32)AES_CR_EN);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set key size.
/// @param keySize: Select key size.
///   This parameter can be one of the following values:
/// @arg AES_KeySize_128
/// @arg AES_KeySize_192
/// @arg AES_KeySize_256
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_KeySize(AES_KeySize_TypeDef keySize)
{
    MODIFY_REG(AES->CR, AES_CR_KSIZE, keySize);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each AES_InitStruct member with its default value.
/// @param pInitStruct: pointer to a AES_InitTypeDef structure
///   which will be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_StructInit(AES_InitTypeDef* pInitStruct)
{
    pInitStruct->AES_Operation = AES_Operation_Encryp;
    pInitStruct->AES_Chaining  = AES_Chaining_ECB;
    pInitStruct->AES_DataType  = AES_DataType_32b;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each AES_KeyInitStruct member with 0x00.
/// @param pKeyInitStruct: pointer to a AES_KeyInitTypeDef structure
///   which will be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_KeyStructInit(AES_KeyInitTypeDef* pKeyInitStruct)
{
    memset(pKeyInitStruct, 0x00, sizeof(AES_KeyInitTypeDef));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Fills each AES_IVInitStruct member with 0x00.
/// @param pIVInitStruct: pointer to a AES_IVInitTypeDef structure
///   which will be initialized.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_IVStructInit(AES_IVInitTypeDef* pIVInitStruct)
{
    memset(pIVInitStruct, 0x00, sizeof(AES_IVInitTypeDef));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Write data to AES data input register.
/// @param data: data to write.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_WriteSubData(u32 data)
{
    WRITE_REG(AES->DINR, data);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Read data from AES data output register.
/// @retval  32bits data read from output register
////////////////////////////////////////////////////////////////////////////////
u32 AES_ReadSubData(void)
{
    return AES->DOUTR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Read key from AES key register.
/// @param pKeyInitStruct:  pointer to a AES_KeyInitTypeDef which will save
///     the key read from AES key register.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_ReadKey(AES_KeyInitTypeDef* pKeyInitStruct)
{
    pKeyInitStruct->AES_Key0 = AES->KEYR0;
    pKeyInitStruct->AES_Key1 = AES->KEYR1;
    pKeyInitStruct->AES_Key2 = AES->KEYR2;
    pKeyInitStruct->AES_Key3 = AES->KEYR3;
    pKeyInitStruct->AES_Key4 = AES->KEYR4;
    pKeyInitStruct->AES_Key5 = AES->KEYR5;
    pKeyInitStruct->AES_Key6 = AES->KEYR6;
    pKeyInitStruct->AES_Key7 = AES->KEYR7;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Read initialization vector from AES initialization vector register.
/// @param: pointer to a AES_IVInitTypeDef which will save initialization vector
/// read from  AES initialization vector register.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_ReadIV(AES_IVInitTypeDef* pIVInitStruct)
{
    pIVInitStruct->AES_IV0 = AES->IVR0;
    pIVInitStruct->AES_IV1 = AES->IVR1;
    pIVInitStruct->AES_IV2 = AES->IVR2;
    pIVInitStruct->AES_IV3 = AES->IVR3;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the AES DMA interface.
/// @param dmaTransfer: specifies the DMA request.
///   This parameter can be
/// @arg: AES_DMATransfer_In
/// @arg: AES_DMATransfer_Out
/// @arg: AES_DMATransfer_InOut
/// @param newState: new state of the AES DMA request.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_DMAConfig(AES_DMATransfer_TypeDef dmaTransfer, FunctionalState newState)
{
    (newState != DISABLE) ? SET_BIT(AES->CR, dmaTransfer) : CLEAR_BIT(AES->CR, (u32)dmaTransfer);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enables or disables the specified AES interrupts.
/// @param interruput: specifies the AES interrupt sources to be
///   enabled or disabled.
///   This parameter can be one of the following values:
/// @arg AES_IT_CC:  Error interrupt
/// @arg AES_IT_ERR: Calculate complete interrupt
/// @param newState: New state of the specified AES interrupts.
///   This parameter can be: ENABLE or DISABLE.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_ITConfig(AES_Interrupt_TypeDef interruput, FunctionalState newState)
{
    (newState != DISABLE) ? SET_BIT(AES->CR, (u32)interruput) : CLEAR_BIT(AES->CR, (u32)interruput);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified AES flag is set or not.
/// @param flag: Specifies the flag to check.
///   This parameter can be one of the following values:
/// @arg AES_FLAG_CCF :Calculate complete flag
/// @arg AES_FLAG_RDERR :Read error flag
/// @arg AES_FLAG_WRERR :Write error flag
/// @retval  The new state of AES_FLAG (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
FlagStatus AES_GetFlagStatus(AES_Flag_TypeDef flag)
{
    return ((AES->SR & flag) != (u32)RESET) ? SET : RESET;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the AES state flag.
/// @param flag: specifies the AES flag to clear.
///   This parameter can be one of the following values:
/// @arg AES_FLAG_CCF :Calculate complete flag
/// @arg AES_FLAG_RDERR :Read error flag
/// @arg AES_FLAG_WRERR :Write error flag
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_ClearFlag(AES_Flag_TypeDef flag)
{
    (flag == AES_FLAG_CCF) ? SET_BIT(AES->CR, (u32)AES_CR_CCFC) : SET_BIT(AES->CR, (u32)AES_CR_ERRC);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Checks whether the specified AES interrupt has occurred or not.
/// @param interruput: Specifies the AES interrupt source to check.
///   This parameter can be one of the following values:
/// @arg AES_IT_CC:  Error interrupt
/// @arg AES_IT_ERR: Calculate complete interrupt
/// @retval  The new state of AES_IT (SET or RESET).
////////////////////////////////////////////////////////////////////////////////
ITStatus AES_GetITStatus(AES_Interrupt_TypeDef interruput)
{
    ITStatus itstatus      = RESET;
    u32      cciebitstatus = RESET, ccfbitstatus = RESET;

    cciebitstatus = AES->CR & AES_CR_CCIE;
    ccfbitstatus  = AES->SR & AES_SR_CCF;

    if (interruput == AES_IT_CC) {
        itstatus = (cciebitstatus && ccfbitstatus) ? SET : RESET;
    }
    else {
        if ((AES->CR & AES_CR_ERRIE) != RESET) {
            itstatus = (AES->SR & (u32)(AES_SR_WRERR | AES_SR_RDERR)) ? SET : RESET;
        }
        else {
            itstatus = RESET;
        }
    }

    return itstatus;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Clears the AES interrupt pending bits.
/// @param interruput: Specifies the interrupt pending bit to clear.
///   This parameter can be one of the following values:
/// @arg AES_IT_CC:  Error interrupt
/// @arg AES_IT_ERR: Calculate complete interrupt
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AES_ClearITPendingBit(AES_Interrupt_TypeDef interruput)
{
    SET_BIT(AES->CR, ((u32)interruput >> 2));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief AES ECB chaining mode encrypt function.
/// @param pKey: Encrypt key
/// @param pBuffer: Input and output data pointer
/// @retval  Encrypt result
///   This parameter can be ERROR or SUCCESS
////////////////////////////////////////////////////////////////////////////////
bool AES_ECB_Encrypt(u8* pKey, u8* pBuffer)
{
    AES_SetMode(AES_Operation_Encryp, AES_Chaining_ECB);  // ECB Encrypt MODE
    LET_AES_KEY(pKey);
    return AES_GetStatus(true, pBuffer);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES ECB chaining mode decrypt function.
/// @param pKey: Decrypt key pointer
/// @param pBuffer: Input and output data pointer
/// @retval  Decrypt result
///   This parameter can be ERROR or SUCCESS
////////////////////////////////////////////////////////////////////////////////
bool AES_ECB_Decrypt(u8* pKey, u8* pBuffer)
{
/*    AES_SetMode(AES_Operation_KeyDeriv, AES_Chaining_ECB);  // Key expasion MODE
    LET_AES_KEY(pKey);
//    AES_GetStatus(false, pBuffer);
    AES_SetMode(AES_Operation_Decryp, AES_Chaining_ECB);  // ECB Decrypt MODE
    return AES_GetStatus(true, pBuffer);
*/
//    AES_SetMode(AES_Operation_KeyDeriv, AES_Chaining_ECB);  // Key expasion MODE
//    LET_AES_KEY(pKey);
//    AES_GetStatus(false, pBuffer);
    AES_SetMode(AES_Operation_Decryp, AES_Chaining_ECB);  // ECB Decrypt MODE
    LET_AES_KEY(pKey);
    return AES_GetStatus(true, pBuffer);



}

////////////////////////////////////////////////////////////////////////////////
/// @brief AES CBC chaining mode encrypt function.
/// @param pKey: Encrypt key pointer
/// @param pInitVectors: initialization vector array pointer
/// @param pBuffer: Input and output data pointer
/// @retval  Encrypt result
///   This parameter can be ERROR or SUCCESS
////////////////////////////////////////////////////////////////////////////////
bool AES_CBC_Encrypt(u8* pKey, u8* pInitVectors, u8* pBuffer)
{
    AES_SetMode(AES_Operation_Encryp, AES_Chaining_CBC);  // CBC Encrypt MODE
    LET_AES_KEY(pKey);
    LET_AES_IVR((u32*)pInitVectors);
    return AES_GetStatus(true, pBuffer);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES CBC chaining mode decrypt function.
/// @param pKey: Decrypt key
/// @param pInitVectors: initialization vector array
/// @param pBuffer: Input and output data pointer
/// @retval  Decrypt result
///   This parameter can be ERROR or SUCCESS
////////////////////////////////////////////////////////////////////////////////
bool AES_CBC_Decrypt(u8* pKey, u8* pInitVectors, u8* pBuffer)
{
    AES_SetMode(AES_Operation_KeyDeriv, AES_Chaining_CBC);  // Key expasion MODE
    LET_AES_KEY(pKey);
    AES_GetStatus(false, pBuffer);
    AES_SetMode(AES_Operation_Decryp, AES_Chaining_CBC);  // CBC Decrypt MODE
    LET_AES_IVR((u32*)pInitVectors);
    return AES_GetStatus(true, pBuffer);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief AES CTR chaining mode encrypt function.
/// @param pKey: Encrypt key
/// @param pInitVectors: initialization vector array
/// @param pBuffer: Input and output data pointer
/// @retval  Encrypt result
///   This parameter can be ERROR or SUCCESS
////////////////////////////////////////////////////////////////////////////////
bool AES_CTR_Encrypt(u8* pKey, u8* pInitVectors, u8* pBuffer)
{
    AES_SetMode(AES_Operation_Encryp, AES_Chaining_CTR);  // CTR Encrypt MODE
    LET_AES_KEY(pKey);
    LET_AES_IVR((u32*)pInitVectors);
    return AES_GetStatus(true, pBuffer);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES CTR chaining mode decrypt function.
/// @param pKey: Decrypt key
/// @param pInitVectors: initialization vector array
/// @param pBuffer: Input and output data pointer
/// @retval  Decrypt result
///   This parameter can be ERROR or SUCCESS
////////////////////////////////////////////////////////////////////////////////
bool AES_CTR_Decrypt(u8* pKey, u8* pInitVectors, u8* pBuffer)
{
    AES_SetMode(AES_Operation_KeyDeriv, AES_Chaining_CTR);  // Key expasion MODE
    LET_AES_KEY(pKey);
    AES_GetStatus(false, pBuffer);
    AES_SetMode(AES_Operation_Decryp, AES_Chaining_CTR);  // ECB Decrypt MODE
    LET_AES_IVR((u32*)pInitVectors);
    return AES_GetStatus(true, pBuffer);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief AES CFB chaining mode encrypt function.
/// @param Key: Encrypt key
/// @param pInitVectors: initialization vector array
/// @param pBuffer: Input and output  data pointer
/// @retval  Encrypt result
///   This parameter can be ERROR or SUCCESS
////////////////////////////////////////////////////////////////////////////////
bool AES_CFB_Encrypt(u8* pKey, u8* pInitVectors, u8* pBuffer)
{
    AES_SetMode(AES_Operation_Encryp, AES_Chaining_CFB);  // CFB Encrypt MODE
    LET_AES_KEY(pKey);
    LET_AES_IVR((u32*)pInitVectors);
    return AES_GetStatus(true, pBuffer);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES CFB chaining mode decrypt function.
/// @param pKey: Decrypt key
/// @param pInitVectors: initialization vector array
/// @param pBuffer: Input and output data pointer
/// @retval  Decrypt result
///   This parameter can be ERROR or SUCCESS
////////////////////////////////////////////////////////////////////////////////
bool AES_CFB_Decrypt(u8* pKey, u8* pInitVectors, u8* pBuffer)
{
    AES_SetMode(AES_Operation_KeyDeriv, AES_Chaining_CFB);  // Key expasion MODE
    LET_AES_KEY(pKey);
    AES_GetStatus(false, pBuffer);
    AES_SetMode(AES_Operation_Decryp, AES_Chaining_CFB);  // CFB Decrypt MODE
    LET_AES_IVR((u32*)pInitVectors);
    return AES_GetStatus(true, pBuffer);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief AES OFB chaining mode encrypt function.
/// @param pKey: Encrypt key
/// @param pInitVectors: initialization vector array
/// @param pBuffer: Input and output data pointer
/// @retval  Encrypt result
///   This parameter can be ERROR or SUCCESS
////////////////////////////////////////////////////////////////////////////////
bool AES_OFB_Encrypt(u8* pKey, u8* pInitVectors, u8* pBuffer)
{
    AES_SetMode(AES_Operation_Encryp, AES_Chaining_OFB);  // OFB Encrypt MODE
    LET_AES_KEY(pKey);
    LET_AES_IVR((u32*)pInitVectors);
    return AES_GetStatus(true, pBuffer);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  AES OFB chaining mode decrypt function.
/// @param pKey: Decrypt key
/// @param pInitVectors: initialization vector array
/// @param pBuffer: Input and output data pointer
/// @retval  Decrypt result
///   This parameter can be ERROR or SUCCESS
////////////////////////////////////////////////////////////////////////////////
bool AES_OFB_Decrypt(u8* pKey, u8* pInitVectors, u8* pBuffer)
{
    AES_SetMode(AES_Operation_KeyDeriv, AES_Chaining_OFB);  // Key expasion MODE
    LET_AES_KEY(pKey);
    AES_GetStatus(false, pBuffer);
    AES_SetMode(AES_Operation_Decryp, AES_Chaining_OFB);  // OFB Decrypt MODE
    LET_AES_IVR((u32*)pInitVectors);
    return AES_GetStatus(true, pBuffer);
}

///@}

///@}

///@}

#endif
