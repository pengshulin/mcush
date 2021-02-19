/*
 * @brief LPC43XX Inter Processor Communication(IPC) functions
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

#include <string.h>
#include "app_multicore_cfg.h"
#include "ipc_msg.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

static struct ipc_queue *q_ipc = (struct ipc_queue *) SHARED_MEM_IPC;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/**
 * @brief	Pointer to a call back function
 * The function pointed by EVENT_IPC_RxHandler will be at the end of Rx ISR
 */
int (*IPC_EventHandler)(IPC_EVENT_T, uint16_t, int);

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*
 * Initiate interrupt on other processor
 * Upon calling this function generates and interrupt on the other
 * core. Ex. if called from M0 core it generates interrupt on M4 core
 * and vice versa.
 */
static void ipc_send_signal(void)
{
	__DSB();
	__SEV();
}

/* Simple event handler for non OS implementations */
static int IPC_EventHandler_sa(IPC_EVENT_T event, uint16_t cpu, int tout)
{
	__WFI();
	return 0;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Function to send notificaton interrupt */
void IPC_msgNotify(void)
{
	ipc_send_signal();
}

/* Function to initialize the IPC message queue */
void IPC_initMsgQueue(void *data, int size, int count)
{
	struct ipc_queue *qrd;

	/* Sanity Check */
	if (!size || !count || !data) {
		DEBUGSTR("ERROR:IPC Queue size invalid parameters\r\n");
		while (1) {}
	}

	/* Check if size is a power of 2 */
	if (count & (count - 1)) {
		DEBUGSTR("ERROR:IPC Queue size not power of 2\r\n");
		while (1) {	/* BUG: Size must always be power of 2 */
		}
	}
	qrd   = &q_ipc[CPUID_CURR];
	memset(qrd, 0, sizeof(*qrd));
	qrd->count = count;
	qrd->size = size;
	qrd->data = data;
	qrd->valid = QUEUE_MAGIC_VALID;
	if (!IPC_EventHandler) {
		IPC_EventHandler = IPC_EventHandler_sa;
	}
}

/* Function to push a message into queue with timeout */
int IPC_pushMsgTout(uint16_t cpuid, const void *data, int tout)
{
	struct ipc_queue *qwr = &q_ipc[cpuid];

	/* Check if write queue is initialized */
	if (!QUEUE_IS_VALID(qwr)) {
		return QUEUE_ERROR;
	}

	if ((tout == 0) && QUEUE_IS_FULL(qwr)) {
		return QUEUE_FULL;
	}

	/* Wait for read queue to have some data */
	while (QUEUE_IS_FULL(qwr)) {
		if (IPC_EventHandler(IPC_EVENT_WAIT_TX, cpuid, tout) && (tout > 0)) {
			return QUEUE_TIMEOUT;
		}
	}

	memcpy(qwr->data + ((qwr->head & (qwr->count - 1)) * qwr->size), data, qwr->size);
	qwr->head++;
	ipc_send_signal();

	return QUEUE_INSERT;
}

/* Function to read a message from queue with timeout */
int IPC_popMsgTout(void *data, int tout)
{
	struct ipc_queue *qrd   = &q_ipc[CPUID_CURR];
	int raise_event = QUEUE_IS_FULL(qrd);

	if (!QUEUE_IS_VALID(qrd)) {
		return QUEUE_ERROR;
	}

	if ((tout == 0) && QUEUE_IS_EMPTY(qrd)) {
		return QUEUE_EMPTY;
	}

	while (QUEUE_IS_EMPTY(qrd)) {
		if (IPC_EventHandler(IPC_EVENT_WAIT_RX, CPUID_CURR, tout) && (tout > 0)) {
			return QUEUE_TIMEOUT;
		}
	}

	/* Pop the queue Item */
	memcpy(data, qrd->data + ((qrd->tail & (qrd->count - 1)) * qrd->size), qrd->size);
	qrd->tail++;

	if (raise_event) {
		ipc_send_signal();
	}
	return QUEUE_VALID;
}

/* Get number of pending items in queue */
int IPC_msgPending(uint16_t cpu)
{
	struct ipc_queue *q = &q_ipc[cpu];
	if (!QUEUE_IS_VALID(q)) {
		return QUEUE_ERROR;
	}

	return QUEUE_DATA_COUNT(q);
}

/* Function to convert IPC error number to string */
const char *IPC_strerror(int errnum)
{
	static const char *ipc_errstr[] = {
		"Queue Insert OK",
		"Queue Pop OK/Valid",
		"Queue is Full",
		"Queue is Empty",
		"Queue Error/Not initialized",
		"Queue operation timed out",
	};
	if (errnum < 0) {
		errnum = 1 - errnum;
	}
	return ipc_errstr[errnum];
}







