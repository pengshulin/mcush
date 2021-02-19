/*
 * @brief LCD Example for LPCXpresso shiled board
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
#include "board.h"
#include "lcd_st7565s.h"

/* Font header file from SWIM sources*/
#include "lpc_rom8x8.h"
/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#if (defined(BOARD_NXP_LPCXPRESSO_4337) || defined(BOARD_NXP_LPCXPRESSO_1837))
	#define SHIELD_I2C          I2C1
	#define IOX_INT_PORT        2
	#define IOX_INT_PIN         2
	#define IOX_INT_GPIO_PORT   5
	#define IOX_INT_GPIO_PIN    2
	#define IOX_MODE_FUNC    SCU_MODE_FUNC4
#else
	#warning "Channel configurations not set for the board!"
#endif

#define TEMPSEN_SLV_ADDR           0x4C    /* Temperature sensor Slave address */
#define IOX_SLV_ADDR               0x20    /* IO eXpansion slave address */
#define IMUSEN_SLV_ADDR            0x68    /* Bosch IMU Sensor slave address */

#define PININT_INDEX   0	/* PININT index used for GPIO mapping */
#define PININT_IRQ_HANDLER  GPIO0_IRQHandler	/* GPIO interrupt IRQ function name */
#define PININT_NVIC_NAME    PIN_INT0_IRQn	/* GPIO interrupt NVIC interrupt name */

static volatile uint32_t tick;
static volatile uint8_t btn_rdy;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

static void i2c_init(void)
{
	Board_I2C_Init(SHIELD_I2C);

	/* Initialize I2C */
	Chip_I2C_Init(SHIELD_I2C);
	Chip_I2C_SetClockRate(SHIELD_I2C, 400000);
	Chip_I2C_SetMasterEventHandler(SHIELD_I2C, Chip_I2C_EventHandler);
	NVIC_SetPriority(PININT_NVIC_NAME, 1);
	NVIC_EnableIRQ(SHIELD_I2C == I2C0 ? I2C0_IRQn : I2C1_IRQn);
}
static void temp_update(void)
{
	const int y = 18;
	uint8_t buf[32] = {0};
	int16_t val;
	int dc, df;
	I2C_XFER_T xfer = {
		TEMPSEN_SLV_ADDR,
		0,
		1,
		0,
		2,
	};
	xfer.txBuff = xfer.rxBuff = &buf[0];
	Chip_I2C_MasterTransfer(SHIELD_I2C, &xfer);
	if (xfer.status != I2C_STATUS_DONE)
		return ;
	val = (buf[0] << 8) | buf[1];
	DEBUGOUT("VAL = %d\r\n", val);
	val = (val >> 5);
	dc = (val * 1000) / 8;
	df = ((dc * 9) / 500) + 320;
	snprintf((char *) buf, 31, "TMP[C]: %d.%03d", dc / 1000, abs(dc) % 1000);
	LCD_FillRect(3, y, LCD_X_RES - 3, y + 8, 0);
	LCD_PutStrXY(3, y, (char *)buf);
	snprintf((char *) buf, 31, "TMP[F]: %d.%01d%", df / 10, abs(df) % 10);
	LCD_FillRect(3, y + 8 + 2, LCD_X_RES - 3, y + 16 + 2, 0);
	LCD_PutStrXY(3, y + 8 + 2, (char *)buf);
}

/* Write data to io expansion register */
static void iox_write_reg(uint8_t reg, uint8_t val)
{
	uint8_t buf[2];
	buf[0] = reg;
	buf[1] = val;
	Chip_I2C_MasterSend(SHIELD_I2C, IOX_SLV_ADDR, buf, 2);
}

/* Read IO expansion register */
static int iox_read_reg(uint8_t reg)
{
	I2C_XFER_T xfer = {
		IOX_SLV_ADDR,
		0,
		1,
		0,
		1,
	};
	xfer.txBuff = xfer.rxBuff = &reg;
	Chip_I2C_MasterTransfer(SHIELD_I2C, &xfer);
	if (xfer.status != I2C_STATUS_DONE)
		return -1;
	return (int) reg;
}

/* Prints ID Read from the BMI sensor */
static int print_bmi_id(void)
{
	char buf[30];
	const int y = 38;
	uint8_t val = 0;
	I2C_XFER_T xfer = {
		IMUSEN_SLV_ADDR,
		0,
		1,
		0,
		1,
	};
	xfer.txBuff = xfer.rxBuff = &val;
	Chip_I2C_MasterTransfer(SHIELD_I2C, &xfer);
	if (xfer.status != I2C_STATUS_DONE)
		return -1;
	snprintf((char *) buf, 31, "IMUSEN ID: %02X", val);
	LCD_FillRect(3, y, LCD_X_RES - 3, y + 8, 0);
	LCD_PutStrXY(3, y, (char *)buf);
	return 0;
}
/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Systick interrupt handler */
void SysTick_Handler(void)
{
	tick ++;
}

/**
 * @brief	I2C Interrupt Handler
 * @return	None
 */
void I2C1_IRQHandler(void)
{
	Chip_I2C_MasterStateHandler(I2C1);
}

/**
 * @brief	I2C0 Interrupt handler
 * @return	None
 */
void I2C0_IRQHandler(void)
{
	Chip_I2C_MasterStateHandler(I2C0);
}

/**
 * @brief	Handle interrupt from GPIO pin or GPIO pin mapped to PININT
 * @return	Nothing
 */
void PININT_IRQ_HANDLER(void)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
	btn_rdy = 1;
}

/**
 * @brief	Initialize the IOX interface
 * @return	Nothing
 */
void iox_init(void)
{
	/* Start with LEDs off */
	iox_write_reg(3, 0xF0);

	/* Set LED Pins as output */
	iox_write_reg(7, 0x0F);

	/* Clear interrupt by reading inputs */
	iox_read_reg(0);
	iox_read_reg(1);

	/* Set pin back to GPIO (on some boards may have been changed to something
	   else by Board_Init()) */
	Chip_SCU_PinMuxSet(IOX_INT_PORT, IOX_INT_PIN,
					   (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | IOX_MODE_FUNC) );

	/* Configure GPIO pin as input */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, IOX_INT_GPIO_PORT, IOX_INT_GPIO_PIN);

	/* Configure interrupt channel for the GPIO pin in SysCon block */
	Chip_SCU_GPIOIntPinSel(PININT_INDEX, IOX_INT_GPIO_PORT, IOX_INT_GPIO_PIN);

	/* Configure channel interrupt as edge sensitive and falling edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(PININT_NVIC_NAME);
	NVIC_SetPriority(PININT_NVIC_NAME, 3);
	NVIC_EnableIRQ(PININT_NVIC_NAME);
}

/**
 * @brief	main routine for lcd hello world example
 * @return	Function should not exit.
 */
int main(void)
{
	uint8_t pbtn = 0, btn_val = 0;
	SystemCoreClockUpdate();
	Board_Init();

	Board_LCD_Init(); /* Pin Mux and Clock initialization */
	LCD_Init(); /* Initialize LCD Device and Turn it ON */
	i2c_init();
	iox_init();

	/* Draw a rectangle border */
	LCD_DrawRect(0, 0, LCD_X_RES - 1, LCD_Y_RES - 1, 1);
	LCD_DrawRect(1, 1, LCD_X_RES - 2, LCD_Y_RES - 2, 1);

	/* Set foreground as ON */
	LCD_SetFontColor(1);
	LCD_SetFontBgColor(0); /* Background is OFF */
	LCD_SetFont(&font_rom8x8); /* Use 8x8 Fonts */
	LCD_PutStrXY(3, 4, "IOX-SENSOR DEMO"); /* Print string */
	print_bmi_id();

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / 10000);

	while (1) {
		if (!(tick & 1023)) {
			temp_update();
		}
		if (!(tick & 255) || btn_rdy) {
			int ret;
			btn_rdy = 0;
			ret = iox_read_reg(0);
			if (ret >= 0) btn_val = ret;
		}

		if (pbtn != btn_val) {
			iox_write_reg(3, ((btn_val & 0x1C) << 3) | ((btn_val & 1) << 4));
			pbtn = btn_val;
		}
		__WFI();
	}

	return 0;
}






