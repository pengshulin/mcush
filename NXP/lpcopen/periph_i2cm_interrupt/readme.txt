I2CM bus master example using interrupt mode

Example description
This example uses I2CM to write and read an I2C slave device using the
interrupt mode of non-blocking operation.

If this example is being executed on a Keil 4357 or 1857, the I2C device accessed 
by this example is the STMPE811 part at address 0x82 (0x41 unshifted).  The example 
flashes an LED to signify continued operation.  The example writes an alternating 
0/1 pattern to register zero then reads the chip ID (0x811) from register zero. If 
the example is running correctly, the LED should be flashing and if there is an 
I2C analyser connected to the SLC and SDA signals, you will see writes to register zero
followed by reads from register zero and the two read bytes should contain the device's
ID value of 0x08 and 0x11.

If this example is being executed on a Hitex 4350 or 1850, the I2C device accessed by 
this example is the PCA9673PW part at address 0x48 (0x24 unshifted).  The example flashes 
an LED to signify continued operation.  The example writes an alternating 0x0000/0xFFFF 
pattern to the sixteen I/O ports then reads the sixteen I/O ports. If the example is 
working correctly, the EXT LEDs (D3 - D6) should be flashing and if there is an I2C  
analyser connected to the SLC and SDA signals, you will see writes followed by reads of 
the sixteen I/O ports.

If this example is being executed on a NGX 4330 or 1830, the I2C device accessed by 
this example is the UDA1380 part at address 0x34 (0x1A unshifted).  The example flashes 
an LED to signify continued operation.  The example also writes an alternating  
0x0000/0xA5DF pattern to the part's audio power control settings register at internal 
address 2. When the value 0x0000 is written, all audio power is cut.  When the value 
0xA5DF is written, all audio power is at its maximum setting. If the example is working 
correctly, an LED should be flashing and if there is an I2C analyzer connected to the 
SLC and SDA signals, you will see writes followed by reads of the audio power control
settings register.

If this example is being executed on a LPC4370, there is no on-board I2C device available
so this example will only NAK since no I2C device responds.  The example flashes an LED 
to signify continued operation.

Executing this example on LPC Xpresso4337 or Xpresso1837 will write to config register
[Reg 00] of power moinitor chip address 0x80 (0x40 unshifted), and will read the current
voltage across the shunt resistor and prints it on to UART (if connected).

Special connection requirements
There are no special connection requirements for this example.

to get started building LPCOpen projects.





