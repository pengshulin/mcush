/*
 * @brief LPC43XX Multicore application configuration file
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

#ifndef __APP_MULTICORE_CFG_H_
#define __APP_MULTICORE_CFG_H_

#include "board.h"

typedef enum CPUID {
	CPUID_MIN,
	CPUID_M4 = CPUID_MIN,
	CPUID_M0APP,
	CPUID_M0SUB,
	CPUID_MAX
} CPUID_T;

#ifdef LPC43XX_CORE_M0APP
#define CPUID_CURR     CPUID_M0APP
#endif

#ifdef LPC43XX_CORE_M0SUB
#define CPUID_CURR     CPUID_M0SUB
#endif

#define SYS_OS_ID      0/* 0 - No OS, 1 - FreeRTOS, 2 - UCOS-III */

#define PID_BLINKY     0x123

#define BLINKY_LED_MASK   0x1

#define BLINKY_ON_CPU   CPUID_M0APP
#define BLINKY_OFF_CPU  CPUID_M0APP

/* Size of applications in flash */
#define IMG_SZ_M4      (32 * 1024)
#define IMG_SZ_M0APP   (16 * 1024)
#define IMG_SZ_M0SUB   (16 * 1024)

#define SPIFI_BASE_ADDR     0x14000000	/* SPIFI BASE ADDR */
#define IFLASH_A_BASE_ADDR  0x1A000000	/* IFLASH BANK-A */
#define IFLASH_B_BASE_ADDR  0x1B000000	/* IFLASH BANK-B */

#define SHARED_MEM_IPC      0x10088000	/* Shared memory used by IPC */

#if !defined(BASE_ADDRESS_M0APP) && (defined(BOARD_NXP_LPCLINK2_4370) || defined(BOARD_HITEX_EVA_4350) || \
	defined(BOARD_NGX_XPLORER_4330))
#define BASE_ADDRESS_M0APP     (SPIFI_BASE_ADDR + IMG_SZ_M4)
#endif

#if !defined(BASE_ADDRESS_M0APP) && (defined(BOARD_KEIL_MCB_4357) || defined(BOARD_NXP_LPCXPRESSO_4337))
#define BASE_ADDRESS_M0APP     (IFLASH_B_BASE_ADDR)
#endif

#if !defined(BASE_ADDRESS_M0SUB) && defined(BOARD_NXP_LPCLINK2_4370)
#define BASE_ADDRESS_M0SUB     (SPIFI_BASE_ADDR + IMAGE_SZ_M4 + IMAGE_SZ_M0APP)
#undef  BLINKY_OFF_CPU
#define BLINKY_OFF_CPU          CPUID_M0SUB
#endif

#endif /* __APP_MULTICORE_CFG_H_ */






