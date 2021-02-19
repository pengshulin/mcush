/*
 * @brief WWDT example
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

static volatile int wdtFeedState;
static volatile bool On = false, On1 = false;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* WDT interrupt handler */
static void wdt_handle(void) {
	uint32_t wdtStatus = Chip_WWDT_GetStatus(LPC_WWDT);

	On = (bool) !On;

	/* The chip will reset before this happens, but if the WDT doesn't
	   have WWDT_WDMOD_WDRESET enabled, this will hit once */
	if (wdtStatus & WWDT_WDMOD_WDTOF) {
		/* A watchdog feed didn't occur prior to window timeout */
		Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF);

		if (wdtFeedState == 2) {
			Chip_WWDT_Start(LPC_WWDT);	/* Needs restart */
		}
	}

	/* Handle warning interrupt */
	if (wdtStatus & WWDT_WDMOD_WDINT) {
		/* A watchdog feed didn't occur prior to warning timeout */
		Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDINT);

		if (wdtFeedState == 1) {
			Chip_WWDT_Feed(LPC_WWDT);
		}
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Event router Interrupt Handler
 * @return	Nothing
 * Handles event router events
 */
void EVRT_IRQHandler(void)
{
	On1 = !On1;

	/* Clear watchdog timer interrupt in event router (ok to clear before
	   watchdog clear on edge based event router events) */
	Chip_EVRT_ClrPendIntSrc(EVRT_SRC_WWDT);

	wdt_handle();
}

/**
 * @brief	watchdog timer Interrupt Handler
 * @return	Nothing
 * @note	Handles watchdog timer warning and timeout events
 */
void WDT_IRQHandler(void)
{
	wdt_handle();
}

/**
 * @brief	SysTick Interrupt Handler
 * @return	Nothing
 * @note	Systick interrupt handler feeds WWDT
 */
void SysTick_Handler(void)
{
	if (wdtFeedState == 0) {
		On = (bool) !On;
		Chip_WWDT_Feed(LPC_WWDT);
	}
}

/**
 * @brief	Main entry point
 * @return	Nothing
 */
int main(void)
{
	uint8_t ch;

	SystemCoreClockUpdate();
	Board_Init();

	/* Watchdog will be fed on each watchdog interrupt */
	wdtFeedState = 0;

	/* Initialize WWDT and event router */
	Chip_WWDT_Init(LPC_WWDT);
	Chip_EVRT_Init();

	/* Set watchdog feed time constant to 0.1s
	   Set watchdog warning time to 512 ticks after feed time constant
	   Set watchdog window time to 0.9s */
	Chip_WWDT_SetTimeOut(LPC_WWDT, WDT_OSC / 10);
	Chip_WWDT_SetWarning(LPC_WWDT, 512);
	Chip_WWDT_SetWindow(LPC_WWDT, WDT_OSC - (WDT_OSC / 10));

	/* Configure WWDT to reset on timeout */
	Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);

	/* Clear watchdog warning and timeout interrupts */
	Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF | WWDT_WDMOD_WDINT);

	/* Initiate EVRT, route interrupt signal from WWDT to EVRT,
	   enable EVRT WWDT interrupt */
	Chip_EVRT_ConfigIntSrcActiveType(EVRT_SRC_WWDT, EVRT_SRC_ACTIVE_RISING_EDGE);
	Chip_EVRT_SetUpIntSrc(EVRT_SRC_WWDT, ENABLE);

	/* Start watchdog */
	Chip_WWDT_Start(LPC_WWDT);

	/* Setup Systick for a 10Hz tick rate. Systick clock is clocked at
	   CPU core clock speed */
	SysTick_Config(Chip_Clock_GetRate(CLK_MX_MXCORE) / 20);

	/* Enable watchdog interrupt */
	NVIC_EnableIRQ(WWDT_IRQn);

	/* Watchdog test options */
	DEBUGOUT("Press '1' to enable watchdog feed on systick interrupt\n\r");
	DEBUGOUT("Press '2' to enable watchdog feed on warning interrupt\n\r");
	DEBUGOUT("Press '3' to disable watchdog feed (will reset device)\n\r");
	DEBUGOUT("Press '4' to switch from WDT interrupt to event router handler\n\r");

	while (1) {
		do {
			ch = (uint8_t) DEBUGIN();
			Board_LED_Set(0, On);
			Board_LED_Set(1, On1);
		} while ((ch < '1') || (ch > '4'));

		switch (ch) {
		case '1':
		default:
			wdtFeedState = 0;
			DEBUGOUT("Watchdog feed on systick interrupt\r\n");
			break;

		case '2':
			wdtFeedState = 1;
			DEBUGOUT("Watchdog feed on warning interrupt\r\n");
			break;

		case '3':
			wdtFeedState = 2;
			DEBUGOUT("No feeding - board will reset\r\n");
			break;

		case '4':
			DEBUGOUT("using event router instead of WDT interrupt\r\n");
			NVIC_DisableIRQ(WWDT_IRQn);
			NVIC_EnableIRQ(EVENTROUTER_IRQn);
			break;
		}
	}
}






