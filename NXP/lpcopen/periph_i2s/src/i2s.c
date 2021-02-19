/*
 * @brief I2S example
 * This example show how to use the I2S in 3 modes : Polling, Interrupt and DMA
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

#if (!defined(BOARD_NXP_LPCXPRESSO_4337) && !defined(BOARD_NXP_LPCXPRESSO_1837))
#define CODEC_I2S_BUS   LPC_I2S0
#define I2S_IRQ         I2S0_IRQn
#define AUDCFG_SAMPLE_RATE 48000
#define I2S_IRQHandler  I2S0_IRQHandler
#define CODEC_INPUT_DEVICE UDA1380_LINE_IN
#define I2S_DMA_TX_CHAN GPDMA_CONN_I2S_Tx_Channel_0
#define I2S_DMA_RX_CHAN GPDMA_CONN_I2S_Rx_Channel_1
#else
#define CODEC_I2S_BUS   LPC_I2S1
#define I2S_IRQ         I2S1_IRQn
#define I2S_IRQHandler  I2S1_IRQHandler
#define CODEC_INPUT_DEVICE CODEC_LINE_IN
#define I2S_DMA_TX_CHAN GPDMA_CONN_I2S1_Tx_Channel_0
#define I2S_DMA_RX_CHAN GPDMA_CONN_I2S1_Rx_Channel_1
#define I2S_SLAVE_RX
#define I2S_SLAVE_TX
#undef DEBUG_ENABLE
#endif

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define BUFFER_FULL 0
#define BUFFER_EMPTY 1
#define BUFFER_AVAILABLE 2

typedef struct ring_buff {
	uint32_t buffer[256];
	uint8_t read_index;
	uint8_t write_index;
} Ring_Buffer_t;

static char WelcomeMenu[] = "\r\nHello NXP Semiconductors \r\n"
							"I2S DEMO : Connect audio headphone out from computer to line-in on tested board to get audio signal\r\n"
							"Please press \'1\' to test Polling mode\r\n"
							"Please press \'2\' to test Interrupt mode\r\n"
							"Please press \'3\' to test DMA mode\r\n"
							"Please press \'x\' to exit test mode\r\n"
							"Please press \'m\' to DISABLE/ENABLE mute\r\n";

static Ring_Buffer_t ring_buffer;

static uint8_t send_flag;
static uint8_t channelTC;
static uint8_t dmaChannelNum_I2S_Tx, dmaChannelNum_I2S_Rx;
static uint8_t dma_send_receive;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/
uint8_t mute_status = 0;

/* Get input from console */
int Con_GetInput(void)
{
#ifdef DEBUG_ENABLE
	return DEBUGIN();
#else
	return '3';
#endif
}

static void mute_toggle()
{
	mute_status = !mute_status;
	if (mute_status) {
		Chip_I2S_EnableMute(CODEC_I2S_BUS);
		DEBUGOUT("MUTE ENABLE\n\r");
	}
	else {
		Chip_I2S_DisableMute(CODEC_I2S_BUS);
		DEBUGOUT("MUTE DISABLE\n\r");
	}
}

/* Get status of the ring buffer */
static uint8_t ring_buff_get_status(Ring_Buffer_t *ring_buff)
{
	if (ring_buff->read_index == ring_buff->write_index) {
		return BUFFER_EMPTY;
	}
	else if (ring_buff->read_index == (ring_buff->write_index) + 1) {
		return BUFFER_FULL;
	}
	else {return BUFFER_AVAILABLE; }
}

/* Interrupt routine for I2S example */
static void App_Interrupt_Test(void)
{
	uint8_t bufferUART, continue_Flag = 1;
	DEBUGOUT("I2S Interrupt mode\r\n");
	Chip_I2S_Int_RxCmd(CODEC_I2S_BUS, ENABLE, 4);
	Chip_I2S_Int_TxCmd(CODEC_I2S_BUS, ENABLE, 4);
	NVIC_EnableIRQ(I2S_IRQ);
	while (continue_Flag) {
		bufferUART = 0xFF;
		bufferUART = Con_GetInput();
		switch (bufferUART) {
		case 'x':
			continue_Flag = 0;
			Chip_I2S_Int_RxCmd(CODEC_I2S_BUS, DISABLE, 4);
			NVIC_DisableIRQ(I2S_IRQ);
			DEBUGOUT(WelcomeMenu);
			break;

		case 'm':
			mute_toggle();
			break;

		default:
			break;
		}
	}
}

/* Polling routine for I2S example */
static void App_Polling_Test(void)
{
	uint32_t polling_data = 0;
	uint8_t bufferUART, continue_Flag = 1;
	DEBUGOUT("I2S Polling mode\r\n");
	while (continue_Flag) {
		bufferUART = 0xFF;
		bufferUART = Con_GetInput();
		switch (bufferUART) {
		case 'x':
			continue_Flag = 0;
			DEBUGOUT(WelcomeMenu);
			break;

		case 'm':
			mute_toggle();
			break;

		default:
			break;
		}

		if (Chip_I2S_GetRxLevel(CODEC_I2S_BUS) > 0) {
			polling_data = Chip_I2S_Receive(CODEC_I2S_BUS);
			send_flag = 1;
		}
		if ((Chip_I2S_GetTxLevel(CODEC_I2S_BUS) < 4) && (send_flag == 1)) {
			Chip_I2S_Send(CODEC_I2S_BUS, polling_data);
			send_flag = 0;
		}
	}
}

/* DMA routine for I2S example */
static void App_DMA_Test(void)
{
	uint8_t continue_Flag = 1, bufferUART = 0xFF;
	Chip_I2S_DMA_TxCmd(CODEC_I2S_BUS, I2S_DMA_REQUEST_CHANNEL_1, ENABLE, 4);
	Chip_I2S_DMA_RxCmd(CODEC_I2S_BUS, I2S_DMA_REQUEST_CHANNEL_2, ENABLE, 4);
	/* Initialize GPDMA controller */
	Chip_GPDMA_Init(LPC_GPDMA);
	/* Setting GPDMA interrupt */
	NVIC_DisableIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, ((0x01 << 3) | 0x01));
	NVIC_EnableIRQ(DMA_IRQn);

	dmaChannelNum_I2S_Rx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, I2S_DMA_RX_CHAN);

	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNum_I2S_Rx,
					  I2S_DMA_RX_CHAN,
					  I2S_DMA_TX_CHAN,
					  GPDMA_TRANSFERTYPE_P2P_CONTROLLER_SrcPERIPHERAL,
					  1);

DEBUGOUT("I2S DMA mode\r\n");
	while (continue_Flag) {
		bufferUART = 0xFF;
		bufferUART = Con_GetInput();
		switch (bufferUART) {
		case 'x':
			continue_Flag = 0;
			Chip_I2S_DMA_RxCmd(CODEC_I2S_BUS, I2S_DMA_REQUEST_CHANNEL_2, DISABLE, 1);
			Chip_I2S_DMA_TxCmd(CODEC_I2S_BUS, I2S_DMA_REQUEST_CHANNEL_1, DISABLE, 1);

			Chip_GPDMA_Stop(LPC_GPDMA, dmaChannelNum_I2S_Rx);
			NVIC_DisableIRQ(DMA_IRQn);
			DEBUGOUT(WelcomeMenu);
			break;

		case 'm':
			mute_toggle();
			break;

		default:
			break;
		}
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	DMA interrupt handler sub-routine
 * @return	Nothing
 */
void DMA_IRQHandler(void)
{
	if (dma_send_receive == 1) {
		if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChannelNum_I2S_Rx) == SUCCESS) {
			channelTC++;
		}
		else {
			/* Process error here */
		}
	}
	else {
		if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChannelNum_I2S_Tx) == SUCCESS) {
			channelTC++;
		}
		else {
			/* Process error here */
		}
	}
}

/**
 * @brief	I2S0 interrupt handler sub-routine
 * @return	Nothing
 */
void I2S_IRQHandler(void)
{
	while ((ring_buff_get_status(&ring_buffer) != BUFFER_FULL) && (Chip_I2S_GetRxLevel(CODEC_I2S_BUS) > 0)) {
		ring_buffer.buffer[ring_buffer.write_index++] = Chip_I2S_Receive(CODEC_I2S_BUS);
	}
	while ((ring_buff_get_status(&ring_buffer) != BUFFER_EMPTY) && (Chip_I2S_GetTxLevel(CODEC_I2S_BUS) < 8)) {
		Chip_I2S_Send(CODEC_I2S_BUS, ring_buffer.buffer[ring_buffer.read_index++]);
	}
}

/**
 * @brief  Main routine for I2S example
 * @return Nothing
 */
int main(void)
{

	I2S_AUDIO_FORMAT_T audio_Confg;
	uint8_t bufferUART, continue_Flag = 1;
	audio_Confg.SampleRate = AUDCFG_SAMPLE_RATE;
	/* Select audio data is 2 channels (1 is mono, 2 is stereo) */
	audio_Confg.ChannelNumber = 2;
	/* Select audio data is 16 bits */
	audio_Confg.WordWidth = 16;

	SystemCoreClockUpdate();
	Board_Init();
	Board_Audio_Init(CODEC_I2S_BUS, CODEC_INPUT_DEVICE);
#if defined( __GNUC__ )
	__sys_write(0, "", 0);
#endif

	DEBUGOUT(WelcomeMenu);

	Chip_I2S_Init(CODEC_I2S_BUS);
	Chip_I2S_RxConfig(CODEC_I2S_BUS, &audio_Confg);
	Chip_I2S_TxConfig(CODEC_I2S_BUS, &audio_Confg);
#if defined(I2S_SLAVE_RX)
	Chip_I2S_RxSlave(CODEC_I2S_BUS);
	Chip_I2S_SetRxXYDivider(CODEC_I2S_BUS, 0, 0);
	Chip_I2S_SetRxBitRate(CODEC_I2S_BUS, 0);
	Chip_I2S_RxModeConfig(CODEC_I2S_BUS, 0, I2S_RXMODE_4PIN_ENABLE, 0);
#endif
#if defined(I2S_SLAVE_TX)
	Chip_I2S_TxSlave(CODEC_I2S_BUS);
	Chip_I2S_SetTxXYDivider(CODEC_I2S_BUS, 0, 0);
	Chip_I2S_SetTxBitRate(CODEC_I2S_BUS, 0);
#endif
	Chip_I2S_TxStop(CODEC_I2S_BUS);
	Chip_I2S_DisableMute(CODEC_I2S_BUS);
	Chip_I2S_TxStart(CODEC_I2S_BUS);
	send_flag = 0;
	while (continue_Flag) {
		bufferUART = 0xFF;
		bufferUART = Con_GetInput();
		switch (bufferUART) {
		case '1':
			App_Polling_Test();
			break;

		case '2':
			App_Interrupt_Test();
			break;

		case '3':
			App_DMA_Test();
			break;

		case 'x':
			continue_Flag = 0;
			DEBUGOUT("Thanks for using\r\n");
			break;

		default:
			break;
		}
	}
	return 0;
}






