/*
 * @brief UART example
 * This example show how to use the UART in 3 modes : Polling, Interrupt and DMA
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

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#if (defined(BOARD_HITEX_EVA_1850) || defined(BOARD_HITEX_EVA_4350))
#define UARTNum 0

#elif (defined(BOARD_KEIL_MCB_1857) || defined(BOARD_KEIL_MCB_4357))
#define UARTNum 3

#elif (defined(BOARD_NGX_XPLORER_1830) || defined (BOARD_NGX_XPLORER_4330))
#define UARTNum 0

#elif defined(BOARD_NXP_LPCLINK2_4370)
#define UARTNum 2

#elif (defined(BOARD_NXP_LPCXPRESSO_4337) || defined(BOARD_NXP_LPCXPRESSO_1837))
#define UARTNum 0

#else
#error No UART selected for undefined board
#endif

#if (UARTNum == 0)
#define LPC_UART LPC_USART0
#define UARTx_IRQn  USART0_IRQn
#define UARTx_IRQHandler UART0_IRQHandler
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART0_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART0_Rx
#elif (UARTNum == 1)
#define LPC_UART LPC_UART1
#define UARTx_IRQn  UART1_IRQn
#define UARTx_IRQHandler UART1_IRQHandler
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART1_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART1_Rx
#elif (UARTNum == 2)
#define LPC_UART LPC_USART2
#define UARTx_IRQn  USART2_IRQn
#define UARTx_IRQHandler UART2_IRQHandler
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART2_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART2_Rx
#elif (UARTNum == 3)
#define LPC_UART LPC_USART3
#define UARTx_IRQn  USART3_IRQn
#define UARTx_IRQHandler UART3_IRQHandler
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART3_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART3_Rx
#endif

/* Transmit and receive ring buffers */
STATIC RINGBUFF_T txring, rxring;

/* Ring buffer size */
#define UART_RB_SIZE 256

/* Transmit and receive buffers */
static uint8_t rxbuff[UART_RB_SIZE], txbuff[UART_RB_SIZE];

static uint8_t uartABStart[] = "Starting UART Auto-Baud - Press 'A' or 'a'! \n\r";
static uint8_t uartABComplete[] = "UART Auto-Baudrate synchronized! \n\r";

/* Uart Polling variables and functions declaration */
static uint8_t uartPolling_menu1[] = "Hello NXP Semiconductors \r\n";
static uint8_t uartPolling_menu2[] = "UART polling mode demo \r\n";
static uint8_t uartPolling_menu3[] = "\r\nUART demo terminated!";
static uint8_t uartPolling_menu4[] = "\r\nPress number 1-3 to choose UART running mode:\r\n"
									 "\t 1: Polling Mode \r\n"
									 "\t 2: Interrupt Mode \r\n"
									 "\t 3: DMA Mode \r\n";
static uint8_t uartPolling_menu5[] = "\r\nPolling mode is running now! Please press \'c\' and choose another mode \r\n";

/* Uart Interrupt variables and functions declaration */
static uint8_t uart_interrupt_menu[] =
	"UART Interrupt mode demo ! \r\nPress '1' to '4' to display 4 menus \r\nPress 'x'to exit UART interrupt mode \r\n";
static uint8_t uart_interrupt_menu1[] = "UART interrupt menu 1 \r\n";
static uint8_t uart_interrupt_menu2[] = "UART interrupt menu 2 \r\n";
static uint8_t uart_interrupt_menu3[] = "UART interrupt menu 3 \r\n";
static uint8_t uart_interrupt_menu4[] = "UART interrupt menu 4 \r\n";
/* static uint8_t rxUartIntBuf[1]; */

#define DMA_TIMEOUT 0xA000000

/* DMA variables and functions declaration */
static uint8_t dmaChannelNumTx, dmaChannelNumRx;
static uint8_t uartDMA_menu[] = "Hello NXP Semiconductors (DMA mode)\r\n"
								"UART DMA mode demo ! Please type 'hello NXP' to return\r\n";

static volatile uint32_t channelTC;	/* Terminal Counter flag for Channel */
static volatile uint32_t channelTCErr;
static FunctionalState  isDMATx = ENABLE;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Initialize DMA for UART, enable DMA controller and enable DMA interrupt */
static void App_DMA_Init(void)
{
	/* Initialize GPDMA controller */
	Chip_GPDMA_Init(LPC_GPDMA);
	/* Setting GPDMA interrupt */
	NVIC_DisableIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, ((0x01 << 3) | 0x01));
	NVIC_EnableIRQ(DMA_IRQn);
}

/* DeInitialize DMA for UART, free transfer channels and disable DMA interrupt */
static void App_DMA_DeInit(void)
{
	Chip_GPDMA_Stop(LPC_GPDMA, dmaChannelNumTx);
	Chip_GPDMA_Stop(LPC_GPDMA, dmaChannelNumRx);
	NVIC_DisableIRQ(DMA_IRQn);
}

/* DMA routine for example_uart */
static void App_DMA_Test(void)
{
	uint8_t receiveBuffer[16];

	App_DMA_Init();
	dmaChannelNumTx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, _GPDMA_CONN_UART_Tx);

	isDMATx = ENABLE;
	channelTC = channelTCErr = 0;
	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNumTx,
					  (uint32_t) &uartDMA_menu[0],
					  _GPDMA_CONN_UART_Tx,
					  GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
					  sizeof(uartDMA_menu));
	while (!channelTC) {}

	dmaChannelNumRx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, _GPDMA_CONN_UART_Rx);
	isDMATx = DISABLE;
	channelTC = channelTCErr = 0;
	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNumRx,
					  _GPDMA_CONN_UART_Rx,
					  (uint32_t) &receiveBuffer[0],
					  GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA,
					  10);
	while (!channelTC) {}

	isDMATx = ENABLE;
	channelTC = channelTCErr = 0;
	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNumTx,
					  (uint32_t) &receiveBuffer[0],
					  _GPDMA_CONN_UART_Tx,
					  GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
					  10);
	while (!channelTC) {}

	App_DMA_DeInit();
}

/* Print Welcome Screen Menu subroutine by Interrupt mode */
static void Print_Menu_Interrupt(LPC_USART_T *UARTx)
{
	uint32_t tmp;
	uint8_t *pDat;

	tmp = sizeof(uart_interrupt_menu);
	pDat = (uint8_t *) &uart_interrupt_menu[0];
	Chip_UART_SendRB(UARTx, &txring, pDat, tmp);
}

/* Initialize Interrupt for UART */
static void App_Interrupt_Init(void)
{
	/* Enable UART Rx & line status interrupts */
	/*
	 * Do not enable transmit interrupt here, since it is handled by
	 * UART_Send() function, just to reset Tx Interrupt state for the
	 * first time
	 */
	Chip_UART_IntEnable(LPC_UART, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_RB_SIZE);

	/* Enable Interrupt for UART channel */
	/* Priority = 1 */
	NVIC_SetPriority(UARTx_IRQn, 1);
	/* Enable Interrupt for UART channel */
	NVIC_EnableIRQ(UARTx_IRQn);
}

/* DeInitialize Interrupt for UART */
static void App_Interrupt_DeInit(void)
{
	/* Disable UART Rx & line status interrupts */
	Chip_UART_IntDisable(LPC_UART, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* Disable Interrupt for UART channel */
	NVIC_DisableIRQ(UARTx_IRQn);
}

/* Interrupt routine for example_uart */
static void App_Interrupt_Test(void)
{
	uint8_t isExit = 0, userInput;
	int len;
	App_Interrupt_Init();

	/* Print out uart interrupt menu */
	Print_Menu_Interrupt(LPC_UART);

	while (!isExit) {
		len = 0;
		while (len == 0) {
			len = Chip_UART_ReadRB(LPC_UART, &rxring, &userInput, 1);
		}
		if (userInput == '1') {
			Chip_UART_SendRB(LPC_UART, &txring, (uint8_t *) &uart_interrupt_menu1[0], sizeof(uart_interrupt_menu1));
		}
		else if (userInput == '2') {
			Chip_UART_SendRB(LPC_UART, &txring, (uint8_t *) &uart_interrupt_menu2[0], sizeof(uart_interrupt_menu2));
		}
		else if (userInput == '3') {
			Chip_UART_SendRB(LPC_UART, &txring, (uint8_t *) &uart_interrupt_menu3[0], sizeof(uart_interrupt_menu3));
		}
		else if (userInput == '4') {
			Chip_UART_SendRB(LPC_UART, &txring, (uint8_t *) &uart_interrupt_menu4[0], sizeof(uart_interrupt_menu4));
		}
		else if (( userInput == 'x') || ( userInput == 'X') ) {
			isExit = 1;
		}
	}

	App_Interrupt_DeInit();
}

/* Print Welcome menu by Polling mode */
static void Print_Menu_Polling(void)
{
	Chip_UART_SendBlocking(LPC_UART, uartPolling_menu1, sizeof(uartPolling_menu1));
	Chip_UART_SendBlocking(LPC_UART, uartPolling_menu2, sizeof(uartPolling_menu2));
	Chip_UART_SendBlocking(LPC_UART, uartPolling_menu5, sizeof(uartPolling_menu5));
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	GPDMA interrupt handler sub-routine
 * @return	Nothing
 */
void DMA_IRQHandler(void)
{
	uint8_t dmaChannelNum;
	if (isDMATx) {
		dmaChannelNum = dmaChannelNumTx;
	}
	else {
		dmaChannelNum = dmaChannelNumRx;
	}
	if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChannelNum) == SUCCESS) {
		channelTC++;
	}
	else {
		channelTCErr++;
	}
}

/**
 * @brief	UART interrupt handler sub-routine
 * @return	Nothing
 */
void UARTx_IRQHandler(void)
{
	Chip_UART_IRQRBHandler(LPC_UART, &rxring, &txring);
}

/**
 * @brief	Main UART program body
 * @return	Always returns -1
 */
int main(void)
{
	FlagStatus exitflag;
	uint8_t buffer[10];
	int ret = 0;
	int len;

	SystemCoreClockUpdate();
	Board_Init();
	Board_UART_Init(LPC_UART);

#if !((defined(CHIP_LPC43XX) && defined(BOARD_KEIL_MCB_18574357) && UARTNum==3) || ((!(defined(CHIP_LPC43XX) && defined(BOARD_KEIL_MCB_18574357))) && UARTNum==0))
	Chip_UART_Init(LPC_UART);
	Chip_UART_SetBaud(LPC_UART, 115200);
	Chip_UART_ConfigData(LPC_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT); /* Default 8-N-1 */

	/* Enable UART Transmit */
	Chip_UART_TXEnable(LPC_UART);
#endif

	/* Reset FIFOs, Enable FIFOs and DMA mode in UART */
	Chip_UART_SetupFIFOS(LPC_UART, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
							UART_FCR_TX_RS | UART_FCR_DMAMODE_SEL | UART_FCR_TRG_LEV0));

	/* Enable UART End of Auto baudrate & Auto baudrate timeout interrupts */
	Chip_UART_IntEnable(LPC_UART, (UART_IER_ABEOINT | UART_IER_ABTOINT));

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(UARTx_IRQn, 1);
	/* Enable Interrupt for UART0 channel */
	NVIC_EnableIRQ(UARTx_IRQn);

	/* Send UART Autobaud completed message */
	Chip_UART_SendBlocking(LPC_UART, uartABStart, sizeof(uartABStart));

	/* ---------------------- Auto baud rate section ----------------------- */
	/* Start auto baudrate mode */
	Chip_UART_ABCmd(LPC_UART, UART_ACR_MODE0, true, ENABLE);

	/* Loop until auto baudrate mode complete */
	while (Chip_UART_GetABEOStatus(LPC_UART) == RESET) {}

	/* Send UART Autobaud completed message */
	Chip_UART_SendBlocking(LPC_UART, uartABComplete, sizeof(uartABComplete));

    /* Disable UART Interrupt */
    NVIC_DisableIRQ(UARTx_IRQn);

	/* Print welcome screen */
	Print_Menu_Polling();

	exitflag = RESET;
	/* Read some data from the buffer */
	while (exitflag == RESET) {
		len = 0;
		while (len == 0) {
			len = Chip_UART_Read(LPC_UART, buffer, 1);
		}
		if (buffer[0] == 27) {
			/* ESC key, set exit flag */
			Chip_UART_SendBlocking(LPC_UART, uartPolling_menu3, sizeof(uartPolling_menu3));
			ret = -1;
			exitflag = SET;
		}
		else if (buffer[0] == 'c') {
			Chip_UART_SendBlocking(LPC_UART, uartPolling_menu4, sizeof(uartPolling_menu4));
			len = 0;
			while (len == 0) {
				len = Chip_UART_Read(LPC_UART, buffer, sizeof(buffer));
				if ((buffer[0] != '1') && (buffer[0] != '2') && (buffer[0] != '3')) {
					len = 0;
				}
			}
			switch (buffer[0]) {
			case '1':		/* Polling Mode */
				Chip_UART_SendBlocking(LPC_UART, uartPolling_menu5, sizeof(uartPolling_menu5));
				break;

			case '2':		/* Interrupt Mode */
				ret = 2;
				/* Exitflag = SET; */
				App_Interrupt_Test();
				Print_Menu_Polling();
				break;

			case '3':		/* DMA mode */
				ret = 3;
				App_DMA_Test();
				Print_Menu_Polling();
				break;
			}
		}
	}

	/* Wait for current transmission complete - THR must be empty */
	while (Chip_UART_CheckBusy(LPC_UART) == SET) {}

	/* DeInitialize UART0 peripheral */
	Chip_UART_DeInit(LPC_UART);

	return ret;
}






