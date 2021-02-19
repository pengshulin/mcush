/*
 * @brief ATIMER example
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

#define PresetCount (1024)

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Clearn interrupts */
static void ATIMER_ClearInts(void)
{
	/* Clear Alarm Timer interrupt status */
	Chip_ATIMER_ClearIntStatus(LPC_ATIMER);

	/* Clear Alarm Timer interrupt flag */
	Chip_EVRT_ClrPendIntSrc(EVRT_SRC_ATIMER);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Handle interrupt from ATIMER source
 * @return	Nothing
 */
void EVRT_IRQHandler(void)
{
	/* Check if source interrupt is ATIMER */
	if (Chip_EVRT_IsSourceInterrupting(EVRT_SRC_ATIMER)) {
		ATIMER_ClearInts();
	}
}

/**
 * @brief	main routine for ATIMER example
 * @return	Nothing (function should not exit)
 */
int main(void)
{
	bool On = false;

	SystemCoreClockUpdate();
	Board_Init();

	/* Init Alarm Timer with Preset Count for about 1s */
	Chip_ATIMER_Init(LPC_ATIMER, PresetCount);

	/* Init EVRT */
	Chip_EVRT_Init();

	/* Enable EVRT in order to be able to read the ATIMER interrupt */
	Chip_EVRT_ConfigIntSrcActiveType(EVRT_SRC_ATIMER, EVRT_SRC_ACTIVE_HIGH_LEVEL);

	/* Enable Alarm Timer Source */
	Chip_EVRT_SetUpIntSrc(EVRT_SRC_ATIMER, ENABLE);

	/* Enable NVIC */
	NVIC_EnableIRQ(EVENTROUTER_IRQn);

	/* Clear the interrupt states */
	ATIMER_ClearInts();

	/* Enable Alarm Timer */
	Chip_ATIMER_IntEnable(LPC_ATIMER);

	while (1) {
		/* Sleep until ATIMER fires */
		__WFI();
		On = (bool) !On;
		Board_LED_Set(1, On);
	}
}






