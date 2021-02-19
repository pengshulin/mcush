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

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
static USBD_HANDLE_T g_hUsb;

/* Endpoint 0 patch that prevents nested NAK event processing */
static uint32_t g_ep0RxBusy = 0;/* flag indicating whether EP0 OUT/RX buffer is busy. */
static USB_EP_HANDLER_T g_Ep0BaseHdlr;	/* variable to store the pointer to base EP0 handler */

#define EP1_OUT_INDEX           2
#define EP1_OUT_BIT             _BIT(1)
#define EP1_IN_INDEX            3
#define EP1_IN_BIT              _BIT(1 + 16)
#define EP2_OUT_INDEX           4
#define EP2_OUT_BIT             _BIT(2)
#define EP2_IN_INDEX            5
#define EP2_IN_BIT              _BIT(2 + 16)

/* dTD field and bit defines */
#define TD_NEXT_TERMINATE       _BIT(0)
#define TD_IOC                  _BIT(15)
#define USB_DEST_ADDR           0x20004000

/* dTD Transfer Description */
typedef volatile struct {
	volatile uint32_t next_dTD;
	volatile uint32_t total_bytes;
	volatile uint32_t buffer0;
	volatile uint32_t buffer1;
	volatile uint32_t buffer2;
	volatile uint32_t buffer3;
	volatile uint32_t buffer4;
	volatile uint32_t reserved;
}  DTD_T;

/* dQH  Queue Head */
typedef volatile struct {
	volatile uint32_t cap;
	volatile uint32_t curr_dTD;
	volatile uint32_t next_dTD;
	volatile uint32_t total_bytes;
	volatile uint32_t buffer0;
	volatile uint32_t buffer1;
	volatile uint32_t buffer2;
	volatile uint32_t buffer3;
	volatile uint32_t buffer4;
	volatile uint32_t reserved;
	volatile uint32_t setup[2];
	volatile uint32_t gap[4];
}  DQH_T;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
const USBD_API_T *g_pUsbApi;

DTD_T *g_dtdPool[4];
int ep_count[4];

/*****************************************************************************
 * Private functions
 ****************************************************************************/

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

/* If there is any error then stop here forever. */
static void vCatchErr(uint8_t u8Err)
{
	volatile uint8_t u8theErr = u8Err;

	while (1) {}
}

/* Program transfer descriptors for given endpoint. */
static void bwtest_USB_Prog_DTD(uint32_t epIndex, uint32_t epBit)
{
	int i;
	DQH_T *ep_QH = (DQH_T *) LPC_USB->ENDPOINTLISTADDR;
	/* Get physical address for TD */
	uint32_t dtd_phys = (uint32_t) &g_dtdPool[epIndex - 2][0];
	/* get DTDs for the given endpoint */
	DTD_T *ep_TD = &g_dtdPool[epIndex - 2][0];

	/* Zero out the device transfer descriptors */
	memset((void *) dtd_phys, 0, 8 * sizeof(DTD_T));

	/* set each TD to transfer 20KB */
	for (i = 0; i < 8; i++) {
		ep_TD[i].next_dTD = (uint32_t) (dtd_phys + ((1 + i) * sizeof(DTD_T)));
		ep_TD[i].total_bytes = (0x5000 << 16) | TD_IOC | 0x80;
		/* we will use same memory area for Rx and Tx for EP1 & EP2. Since we are
		   just interested in throughput instead of data in this test */
		ep_TD[i].buffer0 = USB_DEST_ADDR;
		ep_TD[i].buffer1 = (USB_DEST_ADDR + 0x1000);
		ep_TD[i].buffer2 = (USB_DEST_ADDR + 0x2000);
		ep_TD[i].buffer3 = (USB_DEST_ADDR + 0x3000);
		ep_TD[i].buffer4 = (USB_DEST_ADDR + 0x4000);
		ep_TD[i].reserved = 0;
	}

	ep_TD[7].next_dTD = 0x01;	/* The next DTD pointer is INVALID */

	/* update the hardware endpoint queue */
	ep_QH[epIndex].next_dTD = dtd_phys;
	ep_QH[epIndex].total_bytes &= (~0xC0);

	/* prime the endpoint for transfer to start */
	LPC_USB->ENDPTPRIME |= epBit;
}

/* USB Configure Event Callback. Called automatically after active
 * configuration is selected.
 */
static ErrorCode_t bwtest_ConfigureEvent(USBD_HANDLE_T hUsb)
{
	/* setup the transfer descriptors for all bandwidth test endpoints */
	bwtest_USB_Prog_DTD(EP1_OUT_INDEX, EP1_OUT_BIT);	/* EP1_OUT */
	bwtest_USB_Prog_DTD(EP2_OUT_INDEX, EP2_OUT_BIT);	/* EP2_OUT */
	bwtest_USB_Prog_DTD(EP1_IN_INDEX, EP1_IN_BIT);		/* EP1_IN */
	bwtest_USB_Prog_DTD(EP2_IN_INDEX, EP2_IN_BIT);		/* EP2_IN */
	return LPC_OK;
}

/* Handle endpoint events.
 * OUT endpoints will get USB_EVT_OUT & USB_EVT_OUT_NAK events.
 * IN endpoints will get USB_EVT_IN & USB_EVT_IN_NAK events.
 */
static ErrorCode_t bwtest_EpHandler(uint32_t event, uint32_t epIndex, uint32_t epBit)
{
	switch (event) {

	case USB_EVT_OUT:
	case USB_EVT_IN:
		/* enqueue next TD to transfer to external SRAM */
		ep_count[epIndex - 2]++;
		if (ep_count[epIndex - 2] == 8) {
			ep_count[epIndex - 2] = 0;
			bwtest_USB_Prog_DTD(epIndex, epBit);
		}
		break;

	case USB_EVT_IN_NAK:
		/* should be here for the first time only. */
		break;

	case USB_EVT_OUT_NAK:
		/* should be here for the first time only. */
		break;
	}
	return LPC_OK;
}

/* Bulk IN Endpoint 1 Event Callback.*/
static ErrorCode_t bwtest_Ep1InHandler(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	return bwtest_EpHandler(event, EP1_IN_INDEX, EP1_IN_BIT);
}

/* Bulk IN Endpoint 2 Event Callback.*/
static ErrorCode_t bwtest_Ep2InHandler(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	return bwtest_EpHandler(event, EP2_IN_INDEX, EP2_IN_BIT);
}

/* Bulk OUT Endpoint 1 Event Callback.*/
static ErrorCode_t bwtest_Ep1OutHandler(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	return bwtest_EpHandler(event, EP1_OUT_INDEX, EP1_OUT_BIT);
}

/* Bulk OUT Endpoint 2 Event Callback.*/
static ErrorCode_t bwtest_Ep2OutHandler(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	return bwtest_EpHandler(event, EP2_OUT_INDEX, EP2_OUT_BIT);
}

/* Handler for WCID USB device requests. */
static ErrorCode_t bwtest_ep0_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	USB_CORE_CTRL_T *pCtrl = (USB_CORE_CTRL_T *) hUsb;
	ErrorCode_t ret = ERR_USBD_UNHANDLED;

	if ((event != USB_EVT_SETUP) ||
		( pCtrl->SetupPacket.bmRequestType.BM.Dir != REQUEST_DEVICE_TO_HOST) ) {
		/* For bus powered devices set the device_status to 0. The default handler
		   assumes SELF_POWERED. This step is needed for usb.org Ch9 certification
		   test to pass. */
		if (event == USB_EVT_RESET) {
			pCtrl->device_status = 0;
		}
		return ret;
	}

	/* Handle Microsoft's WCID request for install less WinUSB operation.
	   Check https://github.com/pbatard/libwdi/wiki/WCID-Devices for more details.
	 */
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
		if (pCtrl->SetupPacket.bRequest != WCID_VENDOR_CODE) {
			break;
		}
		switch (pCtrl->SetupPacket.bmRequestType.BM.Recipient) {
		case REQUEST_TO_DEVICE:
			if (pCtrl->SetupPacket.wIndex.W == 0x0004) {
				pCtrl->EP0Data.pData = (uint8_t *) WCID_CompatID_Descriptor;
				pCtrl->EP0Data.Count = pCtrl->SetupPacket.wLength;
				USBD_API->core->DataInStage(pCtrl);
				ret = LPC_OK;
			}
		/* Fall-through. Check note1 of
		   https://github.com/pbatard/libwdi/wiki/WCID-Devices#wiki-Defining_a_Device_Interface_GUID_or_other_device_specific_properties
		   break;
		 */

		case REQUEST_TO_INTERFACE:
			if (pCtrl->SetupPacket.wIndex.W == 0x0005) {
				pCtrl->EP0Data.pData = (uint8_t *) WCID_ExtProp_Descriptor;
				pCtrl->EP0Data.Count = pCtrl->SetupPacket.wLength;
				USBD_API->core->DataInStage(pCtrl);
				ret = LPC_OK;
			}
			break;
		}
		break;
	}

	return ret;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Handle interrupt from USB */
void USB_IRQHandler(void)
{
	USBD_API->hw->ISR(g_hUsb);
}

/* main routine for USBD bandwidth test example */
int main(void)
{
	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	USB_CORE_CTRL_T *pCtrl;

	/* Initialize board and chip */
	SystemCoreClockUpdate();
	Board_Init();

	USB_init_pin_clk();

	/* Init USB API structure */
	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;

	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	usb_param.usb_reg_base = LPC_USB_BASE;
	usb_param.max_num_ep = USB_MAX_EP_NUM;
	usb_param.mem_base = USB_STACK_MEM_BASE;
	usb_param.mem_size = USB_STACK_MEM_SIZE;
	usb_param.USB_Configure_Event = bwtest_ConfigureEvent;

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
	ret = USBD_API->hw->Init(&g_hUsb, &desc, &usb_param);
	if (ret != LPC_OK) {
		vCatchErr(1);
	}

	/*	WORKAROUND for artf45032 ROM driver BUG:
	    Due to a race condition there is the chance that a second NAK event will
	    occur before the default endpoint0 handler has completed its preparation
	    of the DMA engine for the first NAK event. This can cause certain fields
	    in the DMA descriptors to be in an invalid state when the USB controller
	    reads them, thereby causing a hang.
	 */
	pCtrl = (USB_CORE_CTRL_T *) g_hUsb;	/* convert the handle to control structure */
	g_Ep0BaseHdlr = pCtrl->ep_event_hdlr[0];/* retrieve the default EP0_OUT handler */
	pCtrl->ep_event_hdlr[0] = EP0_patch;/* set our patch routine as EP0_OUT handler */

	/* allocate TDs for bandwidth test in USB accessable memory*/
	while (usb_param.mem_base & 0x1F) {
		usb_param.mem_base++; usb_param.mem_size--;
	}
	/* allocate DTDs for EP1_OUT */
	g_dtdPool[0] = (DTD_T *) usb_param.mem_base;
	usb_param.mem_base += (8 * sizeof(DTD_T));
	/* allocate DTDs for EP1_IN */
	g_dtdPool[1] = (DTD_T *) usb_param.mem_base;
	usb_param.mem_base += (8 * sizeof(DTD_T));
	/* allocate DTDs for EP2_OUT */
	g_dtdPool[2] = (DTD_T *) usb_param.mem_base;
	usb_param.mem_base += (8 * sizeof(DTD_T));
	/* allocate DTDs for EP2_IN */
	g_dtdPool[3] = (DTD_T *) usb_param.mem_base;
	usb_param.mem_base += (8 * sizeof(DTD_T));

	/* update size for all four EPs */
	usb_param.mem_size -= (4 * 8 * sizeof(DTD_T));

	/* register EP1 handler */
	ret = USBD_API->core->RegisterEpHandler(g_hUsb, EP1_OUT_INDEX, bwtest_Ep1OutHandler, 0);
	if (ret != LPC_OK) {
		vCatchErr(2);
	}
	ret = USBD_API->core->RegisterEpHandler(g_hUsb, EP1_IN_INDEX, bwtest_Ep1InHandler, 0);
	if (ret != LPC_OK) {
		vCatchErr(3);
	}

	/* register EP2 handler */
	ret = USBD_API->core->RegisterEpHandler(g_hUsb, EP2_OUT_INDEX, bwtest_Ep2OutHandler, 0);
	if (ret != LPC_OK) {
		vCatchErr(4);
	}
	ret = USBD_API->core->RegisterEpHandler(g_hUsb, EP2_IN_INDEX, bwtest_Ep2InHandler, 0);
	if (ret != LPC_OK) {
		vCatchErr(5);
	}

	/* register WCID handler */
	ret = USBD_API->core->RegisterClassHandler(g_hUsb, bwtest_ep0_hdlr, 0);
	if (ret != LPC_OK) {
		vCatchErr(6);
	}
	/* enable USB0 interrrupts */
	NVIC_EnableIRQ(LPC_USB_IRQ);

	/* now connect */
	USBD_API->hw->Connect(g_hUsb, 1);

	while (1) {
		__WFI();
	}
}






