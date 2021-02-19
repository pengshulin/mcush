/*
 * @brief HID SIO example routines
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
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
#include <stdint.h>
#include <string.h>
#include "usbd_rom_api.h"
#include "hid_sio.h"
#include "lpcusbsio_protocol.h"
#include "stopwatch.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define HID_SIO_MAX_DATA_SIZE 				1024
#define HID_SIO_MAX_TRANSFER_BUFFERS	4
#define HID_SIO_MAX_TRANSFER_BUFF_SZ	(HID_SIO_MAX_DATA_SIZE + HID_SIO_PACKET_HEADER_SZ + HID_SIO_MAX_XFER_PARAM_SIZE)

#define HID_SIO_STATE_DISCON        0
#define HID_SIO_STATE_CONNECTED     1

#if (defined(BOARD_HITEX_EVA_4350) || defined(BOARD_KEIL_MCB_4357) || \
		 defined(BOARD_NGX_XPLORER_4330) || defined(BOARD_NGX_XPLORER_1830) || \
		 defined(BOARD_KEIL_MCB_1857) || defined(BOARD_HITEX_EVA_1850))
/* Hitex 4350 Evaluation board, Keil MCB 4357 board and NGX explorer 4330 */
#define HID_SIO_I2C_PORTS           1
/* No SSP devices */
#define HID_SIO_SPI_PORTS           0
#define HID_SIO_GPIO_PORTS      		8

#elif (defined(BOARD_NXP_LPCLINK2_4370))
/* LPC Link2 4370 board */
#define HID_SIO_I2C_PORTS           1
#define HID_SIO_SPI_PORTS           1
#define HID_SIO_GPIO_PORTS      		8

#elif (defined(BOARD_NXP_LPCXPRESSO_4337) || defined(BOARD_NXP_LPCXPRESSO_1837))
/* Expresso 4337 board */
#define HID_SIO_I2C_PORTS           0
#define HID_SIO_SPI_PORTS           0
#define HID_SIO_GPIO_PORTS      		8

#else
/* Default setting is 0 */
#define HID_SIO_I2C_PORTS           0
#define HID_SIO_SPI_PORTS           0
#define HID_SIO_GPIO_PORTS      		0
#error "Unsupported Board"
#endif

#define HID_SIO_NOTHING_PENDING     0
#define HID_SIO_I2C_XFER_PENDING    1
#define HID_SIO_I2C_RW_PENDING      2
#define HID_SIO_SSP_XFER_PENDING    3

#if (defined(BOARD_KEIL_MCB_4357) || defined(BOARD_NGX_XPLORER_4330) || \
	defined(BOARD_NXP_LPCLINK2_4370) || defined(BOARD_NGX_XPLORER_1830) || \
	defined(BOARD_KEIL_MCB_1857) )
#define LPC_SSP             LPC_SSP1
#define SSP_IRQ             SSP1_IRQn
#define SSPIRQHANDLER       SSP1_IRQHandler
#elif (defined(BOARD_HITEX_EVA_4350) || defined(BOARD_HITEX_EVA_1850) || defined(BOARD_NXP_LPCXPRESSO_4337) || defined(BOARD_NXP_LPCXPRESSO_1837))
#define LPC_SSP             LPC_SSP0
#define SSP_IRQ             SSP0_IRQn
#define SSPIRQHANDLER       SSP0_IRQHandler
#else
#warning "Unsupported Board"
#endif

/**
 * Structure containing HID_SIO control data
 */
typedef struct __HID_SIO_CTRL_T {
	USBD_HANDLE_T hUsb;		/*!< Handle to USB stack */
#if (HID_SIO_I2C_PORTS > 0)
	LPC_I2C_T *pI2C[HID_SIO_I2C_PORTS];		/*!< I2C ports this bridge is associated. */
#endif
#if (HID_SIO_SPI_PORTS > 0)
	LPC_SSP_T *pSSP[HID_SIO_SPI_PORTS];		/*!< SPI ports this bridge is associated. */
#endif
	uint8_t reqWrIndx;		/*!< Write index for request queue. */
	uint8_t reqRdIndx;		/*!< Read index for request queue. */
	uint8_t respWrIndx;		/*!< Write index for response queue. */
	uint8_t respRdIndx;		/*!< Read index for response queue. */
	uint8_t reqPending;		/*!< Flag indicating request is pending in EP RAM */
	uint8_t respIdle;			/*!< Flag indicating EP_IN/response interrupt is idling */
	uint8_t state;				/*!< State of the controller */
	uint8_t resetI2CReq;	/*!< Flag indicating if I2C reset is requested by host */
	uint8_t resetSPIReq;	/*!< Flag indicating if SPI reset is requested by host */
	uint8_t epin_adr;			/*!< Interrupt IN endpoint associated with this HID instance. */
	uint8_t epout_adr;		/*!< Interrupt OUT endpoint associated with this HID instance. */
	uint8_t pending;			/*!< Field to track pending I2C/SSP transfers. */

	I2CM_XFER_T i2c_xfer;					/*!< I2C Transfer data structure passed I2CM driver. */
	Chip_SSP_DATA_SETUP_T spi_xfer;			/*!< SPI Transfer data structure passed SPI driver. */
	uint8_t txBuf[HID_SIO_PACKET_SZ];		/*!< Temporary Transmit buffer USB IN transaction. */
	uint8_t rxBuf[HID_SIO_PACKET_SZ];		/*!< Temporary Receive buffer USB OUT transaction. */
	uint8_t reqQ[HID_SIO_MAX_TRANSFER_BUFFERS][HID_SIO_MAX_TRANSFER_BUFF_SZ];		/*!< Requests queue */
	uint8_t respQ[HID_SIO_MAX_TRANSFER_BUFFERS][HID_SIO_MAX_TRANSFER_BUFF_SZ];	/*!< Response queue */
} HID_SIO_CTRL_T;

/* Firmware version number defines and variables. */
#define HID_SIO_MAJOR_VER           2
#define HID_SIO_MINOR_VER           0
static const char *g_fwVersion = "(" __DATE__ " " __TIME__ ")";
static uint16_t i2cWritePos;
#if (HID_SIO_SPI_PORTS > 0)
static uint8_t spiPreDelay;
static uint8_t spiPostDelay;
#endif

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

extern const uint8_t HID_SIO_ReportDescriptor[];
extern const uint16_t HID_SIO_ReportDescSize;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

#if (HID_SIO_I2C_PORTS > 0)

static void InitI2CPorts(HID_SIO_CTRL_T *pHidI2c, uint8_t port, uint32_t busSpeed)
{
	if(port == 0) {
		if(busSpeed >= 1000000) {
			Chip_SCU_I2C0PinConfig(I2C0_FAST_MODE_PLUS);
		}
		else {
			Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
		}
		NVIC_DisableIRQ(I2C0_IRQn);
		pHidI2c->pI2C[0] = LPC_I2C0;
	}

#if (HID_SIO_I2C_PORTS == 2)
	if(port == 1) {
		/* Configure pin function for I2C1 on PE.13 (I2C1_SDA) and PE.15 (I2C1_SCL) */
		Chip_SCU_PinMuxSet(0xE, 13, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC2));
		Chip_SCU_PinMuxSet(0xE, 15, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC2));
		NVIC_DisableIRQ(I2C1_IRQn);
		/* bind to I2C1 port */
		pHidI2c->pI2C[1] = LPC_I2C1;
	}
#endif
}

static void DeInitI2CPorts(HID_SIO_CTRL_T *pHidI2c, uint8_t port)
{
	if(port == 0) {
		Chip_SCU_I2C0PinConfig(0);
	}
#if (HID_SIO_I2C_PORTS == 2)
	if(port == 1) {
		/* Configure pin function for GPIO on PE.13 and PE.15 */
		Chip_SCU_PinMuxSet(0xE, 13, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC4));
		Chip_SCU_PinMuxSet(0xE, 15, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC4));
	}
#endif
}
#endif

#if (HID_SIO_SPI_PORTS > 0)
/* Initialize SPI port pin muxing */
static void InitSPIPorts(HID_SIO_CTRL_T *pHidSPI, uint8_t port)
{
	if(port == 0) {
#if (defined(BOARD_NXP_LPCLINK2_4370))
		/* Set up clock and muxing for SSP1 interface */
		/* PF.4 connected to SCLK	SCU_MODE_FUNC0=SSP1 SCK */
		Chip_SCU_PinMuxSet(0xF, 4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
		/* P1.20 connected to nCS		SCU_MODE_FUNC0=GPIO0.15 */
		Chip_SCU_PinMuxSet(0x1, 20, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
		/* P1.3 connected to MISO			SCU_MODE_FUNC5=SSP1 MISO */
		Chip_SCU_PinMuxSet(0x1, 3, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5));
		/* P1.4 connected to MOSI		SCU_MODE_FUNC5=SSP1 MOSI */
		Chip_SCU_PinMuxSet(0x1, 4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5));
		/* Set SSEL line as GPIO output line and set it to high */
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 15, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 15);

#elif (defined(BOARD_HITEX_EVA_4350) || defined(BOARD_HITEX_EVA_1850))
		/* Setup pin mux for SSP SCK */
		Chip_SCU_PinMuxSet(0xF, 0, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
		/* Setup pin mux for SSP SSEL - GPIO 7.16 */
		Chip_SCU_PinMuxSet(0xF, 1, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));
		/* Setup pin mux for SSP MISO */
		Chip_SCU_PinMuxSet(0xF, 2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC2));
		/* Setup pin mux for SSP MOSI */
		Chip_SCU_PinMuxSet(0xF, 3, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC2));
		/* Set SSEL line as GPIO output line and set it to high */
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, 7, 16, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 7, 16);
#else
#warning "Unsupported Board"
#endif
		pHidSPI->pSSP[0] = LPC_SSP;
	}
}

/* DeInitialize SPI pin muxing */
static void DeInitSPIPorts(HID_SIO_CTRL_T *pHidSPI, uint8_t port)
{
	if(port == 0) {
#if (defined(BOARD_NXP_LPCXPRESSO_4337) || defined(BOARD_NXP_LPCXPRESSO_1837))
		Chip_SCU_PinMuxSet(0x1, 19, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC6));
		Chip_SCU_PinMuxSet(0x1, 5, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
		Chip_SCU_PinMuxSet(0x0, 0, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
		Chip_SCU_PinMuxSet(0x0, 1, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
		Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 1, 8);

#elif (defined(BOARD_NXP_LPCLINK2_4370))
		Chip_SCU_PinMuxSet(0xF, 4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
		Chip_SCU_PinMuxSet(0x1, 20, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
		Chip_SCU_PinMuxSet(0x1, 3, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
		Chip_SCU_PinMuxSet(0x1, 4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
		Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, 15);

#elif (defined(BOARD_HITEX_EVA_4350) || defined(BOARD_HITEX_EVA_1850))
		Chip_SCU_PinMuxSet(0xF, 0, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
		Chip_SCU_PinMuxSet(0xF, 1, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));
		Chip_SCU_PinMuxSet(0xF, 2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));
		Chip_SCU_PinMuxSet(0xF, 3, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));
		Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 7, 16);
#else
#warning "Unsupported Board"
#endif
	}
}
#endif

static INLINE void HID_SIO_IncIndex(uint8_t *index)
{
	*index = (*index + 1) & (HID_SIO_MAX_TRANSFER_BUFFERS - 1);
}

/*  HID get report callback function. */
static ErrorCode_t HID_SIO_GetReport(USBD_HANDLE_T hHid, USB_SETUP_PACKET *pSetup,
									 uint8_t * *pBuffer, uint16_t *plength)
{
	return LPC_OK;
}

/* HID set report callback function. */
static ErrorCode_t HID_SIO_SetReport(USBD_HANDLE_T hHid, USB_SETUP_PACKET *pSetup,
									 uint8_t * *pBuffer, uint16_t length)
{
	return LPC_OK;
}

/* HID_SIO Interrupt endpoint event handler. */
static ErrorCode_t HID_SIO_EpIn_Hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	HID_SIO_CTRL_T *pHidSio = (HID_SIO_CTRL_T *) data;
	HID_SIO_IN_REPORT_T *pIn;

	/* last report is successfully sent. Send next response if in queue. */
	if (pHidSio->respRdIndx != pHidSio->respWrIndx) {
		pHidSio->respIdle = 0;
		pIn = (HID_SIO_IN_REPORT_T *) &pHidSio->respQ[pHidSio->respRdIndx][0];
		/* Copy SIO Header first */
		memcpy(&pHidSio->txBuf[0], pIn, HID_SIO_PACKET_HEADER_SZ);
		/* Copy the data section for the next packet, note that packet_num and packet_len are updated already */
		memcpy(&pHidSio->txBuf[HID_SIO_PACKET_HEADER_SZ],
			   &pIn->data[0] + (pIn->packet_num * HID_SIO_PACKET_DATA_SZ),
			   pIn->packet_len - HID_SIO_PACKET_HEADER_SZ);
		/* If there is no more packets to transmit then increment index */
		if ((pIn->packet_num * HID_SIO_PACKET_SZ + pIn->packet_len) == pIn->transfer_len) {
			HID_SIO_IncIndex(&pHidSio->respRdIndx);
		}
		else {
			/* Otherwise update header for next packet transfer */
			if (pIn->transfer_len - (pIn->packet_num * HID_SIO_PACKET_SZ + pIn->packet_len ) > HID_SIO_PACKET_SZ) {
				pIn->packet_len = HID_SIO_PACKET_SZ;
			}
			else {
				pIn->packet_len = pIn->transfer_len - (pIn->packet_num * HID_SIO_PACKET_SZ + pIn->packet_len);
			}
			pIn->packet_num++;
		}
		/* Write data for IN packet */
		USBD_API->hw->WriteEP(pHidSio->hUsb,
							  pHidSio->epin_adr,
							  &pHidSio->txBuf[0],
							  HID_SIO_PACKET_SZ);
	}
	else {
		pHidSio->respIdle = 1;
	}
	return LPC_OK;
}

/* HID_SIO Interrupt endpoint event handler. */
static ErrorCode_t HID_SIO_EpOut_Hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	HID_SIO_CTRL_T *pHidSio = (HID_SIO_CTRL_T *) data;
	HID_SIO_OUT_REPORT_T *pOut;

	if (event == USB_EVT_OUT) {
		/* Read the new request received. */
		USBD_API->hw->ReadEP(hUsb, pHidSio->epout_adr, &pHidSio->rxBuf[0]);
		pOut = (HID_SIO_OUT_REPORT_T *) &pHidSio->rxBuf[0];
		/* If it is the first USB packet of SIO transfer then copy it to start of buffer */
		if (pOut->packet_num == 0) {
			memcpy(&pHidSio->reqQ[pHidSio->reqWrIndx][0], &pHidSio->rxBuf[0], pOut->packet_len);
		}
		else {
			/* If it is subsequent packet then copy it to appropriate index */
			if (pOut->transId == ((HID_SIO_OUT_REPORT_T *) &pHidSio->reqQ[pHidSio->reqWrIndx][0])->transId) {
				memcpy(&pHidSio->reqQ[pHidSio->reqWrIndx][pOut->packet_num * HID_SIO_PACKET_DATA_SZ +
														  HID_SIO_PACKET_HEADER_SZ],
					   &pOut->data[0],
					   pOut->packet_len - HID_SIO_PACKET_HEADER_SZ);
				/* Update packet number and length in buffer */
				((HID_SIO_OUT_REPORT_T *) &pHidSio->reqQ[pHidSio->reqWrIndx][0])->packet_num = pOut->packet_num;
				((HID_SIO_OUT_REPORT_T *) &pHidSio->reqQ[pHidSio->reqWrIndx][0])->packet_len = pOut->packet_len;
			}
			else {
				/* Queue the next buffer for Rx */
				USBD_API->hw->ReadReqEP(hUsb, pHidSio->epout_adr, &pHidSio->rxBuf[0], HID_SIO_PACKET_SZ);
				return LPC_OK;
			}
		}
		/* If transfer is complete then increment ring buffer index */
		if ((pOut->packet_num * HID_SIO_PACKET_SZ + pOut->packet_len) == pOut->transfer_len) {
			/* handle HID_I2C_REQ_RESET request in IRQ context to abort current
			     transaction and reset the queue states. */
			if (pOut->req == HID_I2C_REQ_RESET) {
				pHidSio->resetI2CReq = 1;
			}
			/* handle HID_SPI_REQ_RESET request in IRQ context to abort current
			     transaction and reset the queue states. */
			else if (pOut->req == HID_SPI_REQ_RESET) {
				pHidSio->resetSPIReq = 1;
			}
			/* normal request queue the buffer */
			HID_SIO_IncIndex(&pHidSio->reqWrIndx);
		}

		/* Queue the next buffer for Rx */
		USBD_API->hw->ReadReqEP(hUsb, pHidSio->epout_adr, &pHidSio->rxBuf[0], HID_SIO_PACKET_SZ);
	}
	return LPC_OK;
}

#if (HID_SIO_I2C_PORTS > 0)
static int32_t HID_I2C_HandleReadStates(HID_SIO_CTRL_T *pHidI2c,
										HID_SIO_OUT_REPORT_T *pOut,
										HID_SIO_IN_REPORT_T *pIn,
										uint32_t status)
{
	HID_I2C_RW_PARAMS_T *pRWParam = (HID_I2C_RW_PARAMS_T *) &pOut->data[0];
	int32_t ret = 0;
	uint8_t *buff = &pIn->data[0];
	uint32_t pos;

	switch (status) {
	case 0x58:			/* Data Received and NACK sent */
		buff[pIn->transfer_len++] = Chip_I2CM_ReadByte(pHidI2c->pI2C[pOut->sesId]);
		pIn->resp = HID_SIO_RES_OK;
		ret = 1;
		break;

	case 0x40:			/* SLA+R sent and ACK received */
		pHidI2c->pI2C[pOut->sesId]->CONSET = I2C_CON_AA;

	case 0x50:			/* Data Received and ACK sent */
		if (status == 0x50) {
			buff[pIn->transfer_len++] = Chip_I2CM_ReadByte(pHidI2c->pI2C[pOut->sesId]);
		}
		pos = pIn->transfer_len;
		if (pRWParam->options & HID_I2C_TRANSFER_OPTIONS_NACK_LAST_BYTE) {
			if ((pRWParam->length - pos) == 1) {
				Chip_I2CM_NackNextByte(pHidI2c->pI2C[pOut->sesId]);
			}
		}
		else if (pRWParam->length == pos) {
			pIn->resp = HID_SIO_RES_OK;
		}
		ret = 1;
		break;
	}

	if(pIn->resp == HID_SIO_RES_OK) {
		if(pRWParam->options & HID_I2C_TRANSFER_OPTIONS_STOP_BIT) {
			Chip_I2CM_SendStop(pHidI2c->pI2C[pOut->sesId]);
		}
		/* indicate the transfer is complete */
		pHidI2c->pending = HID_SIO_NOTHING_PENDING;
	}

	return ret;
}

static int32_t HID_I2C_HandleWriteStates(HID_SIO_CTRL_T *pHidI2c,
										 HID_SIO_OUT_REPORT_T *pOut,
										 HID_SIO_IN_REPORT_T *pIn,
										 uint32_t status)
{
	HID_I2C_RW_PARAMS_T *pRWParam = (HID_I2C_RW_PARAMS_T *) &pOut->data[0];
	int32_t ret = 0;
	uint8_t *buff = &pRWParam->data[0];

	switch (status) {
	case 0x30:			/* DATA sent NAK received */
		if (pRWParam->options & HID_I2C_TRANSFER_OPTIONS_BREAK_ON_NACK) {
			pIn->resp = HID_SIO_RES_NAK;
			ret = 1;
			break;
		}		/* else fall through */

	case 0x08:			/* Start condition on bus */
	case 0x10:			/* Repeated start condition */
		if ((pRWParam->options & HID_I2C_TRANSFER_OPTIONS_NO_ADDRESS) == 0) {
			break;
		}		/* else fall-through */

	case 0x18:			/* SLA+W sent and ACK received */
	case 0x28:			/* DATA sent and ACK received */
		if (i2cWritePos < pRWParam->length) {
			Chip_I2CM_WriteByte(pHidI2c->pI2C[pOut->sesId], buff[i2cWritePos]);
			i2cWritePos++;
		}
		else {
			pIn->resp = HID_SIO_RES_OK;
			if(pRWParam->options & HID_I2C_TRANSFER_OPTIONS_STOP_BIT) {
				Chip_I2CM_SendStop(pHidI2c->pI2C[pOut->sesId]);
			}
			/* indicate the transfer is complete */
			pHidI2c->pending = HID_SIO_NOTHING_PENDING;
		}
		ret = 1;
		break;
	}

	return ret;
}

static void HID_I2C_ProcessRWReq(HID_SIO_CTRL_T *pHidI2c,
								 HID_SIO_OUT_REPORT_T *pOut,
								 HID_SIO_IN_REPORT_T *pIn)
{
	HID_I2C_RW_PARAMS_T *pRWParam = (HID_I2C_RW_PARAMS_T *) &pOut->data[0];
	int32_t handled = 0;
	uint32_t status;
	int32_t read = (pOut->req == HID_I2C_REQ_DEVICE_READ);

	if (pIn->resp == HID_SIO_RES_INVALID_CMD) {
		/* wait for status change interrupt */
		if (Chip_I2CM_StateChanged(pHidI2c->pI2C[pOut->sesId])) {

			status = Chip_I2CM_GetCurState(pHidI2c->pI2C[pOut->sesId]);
			/* handle Read or write states. */
			if (read ) {
				handled = HID_I2C_HandleReadStates(pHidI2c, pOut, pIn, status);
			}
			else {
				handled = HID_I2C_HandleWriteStates(pHidI2c, pOut, pIn, status);
			}

			if (handled == 0) {
				/* check status and send data */
				switch (status) {
				case 0x08:		/* Start condition on bus */
				case 0x10:		/* Repeated start condition */
					if ((pRWParam->options & HID_I2C_TRANSFER_OPTIONS_NO_ADDRESS) == 0) {
						Chip_I2CM_WriteByte(pHidI2c->pI2C[pOut->sesId], (pRWParam->slaveAddr << 1) | read);
					}
					break;

				case 0x20:		/* SLA+W sent NAK received */
				case 0x48:		/* SLA+R sent NAK received */
					pIn->resp = HID_SIO_RES_SLAVE_NAK;
					break;

				case 0x38:		/* Arbitration lost */
					pIn->resp = HID_SIO_RES_ARBLOST;
					break;

				case 0x00:		/* Bus Error */
					pIn->resp = HID_SIO_RES_BUS_ERROR;
					break;

				default:		/* we shouldn't be in any other state */
					pIn->resp = HID_SIO_RES_ERROR;
					break;
				}
			}
			/* clear state change interrupt status */
			Chip_I2CM_ClearSI(pHidI2c->pI2C[pOut->sesId]);
		}
		else if (pHidI2c->resetI2CReq != 0) {
			pIn->resp = HID_SIO_RES_ERROR;
		}
	}
	else {

		if ((pIn->resp != HID_SIO_RES_ARBLOST) &&
			(pRWParam->options & HID_I2C_TRANSFER_OPTIONS_STOP_BIT) ) {
			Chip_I2CM_SendStop(pHidI2c->pI2C[pOut->sesId]);
		}
		/* indicate the transfer is complete */
		pHidI2c->pending = HID_SIO_NOTHING_PENDING;
	}
}

static void HID_I2C_HandleXferReq(HID_SIO_CTRL_T *pHidI2c,
								  HID_SIO_OUT_REPORT_T *pOut,
								  HID_SIO_IN_REPORT_T *pIn)
{
	HID_I2C_XFER_PARAMS_T *pXfrParam = (HID_I2C_XFER_PARAMS_T *) &pOut->data[0];
	I2CM_XFER_T *pXfer = &pHidI2c->i2c_xfer;

	memset(pXfer, 0, sizeof(I2CM_XFER_T));
	pXfer->slaveAddr = pXfrParam->slaveAddr;
	pXfer->txBuff = &pXfrParam->data[0];
	pXfer->rxBuff = &pIn->data[0];
	pXfer->rxSz = pXfrParam->rxLength;
	pXfer->txSz = pXfrParam->txLength;
	pXfer->options = pXfrParam->options;

	/* start transfer */
	Chip_I2CM_Xfer(pHidI2c->pI2C[pOut->sesId], pXfer);
}

static void HID_I2C_ProcessXferReq(HID_SIO_CTRL_T *pHidI2c,
								   HID_SIO_OUT_REPORT_T *pOut,
								   HID_SIO_IN_REPORT_T *pIn)
{
	HID_I2C_XFER_PARAMS_T *pXfrParam = (HID_I2C_XFER_PARAMS_T *) &pOut->data[0];
	uint32_t ret = 0;
	I2CM_XFER_T *pXfer = &pHidI2c->i2c_xfer;

	if (Chip_I2CM_StateChanged(pHidI2c->pI2C[pOut->sesId])) {
		/* call state change handler */
		ret = Chip_I2CM_XferHandler(pHidI2c->pI2C[pOut->sesId], pXfer);
		if (ret != 0) {
			pHidI2c->pending = HID_SIO_NOTHING_PENDING;
		}
	}
	else if (pHidI2c->resetI2CReq != 0) {
		pXfer->status = HID_SIO_RES_ERROR;
		pHidI2c->pending = HID_SIO_NOTHING_PENDING;
	}

	if (pHidI2c->pending == HID_SIO_NOTHING_PENDING) {
		/* Update the length we have to send back */
		if ((pXfrParam->rxLength - pXfer->rxSz) > 0) {
			pIn->transfer_len += pXfrParam->rxLength - pXfer->rxSz;
		}
		/* update response with the I2CM status returned. No translation
		   needed as they map directly to base LPCUSBSIO status. */
		pIn->resp = pXfer->status;
	}
}

#endif /* #if (HID_SIO_I2C_PORTS > 0)*/

#if (HID_SIO_SPI_PORTS > 0)
static void HID_SPI_HandleXferReq(HID_SIO_CTRL_T *pHidSPI,
								  HID_SIO_OUT_REPORT_T *pOut,
								  HID_SIO_IN_REPORT_T *pIn)
{
	HID_SPI_XFER_PARAMS_T *pXfrParam = (HID_SPI_XFER_PARAMS_T *) &pOut->data[0];
	Chip_SSP_DATA_SETUP_T *pSPIXfer = &pHidSPI->spi_xfer;

	Chip_GPIO_SetPinState(LPC_GPIO_PORT, HID_SPI_DEVICE_TO_PORT(pXfrParam->device),
						  HID_SPI_DEVICE_TO_PIN(pXfrParam->device), false);
	/* Wait for Pre Delay time */
	StopWatch_DelayUs(spiPreDelay);
	memset(pSPIXfer, 0, sizeof(Chip_SSP_DATA_SETUP_T));
	/* Update SPI driver structure */
	pSPIXfer->length = pXfrParam->length;
	pSPIXfer->tx_data = &pXfrParam->data[0];
	pSPIXfer->rx_data = &pIn->data[0];
	pSPIXfer->rx_cnt = 0;
	pSPIXfer->tx_cnt = 0;
	/* Clear any previous unread data */
	Chip_SSP_Int_FlushData(pHidSPI->pSSP[pOut->sesId]);
}

static void HID_SPI_ProcessXferReq(HID_SIO_CTRL_T *pHidSPI,
								   HID_SIO_OUT_REPORT_T *pOut,
								   HID_SIO_IN_REPORT_T *pIn)
{
	HID_SPI_XFER_PARAMS_T *pXfrParam = (HID_SPI_XFER_PARAMS_T *) &pOut->data[0];
	Chip_SSP_DATA_SETUP_T *pSPIXfer = &pHidSPI->spi_xfer;
	uint8_t xfer_size;
	Status xfer_status;

	xfer_size = Chip_SSP_GetDataSize(pHidSPI->pSSP[pOut->sesId]) + 1;
	if (xfer_size > 8) {
		/* 16 bit data transfer */
		xfer_status = Chip_SSP_Int_RWFrames16Bits(pHidSPI->pSSP[pOut->sesId], pSPIXfer);
	}
	else {
		/* 8 bit data transfer */
		xfer_status = Chip_SSP_Int_RWFrames8Bits(pHidSPI->pSSP[pOut->sesId], pSPIXfer);
	}
	if ((pHidSPI->resetSPIReq) || !xfer_status) {
		/* If we receive reset request then update error code and end processing */
		Chip_SSP_Int_FlushData(pHidSPI->pSSP[pOut->sesId]);
		/* Release SSEL line */
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, HID_SPI_DEVICE_TO_PORT(pXfrParam->device),
							  HID_SPI_DEVICE_TO_PIN(pXfrParam->device), true);
		pIn->resp = HID_SIO_RES_ERROR;
		pHidSPI->pending = HID_SIO_NOTHING_PENDING;
	}
	/* Check if transfer is complete */
	else if ((pSPIXfer->tx_cnt >= pSPIXfer->length) && (pSPIXfer->rx_cnt >= pSPIXfer->length)) {
		/* Wait until the controller completes the current transfer */
		while (Chip_SSP_GetStatus(pHidSPI->pSSP[pOut->sesId], SSP_STAT_BSY)) {}
		/* Wait for Post Delay time */
		StopWatch_DelayUs(spiPostDelay);
		/* Release SSEL line */
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, HID_SPI_DEVICE_TO_PORT(pXfrParam->device),
							  HID_SPI_DEVICE_TO_PIN(pXfrParam->device), true);
		/* Prepare SIO response */
		pIn->transfer_len += pSPIXfer->rx_cnt;
		pIn->resp = HID_SIO_RES_OK;
		pHidSPI->pending = HID_SIO_NOTHING_PENDING;
	}
}

#endif /* #if (HID_SIO_SPI_PORTS > 0) */

/* Handle SIO requests */
void SIO_RequestHandler(USBD_HANDLE_T hSIOHid, HID_SIO_OUT_REPORT_T *pOut, HID_SIO_IN_REPORT_T *pIn)
{
	switch (pOut->req) {
	case HID_SIO_REQ_DEV_INFO:
		/* Set I2C ports available on this part */
		pIn->data[0] = HID_SIO_I2C_PORTS;
		/* Set SPI ports available on this part */
		pIn->data[1] = HID_SIO_SPI_PORTS;
		pIn->data[2] = HID_SIO_GPIO_PORTS;
		*((uint32_t *) &pIn->data[4]) = HID_SIO_MAX_DATA_SIZE;
		*((uint32_t *) &pIn->data[8]) = (HID_SIO_MAJOR_VER << 16) | HID_SIO_MINOR_VER;
		/* send back firmware version string */
		memcpy(&pIn->data[12], g_fwVersion, strlen(g_fwVersion));
		pIn->transfer_len += strlen(g_fwVersion) + 12;

		/* update response */
		pIn->resp = HID_SIO_RES_OK;
		break;
	}
}

/* Handle I2C requests */
void I2C_RequestHandler(USBD_HANDLE_T hSIOHid, HID_SIO_OUT_REPORT_T *pOut, HID_SIO_IN_REPORT_T *pIn)
{
#if (HID_SIO_I2C_PORTS > 0)
	HID_SIO_CTRL_T *pHidI2c = (HID_SIO_CTRL_T *) hSIOHid;
	HID_I2C_PORTCONFIG_T *pCfg;
	HID_I2C_RW_PARAMS_T *pRWParam = (HID_I2C_RW_PARAMS_T *) &pOut->data[0];

	switch (pOut->req) {
	case HID_I2C_REQ_INIT_PORT:
		pCfg = (HID_I2C_PORTCONFIG_T *) &pOut->data[0];
		/* bind to I2C port */
		InitI2CPorts(pHidI2c, pOut->sesId, pCfg->busSpeed);
		/* Init I2C port */
		Chip_I2CM_Init(pHidI2c->pI2C[pOut->sesId]);
		Chip_I2CM_SetBusSpeed(pHidI2c->pI2C[pOut->sesId], pCfg->busSpeed);

		/* update response */
		pIn->resp = HID_SIO_RES_OK;
		break;

	case HID_I2C_REQ_DEINIT_PORT:
		Chip_I2CM_DeInit(pHidI2c->pI2C[pOut->sesId]);
		DeInitI2CPorts(pHidI2c, pOut->sesId);
		/* update response */
		pIn->resp = HID_SIO_RES_OK;
		break;

	case HID_I2C_REQ_DEVICE_WRITE:
	case HID_I2C_REQ_DEVICE_READ:
		/* set type of I2C operation pending */
		pHidI2c->pending = HID_SIO_I2C_RW_PENDING;
		/* clear state change interrupt status */
		Chip_I2CM_ClearSI(pHidI2c->pI2C[pOut->sesId]);
		/* Reset read write buffer position to zero */
		i2cWritePos = 0;

		if (pRWParam->options & HID_I2C_TRANSFER_OPTIONS_START_BIT) {
			Chip_I2CM_SendStart(pHidI2c->pI2C[pOut->sesId]);
		}
		break;

	case HID_I2C_REQ_DEVICE_XFER:
		pHidI2c->pending = HID_SIO_I2C_XFER_PENDING;
		HID_I2C_HandleXferReq(pHidI2c, pOut, pIn);
		break;

	case HID_I2C_REQ_RESET:
		Chip_I2CM_ResetControl(pHidI2c->pI2C[pOut->sesId]);
		Chip_I2CM_SendStartAfterStop(pHidI2c->pI2C[pOut->sesId]);
		Chip_I2CM_WriteByte(pHidI2c->pI2C[pOut->sesId], 0xFF);
		Chip_I2CM_SendStop(pHidI2c->pI2C[pOut->sesId]);
		pHidI2c->resetI2CReq = 0;
		/* update response */
		pIn->resp = HID_SIO_RES_OK;
		break;

	}
#endif
}

/******* Handle GPIO requests ****************************************/
void GPIO_RequestHandler(USBD_HANDLE_T hSIOHid, HID_SIO_OUT_REPORT_T *pOut, HID_SIO_IN_REPORT_T *pIn)
{
	uint32_t temp, value;

	switch (pOut->req) {

	case HID_GPIO_REQ_PORT_VALUE:

		/* set drive high pins */
		temp = *(uint32_t *) &pOut->data[0];
		Chip_GPIO_SetValue(LPC_GPIO_PORT, pOut->sesId, temp);

		/* clear drive low pins */
		temp = *(uint32_t *) &pOut->data[4];
		Chip_GPIO_ClearValue(LPC_GPIO_PORT, pOut->sesId, temp);

		/* update response */
		*((uint32_t *) &pIn->data[0]) = Chip_GPIO_GetPortValue(LPC_GPIO_PORT, pOut->sesId);
		pIn->transfer_len += 4;
		pIn->resp = HID_SIO_RES_OK;
		break;

	case HID_GPIO_REQ_PORT_DIR:
		/* get current settings */
		value = LPC_GPIO_PORT->DIR[pOut->sesId];
		/* Set direction */
		temp = *(uint32_t *) &pOut->data[0];
		value |= temp;
		/* clear input pins */
		temp = *(uint32_t *) &pOut->data[4];
		value &= ~temp;
		/* update the register */
		LPC_GPIO_PORT->DIR[pOut->sesId] = value;
		/* update response */
		*((uint32_t *) &pIn->data[0]) = LPC_GPIO_PORT->DIR[pOut->sesId];
		pIn->transfer_len += 4;
		pIn->resp = HID_SIO_RES_OK;
		break;

	case HID_GPIO_REQ_TOGGLE_PIN:
		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, pOut->sesId, pOut->data[0]);
		Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, pOut->sesId, pOut->data[0]);
		/* update response */
		pIn->resp = HID_SIO_RES_OK;
		break;

	case HID_GPIO_REQ_IOCONFIG:
		Chip_SCU_PinMuxSet(pOut->sesId, pOut->data[4], *(uint32_t *) &pOut->data[0]);
		/* update response */
		pIn->resp = HID_SIO_RES_OK;
		break;
	}

}

/* Handle SPI requests */
void SPI_RequestHandler(USBD_HANDLE_T hSIOHid, HID_SIO_OUT_REPORT_T *pOut, HID_SIO_IN_REPORT_T *pIn)
{
#if (HID_SIO_SPI_PORTS > 0)
	HID_SIO_CTRL_T              *pHidSPI = (HID_SIO_CTRL_T *) hSIOHid;
	HID_SPI_PORTCONFIG_T    *pCfg;

	switch (pOut->req) {
	case HID_SPI_REQ_INIT_PORT:
		/* bind to SPI port */
		InitSPIPorts(pHidSPI, pOut->sesId);
		/* Init SPI port */
		Chip_SSP_Init(pHidSPI->pSSP[pOut->sesId]);
		pCfg = (HID_SPI_PORTCONFIG_T *) &pOut->data[0];
		/* Set SPI frame size and clock polarity and phase */
		Chip_SSP_SetFormat(pHidSPI->pSSP[pOut->sesId],
						   pCfg->Options & HID_SPI_CONFIG_OPTION_DATA_SIZE_16,
						   SSP_FRAMEFORMAT_SPI,
						   pCfg->Options & (HID_SPI_CONFIG_OPTION_POL_1 | HID_SPI_CONFIG_OPTION_PHA_1));
		/* Set SPI Clock speed */
		Chip_SSP_SetBitRate(pHidSPI->pSSP[pOut->sesId], pCfg->busSpeed);
		Chip_SSP_Enable(pHidSPI->pSSP[pOut->sesId]);
		spiPreDelay = (pCfg->Options >> 8) & 0x00FF;
		spiPostDelay = (pCfg->Options >> 16) & 0x00FF;

		/* update response */
		pIn->resp = HID_SIO_RES_OK;
		break;

	case HID_SPI_REQ_DEINIT_PORT:
		Chip_SSP_DeInit(pHidSPI->pSSP[pOut->sesId]);
		DeInitSPIPorts(pHidSPI, pOut->sesId);
		/* update response */
		pIn->resp = HID_SIO_RES_OK;
		break;

	case HID_SPI_REQ_DEVICE_XFER:
		pHidSPI->pending = HID_SIO_SSP_XFER_PENDING;
		HID_SPI_HandleXferReq(pHidSPI, pOut, pIn);
		break;

	case HID_SPI_REQ_RESET:
		Chip_SSP_Int_FlushData(pHidSPI->pSSP[pOut->sesId]);
		/* Clear status */
		Chip_SSP_ClearIntPending(pHidSPI->pSSP[pOut->sesId], SSP_INT_CLEAR_BITMASK);
		Chip_SSP_Disable(pHidSPI->pSSP[pOut->sesId]);
		Chip_SSP_Enable(pHidSPI->pSSP[pOut->sesId]);
		pHidSPI->resetSPIReq = 0;
		/* update response */
		pIn->resp = HID_SIO_RES_OK;
		break;

	}
#endif
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* HID init routine */
ErrorCode_t HID_SIO_init(USBD_HANDLE_T hUsb,
						 USB_INTERFACE_DESCRIPTOR *pIntfDesc,
						 USBD_API_INIT_PARAM_T *pUsbParam,
						 USBD_HANDLE_T *pSIOHid)
{
	USBD_HID_INIT_PARAM_T hid_param;
	USB_HID_REPORT_T reports_data[1];
	USB_ENDPOINT_DESCRIPTOR *pEpDesc;
	uint32_t new_addr, i, ep_indx;
	HID_SIO_CTRL_T *pHidSio;
	ErrorCode_t ret = LPC_OK;

	if ((pIntfDesc == 0) || (pIntfDesc->bInterfaceClass != USB_DEVICE_CLASS_HUMAN_INTERFACE)) {
		return ERR_FAILED;
	}

	/* HID paramas */
	memset((void *) &hid_param, 0, sizeof(USBD_HID_INIT_PARAM_T));
	hid_param.max_reports = 1;
	/* Init reports_data */
	reports_data[0].len = HID_SIO_ReportDescSize;
	reports_data[0].idle_time = 0;
	reports_data[0].desc = (uint8_t *) &HID_SIO_ReportDescriptor[0];

	hid_param.mem_base = pUsbParam->mem_base;
	hid_param.mem_size = pUsbParam->mem_size;
	hid_param.intf_desc = (uint8_t *) pIntfDesc;
	/* user defined functions */
	hid_param.HID_GetReport = HID_SIO_GetReport;
	hid_param.HID_SetReport = HID_SIO_SetReport;
	hid_param.report_data  = reports_data;

	ret = USBD_API->hid->init(hUsb, &hid_param);
	if (ret == LPC_OK) {
		/* allocate memory for HID_SIO_CTRL_T instance */
		pHidSio = (HID_SIO_CTRL_T *) hid_param.mem_base;
		hid_param.mem_base += sizeof(HID_SIO_CTRL_T);
		hid_param.mem_size -= sizeof(HID_SIO_CTRL_T);
		/* update memory variables */
		pUsbParam->mem_base = hid_param.mem_base;
		pUsbParam->mem_size = hid_param.mem_size;

		/* Init the control structure */
		memset(pHidSio, 0, sizeof(HID_SIO_CTRL_T));
		/* store stack handle*/
		pHidSio->hUsb = hUsb;
		/* set return handle */
		*pSIOHid = (USBD_HANDLE_T) pHidSio;
		/* set response is idling. For HID_I2C_process() to kickstart transmission if response
		   data is pending. */
		pHidSio->respIdle = 1;

		StopWatch_Init();

		/* move to next descriptor */
		new_addr = (uint32_t) pIntfDesc + pIntfDesc->bLength;
		/* loop through descriptor to find EPs */
		for (i = 0; i < pIntfDesc->bNumEndpoints; ) {
			pEpDesc = (USB_ENDPOINT_DESCRIPTOR *) new_addr;
			new_addr = (uint32_t) pEpDesc + pEpDesc->bLength;

			/* parse endpoint descriptor */
			if ((pEpDesc->bDescriptorType == USB_ENDPOINT_DESCRIPTOR_TYPE) &&
				(pEpDesc->bmAttributes == USB_ENDPOINT_TYPE_INTERRUPT)) {

				/* find next endpoint */
				i++;

				if (pEpDesc->bEndpointAddress & USB_ENDPOINT_DIRECTION_MASK) {
					/* store Interrupt IN endpoint */
					pHidSio->epin_adr = pEpDesc->bEndpointAddress;
					ep_indx = ((pHidSio->epin_adr & 0x0F) << 1) + 1;
					/* register endpoint interrupt handler if provided*/
					ret = USBD_API->core->RegisterEpHandler(hUsb, ep_indx, HID_SIO_EpIn_Hdlr, pHidSio);
				}
				else {
					/* store Interrupt OUT endpoint */
					pHidSio->epout_adr = pEpDesc->bEndpointAddress;
					ep_indx = ((pHidSio->epout_adr & 0x0F) << 1);
					/* register endpoint interrupt handler if provided*/
					ret = USBD_API->core->RegisterEpHandler(hUsb, ep_indx, HID_SIO_EpOut_Hdlr, pHidSio);
				}
			}
		}

	}

	return ret;
}

/* Process HID_I2C request and response queue. */
uint8_t HID_SIO_process(USBD_HANDLE_T hSIOHid)
{
	HID_SIO_CTRL_T *pHidSio = (HID_SIO_CTRL_T *) hSIOHid;
	HID_SIO_OUT_REPORT_T *pOut;
	HID_SIO_IN_REPORT_T *pIn;
	// uint8_t len;

	/* Check if we are connected to a host */
	if (USB_IsConfigured(pHidSio->hUsb)) {

		/* check if we just got connected */
		if (pHidSio->state == HID_SIO_STATE_DISCON) {
			/* queue the request */
			USBD_API->hw->ReadReqEP(pHidSio->hUsb, pHidSio->epout_adr, &pHidSio->rxBuf[0], HID_SIO_PACKET_SZ);
		}
		/* set state to connected */
		pHidSio->state = HID_SIO_STATE_CONNECTED;
		if (pHidSio->reqWrIndx != pHidSio->reqRdIndx) {

			/* process the current packet */
			pOut = (HID_SIO_OUT_REPORT_T *) &pHidSio->reqQ[pHidSio->reqRdIndx][0];
			pIn = (HID_SIO_IN_REPORT_T *) &pHidSio->respQ[pHidSio->respWrIndx][0];
			/* construct response template */
			pIn->transId = pOut->transId;
			pIn->sesId = pOut->sesId;

			if (pHidSio->pending == HID_SIO_I2C_XFER_PENDING) {
#if (HID_SIO_I2C_PORTS > 0)
				/* check if previous I2C transfer is still pending. If so process that. */
				HID_I2C_ProcessXferReq(pHidSio, pOut, pIn);
			}
			else if (pHidSio->pending == HID_SIO_I2C_RW_PENDING) {
				/* check if previous I2C transfer is still pending. If so process that. */
				HID_I2C_ProcessRWReq(pHidSio, pOut, pIn);
#endif
			}
			else if (pHidSio->pending == HID_SIO_SSP_XFER_PENDING) {
#if (HID_SIO_SPI_PORTS > 0)
				/* check if previous SSP transfer is still pending. If so process that. */
				HID_SPI_ProcessXferReq(pHidSio, pOut, pIn);
#endif
			}
			else {
				/* Reset response length to zero for the iteration of a new request */
				pIn->transfer_len = 0;
				pIn->resp = HID_SIO_RES_INVALID_CMD;

				if (pOut->req < HID_I2C_REQ_MAX) {
#if (HID_SIO_I2C_PORTS > 0)
					/* Handle I2C requests only if valid port is selected. */
					if (pOut->sesId < HID_SIO_I2C_PORTS) {

						I2C_RequestHandler(hSIOHid, pOut, pIn);
					}
#endif
				}
				else if (pOut->req < HID_SPI_REQ_MAX) {
#if (HID_SIO_SPI_PORTS > 0)
					/* Handle SPI requests only if valid port is selected. */
					if (pOut->sesId < HID_SIO_SPI_PORTS) {

						SPI_RequestHandler(hSIOHid, pOut, pIn);
					}
#endif
				}
				else if (pOut->req < HID_GPIO_REQ_MAX) {
					/* Handle GPIO requests */
					GPIO_RequestHandler(hSIOHid, pOut, pIn);

				}
				else {
					/* Handle SIO requests */
					SIO_RequestHandler(hSIOHid, pOut, pIn);
				}
			}
			if (pHidSio->pending == HID_SIO_NOTHING_PENDING) {
				/* Update SIO Header */
				pIn->transfer_len = HID_SIO_CALC_TRANSFER_LEN(pIn->transfer_len);
				pIn->packet_num = 0;
				if (pIn->transfer_len > HID_SIO_PACKET_SZ) {
					pIn->packet_len = HID_SIO_PACKET_SZ;
				}
				else {
					pIn->packet_len = pIn->transfer_len;
				}
				HID_SIO_IncIndex(&pHidSio->reqRdIndx);
				HID_SIO_IncIndex(&pHidSio->respWrIndx);
			}
		}

		/* Kick-start response tx if it is idling and we have something to send. */
		if ((pHidSio->respIdle) && (pHidSio->respRdIndx != pHidSio->respWrIndx)) {

			pHidSio->respIdle = 0;
			pIn = (HID_SIO_IN_REPORT_T *) &pHidSio->respQ[pHidSio->respRdIndx][0];
			memcpy(&pHidSio->txBuf[0], pIn, pIn->packet_len);
			/* If there is no more packets to transmit then increment index */
			if ((pIn->packet_num * HID_SIO_PACKET_SZ + pIn->packet_len) == pIn->transfer_len) {
				HID_SIO_IncIndex(&pHidSio->respRdIndx);
			}
			else {
				/* Otherwise update header for next packet transfer */
				if (pIn->transfer_len - (pIn->packet_num * HID_SIO_PACKET_SZ + pIn->packet_len ) > HID_SIO_PACKET_SZ) {
					pIn->packet_len = HID_SIO_PACKET_SZ;
				}
				else {
					pIn->packet_len = pIn->transfer_len - (pIn->packet_num * HID_SIO_PACKET_SZ + pIn->packet_len);
				}
				pIn->packet_num++;
			}
			USBD_API->hw->WriteEP(pHidSio->hUsb,
								  pHidSio->epin_adr,
								  &pHidSio->txBuf[0],
								  HID_SIO_PACKET_SZ);
		}
	}
	else {
		/* check if we just got dis-connected */
		if (pHidSio->state != HID_SIO_STATE_DISCON) {
			/* reset indexes */
			pHidSio->reqWrIndx = pHidSio->reqRdIndx = 0;
			pHidSio->respRdIndx = pHidSio->respWrIndx = 0;
			pHidSio->pending = HID_SIO_NOTHING_PENDING;
			pHidSio->respIdle = 1;
			pHidSio->resetI2CReq = 0;
			pHidSio->resetSPIReq = 0;
		}
		pHidSio->state = HID_SIO_STATE_DISCON;
	}
	return pHidSio->pending;
}






