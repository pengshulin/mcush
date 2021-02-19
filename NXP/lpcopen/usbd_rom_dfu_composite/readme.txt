Composite device examples using USBD ROM API.

Example description
The example shows how to use USBD ROM stack to create a composite USB device
having multiple interfaces of DFU, MSC, CDC-ACM and HID classes.
The example also shows how to use ROM based DFU class in implementing in-field 
firmware update. The DFU part of the example shows how to download an image to
to IRAM and execute it. User can use the output of periph_iram_blinky project
to test DFU download feature of the example. Use the DFU-Util.exe available in LPCExpresso.

Special connection requirements
Connect the USB cable between micro connector on board (target port) and to a host.
When connected to Windows host use the .inf included in the project
directory to install the driver. For OSx (Mac) host no drivers is needed. 







