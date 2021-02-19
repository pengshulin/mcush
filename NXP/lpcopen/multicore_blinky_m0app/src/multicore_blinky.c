/*
 * @brief Startup/Main file for Multicore demos
 *
 * Startup file (having reset and main routines)
 * This file provides functions necessary to start all the example tasks
 * based on the configuration.
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

/* General includes */
#include <stdio.h>
#include "app_multicore_cfg.h"
#include "ipc_example.h"
#include "ipc_msg.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define BLINKY_TICK_DELAY  500	/* Every 500 ms blinky will blink */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* initialization routine for Multicore examples */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();

	/* Initialize the IPC Queue */
	IPCEX_Init();

	NVIC_EnableIRQ(M4_IRQn);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * Wait for message from M4 core
 */
void M4_IRQHandler(void)
{
	int i = 0;
	ipcex_msg_t msg;
	Chip_CREG_ClearM4Event();

	if (IPC_tryPopMsg(&msg) != QUEUE_VALID) {
		return;
	}

	switch (msg.id.pid) {
	case PID_BLINKY:
		while (msg.data0) {
			Board_LED_Set(i, msg.data1 & 1);
			msg.data0 >>= 1;
			msg.data1 >>= 1;
		}
		break;

	default:
		/* Not for us just ignore */
		DEBUGOUT("M0 Ignoring unknown message!");
	}

}

/**
 * @brief	Main for Multicore examples
 *
 * Entry point for all the Multicore examples. All the Multicore
 * example execution starts from this function, it is common for
 * code that runs on core-M4 and core-M0.
 *
 * @return  Function should not return.
 */
int main(void)
{
	int loop = 1;
	prvSetupHardware();
	while (loop) {
		__WFI();
	}
	return 0;
}







