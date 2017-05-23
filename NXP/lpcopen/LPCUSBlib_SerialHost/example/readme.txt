USB Mass Storage Host example

Example Description
This example shows how to use the USB host stack to implement USB mass storage
class. This example reads the USB mass storage device connected to USB host
port 0. It will read the contents of file MESSAGE.TXT (if present in the
attached device's root directory and prints its content to the UART console).
It writes a file named HELLO.TXT with the content "Hello World!" and it lists
the content of the root directory.

Connection requirements
A USB Mass storage device to be connected to the USB0 host port. Open terminal
program, connect UART and configure for baud 115200, 8-bits and 1 stopbit
