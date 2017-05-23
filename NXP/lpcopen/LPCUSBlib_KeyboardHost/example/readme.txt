USB Host CDC example

Example Description
This example shows how to use the USB host stack to implement USB CDC class with multiple serial interfaces. 
When a USB CDC device is connected to the host port, this example parses the configuration descriptor to find
out the number of serial interfaces supported by the device and configures the host stack for these interfaces.
It will then, periodically check for any received bulk data from each interface, the received data is modified 
and transmitted back to the device, thus implementing a ping pong transfer. 
The received data is printed to the UART console.

Connection requirements
A USB CDC device to be connected to the USB0 host port. To view debug messages showing the example output, 
open terminal program on PC, connect UART and configure for baud 115200, 8-bits and 1 stopbit
