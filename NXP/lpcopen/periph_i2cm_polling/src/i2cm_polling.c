/*
 * @brief I2CM example
 * This example shows how to use the I2CM interface in a polling context
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

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define SPEED_100KHZ            (100000)
#define ACTIVITY_MASK           (0x003FFFFF)

#if (defined(BOARD_HITEX_EVA_1850) || defined(BOARD_HITEX_EVA_4350))
#define I2C_ADDR_7BIT                       (0x24)
#elif (defined(BOARD_KEIL_MCB_1857) || defined(BOARD_KEIL_MCB_4357))
#define I2C_ADDR_7BIT                       (0x41)
#define I2C_REG_ADDR_7BIT               (0)
#elif (defined(BOARD_NGX_XPLORER_1830) || defined(BOARD_NGX_XPLORER_4330))
#define I2C_ADDR_7BIT                       (0x1A)
#define I2C_REG_ADDR_7BIT               (2)
#elif defined(BOARD_NXP_LPCLINK2_4370)
#define I2C_ADDR_7BIT                       (0x1A)
#elif (defined(BOARD_NXP_LPCXPRESSO_4337) || defined(BOARD_NXP_LPCXPRESSO_1837))
#define I2C_ADDR_7BIT                  (0x40)
#define I2C_REG_ADDR_7BIT              (0x03)
#else
#warning "WARNING: Unknown board configuration!"
#endif

static I2CM_XFER_T  i2cmXferRec;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Toggle LED to show code activity */
static int ShowActivity(int activityCount)
{
	/* Test for time to update LEDs */
	if ((++activityCount & ACTIVITY_MASK) == 0) {
		/* Toggle 1st LED */
		Board_LED_Toggle(0);

		/* Reset activity counter */
		activityCount = 0;
	}

	/* Return new activity count */
	return activityCount;
}

/* Initialize the I2C bus */
static void i2c_app_init(I2C_ID_T id, int speed)
{
	Board_I2C_Init(id);

	/* Initialize I2C */
	Chip_I2C_Init(id);
	Chip_I2C_SetClockRate(id, speed);
}

/* Function to setup and execute I2C transfer request */
static void SetupXferRecAndExecute(uint8_t devAddr,
								   uint8_t *txBuffPtr,
								   uint16_t txSize,
								   uint8_t *rxBuffPtr,
								   uint16_t rxSize)
{
	/* Setup I2C transfer record */
	i2cmXferRec.slaveAddr = devAddr;
	i2cmXferRec.options = 0;
	i2cmXferRec.status = 0;
	i2cmXferRec.txSz = txSize;
	i2cmXferRec.rxSz = rxSize;
	i2cmXferRec.txBuff = txBuffPtr;
	i2cmXferRec.rxBuff = rxBuffPtr;
	Chip_I2CM_XferBlocking(LPC_I2C0, &i2cmXferRec);
}

/* Perform I2CM write on target board */
static void WriteBoard_I2CM(int writeVal)
{
	uint8_t tx_buffer[3];

#if (defined(BOARD_HITEX_EVA_1850) || defined(BOARD_HITEX_EVA_4350))
	/* Test for non-zero write pattern */
	if (writeVal != 0) {
		// Setup all-bit write pattern */
		writeVal = 0xFF;
	}

	/* Setup output buffer */
	tx_buffer[0] = writeVal;
	tx_buffer[1] = writeVal;

	/* Write 0000 or FFFF via I2CM to PCA9673PW CHIP on 1850/4350 board */
	SetupXferRecAndExecute(I2C_ADDR_7BIT, tx_buffer, 2, NULL, 0);
#elif (defined(BOARD_KEIL_MCB_1857) || defined(BOARD_KEIL_MCB_4357))
	/* Write value via I2CM to STMPE811 CHIP ID REG on 1857/4357 board */
	tx_buffer[0] = I2C_REG_ADDR_7BIT;
	tx_buffer[1] = writeVal & 1;
	SetupXferRecAndExecute(I2C_ADDR_7BIT, tx_buffer, 2, NULL, 0);
#elif (defined(BOARD_NGX_XPLORER_1830) || defined(BOARD_NGX_XPLORER_4330))
	/* Write audio power value via I2CM to UDA1380 part on 1830/4330 board */
	tx_buffer[0] = I2C_REG_ADDR_7BIT;

	/* Test for non-zero write pattern */
	if (writeVal != 0) {
		/* Turn all audio power on */
		tx_buffer[1] = 0xA5;
		tx_buffer[2] = 0xDF;
	}
	else {
		/* Turn all audio power off */
		tx_buffer[1] = 0;
		tx_buffer[2] = 0;
	}

	/* Write audio power value */
	SetupXferRecAndExecute(I2C_ADDR_7BIT, tx_buffer, 3, NULL, 0);
#elif defined(BOARD_NXP_LPCLINK2_4370)
	/* This transfer will NAK since no on-board I2C devices exist */
	SetupXferRecAndExecute(I2C_ADDR_7BIT, tx_buffer, 3, NULL, 0);
#elif (defined(BOARD_NXP_LPCXPRESSO_4337) || defined(BOARD_NXP_LPCXPRESSO_1837))
	/* set configuration to default value */
	tx_buffer[0] = 0x00; /* Write to Config register */
	tx_buffer[1] = 0x39;
	tx_buffer[2] = 0x9F;
	SetupXferRecAndExecute(I2C_ADDR_7BIT, tx_buffer, 3, NULL, 0);
#else
#warning "WARNING: Unknown board for I2CM write operation!"
#endif
}

/* Perform I2CM read on target board */
static void ReadBoard_I2CM()
{
	uint8_t tx_buffer[3];
	uint8_t rx_buffer[3];

#if (defined(BOARD_HITEX_EVA_1850) || defined(BOARD_HITEX_EVA_4350))
	/* Read register via I2CM from STMPE811 CHIP ID REG on 1850/4350 board */
	SetupXferRecAndExecute(I2C_ADDR_7BIT, tx_buffer, 0, rx_buffer, 2);
#elif (defined(BOARD_KEIL_MCB_1857) || defined(BOARD_KEIL_MCB_4357))
	/* Read value via I2CM from STMPE811 CHIP ID REG on 1857/4357 board */
	tx_buffer[0] = I2C_REG_ADDR_7BIT;
	SetupXferRecAndExecute(I2C_ADDR_7BIT, tx_buffer, 1, rx_buffer, 2);
#elif (defined(BOARD_NGX_XPLORER_1830) || defined(BOARD_NGX_XPLORER_4330))
	/* Read value via I2CM from UDA1380 part on 1830/4330 board */
	tx_buffer[0] = I2C_REG_ADDR_7BIT;
	SetupXferRecAndExecute(I2C_ADDR_7BIT, tx_buffer, 1, rx_buffer, 2);
#elif defined(BOARD_NXP_LPCLINK2_4370)
	/* This transfer will NAK since no on-board I2C devices exist */
	SetupXferRecAndExecute(I2C_ADDR_7BIT, tx_buffer, 0, rx_buffer, 0);
#elif (defined(BOARD_NXP_LPCXPRESSO_4337) || defined(BOARD_NXP_LPCXPRESSO_1837))
	tx_buffer[0] = I2C_REG_ADDR_7BIT; /* Read the Voltage across the shunt */
	rx_buffer[0] = 0;
	rx_buffer[1] = 0;
	SetupXferRecAndExecute(I2C_ADDR_7BIT, tx_buffer, 1, rx_buffer, 2);
	DEBUGOUT("Voltage Reading across shunt: 0x%02X%02X\r\n", rx_buffer[0], rx_buffer[1]);
#else
#warning "WARNING: Unknown board for I2CM read operation!"
#endif
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Main program body
 * @return	int
 */
int main(void)
{
	int tmp = 0;
	int activityIndex = 0;
	int writeVal = 0;

	SystemCoreClockUpdate();
	Board_Init();
	i2c_app_init(I2C0, SPEED_100KHZ);

	/* Loop forever */
	while (1) {
		/* Toggle LED to show activity. */
		tmp = ShowActivity(tmp);

		/* Test for activity time */
		if ((tmp & ACTIVITY_MASK) == 0) {
			/* Toggle between writes and reads */
			switch (activityIndex++ & 1) {
			case 0:
				/* Perform target board I2CM write */
				WriteBoard_I2CM(writeVal++ & 1);
				break;

			case 1:
			default:
				/* Perform target board I2CM read */
				ReadBoard_I2CM();
				break;
			}
		}
	}
}






