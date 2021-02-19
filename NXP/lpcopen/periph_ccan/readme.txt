CCAN example
This example show how to  the CCAN in 3 mode : Polling, Interrupt and DMA

Example description
This example shows how to use CCAN to send and receive simple CAN frames.
It requires a CAN analyser or, another CAN device connected to CAN bus.
Upon reset, the MCU will transmit a simple CAN frame with ID 0x200 on CAN bus.
It is also configured to reply automatically coming remote messages
with ID of 0x300. 
Besides, whenever the MCU receives a CAN frame whose ID is 0x100, the message content
will be printed out serial terminal and a reply message is sent back with
ID = ID of received message+1.
The baudrate is set to 500kBit/s.

Special connection requirements
	- Hitex LPC1850EVA-A4-2 and LPC4350EVA-A4-2 boards (using UART0)
		SV1: 1-2, 3-4 close
		SV9: 5-6, 7-8 open
		SV10: 5-6, 9-10 open
		Pin 2 of X7 is connected to CAN-L (Pin 2) of CAN Analyser.
		Pin 7 of X7 is connected to CAN-H (Pin 7) of CAN Analyser.
	- Keil MCB1857 and MCB4357 boards (using UART3)
		Pin 2 of CAN port (P12) is connected to CAN-L (Pin 2) of CAN Analyser.
		Pin 7 of CAN port (P12) is connected to CAN-H (Pin 7) of CAN Analyser.
	- LPCXpresso 1837/4337 boards (using connector P2)
		Pin 52 of connector P2 (P3.1) is CAN_RD
		Pin 53 of connector P2 (P3.2) is CAN_TD







