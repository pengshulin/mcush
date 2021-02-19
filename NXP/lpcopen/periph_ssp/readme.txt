SSP example
This example show how to use the SSP in 3 modes : Polling, Interrupt and DMA

Example description
This example describes how to use SSP in POLLING, INTERRUPT or DMA mode.
It is needed to connect 2 hardware boards, one for Master and one for Slave.

    SSP configuration:
        - CPHA = 0: data is sampled on the first clock edge of SCK.
        - CPOL = 0: SCK is active high.
        - Sample rate = 400kHz.
        - DSS = 8: 8 bits per transfer.
        - FRF= 0: SPI Frame format.
    After initialize transmit buffer, SPI master/slave will transfer a number of bytes
    to SPI slave/master and receive data concurrently.
    After a transfer completed, receive and transmit buffer will be compared and
    the result will be print out via UART port.
    This example supports 3 transfer modes: POLLING mode, INTERRUPT mode and DMA mode.

- Connect UART port on the master board and slave board to COM ports on your PC.
- Configure terminal program on the PC per the above Serial display configuration
- Configure hardware, connect master board and slave board as below
- Build and run the example. Following guidance on terminals of master and slave to do test.

Special connection requirements
  - Hitex A4 LPC1850 and LPC4350 boards
    - PF_0 : X16 pin 43 - SCK0 master(slave) - connect to SCK slave(master) board 
    - PF_1 : X16 pin 46 - SEL0 master(slave) - connect to SEL slave(master) board
    - PF_2 : X16 pin 45 - MISO0 master(slave) - connect to MISO slave(master) board
    - PF_3 : X16 pin 44 - MOSI0 master(slave) - connect to MOSI slave(master) board
  - Keil MCB1857 and MCB4357 boards
    - PF_4 - SCK1  master(slave) - connect to SCK slave(master) board
    - PF_5 - SSEL1  master(slave) - connect to SEL slave(master) board
    - PF_6 - MISO1  master(slave) - connect to MISO slave(master) board
    - PF_7 - MOSI1  master(slave) - connect to MOSI slave(master) board
  - NGX Xplorer 1830 and 4330 boards
    - CLK0 : J8 pin 7 - SCK1 master(slave) - connect to SCK slave(master) board 
    - P1_2 : J8 pin 8 - SSEL1 master(slave) - connect to SEL slave(master) board
    - P1_3 : J8 pin 6 - MISO1  master(slave) - connect to MISO slave(master) board
    - P1_4 : J8 pin 5 - MOSI1  master(slave) - connect to MOSI slave(master) board
  - LPC Xpresso4337 and Xpresso1837 boards
    - CLK1  : J1 Pin-5
    - SSEL1 : J1 Pin-8
    - MISO  : J2 Pin-6 [Connect to MOSI of slave J2 Pin-7]
    - MOSI  : J2 Pin-7 [Connect to MISO of slave J2 Pin-6]

Notes : 
  - Common ground must be connected together between two boards.
  - The application default mode is Master mode. Press '1' on main menu to change Slave mode







