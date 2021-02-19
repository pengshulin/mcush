/*
* @brief Blinky Example using Dual Core communication
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

/* The configuration file must use <filename> format
 * since, this source file is shared across projects
 */
#include "app_dualcore_cfg.h"
#include "ipc_msg.h"
#include "ipc_example.h"

#ifdef OS_FREE_RTOS
#include "FreeRTOS.h"
#include "task.h"

#elif defined(OS_UCOS_III)
#include "os.h"

#else
/* No OS configuration */
#endif

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Delay to be used for blinking the LEDs */
static const uint32_t xDelay = BLINKY_DEFAULT_DELAY;

static void LED_Event_Task(void *loop);


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

#ifdef OS_FREE_RTOS
/* Delay function used for blinking LED (FreeRTOS version)
 * Calling this function will cause a delay of \a xDelay
 * returns 0 when the time has lapsed else non zero
 */
static int blink_delay(void)
{
		vTaskDelay(xDelay);
		return 0;
}

#elif defined(OS_UCOS_III)
/* Delay function used for blinking LED (uCOS-III Version) */
static int blink_delay(void)
{
		OS_ERR ret;
		OSTimeDlyHMSM(0, 0, xDelay / 1000, xDelay % 1000, OS_OPT_TIME_HMSM_STRICT, &ret);
		return ret != OS_ERR_NONE;
}

#else

/* Delay function used for blinking LED (noOS version) */
static int blink_delay(void)
{
	static int32_t final, init;
	if (!init) {
		int32_t curr = (int32_t) Chip_RIT_GetCounter(LPC_RITIMER);
		final = curr + (SystemCoreClock / 1000) * xDelay;
		init = 1 + (final < 0 && curr > 0);
	}

	if ((init == 2 && Chip_RIT_GetCounter(LPC_RITIMER) >= (uint32_t) final) ||
		(init == 1 && (int32_t) Chip_RIT_GetCounter(LPC_RITIMER) >= final)) {
			init = 0;
	}
	return init != 0;
}
#endif

/* Blink LED based on event from M0/M4
 * This function when called from M0 will blink the LED
 * based on message sent by M4 and vice-versa.
 */
static void LED_blinkProc(uint32_t val)
{
	Board_LED_Set((val >> 16) & 0xFFFF, val & 0xFFFF);
}

/* Send blink event to M4/M0 from M0/M4
 * This function if called from M4 will send the blink
 * event to M0 and vice-versa.
 */
static void LED_Event_Task(void *loop)
{
	static int blink = 0;

	do {
		if (!blink_delay()) {
			if (ipcex_msgPush(IPCEX_ID_BLINKY, (BLINK_LED << 16) | blink) == QUEUE_INSERT)
				blink = 1 - blink;
		}
	} while(loop);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

#ifdef OS_FREE_RTOS
/* FreeRTOS blinky task */
void blinky_tasks(void)
{
	/* Start Blinky event Task */
	xTaskCreate( LED_Event_Task, "LED Event",
		configMINIMAL_STACK_SIZE, (void *)1, TASK_PRIO_BLINKY_EVENT,
		( TaskHandle_t * ) NULL );
}

#elif defined(OS_UCOS_III)

/* uCOS-III Blinky Task */
void blinky_tasks(void)
{
	OS_ERR ret;
	static OS_TCB    mem_tcb;
	static CPU_STK   mem_stack[UCOS_MIN_STACK_SZ];

	OSTaskCreate   (
		&mem_tcb,
		"Event Tsk",
		LED_Event_Task,
		(void *) 1,
		TASK_PRIO_BLINKY_EVENT,
		mem_stack,
		APP_CFG_TASK_START_STK_SIZE_LIMIT,
		UCOS_MIN_STACK_SZ,
		0,
		0,
		(void *)0,
		(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR *)&ret);
	if (ret != OS_ERR_NONE) {
		printf("Unable to create event task!\r\n");
		while (1);
	}
}

#else
/* Dual core blinky standalone task calling function */
void blinky_tasks(void)
{
	LED_Event_Task((void *) 0);
}
#endif

/* Initialization for Blinky dual core example */
void BLINKY_Init(void)
{
	ipcex_register_callback(IPCEX_ID_BLINKY, LED_blinkProc);
}






