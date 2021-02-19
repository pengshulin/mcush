/*
 * @brief Grouped GPIO Interrupt example
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

#if defined(BOARD_HITEX_EVA_4350) || defined(BOARD_HITEX_EVA_1850)

/* The only switch available on this board is the ISP switch input.  The other
   switches are connected to an I2C device. */
#define TEST_BUTTON_PORT            0
#define TEST_BUTTON_BIT             7
#define TEST_BUTTON_PIN_PORT        2
#define TEST_BUTTON_PIN_BIT         7
#define TEST_BUTTON_MODE_FUNC       SCU_MODE_FUNC0

#elif (defined(BOARD_NXP_LPCXPRESSO_4337) || defined(BOARD_NXP_LPCXPRESSO_1837))

/* The only switch available on this board is the ISP switch input.  The other
   switches are connected to an I2C device. */
#define TEST_BUTTON_PORT            0
#define TEST_BUTTON_BIT             7
#define TEST_BUTTON_PIN_PORT        2
#define TEST_BUTTON_PIN_BIT         7
#define TEST_BUTTON_MODE_FUNC       SCU_MODE_FUNC0

#elif defined(BOARD_KEIL_MCB_4357) || defined(BOARD_KEIL_MCB_1857)

/* This is the P4_0 button. */
#define TEST_BUTTON_PORT            2
#define TEST_BUTTON_BIT             0
#define TEST_BUTTON_PIN_PORT        4
#define TEST_BUTTON_PIN_BIT         0
#define TEST_BUTTON_MODE_FUNC       SCU_MODE_FUNC0

#elif defined(BOARD_NGX_XPLORER_1830) || defined(BOARD_NGX_XPLORER_4330)

// FIXME: - These boards were not tested with IAR toolchain.  There are errors
//          downloading to flash, etc.

/* This is the USER_SWITCH. */
#define TEST_BUTTON_PORT            0
#define TEST_BUTTON_BIT             7
#define TEST_BUTTON_PIN_PORT        2
#define TEST_BUTTON_PIN_BIT         7
#define TEST_BUTTON_MODE_FUNC       SCU_MODE_FUNC0

#elif defined(BOARD_NXP_LPCLINK2_4370)

/* This is JP1.  Install JP1 to invoke interrupt. */
#define TEST_BUTTON_PORT            5
#define TEST_BUTTON_BIT             7
#define TEST_BUTTON_PIN_PORT        2
#define TEST_BUTTON_PIN_BIT         8
#define TEST_BUTTON_MODE_FUNC       SCU_MODE_FUNC4

#else
#error "Grouped GPIO Interrupt not configured for this example"
#endif /* defined(BOARD_HITEX_EVA_4350) || defined(BOARD_HITEX_EVA_1850) */

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
 * @brief	Handle Group GPIO 0 interrupt
 * @return	Nothing
 */
void GINT0_IRQHandler(void)
{
	Chip_GPIOGP_ClearIntStatus(LPC_GPIOGROUP, 0);
	Board_LED_Toggle(0);
}

/**
 * @brief	Main program body
 * @return	Does not return
 */
int main(void)
{
	/* Generic Initialization */
	SystemCoreClockUpdate();

	/* Board_Init calls Chip_GPIO_Init and enables GPIO clock if needed,
	   Chip_GPIO_Init is not called again */
	Board_Init();
	Board_LED_Set(0, false);

	/* Set pin back to GPIO (on some boards may have been changed to something
	   else by Board_Init()) */
	Chip_SCU_PinMuxSet(TEST_BUTTON_PIN_PORT, TEST_BUTTON_PIN_BIT,
					   (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | TEST_BUTTON_MODE_FUNC) );

	/* Group GPIO interrupt 0 will be invoked when the button is pressed. */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, TEST_BUTTON_PORT, TEST_BUTTON_BIT);
	Chip_GPIOGP_SelectLowLevel(LPC_GPIOGROUP, 0, TEST_BUTTON_PORT, 1 << TEST_BUTTON_BIT);
	Chip_GPIOGP_EnableGroupPins(LPC_GPIOGROUP, 0, TEST_BUTTON_PORT, 1 << TEST_BUTTON_BIT);
	Chip_GPIOGP_SelectAndMode(LPC_GPIOGROUP, 0);
	Chip_GPIOGP_SelectEdgeMode(LPC_GPIOGROUP, 0);

	/* Enable Group GPIO interrupt 0 */
	NVIC_EnableIRQ(GINT0_IRQn);

	/* Spin in a loop here.  All the work is done in ISR. */
	while (1) {}
}






