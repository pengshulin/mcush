I2S example
This example show how to use the I2S in 3 modes : Polling, Interrupt and DMA

Example description
The I2S example shows how to configure I2S and UDA1380 to receive audio signal and
play back with three modes: polling, interrupt, and DMA.

To use the example, connect a serial cable to the board's RS232/UART port and
start a terminal program to monitor the port.  The terminal program on the host
PC should be setup for 115K8N1.
For boards that has no default UART (ex: NGX Xplorer), by default example will
start in DMA mode hence no need to slect anything from terminal.
Connect the computer line out to board line-in (using 3.5mm male-to-male cable),
plug the headphone/speaker into board line-out. Change modes by selecting the
option from the terminal. Play audio on computer and listen from target board!

Special connection requirements
There are no special connection requirements for this example.







