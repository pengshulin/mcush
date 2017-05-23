/*
 * @brief Common FreeRTOS functions shared among platforms
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
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

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSCommonHooks.h"

#include "chip.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#if defined ( __ICCARM__ )
#define __WEAK__   __weak
#else
#define __WEAK__   __attribute__((weak))
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

/* Delay for the specified number of milliSeconds */
void FreeRTOSDelay(uint32_t ms)
{
	TickType_t xDelayTime;

	xDelayTime = xTaskGetTickCount();
	vTaskDelayUntil(&xDelayTime, ms);
}

/* FreeRTOS malloc fail hook */
__WEAK__ void vApplicationMallocFailedHook(void)
{
	DEBUGSTR("DIE:ERROR:FreeRTOS: Malloc Failure!\r\n");
	taskDISABLE_INTERRUPTS();
	for (;; ) {}
}

/* FreeRTOS application idle hook */
__WEAK__ void vApplicationIdleHook(void)
{
	/* Best to sleep here until next systick */
	__WFI();
}

/* FreeRTOS stack overflow hook */
__WEAK__ void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	(void) pxTask;
	(void) pcTaskName;

	DEBUGOUT("DIE:ERROR:FreeRTOS: Stack overflow in task %s\r\n", pcTaskName);
	/* Run time stack overflow checking is performed if
	   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	   function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for (;; ) {}
}

/* FreeRTOS application tick hook */
__WEAK__ void vApplicationTickHook(void)
{}
