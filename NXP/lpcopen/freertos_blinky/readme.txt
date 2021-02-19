FreeRTOS Blinky example

Example description
Welcome to the FreeRTOS basic blinky example. This example starts up
FreeRTOS and creates 3 tasks. Tasks 1 and 2 blink different LEDs at
different rates. Task 3 outputs a tick count to the debug channel (UART)
every second.

By default, tickless idle is used with this example for FreeRTOS. Normal
tick operation can be used by setting the configUSE_TICKLESS_IDLE definition
to 0 in FreeRTOSConfig.h.

To use the example, connect a serial cable to the board's RS232/UART port
and start a terminal program (115.2K8N1) to monitor the port. The LEDs will
also toggle based on the task execution.

Special connection requirements
There are no special connection requirements for this example.







