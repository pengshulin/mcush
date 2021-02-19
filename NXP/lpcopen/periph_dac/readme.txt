DAC example
This example show how to use the D/A Conversion in 3 modes: Polling, Interrupt and DMA

Example description
This example shows how to use DAC peripheral with 3 modes: POLLING mode,
INTERRUPT mode or DMA mode.

DAC will be initialized with maximum current is 700uA. This allows a
maximum update rate of 1Mhz DAC updated values have range from 0 to 0x3FF.
So AOUT output voltage will change from: Vss to VDD. This example
configures pin P4_4 as analog function for DAC output through ENAIO2
register. Run and observe AOUT signal by oscilloscope.

Use UART to monitor this demo.

Special connection requirements
  - Hitex LPC1850EVA-A4-2 and LPC4350EVA-A4-2 boards (AOUT: X16.4 (X16 connector - pin LCDVD1))
  - LPC Xpresso4337 board AD0 [Connector P2 Pin-15] must be connected to oscilloscope
	 






