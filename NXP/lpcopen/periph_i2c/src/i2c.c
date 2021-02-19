/*
 * @brief I2C example
 * This example show how to use the I2C interface
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
#include <string.h>
#include "board.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#ifdef DEBUG_ENABLE
#define DEFAULT_I2C          I2C0
#else
#define DEFAULT_I2C          I2C1
#endif

#define I2C_EEPROM_BUS       DEFAULT_I2C
#define I2C_IOX_BUS          DEFAULT_I2C

#define SPEED_100KHZ         100000
#define SPEED_400KHZ         400000

#ifdef DEBUG_ENABLE
static const char menu[] =
	"**************** I2C Demo Menu ****************\r\n"
	"\t0: Exit Demo\r\n"
	"\t1: Select I2C peripheral [\033[1;32mI2C%d\033[0;37m]\r\n"
	"\t2: Toggle mode POLLING/INTERRUPT [\033[1;32m%s\033[0;37m]\r\n"
	"\t3: Probe for Slave devices\r\n"
	"\t4: Read slave data\r\n"
	"\t5: Write slave data\r\n"
	"\t6: Write/Read slave data\r\n";
#endif

static int mode_poll;	/* Poll/Interrupt mode flag */
static I2C_ID_T i2cDev = DEFAULT_I2C;	/* Currently active I2C device */

/* EEPROM SLAVE data */
#define I2C_SLAVE_EEPROM_SIZE       64
#define I2C_SLAVE_EEPROM_ADDR       0x5A
#define I2C_SLAVE_IOX_ADDR          0x5B

/* Xfer structure for slave operations */
static I2C_XFER_T seep_xfer;
static I2C_XFER_T iox_xfer;

/* Data area for slave operations */
static uint8_t seep_data[I2C_SLAVE_EEPROM_SIZE + 1];
static uint8_t buffer[2][256];
static uint8_t iox_data[2];	/* PORT0 input port, PORT1 Output port */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* State machine handler for I2C0 and I2C1 */
static void i2c_state_handling(I2C_ID_T id)
{
	if (Chip_I2C_IsMasterActive(id)) {
		Chip_I2C_MasterStateHandler(id);
	}
	else {
		Chip_I2C_SlaveStateHandler(id);
	}
}

/* Print data to console */
static void con_print_data(const uint8_t *dat, int sz)
{
	int i;
	if (!sz) {
		return;
	}
	for (i = 0; i < sz; i++) {
		if (!(i & 0xF)) {
			DEBUGOUT("\r\n%02X: ", i);
		}
		DEBUGOUT(" %02X", dat[i]);
	}
	DEBUGOUT("\r\n");
}

/* Get an integer input from UART */
static int con_get_input(const char *str)
{
#ifdef DEBUG_ENABLE
	int input_valid = 0;
	int x;
	char ch[16], *ptr;
	int i = 0;

	while (!input_valid) {
		DEBUGOUT("%s", str);
		while (1) {
			/* Setting poll mode for slave is a very bad idea, it works nevertheless */
			if ((mode_poll & (1 << i2cDev)) && Chip_I2C_IsStateChanged(i2cDev)) {
				Chip_I2C_SlaveStateHandler(i2cDev);
			}

			x = DEBUGIN();
			if (x == EOF) {
				continue;
			}
			if (i >= sizeof(ch) - 2) {
				break;
			}
			if (((x == '\r') || (x == '\n')) && i) {
				DEBUGOUT("\r\n");
				break;
			}
			if (x == '\b') {
				if (i) {
					DEBUGOUT("\033[1D \033[1D");
					i--;
				}
				continue;
			}
			DEBUGOUT("%c", x);
			ch[i++] = x;
		}
		ch[i] = 0;
		i = strtol(ch, &ptr, 0);
		if (*ptr) {
			i = 0;
			DEBUGOUT("Invalid input. Retry!\r\n");
			continue;
		}
		input_valid = 1;
	}
	return i;
#else
	int pressed = 0;
	volatile uint8_t *pkey = &iox_data[0];
	static int sind = -1;
	static uint8_t val[] = {5, I2C_SLAVE_IOX_ADDR, 1, 0};
	if (sind >= sizeof(val)) {
		sind = -1;
	}

	while (sind < 0) {
		if (*pkey) {
			pressed = 1;
		}
		if (*pkey && pressed) {
			continue;
		}
		if (!*pkey && !pressed) {
			continue;
		}
		pressed = 0;
		sind = 0;
		val[3]++; val[3] &= 0x3;
	}
	return val[sind++];
#endif
}

static void i2c_rw_input(I2C_XFER_T *xfer, int ops)
{
	int tmp, i;

	tmp = con_get_input("Enter 7-Bit Slave address : ");
	tmp &= 0xFF;
	xfer->slaveAddr = tmp;
	xfer->rxBuff = 0;
	xfer->txBuff = 0;
	xfer->txSz = 0;
	xfer->rxSz = 0;

	if (ops & 1) {
		tmp = con_get_input("Enter number of bytes to read : ");
		tmp &= 0xFF;
		xfer->rxSz = tmp;
		xfer->rxBuff = buffer[1];
	}

	if (ops & 2) {
		tmp = con_get_input("Enter number of bytes to write : ");
		tmp &= 0xFF;
		for (i = 0; i < tmp; i++) {
			DEBUGOUT("%d:", i + 1);
			buffer[0][i] = con_get_input("Enter Data: ");
		}
		xfer->txSz = tmp;
		xfer->txBuff = buffer[0];
	}
}

/* Set I2C mode to polling/interrupt */
static void i2c_set_mode(I2C_ID_T id, int polling)
{
	if (!polling) {
		mode_poll &= ~(1 << id);
		Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandler);
		NVIC_EnableIRQ(id == I2C0 ? I2C0_IRQn : I2C1_IRQn);
	}
	else {
		mode_poll |= 1 << id;
		NVIC_DisableIRQ(id == I2C0 ? I2C0_IRQn : I2C1_IRQn);
		Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandlerPolling);
	}
}

/* Initialize the I2C bus */
static void i2c_app_init(I2C_ID_T id, int speed)
{
	Board_I2C_Init(id);

	/* Initialize I2C */
	Chip_I2C_Init(id);
	Chip_I2C_SetClockRate(id, speed);

	/* Set default mode to interrupt */
	i2c_set_mode(id, 0);
}

/* Function that probes all available slaves connected to an I2C bus */
static void i2c_probe_slaves(I2C_ID_T i2c)
{
	int i;
	uint8_t ch[2];

	DEBUGOUT("Probing available I2C devices...\r\n");
	DEBUGOUT("\r\n     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
	DEBUGOUT("\r\n====================================================");
	for (i = 0; i <= 0x7F; i++) {
		if (!(i & 0x0F)) {
			DEBUGOUT("\r\n%02X  ", i >> 4);
		}
		if ((i <= 7) || (i > 0x78)) {
			DEBUGOUT("   ");
			continue;
		}
		/* Address 0x48 points to LM75AIM device which needs 2 bytes be read */
		if (Chip_I2C_MasterRead(i2c, i, ch, 1 + (i == 0x48)) > 0) {
			DEBUGOUT(" %02X", i);
		}
		else {
			DEBUGOUT(" --");
		}
	}
	DEBUGOUT("\r\n");
}

static int i2c_menu(void)
{
	DEBUGOUT(menu, i2cDev, (mode_poll & (1 << i2cDev)) ? "POLLING" : "INTERRUPT");
	return con_get_input("\r\nSelect an option [0 - 6] :");
}

/* Update the EEPROM state */
static void i2c_eeprom_update_state(I2C_XFER_T *xfer, uint8_t *buff, int sz)
{
	xfer->txBuff = xfer->rxBuff = &buff[buff[0] + 1];
	xfer->rxSz = xfer->txSz = sz - buff[0] + 1;	/* one byte more to avoid last byte problem */
}

/* Slave event handler for simulated EEPROM */
static void i2c_eeprom_events(I2C_ID_T id, I2C_EVENT_T event)
{
	static int is_addr = 1;
	switch (event) {
	case I2C_EVENT_DONE:
		is_addr = 1;
		i2c_eeprom_update_state(&seep_xfer, seep_data, I2C_SLAVE_EEPROM_SIZE);
		seep_xfer.rxBuff = seep_data;
		seep_xfer.rxSz++;
		break;

	case I2C_EVENT_SLAVE_RX:
		if (is_addr) {
			is_addr = 0;
			seep_data[0] &= (I2C_SLAVE_EEPROM_SIZE - 1);	/* Correct addr if required */
			i2c_eeprom_update_state(&seep_xfer, seep_data, I2C_SLAVE_EEPROM_SIZE);
			break;
		}

		seep_data[0]++;
		seep_data[0] &= (I2C_SLAVE_EEPROM_SIZE - 1);
		if (seep_xfer.rxSz == 1) {
			i2c_eeprom_update_state(&seep_xfer, seep_data, I2C_SLAVE_EEPROM_SIZE);
		}
		break;

	case I2C_EVENT_SLAVE_TX:
		seep_data[0]++;
		seep_data[0] &= (I2C_SLAVE_EEPROM_SIZE - 1);
		if (seep_xfer.txSz == 1) {
			i2c_eeprom_update_state(&seep_xfer, seep_data, I2C_SLAVE_EEPROM_SIZE);
		}
		break;

	default: /* Other events are ignored */
		break;
	}
}

/* Simulate an I2C EEPROM slave */
static void i2c_eeprom_init(I2C_ID_T id)
{
	memset(&seep_data[1], 0xFF, I2C_SLAVE_EEPROM_SIZE);
	seep_xfer.slaveAddr = (I2C_SLAVE_EEPROM_ADDR << 1);
	seep_xfer.txBuff = &seep_data[1];
	seep_xfer.rxBuff = seep_data;
	seep_xfer.txSz = seep_xfer.rxSz = sizeof(seep_data);
	Chip_I2C_SlaveSetup(id, I2C_SLAVE_0, &seep_xfer, i2c_eeprom_events, 0);
}

/*-------- IO Expansion slave device implementation ----------*/
/* Update IN/OUT port states to real devices */
void i2c_iox_update_regs(int ops)
{
	if (ops & 1) {	/* update out port */
		Board_LED_Set(0, iox_data[1] & 1);
		Board_LED_Set(1, iox_data[1] & 2);
		Board_LED_Set(2, iox_data[1] & 4);
		Board_LED_Set(3, iox_data[1] & 8);
	}

	if (ops & 2) {	/* update in port */
		iox_data[0] = (uint8_t) Buttons_GetStatus();
	}
}

/* Slave event handler for simulated EEPROM */
static void i2c_iox_events(I2C_ID_T id, I2C_EVENT_T event)
{
	switch (event) {
	case I2C_EVENT_DONE:
		iox_xfer.rxBuff = &iox_data[1];
		iox_xfer.rxSz = sizeof(iox_data);		/* one byte more to avoid last byte problem */
		iox_xfer.txBuff = (const uint8_t *) iox_data;
		iox_xfer.txSz = sizeof(iox_data) + 1;		/* one byte more to avoid last byte problem */
		break;

	case I2C_EVENT_SLAVE_RX:
		iox_xfer.rxBuff = &iox_data[1];
		iox_xfer.rxSz = sizeof(iox_data);		/* one byte more to avoid last byte problem */
		i2c_iox_update_regs(1);
		break;

	case I2C_EVENT_SLAVE_TX:
		if (iox_xfer.txSz == 1) {
			iox_xfer.txBuff = (const uint8_t *) iox_data[0];
			iox_xfer.txSz = sizeof(iox_data) + 1;		/* one byte more to avoid last byte problem */
		}
		break;
	}
}

/* Simulate an IO Expansion slave device */
static void i2c_iox_init(I2C_ID_T id)
{
	Board_Buttons_Init();
	iox_xfer.slaveAddr = (I2C_SLAVE_IOX_ADDR << 1);
	i2c_iox_events(id, I2C_EVENT_DONE);
	Chip_I2C_SlaveSetup(id, I2C_SLAVE_1, &iox_xfer, i2c_iox_events, 0);
	i2c_iox_update_regs(3);
	/* Setup SysTick timer to get the button status updated at regular intervals */
	SysTick_Config(Chip_Clock_GetRate(CLK_MX_MXCORE) / 50);
}

/*-------------------- End of IO Expansion slave device ----------------------*/

/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	SysTick Interrupt Handler
 * @return	Nothing
 * @note	Systick interrupt handler updates the button status
 */
void SysTick_Handler(void)
{
	i2c_iox_update_regs(2);
}

/**
 * @brief	I2C Interrupt Handler
 * @return	None
 */
void I2C1_IRQHandler(void)
{
	i2c_state_handling(I2C1);
}

/**
 * @brief	I2C0 Interrupt handler
 * @return	None
 */
void I2C0_IRQHandler(void)
{
	i2c_state_handling(I2C0);
}

/**
 * @brief	Main program body
 * @return	int
 */
int main(void)
{
	int tmp;
	int xflag = 0;
	static I2C_XFER_T xfer;

	SystemCoreClockUpdate();
	Board_Init();
	i2c_app_init(I2C0, SPEED_100KHZ);
	i2c_app_init(I2C1, SPEED_100KHZ);

	/* Simulate an EEPROM slave in I2C0 */
	i2c_eeprom_init(I2C_EEPROM_BUS);

	/* Simuldate an IO Expansion slave in I2C0 */
	i2c_iox_init(I2C_IOX_BUS);

	while (!xflag) {
		switch (i2c_menu()) {
		case 0:
			xflag = 1;
			DEBUGOUT("End of I2C Demo! Bye!\r\n");
			break;

		case 1:
			tmp = con_get_input("Select I2C device [0 or 1] : ");
			DEBUGOUT("\r\n");
			if ((I2C_ID_T) tmp == I2C0) {
				if (i2cDev == I2C0) {
					break;
				}
				i2c_set_mode(I2C0, 0);
				i2cDev = I2C0;
			}
			else if ((I2C_ID_T) tmp == I2C1) {
				if (i2cDev == I2C1) {
					break;
				}
				i2c_set_mode(I2C1, 0);
				i2cDev = I2C1;
			}
			else {
				DEBUGOUT("Invalid I2C Device [Must be 0 or 1]\r\n");
			}
			break;

		case 2:
			i2c_set_mode(i2cDev, !(mode_poll & (1 << i2cDev)));
			break;

		case 3:
			i2c_probe_slaves(i2cDev);
			break;

		case 4:
			i2c_rw_input(&xfer, 1);
			tmp = Chip_I2C_MasterRead(i2cDev, xfer.slaveAddr, xfer.rxBuff, xfer.rxSz);
			DEBUGOUT("Read %d bytes of data from slave 0x%02X.\r\n", tmp, xfer.slaveAddr);
			con_print_data(buffer[1], tmp);
			break;

		case 5:
			i2c_rw_input(&xfer, 2);
			if (xfer.txSz == 0) {
				break;
			}
			tmp = Chip_I2C_MasterSend(i2cDev, xfer.slaveAddr, xfer.txBuff, xfer.txSz);
			DEBUGOUT("Written %d bytes of data to slave 0x%02X.\r\n", tmp, xfer.slaveAddr);
			break;

		case 6:
			i2c_rw_input(&xfer, 3);
			tmp = xfer.rxSz;
			if (!tmp && !xfer.txSz) {
				break;
			}
			Chip_I2C_MasterTransfer(i2cDev, &xfer);
			DEBUGOUT("Master transfer : %s\r\n",
					 xfer.status == I2C_STATUS_DONE ? "SUCCESS" : "FAILURE");
			DEBUGOUT("Received %d bytes from slave 0x%02X\r\n", tmp - xfer.rxSz, xfer.slaveAddr);
			con_print_data(buffer[1], tmp - xfer.rxSz);
			break;

		default:
			DEBUGOUT("Input Invalid! Try Again.\r\n");
		}
	}
	Chip_I2C_DeInit(I2C0);
	Chip_I2C_DeInit(I2C1);

	return 0;
}






