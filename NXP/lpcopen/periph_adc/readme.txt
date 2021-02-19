ADC example
This example show how to  the ADC in 3 mode : Polling, Interrupt and DMA

Example description
This example describes how to use ADC conversion POLLING mode,
INTERRUPT mode or DMA mode.

The ADC conversion rate is 400KHz. A full conversion requires 11
of these clocks. So ADC clock = 400KHz (number of clocks for 1
measure = 11 clocks). Note that maximum ADC clock input is 4.5MHz.

The ADC value can be read in POLLING mode, INTERRUPT mode or DMA
mode. Converted ADC values displayed periodically via the UART when
Timer 1 ticks. Turn potentiometer to change ADC signal input.

Setting up the demo requires connecting a UART cable between the
board and a host PC. The terminal program on the host PC should be
setup for 115K8N1. Press the appropriate key via the menu to change
the ADC conversion mode.

Special connection requirements
  - Hitex LPC1850EVA-A4-2 and LPC4350EVA-A4-2 boards (uses ADC0, Channel 0)
  - Keil MCB1857 and MCB4357 boards (uses ADC0, Channel 0)
  - LPCXpresso v3 4337 Board must have Network shield board connected (Trim
		  POT RV1 should be used)







