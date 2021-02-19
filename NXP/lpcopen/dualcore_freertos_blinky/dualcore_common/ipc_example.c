/*
 * @brief Example implementation of IPC using IPC library
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

#include <stdlib.h>
#include "app_dualcore_cfg.h"
#include "ipc_msg.h"
#include "ipc_example.h"

#ifdef OS_FREE_RTOS
#include "FreeRTOS.h"
#include "task.h"

#elif defined(OS_UCOS_III)
#include "os.h"
#endif

/** @ingroup EXAMPLE_DUALCORE_CMN_IPC_EX
 * @{
 */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/* Array of IPC Event callback functions */
static void(*ipcex_callback_lookup[IPCEX_MAX_IDS]) (uint32_t);

/* Message QUEUE used by IPC library to exchange message between cores */
static ipcex_msg_t ipcex_queue[IPCEX_QUEUE_SZ];

/* IPC Shared global variables */
static uint32_t gblCfg[IPCEX_MAX_GBLCFG];

/* Internal shared global variabe structure */
struct gbl_val
{
	int index;
	uint32_t val;
};

/* Function pointer to store notification pointer */
static ipcex_gblupdatefn_t gblUpdateFn;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* This task will receive the message from the other core
 * and will invoke the appropriate callback with the message
 */
static void ipcex_dispatch_task(void *loop)
{
	int ret;
	ipcex_msg_t msg;
	do {
		if (loop) {
			ret = IPC_popMsg(&msg);
		}
		else {
			ret = IPC_tryPopMsg(&msg);
		}
		if ((ret == QUEUE_VALID) && (msg.id < IPCEX_MAX_IDS)) {
			if (ipcex_callback_lookup[msg.id]) {
				ipcex_callback_lookup[msg.id](msg.data);
			}
		}
	} while (loop);
}

/* Call back function to update the global variable */
static void ipcex_updateGblVal(uint32_t val)
{
	struct gbl_val *gv = (struct gbl_val *)val;
	if (gv->index < IPCEX_MAX_GBLCFG)
		gblCfg[gv->index] = gv->val;

	/* Call registered functions */
	if (gblUpdateFn)
		gblUpdateFn(gv->index);

	ipcex_msgPush(IPCEX_ID_FREEMEM, val);
}

/* Free memory call */
static void ipcex_freeMem(uint32_t val)
{
	free((void *)val);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* IPC register callback function pointer */
int ipcex_register_callback(uint32_t id, void (*func)(uint32_t))
{
	if (id >= IPCEX_MAX_IDS) {
		return 0;
	}

	ipcex_callback_lookup[id] = func;
	return 1;
}

/* IPC example implementation task */
void ipcex_tasks(void)
{
#ifdef OS_FREE_RTOS
	/* Start Blinky event Task */
	xTaskCreate(ipcex_dispatch_task, "IPC Ex",
				configMINIMAL_STACK_SIZE, (void *) 1, TASK_PRIO_IPC_DISPATCH,
				(TaskHandle_t *) NULL);
#elif defined(OS_UCOS_III)
	OS_ERR ret;
	static OS_TCB    mem_tcb;
	static CPU_STK   mem_stack[UCOS_MIN_STACK_SZ];

	OSTaskCreate(
		&mem_tcb,
		"IPC Ex",
		ipcex_dispatch_task,
		(void *) 1,
		TASK_PRIO_IPC_DISPATCH,
		mem_stack,
		32,
		UCOS_MIN_STACK_SZ,
		0,
		0,
		(void *) 0,
		(OS_OPT) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR *) &ret);
	if (ret != OS_ERR_NONE) {
		DEBUGOUT("Unable to create IPC ex task : ret = %x\r\n", ret);
		while (1) {}
	}
#else
	ipcex_dispatch_task(0);
#endif
}

/* Initialize the IPC queue */
void IPCEX_Init(void)
{
	IPC_initMsgQueue(ipcex_queue, sizeof(ipcex_msg_t), IPCEX_QUEUE_SZ);
	ipcex_register_callback(IPCEX_ID_GBLUPDATE, ipcex_updateGblVal);
	ipcex_register_callback(IPCEX_ID_FREEMEM, ipcex_freeMem);
}

/* Push data on the queue */
int ipcex_msgPush(uint32_t id, uint32_t data)
{
	ipcex_msg_t msg;
	msg.id = id;
	msg.data = data;
#if (defined(OS_FREE_RTOS) || defined(OS_UCOS_III))
	return IPC_pushMsg(&msg);
#else
	return IPC_tryPushMsg(&msg);
#endif
}

/* Sets value of an M0-M4 shared global variable */
int ipcex_setGblVal(int index, uint32_t val)
{
	struct gbl_val *gv;

	if (index >= IPCEX_MAX_GBLCFG)
		return 1;

	gv = malloc(sizeof(struct gbl_val));
	if (gv == NULL)
		return 1; /* Something wrong */

	gv->val = gblCfg[index] = val;
	gv->index = index;
	if(ipcex_msgPush(IPCEX_ID_GBLUPDATE, (uint32_t) gv) != QUEUE_INSERT) {
		free(gv);
		return 1;
	}

	/* Call registered functions */
	if (gblUpdateFn)
		gblUpdateFn(gv->index);

	return 0;
}

/* Gets value of an M0-M4 shared global variable */
uint32_t ipcex_getGblVal(int index)
{
	if (index < IPCEX_MAX_GBLCFG)
		return gblCfg[index];

	return 0;
}

/* Shared global variable update notification register function */
ipcex_gblupdatefn_t ipcex_gblRegisterFn(ipcex_gblupdatefn_t newfn)
{
	ipcex_gblupdatefn_t old = gblUpdateFn;
	gblUpdateFn = newfn;
	return old;
}

/**
 * @}
 */






