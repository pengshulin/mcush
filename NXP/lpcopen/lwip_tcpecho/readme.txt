LWIP TCP Echo without an RTOS

Example Description

Welcome to the LWIP TCP Echo example using the 18xx/43xx LWIP MAC and 
PHY drivers.  The example demonstrates PHY link monitoring, DHCP client, ICMP
Echo (ping) and TCP Echo (RFC 862) operation.

To use the example:

1 - Connect the UART port on the board to a host PC.  Open a terminal program 
    (putty, TeraTerm, etc.), line parameters are 115,200 bps, 8 bit data, no 
    parity, one stop bit.  Link monitoring messages and DHCP client messages 
    are sent out this UART.

2 - Connect the Ethernet port of the board to a LAN. The example will detect
    the link then obtain an IP address from a DHCP server.  Messages similar
    to the following will be sent out the UART.

	Link connect status: 0
	Link connect status: 1
	IP_ADDR    : 10.1.10.34
	NET_MASK   : 255.255.255.0
	GATEWAY_IP : 10.1.10.1

3 - At this point, the example will respond to ICMP Echo (ping) requests.
    E.g. from a Windows Command Prompt:

	C:\>ping 10.1.10.34

	Pinging 10.1.10.34 with 32 bytes of data:
	Reply from 10.1.10.34: bytes=32 time<1ms TTL=255
	Reply from 10.1.10.34: bytes=32 time<1ms TTL=255
	Reply from 10.1.10.34: bytes=32 time<1ms TTL=255
	Reply from 10.1.10.34: bytes=32 time<1ms TTL=255

	Ping statistics for 10.1.10.34:
	    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss),
	Approximate round trip times in milli-seconds:
	    Minimum = 0ms, Maximum = 0ms, Average = 0ms


4 - The example will also respond to TCP Echo (RFC 862) requests.

	- Use a telnet client (such as putty).
	- Configure it to use telnet, port 7 and the IP address obtained via
	  DHCP.
	- Click Open and the port will be opened, now any key pressed on the
	  telnet window will be received by the board and be echoed back.  
	- NOTE: Some terminal programs (like telnet in Linux) do not send a 
	  packet until the user presses Enter.
	- Note also by default the putty program will perform a local echo
	  before sending the character to the board, resulting in characters
	  being displayed twice to the terminal window.

Special connection requirements
There are no special connection requirements for this example.







