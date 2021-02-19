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
#include "app_multicore_cfg.h"
#include "ipc_msg.h"
#include "ipc_example.h"
#include "sys_os.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Array of IPC Event callback functions */
static void(*ipcex_callback_lookup[IPCEX_MAX_PID]) (const ipcex_msg_t *);

/* Message QUEUE used by IPC library to exchange message between cores */
static ipcex_msg_t ipcex_queue[IPCEX_QUEUE_SZ];

/* IPC Shared global variables */
static uint32_t gblCfg[IPCEX_MAX_GBLCFG];

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
static void ipcex_dispatch_task(void *unused)
{
	int ret;
	ipcex_msg_t msg;
	do {
		ret = IPC_popMsgTout(&msg, -SYS_OS_ID);
		if ((ret != QUEUE_VALID) || (msg.id.pid >= IPCEX_MAX_PID)) {
			continue;
		}
		if (ipcex_callback_lookup[msg.id.pid]) {
			ipcex_callback_lookup[msg.id.pid](&msg);
		}
	} while (SYS_OS_ID);
}

/* Call back function to update the global variable */
static void ipcex_updateGblVal(const ipcex_msg_t *msg)
{
	if (msg->data0 >= IPCEX_MAX_GBLCFG) {
		return;
	}
	gblCfg[msg->data0] = msg->data1;

	/* Call registered functions */
	if (gblUpdateFn) {
		gblUpdateFn((int) msg->data0);
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* IPC register callback function pointer */
int ipcex_register_callback(uint16_t pid, void (*func)(const ipcex_msg_t *))
{
	if (pid < IPCEX_MAX_PID) {
		ipcex_callback_lookup[pid] = func;
	}
	return 0;
}

/* IPC example implementation task */
void ipcex_tasks(void)
{
	sys_taskCreate(ipcex_dispatch_task, SYS_OS_ID, "IPC Ex", TASK_PRIO_IPC_DISPATCH);
}

/* Initialize the IPC queue */
void IPCEX_Init(void)
{
	IPC_initMsgQueue(ipcex_queue, sizeof(ipcex_msg_t), IPCEX_QUEUE_SZ);
	ipcex_register_callback(PID_GBLUPDATE, ipcex_updateGblVal);
}

/* Sets value of an M0-M4 shared global variable */
int ipcex_setGblVal(int index, uint32_t val)
{
	CPUID_T i;
	int ret = 0;
	for (i = CPUID_MIN; i < CPUID_MAX; i++) {
		if (i == CPUID_CURR) {
			gblCfg[index] = val;
			if (gblUpdateFn) {
				gblUpdateFn(index);
			}
		}
		else {
			ipcex_msg_t msg;
			msg.id.cpu = (uint16_t) i;
			msg.id.pid = PID_GBLUPDATE;
			msg.data0 = (uint32_t) index;
			msg.data1 = val;
			ret |= (ipcex_msgPush(&msg) != QUEUE_INSERT) << msg.id.cpu;
		}
	}
	return 0;
}

/* Gets value of an M0-M4 shared global variable */
uint32_t ipcex_getGblVal(int index)
{
	if (index < IPCEX_MAX_GBLCFG) {
		return gblCfg[index];
	}

	return 0;
}

/* Shared global variable update notification register function */
ipcex_gblupdatefn_t ipcex_gblRegisterFn(ipcex_gblupdatefn_t newfn)
{
	ipcex_gblupdatefn_t old = gblUpdateFn;
	gblUpdateFn = newfn;
	return old;
}






