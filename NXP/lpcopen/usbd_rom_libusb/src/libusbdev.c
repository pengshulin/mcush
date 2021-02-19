/*
 * @brief USB Band Width test and WCID examples for USB ROM API.
 *
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

#include "board.h"
#include <stdio.h>
#include <string.h>
#include "app_usbd_cfg.h"
#include "libusbdev.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Endpoint 0 patch that prevents nested NAK event processing */
static uint32_t g_ep0RxBusy = 0;/* flag indicating whether EP0 OUT/RX buffer is busy. */
static USB_EP_HANDLER_T g_Ep0BaseHdlr;	/* variable to store the pointer to base EP0 handler */

/**
 * Structure containing Virtual Comm port control data
 */
typedef struct _LUSB_CTRL_ {
	USBD_HANDLE_T hUsb;
	uint8_t *pRxBuf;
	uint32_t rxBuffLen;
	uint8_t *pTxBuf;
	uint32_t txBuffLen;
	uint32_t newStatus;
	uint32_t curStatus;
	volatile uint8_t connected;
} LUSB_CTRL_T;

LUSB_CTRL_T g_lusb;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
const USBD_API_T *g_pUsbApi;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Handle USB RESET event */
ErrorCode_t lusb_ResetEvent(USBD_HANDLE_T hUsb)
{
	/* reset the control structure */
	memset(&g_lusb, 0, sizeof(LUSB_CTRL_T));
	g_lusb.hUsb = hUsb;

	return LPC_OK;
}

/* USB bulk EP_IN endpoint handler */
ErrorCode_t lusb_BulkIN_Hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	LUSB_CTRL_T *pUSB = (LUSB_CTRL_T *) data;

	if (event == USB_EVT_IN) {
		pUSB->txBuffLen = 0;
		pUSB->pTxBuf = 0;
	}
	return LPC_OK;
}

/* USB bulk EP_IN endpoint handler */
ErrorCode_t lusb_IntrIN_Hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	LUSB_CTRL_T *pUSB = (LUSB_CTRL_T *) data;

	if (event == USB_EVT_IN) {
		/* check if we have new status to send */
		if (pUSB->newStatus) {
			/* swap and send status */
			pUSB->curStatus = pUSB->newStatus;
			pUSB->newStatus = 0;
			USBD_API->hw->WriteEP(pUSB->hUsb, LUSB_INT_EP, (uint8_t *) &pUSB->curStatus, sizeof(uint32_t));
		}
		else {
			/* nothing to send */
			pUSB->curStatus = 0;
		}
	}
	return LPC_OK;
}

/* USB bulk EP_OUT endpoint handler */
ErrorCode_t lusb_BulkOUT_Hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	LUSB_CTRL_T *pUSB = (LUSB_CTRL_T *) data;

	/* We received a transfer from the USB host. */
	if (event == USB_EVT_OUT) {
		pUSB->rxBuffLen = USBD_API->hw->ReadEP(hUsb, LUSB_OUT_EP, pUSB->pRxBuf);
		pUSB->pRxBuf = 0;
	}

	return LPC_OK;
}

/* Handler for WCID USB device requests. */
ErrorCode_t WCID_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	USB_CORE_CTRL_T *pCtrl = (USB_CORE_CTRL_T *) hUsb;
	ErrorCode_t ret = ERR_USBD_UNHANDLED;

	if (event == USB_EVT_SETUP) {
		switch (pCtrl->SetupPacket.bmRequestType.BM.Type) {
		case REQUEST_STANDARD:
			if ((pCtrl->SetupPacket.bmRequestType.BM.Recipient == REQUEST_TO_DEVICE) &&
				(pCtrl->SetupPacket.bRequest == USB_REQUEST_GET_DESCRIPTOR) &&
				(pCtrl->SetupPacket.wValue.WB.H == USB_STRING_DESCRIPTOR_TYPE) &&
				(pCtrl->SetupPacket.wValue.WB.L == 0x00EE)) {
				pCtrl->EP0Data.pData = (uint8_t *) WCID_String_Descriptor;
				pCtrl->EP0Data.Count = pCtrl->SetupPacket.wLength;
				USBD_API->core->DataInStage(pCtrl);
				ret = LPC_OK;
			}
			break;

		case REQUEST_VENDOR:
			switch (pCtrl->SetupPacket.bRequest) {
			case 0x0E:		/* libusbK benchmark test */
				pCtrl->EP0Buf[0] = 0x02;
				pCtrl->EP0Data.pData = (uint8_t *) &pCtrl->EP0Buf[0];
				pCtrl->EP0Data.Count = pCtrl->SetupPacket.wLength;
				USBD_API->core->DataInStage(pCtrl);
				ret = LPC_OK;
				break;

			case 0x11:		/* libusbK benchmark test */
				pCtrl->EP0Buf[0] = 'A';
				pCtrl->EP0Buf[1] = 'B';
				pCtrl->EP0Buf[2] = 'C';
				pCtrl->EP0Data.pData = (uint8_t *) &pCtrl->EP0Buf[0];
				pCtrl->EP0Data.Count = pCtrl->SetupPacket.wLength;
				USBD_API->core->DataInStage(pCtrl);
				ret = LPC_OK;
				break;

			case 0x10:		/* libusbK benchmark test */
				pCtrl->EP0Data.pData = pCtrl->EP0Buf;	/* data to be received */
				ret = LPC_OK;
				break;

			case LUSB_PID:
				switch (pCtrl->SetupPacket.bmRequestType.BM.Recipient) {
				case REQUEST_TO_DEVICE:
					if (pCtrl->SetupPacket.wIndex.W == 0x0004) {
						pCtrl->EP0Data.pData = (uint8_t *) WCID_CompatID_Descriptor;
						pCtrl->EP0Data.Count = pCtrl->SetupPacket.wLength;
						USBD_API->core->DataInStage(pCtrl);
						ret = LPC_OK;
					}
					break;
				}
				break;
			}
		}
	}
	else if ((event == USB_EVT_OUT) && (pCtrl->SetupPacket.bmRequestType.BM.Type == REQUEST_VENDOR)) {
		if (pCtrl->SetupPacket.bRequest == 0x10) {
			USBD_API->core->StatusInStage(pCtrl);
			ret = LPC_OK;
		}
	}

	return ret;
}

/* EP0_patch part of WORKAROUND for artf45032. */
static ErrorCode_t EP0_patch(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	switch (event) {
	case USB_EVT_OUT_NAK:
		if (g_ep0RxBusy) {
			/* we already queued the buffer so ignore this NAK event. */
			return LPC_OK;
		}
		else {
			/* Mark EP0_RX buffer as busy and allow base handler to queue the buffer. */
			g_ep0RxBusy = 1;
		}
		break;

	case USB_EVT_SETUP:	/* reset the flag when new setup sequence starts */
	case USB_EVT_OUT:
		/* we received the packet so clear the flag. */
		g_ep0RxBusy = 0;
		break;
	}
	return g_Ep0BaseHdlr(hUsb, data, event);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Handle interrupt from USB */
void USB_IRQHandler(void)
{
	USBD_API->hw->ISR(g_lusb.hUsb);
}

/* Initialize USB interface. */
int libusbdev_init(uint32_t memBase, uint32_t memSize)
{
	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	USB_CORE_CTRL_T *pCtrl;

	/* enable clocks and USB PHY/pads */
	USB_init_pin_clk();

	/* Init USB API structure */
	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;

	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	usb_param.usb_reg_base = LPC_USB_BASE;
	usb_param.max_num_ep = USB_MAX_EP_NUM;
	usb_param.mem_base = memBase;
	usb_param.mem_size = memSize;
	usb_param.USB_Reset_Event = lusb_ResetEvent;

	/* Set the USB descriptors */
	desc.device_desc = (uint8_t *) USB_DeviceDescriptor;
	desc.string_desc = (uint8_t *) USB_StringDescriptor;
#ifdef USE_USB0
	desc.high_speed_desc = USB_HsConfigDescriptor;
	desc.full_speed_desc = USB_FsConfigDescriptor;
	desc.device_qualifier = (uint8_t *) USB_DeviceQualifier;
#else
	/* Note, to pass USBCV test full-speed only devices should have both
	 * descriptor arrays point to same location and device_qualifier set
	 * to 0.
	 */
	desc.high_speed_desc = (uint8_t *) USB_FsConfigDescriptor;
	desc.full_speed_desc = (uint8_t *) USB_FsConfigDescriptor;
	desc.device_qualifier = 0;
#endif

	/* USB Initialization */
	ret = USBD_API->hw->Init(&g_lusb.hUsb, &desc, &usb_param);
	if (ret == LPC_OK) {

		/*	WORKAROUND for artf45032 ROM driver BUG:
		    Due to a race condition there is the chance that a second NAK event will
		    occur before the default endpoint0 handler has completed its preparation
		    of the DMA engine for the first NAK event. This can cause certain fields
		    in the DMA descriptors to be in an invalid state when the USB controller
		    reads them, thereby causing a hang.
		 */
		pCtrl = (USB_CORE_CTRL_T *) g_lusb.hUsb;/* convert the handle to control structure */
		g_Ep0BaseHdlr = pCtrl->ep_event_hdlr[0];/* retrieve the default EP0_OUT handler */
		pCtrl->ep_event_hdlr[0] = EP0_patch;/* set our patch routine as EP0_OUT handler */

		/* register WCID handler */
		ret = USBD_API->core->RegisterClassHandler(g_lusb.hUsb, WCID_hdlr, &g_lusb);
		if (ret == LPC_OK) {
			/* register EP2 interrupt handler */
			ret = USBD_API->core->RegisterEpHandler(g_lusb.hUsb, 2, lusb_BulkOUT_Hdlr, &g_lusb);
			if (ret == LPC_OK) {

				ret = USBD_API->core->RegisterEpHandler(g_lusb.hUsb, 3, lusb_BulkIN_Hdlr, &g_lusb);
				if (ret == LPC_OK) {
					ret = USBD_API->core->RegisterEpHandler(g_lusb.hUsb, 5, lusb_IntrIN_Hdlr, &g_lusb);
					if (ret == LPC_OK) {
						NVIC_EnableIRQ(LPC_USB_IRQ);/*  enable USB interrrupts */

						/* now connect */
						USBD_API->hw->Connect(g_lusb.hUsb, 1);
					}
				}
			}
		}
	}

	return ret;
}

/* Check if libusbdev is connected USB host application. */
bool libusbdev_Connected(void)
{
	return USB_IsConfigured(g_lusb.hUsb);
}

/* Queue the read buffer to USB DMA */
ErrorCode_t libusbdev_QueueReadReq(uint8_t *pBuf, uint32_t buf_len)
{
	LUSB_CTRL_T *pUSB = (LUSB_CTRL_T *) &g_lusb;
	ErrorCode_t ret = ERR_FAILED;

	/* Check if a read request is pending */
	if (pUSB->pRxBuf == 0) {
		/* Queue the read request */
		pUSB->pRxBuf = pBuf;
		pUSB->rxBuffLen = buf_len;
		USBD_API->hw->ReadReqEP(pUSB->hUsb, LUSB_OUT_EP, pBuf, buf_len);
		ret = LPC_OK;
	}

	return ret;
}

/* Check if queued read buffer got any data */
int32_t libusbdev_QueueReadDone(void)
{
	LUSB_CTRL_T *pUSB = (LUSB_CTRL_T *) &g_lusb;

	/* A read request is pending */
	if (pUSB->pRxBuf) {
		return -1;
	}
	/* if data received return the length */
	return pUSB->rxBuffLen;
}

/* A blocking read call */
int32_t libusbdev_Read(uint8_t *pBuf, uint32_t buf_len)
{
	int32_t ret = -1;

	/* Queue read request  */
	if (libusbdev_QueueReadReq(pBuf, buf_len) == LPC_OK) {
		/* wait for Rx to complete */
		while ( (ret = libusbdev_QueueReadDone()) == -1) {
			/* Sleep until next IRQ happens */
			__WFI();
		}
	}

	return ret;
}

/* Queue the given buffer for transmision to USB host application. */
ErrorCode_t libusbdev_QueueSendReq(uint8_t *pBuf, uint32_t buf_len)
{
	LUSB_CTRL_T *pUSB = (LUSB_CTRL_T *) &g_lusb;
	ErrorCode_t ret = ERR_FAILED;

	/* Check if a read request is pending */
	if (pUSB->pTxBuf == 0) {
		/* Queue the read request */
		pUSB->pTxBuf = pBuf;
		pUSB->txBuffLen = buf_len;

		USBD_API->hw->WriteEP(pUSB->hUsb, LUSB_IN_EP, pBuf, buf_len);
		ret = LPC_OK;
	}

	return ret;
}

/* Check if queued send is done. */
int32_t libusbdev_QueueSendDone(void)
{
	LUSB_CTRL_T *pUSB = (LUSB_CTRL_T *) &g_lusb;

	/* return remaining length */
	return pUSB->txBuffLen;
}

/* Send the given buffer to USB host application */
ErrorCode_t libusbdev_Send(uint8_t *pBuf, uint32_t buf_len)
{
	ErrorCode_t ret = libusbdev_QueueSendReq(pBuf, buf_len);

	/* Queue read request  */
	if (ret == LPC_OK) {
		/* wait for Rx to complete */
		while ( libusbdev_QueueSendDone() != 0) {
			/* Sleep until next IRQ happens */
			__WFI();
		}
	}

	return ret;
}

/* Send interrupt signal to USB host application. */
ErrorCode_t libusbdev_SendInterrupt(uint32_t status)
{
	LUSB_CTRL_T *pUSB = (LUSB_CTRL_T *) &g_lusb;

	/* enter critical section */
	NVIC_DisableIRQ(LPC_USB_IRQ);
	/* update new status */
	pUSB->newStatus = status;
	/* exit critical section */
	NVIC_EnableIRQ(LPC_USB_IRQ);

	/* check if we are in middle of sending current status */
	if ( pUSB->curStatus == 0) {
		/* If not update current status and send */
		pUSB->curStatus = pUSB->newStatus;
		pUSB->newStatus = 0;
		USBD_API->hw->WriteEP(pUSB->hUsb, LUSB_INT_EP, (uint8_t *) &pUSB->curStatus, sizeof(uint32_t));
	}

	return LPC_OK;
}






