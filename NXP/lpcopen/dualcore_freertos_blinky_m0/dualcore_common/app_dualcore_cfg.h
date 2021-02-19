/*
 * @brief LPC43xx dual-core example configuration file
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

#ifndef __APP_DUALCORE_CFG_H_
#define __APP_DUALCORE_CFG_H_

/*
 * Users can enable any of the following macros so that the examples will be
 * built with that functionality.
 */
#define EXAMPLE_BLINKY
#if 0 /* These are now defined in project files */
#undef  EXAMPLE_USB_DEVICE
#undef  EXAMPLE_LWIP
#undef  EXAMPLE_EMWIN
#undef  EXAMPLE_USB_HOST      /* Not supported yet */
#endif

/*
 * Users can define any one of the following macros to use the appropriate OS.
 * For standalone executables both the macros should be disabled.
 */
//#undef  OS_FREE_RTOS
//#undef  OS_UCOS_III

/*
 * The following defines are used by the M4 image to locate the M0 image, and
 * one of the below macro must be defined based on your linker definition file.
 * If the linker file is made to generate image in SPIFI area then define
 * TARGET_SPIFI macro (so that M4 image will locate the corresponding M0 in the
 * SPIFI region). If none of them r defined the following defaults will be used
 * For LPC4330 NGX XPLORER BOARD:      TARGET_SPIFI
 * For LPC4357 KEIL MCB BOARD:         TARGET_IFLASH
 * For LPC4350 HITEX EVALUATION BOARD: TARGET_SPIFI
 */
#if 0 /* These are now defined in project files */
#undef TARGET_SPIFI     /* SPIFI Flash */
#undef TARGET_IFLASH    /* Internal Flash */
#undef TARGET_XFLASH    /* External NOR Flash */
#endif

/* Include Common header here */
#include "dualcore_common.h"

#endif /* ifndef __APP_DUALCORE_CFG_H_ */






