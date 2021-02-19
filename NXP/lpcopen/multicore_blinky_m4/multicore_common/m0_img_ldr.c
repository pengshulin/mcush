/*
 * @brief M0 Image loader module
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
#include "app_multicore_cfg.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* M0 Boot loader */
int M0Image_Boot(CPUID_T cpuid, uint32_t m0_image_addr)
{
	/* Make sure the alignment is OK */
	if (m0_image_addr & 0xFFF) {
		return -1;
	}

	/* Sanity check, see if base address and reset handler address resides in same region */
	if ((m0_image_addr & 0xFFF00000) != ((*(unsigned long *) (m0_image_addr + 4)) & 0xFFF00000)) {
		return -2;
	}

	if (cpuid == CPUID_M0APP) {
		/* Make sure the M0 core is being held in reset via the RGU */
		Chip_RGU_TriggerReset(RGU_M0APP_RST);

		Chip_Clock_Enable(CLK_M4_M0APP);

		/* Keep in mind the M0 image must be aligned on a 4K boundary */
		Chip_CREG_SetM0AppMemMap(m0_image_addr);

		Chip_RGU_ClearReset(RGU_M0APP_RST);
	}

	if (cpuid == CPUID_M0SUB) {
		/* Make sure the M0 core is being held in reset via the RGU */
		Chip_RGU_TriggerReset(RGU_M0SUB_RST);

		Chip_Clock_Enable(CLK_PERIPH_CORE);

		/* Keep in mind the M0 image must be aligned on a 4K boundary */
		Chip_CREG_SetM0SubMemMap(m0_image_addr);

		Chip_RGU_ClearReset(RGU_M0SUB_RST);
	}

	return 0;
}







