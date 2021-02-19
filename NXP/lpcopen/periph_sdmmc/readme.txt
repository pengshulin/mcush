SD/MMC example

Example description
This example describes how to use the SD/MMC interface with a SD card
and a FATFS filesystem.

If the SD-CARD has file "message.txt" at the root folder the content will be
read and displayed on to the terminal. The example writes a file named "hello.txt"
with a simple text content on to the card, then it lists all the files inside the
root folder of the card.

To use the example, plug a SD card (Hitex A4 board) or microSD card (NGX or Keil
boards) and connect a serial cable to the board's RS232/UART port start a terminal
program to monitor the port.  The terminal program on the host PC should be setup
for 115K8N1. 

Special connection requirements
  - Hitex LPC1850EVA-A4-2 and LPC4350EVA-A4-2 boards
     - Close pin 1-2 of JP9
     - Open all SV3, SV6
     - Close all SV12
     - Uart1 terminal port: SV11-p2(U1_RXD) connect to SV1-p7, SV11-p4 (U1_TXD) connect to SV1-p5
  - Keil MCB1857 and MCB4357 boards (debug output terminal on Uart3)
  - NGX LPC1830 LPC4330 (no debug output terminal)
  - NXP LPCXpresso 1837/4337 boards (debug output terminal on Uart0 FTDI UART connector)







