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
	Board_Init();

	/* Time to Start M0APP */
	if (M0Image_Boot(CPUID_M0APP, (uint32_t) BASE_ADDRESS_M0APP) < 0) {
		DEBUGSTR("Unable to BOOT M0APP Core!");
	}

#ifdef BASE_ADDRESS_M0SUB
	if (M0Image_Boot(CPUID_M0SUB, (uint32_t) BASE_ADDRESS_M0SUB) < 0) {
		DEBUGSTR("Unable to BOOT M0SUB Core!");
	}
#endif

	/* Initialize the IPC Queue */
	IPCEX_Init();

	/* Start the tick timer */
	SysTick_Config(SystemCoreClock / 1000);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

void SysTick_Handler(void)
{
	static int on, cnt;
	ipcex_msg_t msg;

	/* Wait for the apt time */
	if (cnt < BLINKY_TICK_DELAY) {
		cnt++;
		return;
	}

	msg.id.pid = PID_BLINKY;
	msg.data0 = BLINKY_LED_MASK;
	if (on) {
		msg.id.cpu = (uint16_t) BLINKY_ON_CPU;
		msg.data1 = BLINKY_LED_MASK;
	}
	else {
		msg.id.cpu = (uint16_t) BLINKY_OFF_CPU;
		msg.data1 = 0;
	}
	on = !on;
	IPC_tryPushMsg(msg.id.cpu, &msg);
	cnt = 0;
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







