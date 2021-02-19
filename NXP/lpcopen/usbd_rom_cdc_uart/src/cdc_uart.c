/*
 * @brief UART Comm port call back routines
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
#include "cdc_uart.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#if (defined(BOARD_HITEX_EVA_1850) || defined(BOARD_HITEX_EVA_4350))
#define UARTNum 0

#elif (defined(BOARD_KEIL_MCB_1857) || defined(BOARD_KEIL_MCB_4357))
#define UARTNum 3

#elif (defined(BOARD_NGX_XPLORER_1830) || defined(BOARD_NGX_XPLORER_4330))
#define UARTNum 0

#elif (defined(BOARD_NXP_LPCXPRESSO_4337) || defined(BOARD_NXP_LPCXPRESSO_1837))
#define UARTNum 0

#elif defined(BOARD_NXP_LPCLINK2_4370)
#define UARTNum 2

#else
#error No UART selected for undefined board
#endif

#if (UARTNum == 0)
#define LPC_UART LPC_USART0
#define UARTx_IRQn  USART0_IRQn
#define UARTx_IRQHandler UART0_IRQHandler
#elif (UARTNum == 1)
#define LPC_UART LPC_UART1
#define UARTx_IRQn  UART1_IRQn
#define UARTx_IRQHandler UART1_IRQHandler
#elif (UARTNum == 2)
#define LPC_UART LPC_USART2
#define UARTx_IRQn  USART2_IRQn
#define UARTx_IRQHandler UART2_IRQHandler
#elif (UARTNum == 3)
#define LPC_UART LPC_USART3
#define UARTx_IRQn  USART3_IRQn
#define UARTx_IRQHandler UART3_IRQHandler
#endif

/* Ring buffer size */
#define UCOM_TXBUF_SZ           USB_HS_MAX_BULK_PACKET	/* This should be as big as MAXP */
#define UCOM_RXBUF_SZ           64			/* The size should always be 2^n type.*/

/**
 * Structure containing Virtual Comm port control data
 */
typedef struct UCOM_DATA {
	USBD_HANDLE_T hUsb;			/*!< Handle to USB stack */
	USBD_HANDLE_T hCdc;			/*!< Handle to CDC class controller */

	uint8_t *rxBuf;				/*!< UART Rx buffer */
	uint8_t *txBuf;				/*!< UART Tx buffer */

	uint16_t txBuf_uartIndex;	/*!< UART index in Tx buffer */
	int16_t txBuf_count;		/*!< Bytes present in Tx buffer */

	uint8_t rxBuf_uartIndex;	/*!< UART index in Rx buffer */
	uint8_t rxBuf_usbIndex;		/*!< USB index in Rx buffer */

	volatile uint8_t usbTxBusy;	/*!< USB is busy sending previous packet */
	volatile uint8_t uartTxBusy;/*!< UART is busy sending packet */
} UCOM_DATA_T;

/** Virtual Comm port control data instance. */
static UCOM_DATA_T g_uCOM;

/* Part of WORKAROUND for artf42016. */
static USB_EP_HANDLER_T g_defaultCdcHdlr;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* UART port init routine */
static void UCOM_UartInit(void)
{
	Board_UART_Init(LPC_UART);

	Chip_UART_Init(LPC_UART);
	Chip_UART_SetBaud(LPC_UART, 115200);
	Chip_UART_ConfigData(LPC_UART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_UART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_UART);

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(LPC_UART, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* Enable Interrupt for UART channel */
	/* Priority = 1 */
	NVIC_SetPriority(UARTx_IRQn, 1);
	/* Enable Interrupt for UART channel */
	NVIC_EnableIRQ(UARTx_IRQn);
}

/* UCOM bulk EP_IN and EP_OUT endpoints handler */
static ErrorCode_t UCOM_bulk_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	UCOM_DATA_T *pUcom = (UCOM_DATA_T *) data;
	uint32_t count = 0;

	switch (event) {
	/* A transfer from us to the USB host that we queued has completed. */
	case USB_EVT_IN:
		/* check if UART had more data to send */
		if (pUcom->rxBuf_uartIndex < pUcom->rxBuf_usbIndex) {
			count = UCOM_RXBUF_SZ - pUcom->rxBuf_usbIndex;
		}
		else {
			count = pUcom->rxBuf_uartIndex - pUcom->rxBuf_usbIndex;
		}
		if (count) {
			pUcom->usbTxBusy = 1;
			count = USBD_API->hw->WriteEP(pUcom->hUsb, USB_CDC_IN_EP, &pUcom->rxBuf[g_uCOM.rxBuf_usbIndex], count);
			g_uCOM.rxBuf_usbIndex = (g_uCOM.rxBuf_usbIndex + count) & (UCOM_RXBUF_SZ - 1);
		}
		else {
			pUcom->usbTxBusy = 0;
		}
		break;

	case USB_EVT_OUT_NAK:
		if (pUcom->uartTxBusy == 0) {
			USBD_API->hw->ReadReqEP(hUsb, USB_CDC_OUT_EP, &pUcom->txBuf[0], UCOM_TXBUF_SZ);
			pUcom->uartTxBusy = 1;
		}
		break;

	/* We received a transfer from the USB host . */
	case USB_EVT_OUT:
		if ((Chip_UART_GetIntsEnabled(LPC_UART) & UART_IER_THREINT) == 0) {
			pUcom->txBuf_count = USBD_API->hw->ReadEP(hUsb, USB_CDC_OUT_EP, &pUcom->txBuf[0]);
			pUcom->txBuf_uartIndex = 0;
			/* kick start UART tranmission */
			pUcom->txBuf_uartIndex = Chip_UART_Send(LPC_UART,
													&pUcom->txBuf[g_uCOM.txBuf_uartIndex],
													pUcom->txBuf_count);
			pUcom->txBuf_count -= pUcom->txBuf_uartIndex;
			/* Enable UART transmit interrupt */
			Chip_UART_IntEnable(LPC_UART, UART_IER_THREINT);
		}
		break;

	default:
		break;
	}

	return LPC_OK;
}

/* Set line coding call back routine */
static ErrorCode_t UCOM_SetLineCode(USBD_HANDLE_T hCDC, CDC_LINE_CODING *line_coding)
{
	uint32_t config_data = 0;

	switch (line_coding->bDataBits) {
	case 5:
		config_data |= UART_LCR_WLEN5;
		break;

	case 6:
		config_data |= UART_LCR_WLEN6;
		break;

	case 7:
		config_data |= UART_LCR_WLEN7;
		break;

	case 8:
	default:
		config_data |= UART_LCR_WLEN8;
		break;
	}

	switch (line_coding->bCharFormat) {
	case 1:		/* 1.5 Stop Bits */
		/* In the UART hardware 1.5 stop bits is only supported when using 5
		 * data bits. If data bits is set to 5 and stop bits is set to 2 then
		 * 1.5 stop bits is assumed. Because of this 2 stop bits is not support
		 * when using 5 data bits.
		 */
		if (line_coding->bDataBits == 5) {
			config_data |= UART_LCR_SBS_2BIT;
		}
		else {
			return ERR_USBD_UNHANDLED;
		}
		break;

	case 2:		/* 2 Stop Bits */
		/* In the UART hardware if data bits is set to 5 and stop bits is set to 2 then
		 * 1.5 stop bits is assumed. Because of this 2 stop bits is
		 * not support when using 5 data bits.
		 */
		if (line_coding->bDataBits != 5) {
			config_data |= UART_LCR_SBS_2BIT;
		}
		else {
			return ERR_USBD_UNHANDLED;
		}
		break;

	default:
	case 0:	/* 1 Stop Bit */
		config_data |= UART_LCR_SBS_1BIT;
		break;
	}

	switch (line_coding->bParityType) {
	case 1:
		config_data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_ODD);
		break;

	case 2:
		config_data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_EVEN);
		break;

	case 3:
		config_data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_F_1);
		break;

	case 4:
		config_data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_F_0);
		break;

	default:
	case 0:
		config_data |= UART_LCR_PARITY_DIS;
		break;
	}

	if (line_coding->dwDTERate < 3125000) {
		Chip_UART_SetBaud(LPC_UART, line_coding->dwDTERate);
	}
	Chip_UART_ConfigData(LPC_UART, config_data);

	return LPC_OK;
}

/* CDC EP0_patch part of WORKAROUND for artf42016. */
static ErrorCode_t CDC_ep0_override_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	USB_CORE_CTRL_T *pCtrl = (USB_CORE_CTRL_T *) hUsb;
	USB_CDC_CTRL_T *pCdcCtrl = (USB_CDC_CTRL_T *) data;
	USB_CDC0_CTRL_T *pCdc0Ctrl = (USB_CDC0_CTRL_T *) data;
	uint8_t cif_num, dif_num;
	CIC_SetRequest_t setReq;
	ErrorCode_t ret = ERR_USBD_UNHANDLED;

	if ( (event == USB_EVT_OUT) &&
		 (pCtrl->SetupPacket.bmRequestType.BM.Type == REQUEST_CLASS) &&
		 (pCtrl->SetupPacket.bmRequestType.BM.Recipient == REQUEST_TO_INTERFACE) ) {

		/* Check which CDC control structure to use. If epin_num doesn't have BIT7 set then we are
		   at wrong offset so use the old CDC control structure. BIT7 is set for all EP_IN endpoints.

		 */
		if ((pCdcCtrl->epin_num & 0x80) == 0) {
			cif_num = pCdc0Ctrl->cif_num;
			dif_num = pCdc0Ctrl->dif_num;
			setReq = pCdc0Ctrl->CIC_SetRequest;
		}
		else {
			cif_num = pCdcCtrl->cif_num;
			dif_num = pCdcCtrl->dif_num;
			setReq = pCdcCtrl->CIC_SetRequest;
		}
		/* is the request target is our interfaces */
		if (((pCtrl->SetupPacket.wIndex.WB.L == cif_num)  ||
			 (pCtrl->SetupPacket.wIndex.WB.L == dif_num)) ) {

			pCtrl->EP0Data.pData -= pCtrl->SetupPacket.wLength;
			ret = setReq(pCdcCtrl, &pCtrl->SetupPacket, &pCtrl->EP0Data.pData,
						 pCtrl->SetupPacket.wLength);
			if ( ret == LPC_OK) {
				/* send Acknowledge */
				USBD_API->core->StatusInStage(pCtrl);
			}
		}

	}
	else {
		ret = g_defaultCdcHdlr(hUsb, data, event);
	}
	return ret;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	UART interrupt handler sub-routine
 * @return	Nothing
 */
void UARTx_IRQHandler(void)
{
	uint32_t count = 0;
	/* Handle transmit interrupt if enabled */
	if (LPC_UART->IER & UART_IER_THREINT) {
		if (g_uCOM.txBuf_count > 0) {
			count = Chip_UART_Send(LPC_UART, &g_uCOM.txBuf[g_uCOM.txBuf_uartIndex], g_uCOM.txBuf_count);
			g_uCOM.txBuf_count -= count;
			g_uCOM.txBuf_uartIndex += count;
		}
		/* If UART tx is done allow USB RX to queue buffer on next OUT_NAK event */
		if (g_uCOM.txBuf_count < 1) {
			g_uCOM.txBuf_count = 0;
			USBD_API->hw->ReadReqEP(g_uCOM.hUsb, USB_CDC_OUT_EP, &g_uCOM.txBuf[0], UCOM_TXBUF_SZ);
			g_uCOM.uartTxBusy = 1;
			/* all data transmitted on UART disable UART_IER_THREINT */
			Chip_UART_IntDisable(LPC_UART, UART_IER_THREINT);
		}
	}

	/* Handle receive interrupt */
	count = Chip_UART_Read(LPC_UART, &g_uCOM.rxBuf[g_uCOM.rxBuf_uartIndex], UCOM_RXBUF_SZ - g_uCOM.rxBuf_uartIndex);

	if (count) {
		/* Note, following logic works if UCOM_RXBUF_SZ is 2^n size only. */
		g_uCOM.rxBuf_uartIndex = (g_uCOM.rxBuf_uartIndex + count) & (UCOM_RXBUF_SZ - 1);
		/* If USB Tx is not busy kick start USB Tx */
		if ((g_uCOM.usbTxBusy == 0) && USB_IsConfigured(g_uCOM.hUsb)) {
			g_uCOM.usbTxBusy = 1;
			count = USBD_API->hw->WriteEP(g_uCOM.hUsb, USB_CDC_IN_EP, &g_uCOM.rxBuf[g_uCOM.rxBuf_usbIndex], count);
			g_uCOM.rxBuf_usbIndex = (g_uCOM.rxBuf_usbIndex + count) & (UCOM_RXBUF_SZ - 1);
		}
	}

}

/* UART to USB com port init routine */
ErrorCode_t UCOM_init(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T *pDesc, USBD_API_INIT_PARAM_T *pUsbParam)
{
	USBD_CDC_INIT_PARAM_T cdc_param;
	ErrorCode_t ret = LPC_OK;
	uint32_t ep_indx;
	USB_CDC_CTRL_T *pCDC;
	USB_CORE_CTRL_T *pCtrl = (USB_CORE_CTRL_T *) hUsb;

	/* Store USB stack handle for future use. */
	g_uCOM.hUsb = hUsb;
	/* Initi CDC params */
	memset((void *) &cdc_param, 0, sizeof(USBD_CDC_INIT_PARAM_T));
	cdc_param.mem_base = pUsbParam->mem_base;
	cdc_param.mem_size = pUsbParam->mem_size;
	cdc_param.cif_intf_desc = (uint8_t *) find_IntfDesc(pDesc->high_speed_desc, CDC_COMMUNICATION_INTERFACE_CLASS);
	cdc_param.dif_intf_desc = (uint8_t *) find_IntfDesc(pDesc->high_speed_desc, CDC_DATA_INTERFACE_CLASS);
	cdc_param.SetLineCode = UCOM_SetLineCode;

	/* Init CDC interface */
	ret = USBD_API->cdc->init(hUsb, &cdc_param, &g_uCOM.hCdc);

	if (ret == LPC_OK) {
		/* allocate transfer buffers */
		g_uCOM.txBuf = (uint8_t *) cdc_param.mem_base;
		cdc_param.mem_base += UCOM_TXBUF_SZ;
		cdc_param.mem_size -= UCOM_TXBUF_SZ;
		g_uCOM.rxBuf = (uint8_t *) cdc_param.mem_base;
		cdc_param.mem_base += UCOM_RXBUF_SZ;
		cdc_param.mem_size -= UCOM_RXBUF_SZ;

		/* register endpoint interrupt handler */
		ep_indx = (((USB_CDC_IN_EP & 0x0F) << 1) + 1);
		ret = USBD_API->core->RegisterEpHandler(hUsb, ep_indx, UCOM_bulk_hdlr, &g_uCOM);

		if (ret == LPC_OK) {
			/*	WORKAROUND for artf42016 ROM driver BUG:
			    The default CDC class handler in initial ROM (REV A silicon) was not
			    sending proper handshake after processing SET_REQUEST messages targeted
			    to CDC interfaces. The workaround will send the proper handshake to host.
			    Due to this bug some terminal applications such as Putty have problem
			    establishing connection.
			 */
			g_defaultCdcHdlr = pCtrl->ep0_hdlr_cb[pCtrl->num_ep0_hdlrs - 1];
			/* store the default CDC handler and replace it with ours */
			pCtrl->ep0_hdlr_cb[pCtrl->num_ep0_hdlrs - 1] = CDC_ep0_override_hdlr;

			/* register endpoint interrupt handler */
			ep_indx = ((USB_CDC_OUT_EP & 0x0F) << 1);
			ret = USBD_API->core->RegisterEpHandler(hUsb, ep_indx, UCOM_bulk_hdlr, &g_uCOM);
			/* Init UART port for bridging */
			UCOM_UartInit();
			/* Set the line coding values as per UART Settings */
			pCDC = (USB_CDC_CTRL_T *) g_uCOM.hCdc;
			pCDC->line_coding.dwDTERate = 115200;
			pCDC->line_coding.bDataBits = 8;
		}

		/* update mem_base and size variables for cascading calls. */
		pUsbParam->mem_base = cdc_param.mem_base;
		pUsbParam->mem_size = cdc_param.mem_size;
	}

	return ret;
}






