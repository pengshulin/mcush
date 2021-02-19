/*
 * @brief This file contains USB DFU functions for composite device.
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
#include "app_usbd_cfg.h"
#include "ms_timer.h"
#include "dfu.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/**
 * @brief Structure to hold DFU control data
 */
typedef struct {
	USBD_HANDLE_T hUsb;				/*!< Handle to USB stack. */
	volatile uint32_t fDetach;		/*!< Flag indicating DFU_DETACH request is received. */
	volatile uint32_t fDownloadDone;/*!< Flag indicating DFU_DOWNLOAD finished. */
} DFU_Ctrl_T;

/** Singleton instance of DFU control */
static DFU_Ctrl_T g_dfu;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Rewrite USB descriptors so that DFU is the only interface. */
static void dfu_detach(USBD_HANDLE_T hUsb)
{
	USB_CORE_CTRL_T *pCtrl = (USB_CORE_CTRL_T *) hUsb;
	USB_CONFIGURATION_DESCRIPTOR *pD = (USB_CONFIGURATION_DESCRIPTOR *) &DFU_ConfigDescriptor[0];

	/* update configuration descriptors to have only DFU interface before
	 * reconnecting to host.
	 */
	memcpy(pCtrl->full_speed_desc, (void *) pD, pD->wTotalLength);
	memcpy(pCtrl->high_speed_desc, (void *) pD, pD->wTotalLength);

	/* Signal DFU user task that detach command is received. */
	g_dfu.fDetach = 1;
}

/* Set a flag when a DFU firmware reload has finished. */
static void dfu_done(void)
{
	/* Signal DFU user task that DFU download has finished. */
	g_dfu.fDownloadDone = 1;
}

/* Execute the new image. */
static void start_image(uint32_t *image_ptr)
{
	/* disconnect and load the new image */
	USBD_API->hw->Connect(g_dfu.hUsb, 0);

	/* disable all IRQs enabled in this example */
	NVIC_DisableIRQ(LPC_USB_IRQ);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

	/* get and set the stack pointer of the new image */
	__set_MSP(*image_ptr++);
	/* jump to new image's execution area */
	((void (*)(void)) * image_ptr)();
}

/* DFU read callback is called during DFU_UPLOAD state. In this example
 * we will return the data present at DFU_DEST_BASE memory.
 */
static uint32_t dfu_rd(uint32_t block_num, uint8_t * *pBuff, uint32_t length)
{
	uint32_t src_addr = DFU_DEST_BASE;

	if (block_num == DFU_MAX_BLOCKS) {
		return 0;
	}

	if (block_num > DFU_MAX_BLOCKS) {
		return DFU_STATUS_errADDRESS;
	}

	src_addr += (block_num * DFU_XFER_BLOCK_SZ);
	*pBuff = (uint8_t *) src_addr;

	return length;
}

/* DFU write callback is called during DFU_DOWNLOAD state. In this example
 * we will write the data to DFU_DEST_BASE memory area.
 */
uint8_t dfu_wr(uint32_t block_num, uint8_t * *pBuff, uint32_t length, uint8_t *bwPollTimeout)
{
	uint32_t dest_addr = DFU_DEST_BASE;

	if (block_num >= DFU_MAX_BLOCKS) {
		return DFU_STATUS_errADDRESS;
	}

	dest_addr += (block_num * DFU_XFER_BLOCK_SZ);
	dest_addr += length;
	*pBuff = (uint8_t *) dest_addr;

	return DFU_STATUS_OK;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* DFU interface init routine.*/
ErrorCode_t DFU_init(USBD_HANDLE_T hUsb,
					 USB_INTERFACE_DESCRIPTOR *pIntfDesc,
					 uint32_t *mem_base,
					 uint32_t *mem_size)
{
	USBD_DFU_INIT_PARAM_T dfu_param;
	ErrorCode_t ret = LPC_OK;

	/* store stack handle */
	g_dfu.hUsb = hUsb;

	/* Init DFU paramas */
	memset((void *) &dfu_param, 0, sizeof(USBD_DFU_INIT_PARAM_T));
	dfu_param.mem_base = *mem_base;
	dfu_param.mem_size = *mem_size;
	dfu_param.wTransferSize = USB_DFU_XFER_SIZE;

	/* check if interface descriptor pointer is pointing to right interface */
	if ((pIntfDesc == 0) ||
		(pIntfDesc->bInterfaceClass != USB_DEVICE_CLASS_APP) ||
		(pIntfDesc->bInterfaceSubClass != USB_DFU_SUBCLASS) ) {
		return ERR_FAILED;
	}

	dfu_param.intf_desc = (uint8_t *) pIntfDesc;
	/* user defined functions */
	dfu_param.DFU_Write = dfu_wr;
	dfu_param.DFU_Read = dfu_rd;
	dfu_param.DFU_Done = dfu_done;
	dfu_param.DFU_Detach = dfu_detach;

	ret = USBD_API->dfu->init(hUsb, &dfu_param, DFU_STATE_appIDLE);

	/* update memory variables */
	*mem_base = dfu_param.mem_base;
	*mem_size = dfu_param.mem_size;
	return ret;
}

/* DFU tasks */
void DFU_Tasks(void)
{
	/* check if we received DFU_DETACH command from host. */
	if (g_dfu.fDetach) {

		/* disconnect */
		USBD_API->hw->Connect(g_dfu.hUsb, 0);

		/* wait for 10 msec before reconnecting */
		ms_timerDelay(10);

		/* connect the device back */
		USBD_API->hw->Connect(g_dfu.hUsb, 1);
		/* reset detach signal */
		g_dfu.fDetach = 0;
	}

	/* check if DFU_DOWNLOAD finished. Note, the following code is needed
	 * only if the host application is not able to send USB_REST event after
	 * DFU_DOWNLOAD. On Windows systems the dfu-util uses WinUSB driver which
	 * doesn't permit user applications to issue USB_RESET event on bus.
	 */
	if (g_dfu.fDownloadDone) {

		/* wait for 1 sec before executing the new image. */
		ms_timerDelay(1000);
		/* execute new image */
		start_image((void *) DFU_DEST_BASE);
	}
}

/* DFU interfaces USB_RESET event handler.. */
ErrorCode_t DFU_reset_handler(USBD_HANDLE_T hUsb)
{
	if (g_dfu.fDownloadDone) {
		start_image((void *) DFU_DEST_BASE);
	}

	return LPC_OK;
}






