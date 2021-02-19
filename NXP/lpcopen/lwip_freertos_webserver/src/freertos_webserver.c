/*
 * @brief	HTTP server dual core example using lwIP ethernet stack
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

/**
 * \file webserver\dualcore_webserver.c
 * LWIP HTTP Server dual core example application source code
 * This file implements the HTTP server functionality for stand-alone,
 * FreeRTOS, and uCOS-III configurations
 */

/* General Includes */
#include <string.h>

/* lwIP include files */
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/memp.h"
#include "lwip/tcpip.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/timers.h"

#include "netif/etharp.h"

#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif

#include "arch/lpc18xx_43xx_emac.h"
#include "arch/lpc_arch.h"
#include "lpc_phy.h"/* For the PHY monitor support */

extern void http_server_netconn_init(void);

#include "FreeRTOS.h"
#include "task.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* LWIP Network interface object structure */
static struct netif lpc_netif;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

static void ip_addr_changed(const struct netif *nwif)
{
	char tmp_buff[16];

	if (!nwif->ip_addr.addr) return;

	DEBUGOUT("IP_ADDR    : %s\r\n", ipaddr_ntoa_r((const ip_addr_t *) &nwif->ip_addr, tmp_buff, 16));
	DEBUGOUT("NET_MASK   : %s\r\n", ipaddr_ntoa_r((const ip_addr_t *) &nwif->netmask, tmp_buff, 16));
	DEBUGOUT("GATEWAY_IP : %s\r\n", ipaddr_ntoa_r((const ip_addr_t *) &nwif->gw, tmp_buff, 16));
}

/**
 * Callback function for TCPIP thread to indicate TCPIP init is done
 * (Only used in case of FreeRTOS/uCOS-III configuration)
 */
static void tcpip_init_done_signal(void *arg)
{
	/* Tell main thread TCP/IP init is done */
	*(s32_t *) arg = 1;
}

/**
 * Network interface setup function
 * (Only used in case of FreeRTOS/uCOS-III configuration)
 */
static void vSetupIFTask(void *pvParameters)
{
	ip_addr_t ipaddr, netmask, gw;
	volatile s32_t tcpipdone = 0;
	uint32_t physts;
	static int prt_ip = 0;

	/* Wait until the TCP/IP thread is finished before
	   continuing or wierd things may happen */
	DEBUGSTR("Waiting for TCPIP thread to initialize...\r\n");
	tcpip_init(tcpip_init_done_signal, (void *) &tcpipdone);
	while (!tcpipdone) ;

	DEBUGSTR("Starting LWIP HTTP server...\r\n");

	/* Static IP assignment */
#if LWIP_DHCP
	IP4_ADDR(&gw, 0, 0, 0, 0);
	IP4_ADDR(&ipaddr, 0, 0, 0, 0);
	IP4_ADDR(&netmask, 0, 0, 0, 0);
#else
	IP4_ADDR(&gw, 10, 1, 10, 1);
	IP4_ADDR(&ipaddr, 10, 1, 10, 234);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
#endif

	/* Add netif interface for lpc17xx_8x */
	memset(&lpc_netif, 0, sizeof(lpc_netif));
	if (!netif_add(&lpc_netif, &ipaddr, &netmask, &gw, NULL, lpc_enetif_init,
				   tcpip_input)) {
		DEBUGSTR("Net interface failed to initialize ..\r\n");
		while (1) ;
	}

	netif_set_default(&lpc_netif);
	netif_set_up(&lpc_netif);

	/* Enable MAC interrupts only after LWIP is ready */
	NVIC_SetPriority(ETHERNET_IRQn, IRQ_PRIO_ETHERNET);
	NVIC_EnableIRQ(ETHERNET_IRQn);

#if LWIP_DHCP
	dhcp_start(&lpc_netif);
#endif

	/* Initialize and start application */
	http_server_netconn_init();

	/* This loop monitors the PHY link and will handle cable events
	   via the PHY driver. */
	while (1) {
		/* Call the PHY status update state machine once in a while
		   to keep the link status up-to-date */
		physts = lpcPHYStsPoll();

		/* Only check for connection state when the PHY status has changed */
		if (physts & PHY_LINK_CHANGED) {
			if (physts & PHY_LINK_CONNECTED) {
				/* Set interface speed and duplex */
				if (physts & PHY_LINK_SPEED100) {
					NETIF_INIT_SNMP(&lpc_netif, snmp_ifType_ethernet_csmacd, 100000000);
				}
				else {
					NETIF_INIT_SNMP(&lpc_netif, snmp_ifType_ethernet_csmacd, 10000000);
				}
				if (physts & PHY_LINK_FULLDUPLX) {
					Chip_ENET_SetDuplex(LPC_ETHERNET, true);
				}
				else {
					Chip_ENET_SetDuplex(LPC_ETHERNET, false);
				}

				tcpip_callback_with_block((tcpip_callback_fn) netif_set_link_up,
										  (void *) &lpc_netif, 1);
			}
			else {
				tcpip_callback_with_block((tcpip_callback_fn) netif_set_link_down,
										  (void *) &lpc_netif, 1);
			}
		}

		// DEBUGOUT("Link connect status: %d\n", ((physts & PHY_LINK_CONNECTED) != 0));

		/* Delay for link detection */
		msDelay(250);

		if (!prt_ip && lpc_netif.ip_addr.addr) {
			prt_ip = 1;
			ip_addr_changed(&lpc_netif);
		}
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/

extern void lcd_update_hostip(uint32_t host_ip);

/**
 * @brief	Delay function
 * @param ms	:   Delay value in milliseconds
 * @return None
 * The function provides the delay (Only used in FreeRTOS configuration)
 */
/*********************************************************************//**
 * @brief  MilliSecond delay function based on FreeRTOS
 * @param[in] ms Number of milliSeconds to delay
 * @note		Needed for some functions, do not use prior to FreeRTOS running
 * @return Nothing
 **********************************************************************/
void msDelay(uint32_t ms)
{
	vTaskDelay((configTICK_RATE_HZ * ms) / 1000);
}


/* lwIP initialization function */
void LWIP_Init(void)
{
	return;
}

/**
 * @brief	LWIP Task function
 * @return None
 * The function is called from the main task  function.
 * In stand-alone configuration, this function will monitor the link status and
 * handles the packets.
 * In FreeRTOS/uCOS-III configurations, it will create the network interface task
 */
int main(void)
{
	/* Update system clock and initialize board */
	SystemCoreClockUpdate();
	Board_Init();
	LWIP_Init();

	xTaskCreate(vSetupIFTask, "SetupIFx",
				256, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();
	return 0;
}







