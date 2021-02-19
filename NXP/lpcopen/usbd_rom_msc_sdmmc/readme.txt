Mass storage example using SDMMC for USBD ROM stack

Example description
The example shows how to use USBD ROM stack to create a USB MSC example
that uses SD/MMC.
 
Special connection requirements
Connect the USB cable between micro connector on board and to a host.
The example exposes the SD card as a USB memory storage device. 
For Hitex LPC1850EVA-A4-2 and LPC4350EVA-A4-2 boards
     - Close pin 1-2 of JP9
     - Open all SV3, SV6
     - Close all SV12
     - Uart1 terminal port: SV11-p2(U1_RXD) connect to SV1-p7, SV11-p4 (U1_TXD) connect to SV1-p5







