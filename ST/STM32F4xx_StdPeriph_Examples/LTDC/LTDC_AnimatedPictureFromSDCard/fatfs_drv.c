/**
  ******************************************************************************
  * @file    LTDC/LTDC_AnimatedPictureFromSDCard/fatfs_drv.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   diskio interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "diskio.h"
#include "stm324x9i_eval_sdio_sd.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/                
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
   * @brief  Initialize Disk Drive  
   * @param   drv : driver index
   * @retval DSTATUS : operation status
  */

DSTATUS disk_initialize ( BYTE drv )
{
  
  NVIC_InitTypeDef NVIC_InitStructure;  
  Stat = STA_NOINIT;
  
  if (drv == 0)
  {
    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    if( SD_Init() == 0)
    {
      Stat &= ~STA_NOINIT;
    }     
  }
  
  return Stat;
}

/**
   * @brief  Get Disk Status  
   * @param   drv : driver index
   * @retval DSTATUS : operation status
*/
DSTATUS disk_status ( BYTE drv	)
{  
  Stat = STA_NOINIT;
  
  if ((drv == 0) && (SD_GetStatus() == 0))
  {
    Stat &= ~STA_NOINIT;
  }
  
  return Stat;  
}

/**
   * @brief  Read Sector(s) 
   * @param   drv : driver index
   * @retval DSTATUS : operation status
  */
DRESULT disk_read (
                   BYTE drv,			  /* Physical drive number (0) */
                   BYTE *buff,			/* Pointer to the data buffer to store read data */
                   DWORD sector,		/* Start sector number (LBA) */
                   BYTE count			  /* Sector count (1..255) */
                     )
{
  
  SD_Error sdstatus = SD_OK;
 
  if (drv == 0)
  {
    SD_ReadMultiBlocks(buff, sector << 9, 512, count);
    
    /* Check if the Transfer is finished */
    sdstatus =  SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
    
    if (sdstatus == SD_OK)
    {
      return RES_OK;
    }
  }
  return RES_ERROR;
}
/**
   * @brief  write Sector(s) 
   * @param   drv : driver index
   * @retval DSTATUS : operation status
  */

#if _READONLY == 0
DRESULT disk_write (
                    BYTE drv,			    /* Physical drive number (0) */
                    const BYTE *buff,	/* Pointer to the data to be written */
                    DWORD sector,		  /* Start sector number (LBA) */
                    BYTE count			  /* Sector count (1..255) */
                      )
{
  
  SD_Error sdstatus = SD_OK;
  
  if (drv == 0)
  {
    SD_WriteMultiBlocks((BYTE *)buff, sector << 9, 512, count);
    /* Check if the Transfer is finished */
    sdstatus = SD_WaitWriteOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);     
    
    if (sdstatus == SD_OK)
    {
      return RES_OK;
    }
  }
  return RES_ERROR;
}
#endif /* _READONLY == 0 */


/**
   * @brief  I/O control operation
   * @param   drv : driver index
   * @retval DSTATUS : operation status
  */


#if _USE_IOCTL != 0
DRESULT disk_ioctl (
                    BYTE drv,		  /* Physical drive number (0) */
                    BYTE ctrl,		/* Control code */
                    void *buff		/* Buffer to send/receive control data */
                      )
{
  DRESULT res;
  SD_CardInfo SDCardInfo;    
  
  if (drv) return RES_PARERR;
  
  res = RES_ERROR;
  
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  switch (ctrl) {
  case CTRL_SYNC :		/* Make sure that no pending write process */
    
    res = RES_OK;
    break;
    
  case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
    if(drv == 0)
    {
      SD_GetCardInfo(&SDCardInfo);  
      *(DWORD*)buff = SDCardInfo.CardCapacity / 512; 
    }
    
    res = RES_OK;
    break;
    
  case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
    *(WORD*)buff = 512;
    res = RES_OK;
    break;
    
  case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
    if(drv == 0)
      *(DWORD*)buff = 512;
    else
      *(DWORD*)buff = 32;
    
    
    break;
    
    
  default:
    res = RES_PARERR;
  }
  
  
  
  return res;
}
#endif /* _USE_IOCTL != 0 */
/**
   * @brief  Get Time from RTC
   * @param   None
   * @retval Time in DWORD
  */

DWORD get_fattime (void)
{
  return 0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
