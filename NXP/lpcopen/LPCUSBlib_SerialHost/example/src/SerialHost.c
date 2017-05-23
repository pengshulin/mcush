/*
 * @brief Serial Host example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * Copyright(C) Dean Camera, 2011, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "SerialHost.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/** LPCUSBlib Communication Device Class driver interface configuration and state information. This structure is
 *  passed to each CDC driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */

/* Max CDC interfaces supported depends on the total number of pipes the host stack supports */
#define MAX_SERIAL_INTERFACES		((PIPE_TOTAL_PIPES - 1)/3)

static USB_ClassInfo_CDC_Host_t Serial_Interface[MAX_SERIAL_INTERFACES] = {
	{
		.Config = {
			.NotificationPipeNumber			= 1,
			.NotificationPipeDoubleBank	= false,
			
			.DataINPipeNumber       		= 2,
			.DataINPipeDoubleBank   		= false,

			.DataOUTPipeNumber      		= 3,
			.DataOUTPipeDoubleBank  		= false,
			.PortNumber = 0,
		},
	},
	{
		.Config = {
			.NotificationPipeNumber			= 4,
			.NotificationPipeDoubleBank	= false,
			
			.DataINPipeNumber       		= 5,
			.DataINPipeDoubleBank   		= false,

			.DataOUTPipeNumber      		= 6,
			.DataOUTPipeDoubleBank  		= false,
			.PortNumber = 0,
		},
	},
};

/* Buffer for serial data */
static char buffer[MAX_SERIAL_INTERFACES][PIPE_MAX_SIZE] = { "SERIAL_ONE_MSG_0", "SERIAL_TWO_MSG_0" };
/* Total number of CDC  interfaces seen in the device configuration descriptor */
static uint8_t serialInfCount;

/* CDC application specific task */
static void SerialHost_Task(void)
{
	uint16_t dataLen;
	uint16_t i, j;

	if (USB_HostState[Serial_Interface[0].Config.PortNumber] != HOST_STATE_Configured) {
		return;
	}
	/* For each CDC interface check if any data is received, if so read data, modify data and send it back to device. */
	for (i = 0; i < serialInfCount; i++) {
		if((dataLen = CDC_Host_BytesReceived(&Serial_Interface[i])) > 0) {
			for(j = 0; j < dataLen; j++) {
				buffer[i][j] = CDC_Host_ReceiveByte(&Serial_Interface[i]);
			}
			DEBUGOUT("Received data from interface %d: %s", i, buffer[i]);
			/* Modify data and send it back to device */
			buffer[i][15]++;
			if(buffer[i][15] >= 0x3A) {
				buffer[i][15] = 0x30;
			}
			CDC_Host_SendData(&Serial_Interface[i], (const uint8_t *)&buffer[i][0], 19);
		}
	}
}

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/** Configures the board hardware and chip peripherals for the demo's functionality. */
static void SetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();
#if (defined(CHIP_LPC43XX) || defined(CHIP_LPC18XX))
	if (Serial_Interface[0].Config.PortNumber == 0) {
		Chip_USB0_Init();
	} else {
		Chip_USB1_Init();
#if (defined(BOARD_NXP_LPCXPRESSO_4337))
		Board_USB1_EnableVbus();
#endif
	}
#endif
	USB_Init(Serial_Interface[0].Config.PortNumber, USB_MODE_Host);
	/* Hardware Initialization */
	Board_Debug_Init();
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/

/** Main program entry point. This routine configures the hardware required by the application, then
		keeps calling the CDC application task for bulk data, CDC Host task for processing interrupt endpoints
		and the core USB host task.
 */
int main(void)
{
	uint8_t i;
	
	SetupHardware();
	
	serialInfCount = 0;

	DEBUGOUT("Serial Host Demo running.\r\n");

	for (;; ) {
		SerialHost_Task();

		for (i = 0; i < serialInfCount; i++) {
			CDC_Host_USBTask(&Serial_Interface[i]);
		}
		USB_USBTask(Serial_Interface[0].Config.PortNumber, USB_MODE_Host);
	}
}

/** Event handler for the USB_DeviceAttached event. This indicates that a device has been attached to the host, and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Host_DeviceAttached(const uint8_t corenum)
{
	DEBUGOUT(("Device Attached on port %d\r\n"), corenum);
}

/** Event handler for the USB_DeviceUnattached event. This indicates that a device has been removed from the host, and
 *  stops the library USB task management process.
 */
void EVENT_USB_Host_DeviceUnattached(const uint8_t corenum)
{
	DEBUGOUT(("\r\nDevice Unattached on port %d\r\n"), corenum);
}

/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
void EVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum)
{
	uint16_t ConfigDescriptorSize;
	uint8_t  ConfigDescriptorData[512];
	uint8_t *pCfgDesc;
	uint8_t i;
	uint8_t	err_code;

	/* Read Configuration descriptor */
	if (USB_Host_GetDeviceConfigDescriptor(corenum, 1, &ConfigDescriptorSize, ConfigDescriptorData,
										   sizeof(ConfigDescriptorData)) != HOST_GETCONFIG_Successful) {
		DEBUGOUT("Error Retrieving Configuration Descriptor.\r\n");
		return;
	}
	
	serialInfCount = 0;
	pCfgDesc = ConfigDescriptorData;
	/* Parse configuration descriptor to find CDC interfaces, note that the pointer to the configuration data is changed each time the function is called */
	while((err_code = CDC_Host_FindConfigInterface(&Serial_Interface[serialInfCount], &ConfigDescriptorSize, (void **)&pCfgDesc)) == CDC_ENUMERROR_NoError) {
		serialInfCount++;
		if(serialInfCount >= MAX_SERIAL_INTERFACES) {
			DEBUGOUT("Reached Max Serial Interface of %d\r\n", MAX_SERIAL_INTERFACES);
			break;
		}
	}
	
	if(err_code == CDC_ENUMERROR_PipeConfigurationFailed) {
		DEBUGOUT("Error in Configuring Pipe.\r\n");
		return;
	}
	/* No CDC interface was found */
	if(serialInfCount == 0) {
		DEBUGOUT("Attached Device Not a Valid CDC Device.\r\n");
		return;
	}

	if (USB_Host_SetDeviceConfiguration(Serial_Interface[0].Config.PortNumber, 1) != HOST_SENDCONTROL_Successful) {
		DEBUGOUT("Error Setting Device Configuration.\r\n");
		return;
	}
	
	/* Set line coding for each interface */
	for (i = 0; i < serialInfCount; i++) {
		Serial_Interface[i].State.LineEncoding.BaudRateBPS = 115200;
		Serial_Interface[i].State.LineEncoding.CharFormat = 0;
		Serial_Interface[i].State.LineEncoding.ParityType = 0;
		Serial_Interface[i].State.LineEncoding.DataBits = 8;
		CDC_Host_SetLineEncoding(&Serial_Interface[i]);
	}

	DEBUGOUT("Serial Device Enumerated.\r\n");
}

void EVENT_CDC_Host_ControLineStateChanged(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo)
{
	/* Called when data is received on interrupt endpoint, the data is present in CDCInterfaceInfo->State.ControlLineStates.DeviceToHost*/
}

/** Event handler for the USB_HostError event. This indicates that a hardware error occurred while in host mode. */
void EVENT_USB_Host_HostError(const uint8_t corenum, const uint8_t ErrorCode)
{
	USB_Disable(corenum, USB_MODE_Host);

	DEBUGOUT(("Host Mode Error\r\n"
			  " -- Error port %d\r\n"
			  " -- Error Code %d\r\n" ), corenum, ErrorCode);

	for (;; ) {}
}

/** Event handler for the USB_DeviceEnumerationFailed event. This indicates that a problem occurred while
 *  enumerating an attached USB device.
 */
void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t corenum,
											const uint8_t ErrorCode,
											const uint8_t SubErrorCode)
{
	DEBUGOUT(("Dev Enum Error\r\n"
			  " -- Error port %d\r\n"
			  " -- Error Code %d\r\n"
			  " -- Sub Error Code %d\r\n"
			  " -- In State %d\r\n" ),
			 corenum, ErrorCode, SubErrorCode, USB_HostState[corenum]);

}
