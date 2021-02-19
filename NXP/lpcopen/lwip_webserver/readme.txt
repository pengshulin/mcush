LWIP standalone http server example

Example description
This example uses the raw API for standalone (without an RTOS) operation to
implement a simple HTTP server. This example shows how to use the raw API with
the LWIP contrib HTTP example (modified) using the 18xx/43xx LWIP MAC and PHY drivers.
The example shows how to handle PHY link monitoring and indicate to LWIP that
a ethernet cable is plugged in.

To use the example, Simply connect an ethernet cable to the board. The board
will acquire an IP address via DHCP and will print it in UART (115.2K baud).
In a webbrowser enter http://{ip addr printed on UART} to see the default
webpage. Copy the html files (including index.htm) to an SDCARD and insert
the card before power-on/reset. The webserver will read the files based on
request from the browser

Special connection requirements
There are no special connection requirements

@warning
HITEX boards does not support reading files from SDCARD due to the fact that
board design makes Ethernet/UART/SDCARD functionality, mutually exclusive.







