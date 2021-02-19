/*
 * @brief UART interrupt example with ring buffers
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

#include "chip.h"
#include "board.h"
#include "string.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Transmit and receive ring buffers */
STATIC RINGBUFF_T txring, rxring;

/* Transmit and receive ring buffer sizes */
#define UART_SRB_SIZE 128	/* Send */
#define UART_RRB_SIZE 32	/* Receive */

/* Transmit and receive buffers */
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];

const char inst1[] = "LPC18xx/43xx UART example using ring buffers\r\n";
const char inst2[] = "Press a key to echo it back or ESC to quit\r\n";

#if (defined(BOARD_NGX_XPLORER_1830) || defined(BOARD_NGX_XPLORER_4330))
/* Use UART0 for NGX boards */
#define LPC_UARTX       LPC_USART0
#define UARTx_IRQn      USART0_IRQn
#define UARTx_IRQHandler UART0_IRQHandler
#endif

#if (defined(BOARD_NXP_LPCXPRESSO_4337) || defined(BOARD_NXP_LPCXPRESSO_1837))
/* Use UART0 for LPC-Xpresso boards */
#define LPC_UARTX       LPC_USART0
#define UARTx_IRQn      USART0_IRQn
#define UARTx_IRQHandler UART0_IRQHandler
#endif

#if (defined(BOARD_KEIL_MCB_1857) || defined(BOARD_KEIL_MCB_4357))
/* Use UART3 for Keil boards */
#define LPC_UARTX       LPC_USART3
#define UARTx_IRQn      USART3_IRQn
#define UARTx_IRQHandler UART3_IRQHandler
#endif
#if (defined(BOARD_HITEX_EVA_1850) || defined(BOARD_HITEX_EVA_4350))
/* Use UART0 for Hitex boards */
#define LPC_UARTX       LPC_USART0
#define UARTx_IRQn      USART0_IRQn
#define UARTx_IRQHandler UART0_IRQHandler
#endif
#if defined(BOARD_NXP_LPCLINK2_4370)
#define LPC_UARTX       LPC_USART2
#define UARTx_IRQn      USART2_IRQn
#define UARTx_IRQHandler UART2_IRQHandler
#endif

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	UART interrupt handler using ring buffers
 * @return	Nothing
 */
void UARTx_IRQHandler(void)
{
	/* Want to handle any errors? Do it here. */

	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART_IRQRBHandler(LPC_UARTX, &rxring, &txring);
}

/**
 * @brief	Main UART program body
 * @return	Always returns 1
 */
int main(void)
{
	uint8_t key;
	int bytes;

	SystemCoreClockUpdate();
	Board_Init();
	Board_UART_Init(LPC_UARTX);
	Board_LED_Set(0, false);

	/* Setup UART for 115.2K8N1 */
	Chip_UART_Init(LPC_UARTX);
	Chip_UART_SetBaud(LPC_UARTX, 115200);
	Chip_UART_ConfigData(LPC_UARTX, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_UARTX, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_UARTX);

	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);

	/* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
	Chip_UART_SetupFIFOS(LPC_UARTX, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
							UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(LPC_UARTX, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(UARTx_IRQn, 1);
	NVIC_EnableIRQ(UARTx_IRQn);

	/* Send initial messages */
	Chip_UART_SendRB(LPC_UARTX, &txring, inst1, sizeof(inst1) - 1);
	Chip_UART_SendRB(LPC_UARTX, &txring, inst2, sizeof(inst2) - 1);

	/* Poll the receive ring buffer for the ESC (ASCII 27) key */
	key = 0;
	while (key != 27) {
		bytes = Chip_UART_ReadRB(LPC_UARTX, &rxring, &key, 1);
		if (bytes > 0) {
			/* Wrap value back around */
			if (Chip_UART_SendRB(LPC_UARTX, &txring, (const uint8_t *) &key, 1) != 1) {
				Board_LED_Toggle(0);/* Toggle LED if the TX FIFO is full */
			}
		}
	}

	/* DeInitialize UART0 peripheral */
	NVIC_DisableIRQ(UARTx_IRQn);
	Chip_UART_DeInit(LPC_UARTX);

	return 1;
}






