/*
 * @brief OTP Controller example
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
#include "chip.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

static void OTP_fix(volatile uint32_t dummy0, volatile uint32_t dummy1,
        volatile uint32_t dummy2, volatile uint32_t dummy3)
{
    return;
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Main entry point
 * @return	Nothing
 */
int main(void)
{
	volatile uint32_t status;

	SystemCoreClockUpdate();
	Board_Init();

	/* Initialize the OTP Controller */
	status = Chip_OTP_Init();

    /* Fix as per Errata, required for some LPC43xx parts */
    OTP_fix(0, 0, 0, 0);

    /* Set Boot Source */
    /* Please note that this function is commented to avoid accidental
     * programming of OTP values (which can result in not booting of boards).
     * Make sure that you understand the OTP programming before you
     * try this example
     */
#if (defined(BOARD_KEIL_MCB_1857) || defined(BOARD_KEIL_MCB_4357))
    //status = Chip_OTP_ProgBootSrc(CHIP_OTP_BOOTSRC_PINS);
#else
    //status = Chip_OTP_ProgBootSrc(CHIP_OTP_BOOTSRC_SPIFI);
#endif

	while (1);
}






