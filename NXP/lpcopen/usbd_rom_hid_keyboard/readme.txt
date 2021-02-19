This file contains USB HID Keyboard example using USB ROM Drivers.

Example description
The example shows how to us USBD ROM stack to creates a HID keyboard. 

Special connection requirements
The tiny joystick that is surface mounted on the eval board has 4 positions
and a button press. They act as number keys '2', '4', '6', '8', and NUMLOCK.
The joystick press toggles NUMLOCK. When NUMLOCK is ON the joystick positions
are mapped to the arrow keys. For most OSs no drivers are needed. If no
joystick is available push button can be used to simulate '+' key







