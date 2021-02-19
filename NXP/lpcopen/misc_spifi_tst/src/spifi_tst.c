/*
 * @brief LPCSPIFILIB example
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
#include <string.h>
#include "spifi_18xx_43xx.h"


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define TICKRATE_HZ1 (1000)	/* 1000 ticks per second I.e 1 mSec / tick */

/* Maximum clock supported by test environment */
#define MAX_SPIFI_CLOCK		50000000

#ifndef SPIFLASH_BASE_ADDRESS
#define SPIFLASH_BASE_ADDRESS (0x14000000)
#endif

// spifi_reset CAN NOT be called while running code from SPIFI

void spifi_reset(void) {
	LPC_SPIFI->STAT = SPIFI_STAT_RESET;
	while ((LPC_SPIFI->STAT & SPIFI_STAT_RESET) != 0) {}

	LPC_SPIFI->MEMCMD =0;
	LPC_SPIFI->DATINTM =0;

	LPC_SPIFI->STAT = SPIFI_STAT_RESET;
	while ((LPC_SPIFI->STAT & SPIFI_STAT_RESET) != 0) {}		
}

// spifi_reset CAN be called while running code from SPIFI

void spifi_init(void) {
	
	// when booting from SPIFI these pins are all enabled as SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC3

	LPC_SCU->SFSP[0x3][3] = (SCU_PINIO_FAST | SCU_MODE_FUNC3);	/* SPIFI CLK */
	LPC_SCU->SFSP[0x3][4] = (SCU_PINIO_FAST | SCU_MODE_FUNC3);	/* SPIFI D3 */
	LPC_SCU->SFSP[0x3][5] = (SCU_PINIO_FAST | SCU_MODE_FUNC3);	/* SPIFI D2 */
	LPC_SCU->SFSP[0x3][6] = (SCU_PINIO_FAST | SCU_MODE_FUNC3);	/* SPIFI D1 */
	LPC_SCU->SFSP[0x3][7] = (SCU_PINIO_FAST | SCU_MODE_FUNC3);	/* SPIFI D0 */
	LPC_SCU->SFSP[0x3][8] = (SCU_PINIO_FAST | SCU_MODE_FUNC3);	/* SPIFI CS/SSEL */
	
	// enable FBCLK and change defaults
	
	LPC_SPIFI->CTRL = SPIFI_CTRL_TO(1000) |			// default value is 0xFFFF
										SPIFI_CTRL_CSHI(15) |			// this is default value
										SPIFI_CTRL_RFCLK(1) |			// this is default value
										SPIFI_CTRL_FBCLK(1);

};


/*****************************************************************************
 * Switch between command mode and memory mode on SPIFI 
 *	-- memory mode reads as code/data memory at 0x14000000 / 0x80000000
 ****************************************************************************/

void spifi_memory_mode(void) {
  #ifdef CORE_M4	
	SCnSCB->ACTLR &= ~2; // disable Cortex write buffer to avoid exceptions when switching back to SPIFI for execution
  #endif
	
	LPC_SPIFI->STAT = 0x10;	// reset memory mode
	while(LPC_SPIFI->STAT & 0x10);	// wait for reset to complete

	LPC_SPIFI->CTRL = // set up new CTRL register with high speed options
		(0x100 << 0) | 	// timeout
		(0x1 << 16) | 	// cs high, this parameter is dependent on the SPI Flash part and is in SPIFI_CLK cycles. A lower cs high performs faster
		(1 << 29) | 		// receive full clock (rfclk) - allows for higher speeds
		(1 << 30); 			// feedback clock (fbclk) - allows for higher speeds
	
	// put part in high speed mode (skipping opcodes)
	LPC_SPIFI->DATINTM = 0xa5; // 0xAx will cause part to use high performace reads (skips opcode on subsequent accesses)
	LPC_SPIFI->CMD =
		(0xebul << 24) | 	// opcode 0xeb Quad IO High Performance Read for Spansion
		(0x4 << 21) | 		// frame form indicating opcode and 3 address bytes
		(0x2 << 19) | 		// field form indicating serial opcode and dual/quad other fields
		(0x3 << 16); 			// 3 intermediate data bytes
	while(LPC_SPIFI->STAT & 2); // wait for command to complete

	LPC_SPIFI->MEMCMD =
		(0xebul << 24) | 	// opcode 0xeb Quad IO High Performance Read for Spansion
		(0x6 << 21) | 		// frame form indicating no opcode and 3 address bytes
		(0x2 << 19) | 		// field form indicating serial opcode and dual/quad other fields
		(0x3 << 16); 			// 3 intermediate data bytes
	
  #ifdef CORE_M4	
	SCnSCB->ACTLR |= 2; // reenable Cortex write buffer
  #endif
}

void spifi_command_mode(void) {
	LPC_SPIFI->STAT = 0x10;	// reset memory mode
	while(LPC_SPIFI->STAT & 0x10);	// wait for reset to complete

	LPC_SPIFI->ADDR = 0xffffffff;
	LPC_SPIFI->DATINTM = 0xffffffff;
	LPC_SPIFI->CMD = 		// send all ones for a while to hopefully reset SPI Flash
		(0xfful << 24) | 	// opcode 0xff
		(0x5 << 21) | 		// frame form indicating opcode and 4 address bytes
		(0x0 << 19) | 		// field form indicating all serial fields
		(0x4 << 16); 			// 3 intermediate data bytes
	while(LPC_SPIFI->STAT & 2); // wait for command to complete
}

//#define QUAD_WRITE

void spifi_4K_write(int address, int * copy) {
	int aligned_address = address & ~(0xfff);
	int offset = (address & 0xfff)>>2;
	int i, j;

	LPC_SPIFI->DATINTM = 0x0; // next read command will remove high performance mode
	LPC_SPIFI->ADDR = aligned_address;

	LPC_SPIFI->CMD =
		(0x06ul << 24) | 	// opcode 0x06 Write Enable for Spansion
		(0x1 << 21) | 		// frame form indicating opcode only
		(0x0 << 19); 			// field form indicating all serial
	while(LPC_SPIFI->STAT & 2); // wait for command to complete
	LPC_SPIFI->CMD =
		(0x20 << 24) | 		// opcode 0x20 Sector Erase for Spansion
		(0x4 << 21) | 		// frame form indicating opcode and 3 address bytes
		(0x0 << 19); 			// field form indicating all serial
	while(LPC_SPIFI->STAT & 2); // wait for command to complete
	LPC_SPIFI->CMD =
		(0x05ul << 24) | 	// opcode 0x05 Read Status for Spansion
		(0x1 << 21) | 		// frame form indicating opcode only
		(0x0 << 19) | 		// field form indicating all serial
		(0x1 << 14) | 		// POLLRS polling command
		(0x0 << 2) | 			// check bit 0
		(0x0 << 0); 			// wait till 0
	while(LPC_SPIFI->STAT & 2); // wait for command to complete
	*(volatile char*)&LPC_SPIFI->DAT8;
	
	for(j = 0; j < 1024; j += 64) {
		LPC_SPIFI->CMD =
			(0x06ul << 24) | // opcode 0x06 Write Enable for Spansion
			(0x1 << 21) | 	// frame form indicating opcode only
			(0x0 << 19); 		// field form indicating all serial
		while(LPC_SPIFI->STAT & 2); // wait for command to complete
#ifdef QUAD_WRITE		
		LPC_SPIFI->CMD =
			(0x32ul << 24) | // opcode 0x32 Quad Page Programming for Spansion
			(0x4 << 21) | 	// frame form indicating opcode and 3 address bytes
			(0x1 << 19) | 	// field form indicating quad data field, others serial
			(0x0 << 16) | 	// 0 intermediate data bytes
			(0x1 << 15) | 	// dout indicates that it is a write
			(256); 					// datalen
#else	// SPI Serial WRITE
		LPC_SPIFI->CMD =
			(0x02ul << 24) | // opcode 0x02 Page Programming for Spansion
			(0x4 << 21) | 	// frame form indicating opcode and 3 address bytes
			(0x0 << 19) |		// field form indicating all serial
			(0x0 << 16) | 	// 0 intermediate data bytes
			(0x1 << 15) | 	// dout indicates that it is a write
			(256); 					// datalen
#endif		
		for(i = 0; i < 64; i++) {
			if (j+i >= offset) {
				*(volatile int*)&LPC_SPIFI->DAT32 = copy[j + i - offset];
			} else {
				*(volatile int*)&LPC_SPIFI->DAT32 = 0;
			}
		}
		while(LPC_SPIFI->STAT & 2); // wait for command to complete
		LPC_SPIFI->CMD =
			(0x05ul << 24) | 	// opcode 0x05 Read Status for Spansion
			(0x1 << 21) | 		// frame form indicating opcode only
			(0x0 << 19) | 		// field form indicating all serial
			(0x1 << 14) | 		// POLLRS polling command
			(0x0 << 2) | 			// check bit 0
			(0x0 << 0); 			// wait till 0
		while(LPC_SPIFI->STAT & 2); // wait for command to complete
		*(volatile char*)&LPC_SPIFI->DAT8;
		LPC_SPIFI->ADDR += 256;
	}
}


/*****************************************************************************
 * Private functions
 ****************************************************************************/
static uint32_t CalculateDivider(uint32_t baseClock, uint32_t target)
{
	uint32_t divider = (baseClock / target);

	/* If there is a remainder then increment the divider so that the resultant
	   clock is not over the target */
	if(baseClock % target) {
		++divider;
	}
	return divider;
}

unsigned int blinkRate=500;

#define SPIFI_SCRATCH_AREA	SPIFLASH_BASE_ADDRESS+0x8000
#define PSUEDO_RANDOM_RAM		0x20000000

int compareSerialFlash(void) {
	int i,j;
	for (i=0; i<1024; i++) {
		j = j+129;
		j = j & 1023;
		if (*(int *)(SPIFI_SCRATCH_AREA+j) != *(int *)(PSUEDO_RANDOM_RAM + j)) {
			DEBUGOUT("SPIFI mismatch at %x %x != %x\r\n",SPIFI_SCRATCH_AREA+j, *(unsigned int *)(SPIFI_SCRATCH_AREA+j),*(unsigned int *)(PSUEDO_RANDOM_RAM + j));

			blinkRate=100;
			while(1);
			return 0;
		}
	}
	return 1;
}

static void RunExample(void){
	DEBUGOUT("SPIFI clock rate %d\r\n", Chip_Clock_GetClockInputHz(CLKIN_IDIVE));
	
	DEBUGOUT("SPIFI CMD register %x\r\n", LPC_SPIFI->CTRL);
	spifi_init();
	DEBUGOUT("SPIFI CMD register %x\r\n", LPC_SPIFI->CTRL);
	
	// copy random data in RAM to SPIFI Flash 
	
	spifi_command_mode();
	spifi_4K_write(SPIFI_SCRATCH_AREA, (int *)PSUEDO_RANDOM_RAM);
	spifi_memory_mode();	

	DEBUGOUT("SPIFI CMD register %x\r\n", LPC_SPIFI->CTRL);
	while (1) {
		compareSerialFlash();
		__WFI();
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

void SysTick_Handler(void)
{
	static int x = 0;
	
	/* If no blink rate just leave the light on */
	if (blinkRate == 0) {
		Board_LED_Set(0, true);
		return;
	}
	
	if (x++ > blinkRate) {
		Board_LED_Toggle(0);
		x = 0;
	}
}

/**
 * @brief	Main entry point
 * @return	Nothing
 */
int main(void)
{
		/* Initialize the board & LEDs for error indication */
	SystemCoreClockUpdate();
	Board_Init();
	
	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ1);
	
	/* Run the example code */
	RunExample();
	
	return 0;
}






