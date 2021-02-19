Blinky Example using Dual Core communication

Example description
This example blinks LED1 and LED2 at various rates, one of the LED
is turned on/off by M4 based on message received from M0. Meanwhile
M4 also sends blink message to M0 so that M0 core blinks the other LED.
LED1 will blink at the rate of 500ms/blink, LED2 will blink at the rate of
1s/blink.

This example is included by default in all other dual-core examples,
hence all the dual-core examples will blink LED1 and LED2.

*IMPORTANT NOTE:**
 
 M0 image boot failure will also cause (by default) LED2 to blink. This
 should not be confused with the Blinky task blinking the LED. The error
 blink will be at 20ms or 2s rate, the former rate indicates that the M0
 image loader code running on core-M4 could not locate a valid M0 image,
 whereas the latter rate indicates that a valid M0 image is found but its
 functionality is mutually exclusive with the functionalities of the image
 running in core-M4. In case of M0 boot error the blink will stop after 1
 minute.

Special connection requirements
   - Hitex LPC4350EVA-A4-2
      + LED1 is D10 (located below micro USB connector X9,*needs jumper JP3 closed**)
      + LED2 is RED of RGB LED
   - KEIL MCB4357
      + LED1 is PD_10 (located above push button P4_0)
      + LED2 is PD_11 (located next to LED1)
   - NGX Explorer4330
      + LED1 is D2 (Dull Green LED)
      + LED2 is D3 (Bright Blue LED)







