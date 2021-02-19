/*
 * @brief DMA example using a timer capture input as a trigger
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

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define SOURCEARRAYSIZE (4)

/* Source and destination DMA areas */
static uint32_t source[SOURCEARRAYSIZE], dest[SOURCEARRAYSIZE];

/* Counters used for DMA transfers and passed comparisons */
static volatile int dmaXfers;
static int compPass;

/* DMA channel */
static uint8_t dmaCh;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Setup tiemr capture input used for DMA trigger */
void setupTimerCapInput(void)
{
	/* This function is not board specific and always assumes the cap0.0
	   signal will be on pin P8_4 with a pullup required */
	Chip_SCU_PinMuxSet(8, 4, (SCU_MODE_FUNC7 | SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP));
}

/* Setup a timer for external capture input trigger */
static void setupTimerTrigger(void)
{
	/* Enable timer 0 clock and reset it */
	Chip_TIMER_Init(LPC_TIMER0);
	Chip_RGU_TriggerReset(RGU_TIMER0_RST);
	while (Chip_RGU_InReset(RGU_TIMER0_RST)) {}

	/* Setup timer capture 0 input (cap0.0) for low edge as timer
	   clock source */
	Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER0, TIMER_CAPSRC_FALLING_CAPN, 0);

	/* Set timer prescale to divide by 1 */
	Chip_TIMER_PrescaleSet(LPC_TIMER0, 0);

	/* Reset initial timer count and clear match count 0 interrupt */
	Chip_TIMER_Reset(LPC_TIMER0);
	Chip_TIMER_ClearMatch(LPC_TIMER0, 0);

	/* Setup match 0 interrupt to trigger on increment to 1,
       reset on match	*/
	Chip_TIMER_SetMatch(LPC_TIMER0, 0, 1);
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 0);

	/* Enable match interrupt */ // FIXME is this needed?
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 1);

	/* Enable timer */
	Chip_TIMER_Enable(LPC_TIMER0);
}

/* Setup DMA M2M transfer to trigger on timer match */
static void setupDMATrigger(void)
{
	/* Initialize GPDMA controller */
	Chip_GPDMA_Init(LPC_GPDMA);

	/* Get DMA channel */
	dmaCh = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, GPDMA_CONN_MAT0_0);

	/* Setup DMA transfer */
	Chip_GPDMA_Transfer(LPC_GPDMA, dmaCh,
					  (uint32_t) &source[0], GPDMA_CONN_MAT0_0, //(uint32_t) &dest[0],
					  GPDMA_TRANSFERTYPE_M2M_CONTROLLER_DMA,
					  sizeof(source));

//			Chip_GPDMA_Stop(LPC_GPDMA, dmaChSSPTx);

	/* Enable GPDMA interrupt */
	NVIC_EnableIRQ(DMA_IRQn);
}

/* Put some varying data in the source buffer */
static void fillSourceBuff(void)
{
	int i;
	static uint32_t seed;

	for (i = 0; i < SOURCEARRAYSIZE; i++) {
		seed += 1 + (seed << 1);
		source[i] = seed;
	}
}

/* Verify source and destination data (after DMA transfer) */
static int checkSourceDestData(void)
{
	int i, matches = 1;

	for (i = 0; i < SOURCEARRAYSIZE; i++) {
		if (source[i] != dest[i]) {
			matches = 0;
		}
	}

	return matches;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	DMA interrupt handler
 * @return	Nothing
 */
void DMA_IRQHandler(void)
{
	Board_LED_Toggle(0);

	/* Increase DMA transfer count */
	dmaXfers++;

//	if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaCh) == SUCCESS) {
//		isDmaTxfCompleted = 1;
//	}
}

void TIMER0_IRQHandler(void)
{
	static bool On = false;

	if (Chip_TIMER_MatchPending(LPC_TIMER0, 0)) {
		Chip_TIMER_ClearMatch(LPC_TIMER0, 0);
		On = (bool) !On;
		Board_LED_Set(0, On);
	}
}

/**
 * @brief	Main routine for DMA timer trigger example
 * @return	Nothing
 */
int main(void)
{
	int numDmaXfers = 0;

	SystemCoreClockUpdate();
	Board_Init();

	/* Setup tiemr capture input used for DMA trigger */
	setupTimerCapInput();

	/* Setup timer for match event on each capture event */
	setupTimerTrigger();

	/* Setup DMA for memory to memory transfer on timer match event */
//	setupDMATrigger();
	/* Enable timer interrupt */
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_ClearPendingIRQ(TIMER0_IRQn);

	/* Idle in background waiting for DMA events */
	while (1) {
		/* Setup source data */
		fillSourceBuff();

		/* Wait for interrupt */
		__WFI();

		if (numDmaXfers != dmaXfers) {
			numDmaXfers = dmaXfers;

			/* Compare data */
			compPass += checkSourceDestData();

			DEBUGOUT("DMA transfer triggered by timer: %d\r\n", numDmaXfers);
			DEBUGOUT("DMA transfer memory comparisons: %d\r\n", compPass);
		}
	}

	return 0;
}






