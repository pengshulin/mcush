/*
 * @brief File contains callback to MSC driver backed by a memory disk.
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include <string.h>
#include "board.h"
#include "app_usbd_cfg.h"
#include "msc_disk.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/* Buffer size for Mass Storage data */
/* Note that the RW speeds are reduced with smaller buffer size */
#define MSC_BUFF_SIZE (16 * 1024)

/*Mass Storage Control Structure for older ROM stack i.e version 0x01111101 */
typedef struct OLD_MSC_CTRL_T
{
  /* If it's a USB HS, the max packet is 512, if it's USB FS,
  the max packet is 64. Use 512 for both HS and FS. */
  /*ALIGNED(4)*/ uint8_t  BulkBuf[USB_HS_MAX_BULK_PACKET]; /* Bulk In/Out Buffer */
  /*ALIGNED(4)*/MSC_CBW CBW;                   /* Command Block Wrapper */
  /*ALIGNED(4)*/MSC_CSW CSW;                   /* Command Status Wrapper */

  USB_CORE_CTRL_T*  pUsbCtrl;

  uint32_t Offset;                  /* R/W Offset */
  uint32_t Length;                  /* R/W Length */
  uint32_t BulkLen;                 /* Bulk In/Out Length */
  uint8_t* rx_buf;

  uint8_t BulkStage;               /* Bulk Stage */
  uint8_t if_num;                  /* interface number */
  uint8_t epin_num;                /* BULK IN endpoint number */
  uint8_t epout_num;               /* BULK OUT endpoint number */
  uint32_t MemOK;                  /* Memory OK */

  uint8_t*  InquiryStr;
  uint32_t  BlockCount;
  uint32_t  BlockSize;
  uint32_t  MemorySize;
  /* user defined functions */
  void (*MSC_Write)( uint32_t offset, uint8_t** src, uint32_t length);
  void (*MSC_Read)( uint32_t offset, uint8_t** dst, uint32_t length);
  ErrorCode_t (*MSC_Verify)( uint32_t offset, uint8_t src[], uint32_t length);
  /* optional call back for MSC_Write optimization */
  void (*MSC_GetWriteBuf)( uint32_t offset, uint8_t** buff_adr, uint32_t length);

}USB_OLD_MSC_CTRL_T;

static volatile int32_t sdio_wait_exit = 0;
/* SDMMC card info structure */
mci_card_struct sdCardInfo;
volatile uint32_t timerCntms = 0; /* Free running milli second timer */

static USB_EP_HANDLER_T	default_bulk_out_hdlr;
static const uint8_t g_InquiryStr[] = {'N', 'X', 'P', ' ', ' ', ' ', ' ', ' ',	   \
									   'L', 'P', 'C', ' ', 'M', 'e', 'm', ' ',	   \
									   'D', 'i', 's', 'k', ' ', ' ', ' ', ' ',	   \
									   '1', '.', '0', ' ', };
static void	*g_pMscCtrl;
static uint32_t	wrBuffIndex = 0;
static uint32_t	startSector;
static uint32_t	xfer_buff_len;
static uint32_t	total_xfer_len;
static uint8_t	update_xfer_len;

/* In GCC Place it in RamLoc40 */
#ifdef __GNUC__
__attribute__((section(".bss.$RamLoc40")))
#endif
/* Buffer for Mass Storage data */
ALIGNED(512) uint8_t msc_sd_buf[MSC_BUFF_SIZE];

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Delay callback for timed SDIF/SDMMC functions */
STATIC void waitMs(uint32_t time)
{
	uint32_t init = timerCntms;

	/* In an RTOS, the thread would sleep allowing other threads to run.
	   For standalone operation, we just spin on a timer */
	while (timerCntms < init + time) {}
}

/**
 * @brief	Sets up the event driven wakeup
 * @param	pEvent : Event information
 * @return	Nothing
 */
STATIC void setupEvWakeup(void *bits)
{
	uint32_t bit_mask = *((uint32_t *)bits);
	/* Wait for IRQ - for an RTOS, you would pend on an event here with a IRQ based wakeup. */
	NVIC_ClearPendingIRQ(SDIO_IRQn);
	sdio_wait_exit = 0;
	Chip_SDIF_SetIntMask(LPC_SDMMC, bit_mask);
	NVIC_EnableIRQ(SDIO_IRQn);
}

/**
 * @brief	A better wait callback for SDMMC driven by the IRQ flag
 * @return	0 on success, or failure condition (Nonzero)
 */
STATIC uint32_t waitEvIRQDriven(void)
{
	uint32_t status;

	/* Wait for event, would be nice to have a timeout, but keep it  simple */
	while (sdio_wait_exit == 0) {}

	/* Get status and clear interrupts */
	status = Chip_SDIF_GetIntStatus(LPC_SDMMC);
	Chip_SDIF_ClrIntStatus(LPC_SDMMC, status);
	Chip_SDIF_SetIntMask(LPC_SDMMC, 0);

	return status;
}

/* Initialize the Timer at 1us */
STATIC void initAppTimer(void)
{
	/* Setup Systick to tick every 1 milliseconds */
	SysTick_Config(SystemCoreClock / 1000);
}

/* Initialize SD/MMC */
STATIC void initAppSDMMC()
{
	memset(&sdCardInfo, 0, sizeof(sdCardInfo));
	sdCardInfo.card_info.evsetup_cb = setupEvWakeup;
	sdCardInfo.card_info.waitfunc_cb = waitEvIRQDriven;
	sdCardInfo.card_info.msdelay_func = waitMs;

	/*  SD/MMC initialization */
	Board_SDMMC_Init();

	/* The SDIO driver needs to know the SDIO clock rate */
	Chip_SDIF_Init(LPC_SDMMC);
}

/* USB device mass storage class read callback routine */
static void translate_rd(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset)
{
	if(USBD_API->version == 0x01111101) {
		/* No high offset for older stack */
		hi_offset = 0;
	}
	/* If a new RW request has started then copy total transfer length from control structure
		 and read a new block of data from SD card */
	if(update_xfer_len) {
		/* Start sector for the read request is updated */
		startSector = (((uint64_t) offset) | (((uint64_t) hi_offset) << 32))/MMC_SECTOR_SIZE;
		if(USBD_API->version > 0x01111101) {
			/* New ROM stack version, use new control structure */
			total_xfer_len = ((USB_MSC_CTRL_T *)g_pMscCtrl)->Length;
		}
		else {
			/* Old ROM stack version, use old control structure */
			total_xfer_len = ((USB_OLD_MSC_CTRL_T *)g_pMscCtrl)->Length;
		}
		/* Transfer size from SD card is the minimum of buffer size and total transfer length */
		if(total_xfer_len > MSC_BUFF_SIZE) {
			xfer_buff_len = MSC_BUFF_SIZE;
		}
		else {
			xfer_buff_len = total_xfer_len;
		}
		update_xfer_len = 0;
		/* Read from SD card */
		Chip_SDMMC_ReadBlocks(LPC_SDMMC, &msc_sd_buf[0], startSector, (xfer_buff_len + MMC_SECTOR_SIZE - 1)/MMC_SECTOR_SIZE);
	}
	/* For a previous request when the buffered data is read out completely then refill the buffer with next set of data */
	else if(((((uint64_t) offset) | (((uint64_t) hi_offset) << 32)) - (startSector * MMC_SECTOR_SIZE)) == xfer_buff_len) {
		/* Update the start sector, total transfer length and data length to be read from SD card */
		startSector += (xfer_buff_len + MMC_SECTOR_SIZE - 1)/MMC_SECTOR_SIZE;
		total_xfer_len -= xfer_buff_len;
		if(total_xfer_len > MSC_BUFF_SIZE) {
			xfer_buff_len = MSC_BUFF_SIZE;
		}
		else {
			xfer_buff_len = total_xfer_len;
		}
		/* Read next set of data from SD card */
		Chip_SDMMC_ReadBlocks(LPC_SDMMC, &msc_sd_buf[0], startSector, (xfer_buff_len + MMC_SECTOR_SIZE - 1)/MMC_SECTOR_SIZE);
	}
	/* Data pointer to the next USB bulk IN packet */
	*buff_adr =  &msc_sd_buf[(((uint64_t) offset) | (((uint64_t) hi_offset) << 32)) - (startSector * MMC_SECTOR_SIZE)];
}

/* USB device mass storage class write callback routine */
static void translate_wr(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset)
{
	if(USBD_API->version == 0x01111101) {
		/* No high offset for older stack */
		hi_offset = 0;
	}
	/* If a new write request has started then copy total transfer length from control structure
		 and calculate the data block size for SD write */
	if(update_xfer_len) {
		/* Start sector for the write request is updated */
		startSector = (((uint64_t) offset) | (((uint64_t) hi_offset) << 32))/MMC_SECTOR_SIZE;
		if(USBD_API->version > 0x01111101) {
			/* New ROM stack version, use new control structure */
			total_xfer_len = ((USB_MSC_CTRL_T *)g_pMscCtrl)->Length;
		}
		else {
			/* Old ROM stack version, use old control structure */
			total_xfer_len = ((USB_OLD_MSC_CTRL_T *)g_pMscCtrl)->Length;
		}
		/* Transfer size from SD card is the minimum of buffer size and total transfer length */
		if(total_xfer_len > MSC_BUFF_SIZE) {
			xfer_buff_len = MSC_BUFF_SIZE;
		}
		else {
			xfer_buff_len = total_xfer_len;
		}
		wrBuffIndex = 0;
		update_xfer_len = 0;
	}
	/* Increment the index for the buffer */
	wrBuffIndex += length;
	/* When entire buffer is written, write the data block to the SD card */
	if(wrBuffIndex == xfer_buff_len) {
		Chip_SDMMC_WriteBlocks(LPC_SDMMC, &msc_sd_buf[0], startSector, (xfer_buff_len + MMC_SECTOR_SIZE - 1)/MMC_SECTOR_SIZE);
		/* Reset index*/
		wrBuffIndex = 0;
		/* Update the start sector, total transfer length and data block size for SD write */
		startSector += (xfer_buff_len + MMC_SECTOR_SIZE - 1)/MMC_SECTOR_SIZE;
		total_xfer_len -= xfer_buff_len;
		if(total_xfer_len > MSC_BUFF_SIZE) {
			xfer_buff_len = MSC_BUFF_SIZE;
		}
		else {
			xfer_buff_len = total_xfer_len;
		}
	}
	/* Data pointer to the next USB bulk OUT packet */
	*buff_adr =  &msc_sd_buf[wrBuffIndex];
}

/* USB device mass storage class get write buffer callback routine */
static void translate_GetWrBuf(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset)
{
	*buff_adr =  &msc_sd_buf[0];
}

/* USB device mass storage class verify callback routine */
static ErrorCode_t translate_verify(uint32_t offset, uint8_t *src, uint32_t length, uint32_t hi_offset)
{
	if(USBD_API->version == 0x01111101) {
		/* No high offset for older stack */
		hi_offset = 0;
	}
	/* If a new RW request has started then copy total transfer length from control structure
		 and read a new block of data from SD card */
	if(update_xfer_len) {
		/* Start sector for the read request is updated */
		startSector = (((uint64_t) offset) | (((uint64_t) hi_offset) << 32))/MMC_SECTOR_SIZE;
		if(USBD_API->version > 0x01111101) {
			/* New ROM stack version, use new control structure */
			total_xfer_len = ((USB_MSC_CTRL_T *)g_pMscCtrl)->Length;
		}
		else {
			/* Old ROM stack version, use old control structure */
			total_xfer_len = ((USB_OLD_MSC_CTRL_T *)g_pMscCtrl)->Length;
		}
		/* Transfer size from SD card is the minimum of buffer size and total transfer length */
		if(total_xfer_len > MSC_BUFF_SIZE) {
			xfer_buff_len = MSC_BUFF_SIZE;
		}
		else {
			xfer_buff_len = total_xfer_len;
		}
		update_xfer_len = 0;
		/* Read from SD card */
		Chip_SDMMC_ReadBlocks(LPC_SDMMC, &msc_sd_buf[0], startSector, (xfer_buff_len + MMC_SECTOR_SIZE - 1)/MMC_SECTOR_SIZE);
	}
	/* For a previous request when the buffered data is read out completely then refill the buffer with next set of data */
	else if(((((uint64_t) offset) | (((uint64_t) hi_offset) << 32)) - (startSector * MMC_SECTOR_SIZE)) == xfer_buff_len) {
		/* Update the start sector, total transfer length and data length to be read from SD card */
		startSector += (xfer_buff_len + MMC_SECTOR_SIZE - 1)/MMC_SECTOR_SIZE;
		total_xfer_len -= xfer_buff_len;
		if(total_xfer_len > MSC_BUFF_SIZE) {
			xfer_buff_len = MSC_BUFF_SIZE;
		}
		else {
			xfer_buff_len = total_xfer_len;
		}
		/* Read next set of data from SD card */
		Chip_SDMMC_ReadBlocks(LPC_SDMMC, &msc_sd_buf[0], startSector, (xfer_buff_len + MMC_SECTOR_SIZE - 1)/MMC_SECTOR_SIZE);
	}
	/* Compare data return accordingly*/
	if (memcmp((void *) &msc_sd_buf[(((uint64_t) offset) | (((uint64_t) hi_offset) << 32)) - (startSector * MMC_SECTOR_SIZE)], src, length)) {
		return ERR_FAILED;
	}

	return LPC_OK;
}

/* Override for Bulk out handler */
static ErrorCode_t app_bulk_out_hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event)
{
	void *pMscCtrl = data;
	if(USBD_API->version > 0x01111101) {
		/* If a new RW request is received then set flag for updating total transfer length */
		if( (event == USB_EVT_OUT) && (((USB_MSC_CTRL_T*)pMscCtrl)->BulkStage == MSC_BS_CBW) ) {
			update_xfer_len = 1;
		}
	}
	else {
		/* If a new RW request is received then set flag for updating total transfer length */
		if( (event == USB_EVT_OUT) && (((USB_OLD_MSC_CTRL_T*)pMscCtrl)->BulkStage == MSC_BS_CBW) ) {
			update_xfer_len = 1;
		}
	}
	/* Call the default handler */
  return default_bulk_out_hdlr(hUsb, data, event);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	System tick interrupt handler
 * @return	Nothing
 */
void SysTick_Handler(void)
{
	timerCntms++;
}

/**
 * @brief	SDC interrupt handler sub-routine
 * @return	Nothing
 */
void SDIO_IRQHandler(void)
{
	/* All SD based register handling is done in the callback
	   function. The SDIO interrupt is not enabled as part of this
	   driver and needs to be enabled/disabled in the callbacks or
	   application as needed. This is to allow flexibility with IRQ
	   handling for applicaitons and RTOSes. */
	/* Set wait exit flag to tell wait function we are ready. In an RTOS,
	   this would trigger wakeup of a thread waiting for the IRQ. */
	NVIC_DisableIRQ(SDIO_IRQn);
	sdio_wait_exit = 1;
}

/* Initialize Disk Drive */
void card_init(void)
{
	uint32_t i;

	initAppSDMMC();

	/* Initialize Systick Timer */
	initAppTimer();

	/* Enable SD interrupt */
	NVIC_EnableIRQ(SDIO_IRQn);

#if (!defined(BOARD_NGX_XPLORER_4330) && !defined(BOARD_NGX_XPLORER_1830))
	/* Wait for card to be inserted */
	while (Chip_SDIF_CardNDetect(LPC_SDMMC)) {}
#endif

	/* Enumerate the card once detected. Note this function may block for a little while. */
	while(1) {
		if(!Chip_SDMMC_Acquire(LPC_SDMMC, &sdCardInfo)) {
			DEBUGOUT("Card Acquire failed...\r\n");
			for( i = 0; i < 10000; i++);
			continue;
		}
		break;
	}
}

/* Memory storage based MSC_Disk init routine */
ErrorCode_t mscDisk_init(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T *pDesc, USBD_API_INIT_PARAM_T *pUsbParam)
{
	USBD_MSC_INIT_PARAM_T msc_param;
	ErrorCode_t ret = LPC_OK;
	USB_CORE_CTRL_T* pCtrl = (USB_CORE_CTRL_T*)hUsb;

	memset((void *) &msc_param, 0, sizeof(USBD_MSC_INIT_PARAM_T));
	msc_param.mem_base = pUsbParam->mem_base;
	msc_param.mem_size = pUsbParam->mem_size;
	g_pMscCtrl = (void *)msc_param.mem_base;
	/* mass storage paramas */
	msc_param.InquiryStr = (uint8_t *) g_InquiryStr;
	msc_param.BlockCount = sdCardInfo.card_info.blocknr;
	msc_param.BlockSize = MMC_SECTOR_SIZE;
	/*Update memory size based on the stack version */
	if(USBD_API->version > 0x01111101) {
		/* New ROM stack version */
		msc_param.MemorySize64 = sdCardInfo.card_info.device_size;
		msc_param.MemorySize = 0;
	}
	else {
		/* Old ROM stack version - cannot support more than 4GB of memory card */
		msc_param.MemorySize = sdCardInfo.card_info.device_size;
	}
	/* Install memory storage callback routines */
	msc_param.MSC_Write = translate_wr;
	msc_param.MSC_Read = translate_rd;
	msc_param.MSC_Verify = translate_verify;
	msc_param.MSC_GetWriteBuf = translate_GetWrBuf;
	msc_param.intf_desc = (uint8_t *) find_IntfDesc(pDesc->high_speed_desc, USB_DEVICE_CLASS_STORAGE);

	ret = USBD_API->msc->init(hUsb, &msc_param);

	update_xfer_len = 0;
	if(USBD_API->version > 0x01111101) {
		/* New ROM stack version */
		default_bulk_out_hdlr = pCtrl->ep_event_hdlr[(((USB_MSC_CTRL_T *)g_pMscCtrl)->epout_num & 0x0F) << 1];
		USBD_API->core->RegisterEpHandler(hUsb, (((USB_MSC_CTRL_T *)g_pMscCtrl)->epout_num & 0x0F) << 1, app_bulk_out_hdlr, g_pMscCtrl);
	}
	else {
		/* Old ROM stack version */
		default_bulk_out_hdlr = pCtrl->ep_event_hdlr[(((USB_OLD_MSC_CTRL_T *)g_pMscCtrl)->epout_num & 0x0F) << 1];
		USBD_API->core->RegisterEpHandler(hUsb, (((USB_OLD_MSC_CTRL_T *)g_pMscCtrl)->epout_num & 0x0F) << 1, app_bulk_out_hdlr, g_pMscCtrl);
	}
	/* update memory variables */
	pUsbParam->mem_base = msc_param.mem_base;
	pUsbParam->mem_size = msc_param.mem_size;

	return ret;
}






