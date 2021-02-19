/*
 * @brief This file contains USB Composite example using USB ROM Drivers.
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
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
#include "hid_mouse.h"
#include "msc_disk.h"
#include "hid_mouse.h"
#include "cdc_vcom.h"
#include "ms_timer.h"
#include "dfu.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
static USBD_HANDLE_T g_hUsb;

/* Endpoint 0 patch that prevents nested NAK event processing */
static uint32_t g_ep0RxBusy = 0;/* flag indicating whether EP0 OUT/RX buffer is busy. */
static USB_EP_HANDLER_T g_Ep0BaseHdlr;	/* variable to store the pointer to base EP0 handler */
static uint8_t g_rxBuff[256];

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
const USBD_API_T *g_pUsbApi;
volatile uint32_t g_msTicks;		/** counts 1ms timeTicks */

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* EP0_patch part of WORKAROUND for artf45032. */
ErrorCode_t EP0_patch(USBD_HANDLE_T hUsb, void *data, uint32_t event)
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

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Handle interrupt from USB */
void USB_IRQHandler(void)
{
	USBD_API->hw->ISR(g_hUsb);
}

/* SysTick IRQ handler */
void SysTick_Handler(void)
{
	g_msTicks++;						/* increment counter necessary in Delay() */
}

/* Waits for given number of milliseconds */
void ms_timerDelay(uint32_t n)
{
	ms_timer_t  timer;

	ms_timerInit(&timer, n);

	while (ms_timerExpired(&timer) == false) {
		__NOP();
		__WFI();/* conserve power */
	}

}

/* Find the address of interface descriptor for given class type. */
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass)
{
	USB_COMMON_DESCRIPTOR *pD;
	USB_INTERFACE_DESCRIPTOR *pIntfDesc = 0;
	uint32_t next_desc_adr;

	pD = (USB_COMMON_DESCRIPTOR *) pDesc;
	next_desc_adr = (uint32_t) pDesc;

	while (pD->bLength) {
		/* is it interface descriptor */
		if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

			pIntfDesc = (USB_INTERFACE_DESCRIPTOR *) pD;
			/* did we find the right interface descriptor */
			if (pIntfDesc->bInterfaceClass == intfClass) {
				break;
			}
		}
		pIntfDesc = 0;
		next_desc_adr = (uint32_t) pD + pD->bLength;
		pD = (USB_COMMON_DESCRIPTOR *) next_desc_adr;
	}

	return pIntfDesc;
}

/* main routine for USBD composite device example */
int main(void)
{
	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	USB_CORE_CTRL_T *pCtrl;
	USB_INTERFACE_DESCRIPTOR *dfu_interface = NULL;
	USB_INTERFACE_DESCRIPTOR *hid_mouse_interface = NULL;

	/* CDC VCOM */
	uint32_t prompt = 0;
	uint32_t rdCnt = 0;

	/* Initialize board and chip */
	SystemCoreClockUpdate();
	Board_Init();

	/* Initialize systick */
	systick_init();

	/* enable clocks and pinmux */
	USB_init_pin_clk();

	/* Init USB API structure */
	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;

	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	usb_param.usb_reg_base = LPC_USB_BASE;
	usb_param.mem_base = USB_STACK_MEM_BASE;
	usb_param.mem_size = USB_STACK_MEM_SIZE;
	usb_param.max_num_ep = USB_MAX_EP_NUM;
	usb_param.USB_Reset_Event = DFU_reset_handler;

	/* Set the USB descriptors */
	desc.device_desc = (uint8_t *) USB_DeviceDescriptor;
	desc.string_desc = (uint8_t *) USB_StringDescriptor;

#ifdef USE_USB0
	desc.high_speed_desc = USB_HsConfigDescriptor;
	desc.full_speed_desc = USB_FsConfigDescriptor;
	desc.device_qualifier = (uint8_t *) USB_DeviceQualifier;
#else
	/* Note, to pass USBCV test full-speed only devices should have both
	   descriptor arrays point to same location and device_qualifier set to 0.
	 */
	desc.high_speed_desc = USB_FsConfigDescriptor;
	desc.full_speed_desc = USB_FsConfigDescriptor;
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

	/* DFU Firmware Update Interface */
	dfu_interface = find_IntfDesc(USB_FsConfigDescriptor, USB_DEVICE_CLASS_APP);
	if ((dfu_interface) && (dfu_interface->bInterfaceSubClass == USB_DFU_SUBCLASS)) {
		ret = DFU_init(g_hUsb, dfu_interface, &usb_param.mem_base, &usb_param.mem_size);
		if (ret != LPC_OK) {
			vCatchErr(2);
		}
	}

	/* CDC VCOM Interface */
	ret = vcom_init(g_hUsb, &desc, &usb_param);
	if (ret != LPC_OK) {
		vCatchErr(5);
	}

	/* MSC RAM Interface */
	ret = mscDisk_init(g_hUsb, &desc, &usb_param);
	if (ret != LPC_OK) {
		vCatchErr(4);
	}

	/* HID Mouse Interface */
	hid_mouse_interface = find_IntfDesc(USB_FsConfigDescriptor, USB_DEVICE_CLASS_HUMAN_INTERFACE);
	if ((hid_mouse_interface) && (hid_mouse_interface->bInterfaceSubClass == HID_SUBCLASS_BOOT) &&
		(hid_mouse_interface->bInterfaceProtocol == HID_PROTOCOL_MOUSE)) {
		ret = Mouse_Init(g_hUsb, hid_mouse_interface, &usb_param.mem_base, &usb_param.mem_size);
		if (ret != LPC_OK) {
			vCatchErr(3);
		}
	}

	/* enable USB interrupts */
	NVIC_EnableIRQ(LPC_USB_IRQ);
	/* now connect */
	USBD_API->hw->Connect(g_hUsb, 1);

	while (1) {

		/* Do DFU Tasks */
		DFU_Tasks();

		/* Do CDC VCOM Tasks */
		/* Check if host has connected and opened the VCOM port */
		if ((vcom_connected() != 0) && (prompt == 0)) {
			vcom_write("Hello World!!\r\n", 15);
			prompt = 1;
		}
		/* If VCOM port is opened echo whatever we receive back to host. */
		if (prompt) {
			rdCnt = vcom_bread(&g_rxBuff[0], 256);
			if (rdCnt) {
				vcom_write(&g_rxBuff[0], rdCnt);
			}
		}

		/* Do MSC RAM tasks */

		/* Do HID Mouse tasks */
		Mouse_Tasks();

		/* Sleep until next IRQ happens */
		__WFI();
	}
}






