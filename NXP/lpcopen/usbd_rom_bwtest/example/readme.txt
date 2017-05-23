USB Band Width test and WCID examples for USB ROM API.

Example description
The example shows how to us USBD ROM stack to creates a WCID based bandwidth test port.
The examples shows how to handle Microsoft's specific component identifier (WCID) requests
for install less WinUSB operation. Check https://github.com/pbatard/libwdi/wiki/WCID-Devices 
for more details.
No driver install is required on Windows 8 and Windows 7 systems which can update 
automatically connecting "Windows update server". For WinXP and Windows 7 machine which 
can't update automatically use latest version of Zadig tool available at 
http://sourceforge.net/projects/libwdi/files/zadig/ to install drivers. When Zadig
is used restart of the machine is required for the test tool to recognize the device.
 
Special connection requirements
Connect the USB cable between micro connector on board and to a Windows host.
Run LPCUsbTest.exe on the host. The executable uses .net 3.5 framework. 
Once the application is running click "Find LPC Device" button. Check if the
list box shows whether the app detected the device. Then select "Rx" or "Tx" radio
buttons before clicking "Start Test" button. Rx and Tx numbers are based on host receiving
and transmitting data. Since this example is just interested in through-put test it use
same memory area for Rx and Tx. 

