/*
 * @brief Generic USB example for USB ROM API.
 *
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
#include <stdio.h>
#include <string.h>
#include "libusbdev.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* The size of the packet buffer. */
#define PACKET_BUFFER_SIZE        4096

/* Application defined LUSB interrupt status  */
#define LUSB_DATA_PENDING       _BIT(0)

/* Packet buffer for processing */
static uint8_t g_rxBuff[PACKET_BUFFER_SIZE];

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for USB example
 * @return	Function should not exit.
 */
int main(void)
{
	/* Initialize board and chip */
	SystemCoreClockUpdate();
	Board_Init();

	/* Init USB subsystem and LibUSBDevice */
	libusbdev_init(USB_STACK_MEM_BASE, USB_STACK_MEM_SIZE);

	while (1) {
		/* wait until host is connected */
		while (libusbdev_Connected() == 0) {
			/* Sleep until next IRQ happens */
			__WFI();
		}

		while (libusbdev_Connected()) {

			if (libusbdev_QueueReadDone() != -1) {

				/* Dummy process read data ......*/
				/* requeue read request */
				libusbdev_QueueReadReq(g_rxBuff, PACKET_BUFFER_SIZE);
			}

			if (libusbdev_QueueSendDone() == 0) {
				/* Queue send request */
				libusbdev_QueueSendReq(g_rxBuff, PACKET_BUFFER_SIZE);
			}
		}
	}
}






