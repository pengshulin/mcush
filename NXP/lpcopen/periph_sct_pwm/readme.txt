State Configurable Timer (SCT) PWM example

Example description
This example, uses the SCT PWM driver to create 2 independent PWMs (running at
the same frequency). The PWM1 is used to output a square wave to an Output pin
and PWM2 is used to control the brightness of the LED, the brightness goes
from 0% to 100% in steps of 0.5% every 20 Milliseconds and goes down to 0% in
the same step. Connect oscilloscope to the PWM OUT pin and the waveform
displayed on the scope will have its duty cycle increasing from 0% to 100% in
steps of 10% every second

Special connection requirements
KEIL MCB 1857/4357 BOARD:
PWM OUT ON PD_12 [Also connected to LED3]

HITEX EVA 1850/4350 BOARD:
PWM OUT ON X19 PIN-9 [Motor Connector]

NGX XPLORER 1830/4330 BOARD:
PWM OUT ON J10 PIN-20 [27-pin single row header]

NXP LPCXpresso 1837/4337
PWM OUT ON J2 PIN-5 [INT PWM]

NXP LPC Link-II 4370 BOARD:
PWM OUT ON J9 PIN-24 [24 pin Digital Channels Header]







