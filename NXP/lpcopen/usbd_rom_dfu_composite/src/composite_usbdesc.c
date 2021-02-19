/*
 * @brief This file contains USB composite class example using USBD ROM stack.
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
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

#include <string.h>
#include "app_usbd_cfg.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/**
 * HID Mouse Report Descriptor
 */
const uint8_t Mouse_ReportDescriptor[] = {
	HID_UsagePage(HID_USAGE_PAGE_GENERIC),
	HID_Usage(HID_USAGE_GENERIC_MOUSE),
	HID_Collection(HID_Application),
	HID_Usage(HID_USAGE_GENERIC_POINTER),
	HID_Collection(HID_Physical),
	HID_UsagePage(HID_USAGE_PAGE_BUTTON),
	HID_UsageMin(1),
	HID_UsageMax(3),
	HID_LogicalMin(0),
	HID_LogicalMax(1),
	HID_ReportCount(3),
	HID_ReportSize(1),
	HID_Input(HID_Data | HID_Variable | HID_Absolute),
	HID_ReportCount(1),
	HID_ReportSize(5),
	HID_Input(HID_Constant),
	HID_UsagePage(HID_USAGE_PAGE_GENERIC),
	HID_Usage(HID_USAGE_GENERIC_X),
	HID_Usage(HID_USAGE_GENERIC_Y),
	HID_LogicalMin( (uint8_t) -127),
	HID_LogicalMax(127),
	HID_ReportSize(8),
	HID_ReportCount(2),
	HID_Input(HID_Data | HID_Variable | HID_Relative),
	HID_EndCollection,
	HID_EndCollection,
};
const uint16_t Mouse_ReportDescSize = sizeof(Mouse_ReportDescriptor);

/**
 * USB Standard Device Descriptor. In this example we will overwrite
 * this descriptor in DFU_AppDetach state so this array has to be in
 * RW area hence no const keyword.
 */
ALIGNED(4) uint8_t USB_DeviceDescriptor[] = {
	USB_DEVICE_DESC_SIZE,				/* bLength */
	USB_DEVICE_DESCRIPTOR_TYPE,			/* bDescriptorType */
	WBVAL(0x0200),						/* bcdUSB: 2.00 */
	0x00,								/* bDeviceClass */
	0x00,								/* bDeviceSubClass */
	0x00,								/* bDeviceProtocol */
	USB_MAX_PACKET0,					/* bMaxPacketSize0 */
	WBVAL(0x1FC9),						/* idVendor */
	WBVAL(0x0089),						/* idProduct */
	WBVAL(0x0100),						/* bcdDevice: 1.00 */
	0x01,								/* iManufacturer */
	0x02,								/* iProduct */
	0x03,								/* iSerialNumber */
	0x01								/* bNumConfigurations */
};

/** USB Device Qualifier Descriptor */
ALIGNED(4) const uint8_t USB_DeviceQualifier[] = {
	USB_DEVICE_QUALI_SIZE,				/* bLength */
	USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE,	/* bDescriptorType */
	WBVAL(0x0200),						/* bcdUSB: 2.00 */
	0x00,								/* bDeviceClass */
	0x00,								/* bDeviceSubClass */
	0x00,								/* bDeviceProtocol */
	USB_MAX_PACKET0,					/* bMaxPacketSize0 */
	0x01,								/* bNumOtherSpeedConfigurations */
	0x00								/* bReserved */
};

/**
 * USB FSConfiguration Descriptor
 * All Descriptors (Configuration, Interface, Endpoint, Class, Vendor)
 */
ALIGNED(4) uint8_t USB_FsConfigDescriptor[] = {
	/* Configuration 1 */
	USB_CONFIGURATION_DESC_SIZE,		/* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	WBVAL(								/* wTotalLength */
		1 * USB_CONFIGURATION_DESC_SIZE     +
		HID_DESC_SIZE                       +
		1 * USB_ENDPOINT_DESC_SIZE          +
		1 * USB_INTERFACE_ASSOC_DESC_SIZE   +	/* interface association */
		1 * USB_INTERFACE_DESC_SIZE         +	/* communication interface */
		0x0013                              +	/* CDC functions */
		1 * USB_ENDPOINT_DESC_SIZE          +	/* interrupt endpoint */
		1 * USB_INTERFACE_DESC_SIZE         +	/* data interface */
		2 * USB_ENDPOINT_DESC_SIZE          +	/* bulk endpoints */
		1 * USB_INTERFACE_DESC_SIZE         +
		2 * USB_ENDPOINT_DESC_SIZE          +
		1 * USB_INTERFACE_DESC_SIZE         +
		1 * USB_INTERFACE_DESC_SIZE         +
		DFU_FUNC_DESC_SIZE                  +
		0
		),
	0x05,								/* bNumInterfaces */
	0x01,								/* bConfigurationValue */
	0x00,								/* iConfiguration */
	USB_CONFIG_SELF_POWERED,			/* bmAttributes */
	USB_CONFIG_POWER_MA(100),			/* bMaxPower */

	/* Interface 4, Alternate Setting 0, DFU Class */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_DFUAPP_IF_NUM,					/* bInterfaceNumber */
	0x00,								/* bAlternateSetting */
	0x00,								/* bNumEndpoints */
	USB_DEVICE_CLASS_APP,				/* bInterfaceClass */
	USB_DFU_SUBCLASS,					/* bInterfaceSubClass */
	0x01,								/* bInterfaceProtocol */
	0x04,								/* iInterface */
	/* DFU RunTime/DFU Mode Functional Descriptor */
	DFU_FUNC_DESC_SIZE,					/* bLength */
	USB_DFU_DESCRIPTOR_TYPE,			/* bDescriptorType */
	USB_DFU_CAN_DOWNLOAD | USB_DFU_CAN_UPLOAD | USB_DFU_MANIFEST_TOL,	/* bmAttributes */
	WBVAL(0x1000),						/* wDetachTimeout */
	WBVAL(USB_DFU_XFER_SIZE),			/* wTransferSize */
	WBVAL(0x100),						/* bcdDFUVersion */

	/* IAD to associate the two CDC interfaces */
	USB_INTERFACE_ASSOC_DESC_SIZE,		/* bLength */
	USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB_CDC_CIF_NUM,					/* bFirstInterface */
	2,									/* bInterfaceCount */
	CDC_COMMUNICATION_INTERFACE_CLASS,	/* bFunctionClass */
	CDC_ABSTRACT_CONTROL_MODEL,			/* bFunctionSubClass */
	0,									/* bFunctionProtocol */
	0x05,								/* iFunction (Index of string descriptor describing this function) */

	/* Interface 1, Alternate Setting 0, Communication class interface descriptor */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_CDC_CIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: Alternate setting */
	0x01,								/* bNumEndpoints: One endpoint used */
	CDC_COMMUNICATION_INTERFACE_CLASS,	/* bInterfaceClass: Communication Interface Class */
	CDC_ABSTRACT_CONTROL_MODEL,			/* bInterfaceSubClass: Abstract Control Model */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x05,								/* iInterface: */

	/*Header Functional Descriptor*/
	0x05,								/* bLength: Endpoint Descriptor size */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_HEADER,							/* bDescriptorSubtype: Header Func Desc */
	WBVAL(CDC_V1_10),	/* 1.10 */       /* bcdCDC */

	/*Call Management Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_CALL_MANAGEMENT,				/* bDescriptorSubtype: Call Management Func Desc */
	0x01,								/* bmCapabilities: device handles call management */
	USB_CDC_DIF_NUM,					/* bDataInterface: CDC data IF ID */

	/*Abstract Control Management Functional Descriptor*/
	0x04,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_ABSTRACT_CONTROL_MANAGEMENT,	/* bDescriptorSubtype: Abstract Control Management desc */
	0x02,								/* bmCapabilities: SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported */

	/*Union Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_UNION,							/* bDescriptorSubtype: Union func desc */
	USB_CDC_CIF_NUM,					/* bMasterInterface: Communication class interface is master */
	USB_CDC_DIF_NUM,					/* bSlaveInterface0: Data class interface is slave 0 */

	/*Endpoint 1 Descriptor*/          /* event notification (optional) */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_CDC_CIF_IN_EP,					/* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
	WBVAL(0x0010),						/* wMaxPacketSize */
	0x02,			/* 2ms */           /* bInterval */

	/* Interface 2, Alternate Setting 0, Data class interface descriptor*/
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_CDC_DIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: no alternate setting */
	0x02,								/* bNumEndpoints: two endpoints used */
	CDC_DATA_INTERFACE_CLASS,			/* bInterfaceClass: Data Interface Class */
	0x00,								/* bInterfaceSubClass: no subclass available */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x05,								/* iInterface: */

	/* Endpoint, CDC Bulk Out */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_CDC_OUT_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* Endpoint, CDC Bulk In */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_CDC_IN_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* Interface 3 Alternate Setting 0, MSC Class */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_MSC_IF_NUM,						/* bInterfaceNumber */
	0x00,								/* bAlternateSetting */
	0x02,								/* bNumEndpoints */
	USB_DEVICE_CLASS_STORAGE,			/* bInterfaceClass */
	MSC_SUBCLASS_SCSI,					/* bInterfaceSubClass */
	MSC_PROTOCOL_BULK_ONLY,				/* bInterfaceProtocol */
	0x06,								/* iInterface */
	/* Bulk In Endpoint */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	MSC_EP_IN,							/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0,									/* bInterval */
	/* Bulk Out Endpoint */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	MSC_EP_OUT,							/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0,									/* bInterval */

	/* Interface 0, Alternate Setting 0, HID Class */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_HID_IF_NUM,						/* bInterfaceNumber */
	0x00,								/* bAlternateSetting */
	0x01,								/* bNumEndpoints */
	USB_DEVICE_CLASS_HUMAN_INTERFACE,	/* bInterfaceClass */
	HID_SUBCLASS_BOOT,					/* bInterfaceSubClass */
	HID_PROTOCOL_MOUSE,					/* bInterfaceProtocol */
	0x07,								/* iInterface */
	/* HID Class Descriptor */
	/* HID_DESC_OFFSET = 0x0012 */
	HID_DESC_SIZE,						/* bLength */
	HID_HID_DESCRIPTOR_TYPE,			/* bDescriptorType */
	WBVAL(0x0111),						/* bcdHID : 1.11*/
	0x00,								/* bCountryCode */
	0x01,								/* bNumDescriptors */
	HID_REPORT_DESCRIPTOR_TYPE,			/* bDescriptorType */
	WBVAL(sizeof(Mouse_ReportDescriptor)),		/* wDescriptorLength */
	/* Endpoint, HID Interrupt In */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	HID_EP_IN,							/* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
	WBVAL(0x0008),						/* wMaxPacketSize */
	0x0A,								/* bInterval: 10ms */

	/* Terminator */
	0									/* bLength */
};

/**
 * USB HSConfiguration Descriptor
 * All Descriptors (Configuration, Interface, Endpoint, Class, Vendor)
 */
ALIGNED(4) uint8_t USB_HsConfigDescriptor[] = {
	/* Configuration 1 */
	USB_CONFIGURATION_DESC_SIZE,		/* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	WBVAL(								/* wTotalLength */
		1 * USB_CONFIGURATION_DESC_SIZE     +
		1 * USB_INTERFACE_DESC_SIZE         +
		DFU_FUNC_DESC_SIZE                  +
		1 * USB_INTERFACE_ASSOC_DESC_SIZE   +	/* interface association */
		1 * USB_INTERFACE_DESC_SIZE         +	/* communication interface */
		0x0013                              +	/* CDC functions */
		1 * USB_ENDPOINT_DESC_SIZE          +	/* interrupt endpoint */
		1 * USB_INTERFACE_DESC_SIZE         +	/* data interface */
		2 * USB_ENDPOINT_DESC_SIZE          +	/* bulk endpoints */
		1 * USB_INTERFACE_DESC_SIZE         +
		2 * USB_ENDPOINT_DESC_SIZE          +
		1 * USB_INTERFACE_DESC_SIZE         +
		HID_DESC_SIZE                       +
		1 * USB_ENDPOINT_DESC_SIZE          +
		0
		),
	0x05,								/* bNumInterfaces */
	0x01,								/* bConfigurationValue */
	0x00,								/* iConfiguration */
	USB_CONFIG_SELF_POWERED,			/* bmAttributes */
	USB_CONFIG_POWER_MA(100),			/* bMaxPower */

	/* Interface 4, Alternate Setting 0, DFU Class */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_DFUAPP_IF_NUM,					/* bInterfaceNumber */
	0x00,								/* bAlternateSetting */
	0x00,								/* bNumEndpoints */
	USB_DEVICE_CLASS_APP,				/* bInterfaceClass */
	USB_DFU_SUBCLASS,					/* bInterfaceSubClass */
	0x01,								/* bInterfaceProtocol */
	0x04,								/* iInterface */
	/* DFU RunTime/DFU Mode Functional Descriptor */
	DFU_FUNC_DESC_SIZE,					/* bLength */
	USB_DFU_DESCRIPTOR_TYPE,			/* bDescriptorType */
	USB_DFU_CAN_DOWNLOAD | USB_DFU_CAN_UPLOAD | USB_DFU_MANIFEST_TOL,	/* bmAttributes */
	WBVAL(0x1000),						/* wDetachTimeout */
	WBVAL(USB_DFU_XFER_SIZE),			/* wTransferSize */
	WBVAL(0x100),						/* bcdDFUVersion */

	/* IAD to associate the two CDC interfaces */
	USB_INTERFACE_ASSOC_DESC_SIZE,		/* bLength */
	USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB_CDC_CIF_NUM,					/* bFirstInterface */
	2,									/* bInterfaceCount */
	CDC_COMMUNICATION_INTERFACE_CLASS,	/* bFunctionClass */
	CDC_ABSTRACT_CONTROL_MODEL,			/* bFunctionSubClass */
	0,									/* bFunctionProtocol */
	0x05,								/* iFunction (Index of string descriptor describing this function) */

	/* Interface 1, Alternate Setting 0, Communication class interface descriptor */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_CDC_CIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: Alternate setting */
	0x01,								/* bNumEndpoints: One endpoint used */
	CDC_COMMUNICATION_INTERFACE_CLASS,	/* bInterfaceClass: Communication Interface Class */
	CDC_ABSTRACT_CONTROL_MODEL,			/* bInterfaceSubClass: Abstract Control Model */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x05,								/* iInterface: */

	/*Header Functional Descriptor*/
	0x05,								/* bLength: Endpoint Descriptor size */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_HEADER,							/* bDescriptorSubtype: Header Func Desc */
	WBVAL(CDC_V1_10),	/* 1.10 */       /* bcdCDC */

	/*Call Management Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_CALL_MANAGEMENT,				/* bDescriptorSubtype: Call Management Func Desc */
	0x01,								/* bmCapabilities: device handles call management */
	USB_CDC_DIF_NUM,					/* bDataInterface: CDC data IF ID */

	/*Abstract Control Management Functional Descriptor*/
	0x04,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_ABSTRACT_CONTROL_MANAGEMENT,	/* bDescriptorSubtype: Abstract Control Management desc */
	0x02,								/* bmCapabilities: SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported */

	/*Union Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_UNION,							/* bDescriptorSubtype: Union func desc */
	USB_CDC_CIF_NUM,					/* bMasterInterface: Communication class interface is master */
	USB_CDC_DIF_NUM,					/* bSlaveInterface0: Data class interface is slave 0 */

	/*Endpoint 1 Descriptor*/            /* event notification (optional) */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_CDC_CIF_IN_EP,					/* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
	WBVAL(0x0010),						/* wMaxPacketSize */
	0x02,			/* 2ms */           /* bInterval */

	/* Interface 2, Alternate Setting 0, Data class interface descriptor*/
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_CDC_DIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: no alternate setting */
	0x02,								/* bNumEndpoints: two endpoints used */
	CDC_DATA_INTERFACE_CLASS,			/* bInterfaceClass: Data Interface Class */
	0x00,								/* bInterfaceSubClass: no subclass available */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x05,								/* iInterface: */

	/* Endpoint, CDC Bulk Out */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_CDC_OUT_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_HS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* Endpoint, CDC Bulk In */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_CDC_IN_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_HS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* Interface 3, Alternate Setting 0, MSC Class */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_MSC_IF_NUM,						/* bInterfaceNumber */
	0x00,								/* bAlternateSetting */
	0x02,								/* bNumEndpoints */
	USB_DEVICE_CLASS_STORAGE,			/* bInterfaceClass */
	MSC_SUBCLASS_SCSI,					/* bInterfaceSubClass */
	MSC_PROTOCOL_BULK_ONLY,				/* bInterfaceProtocol */
	0x06,								/* iInterface */
	/* Bulk In Endpoint */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	MSC_EP_IN,							/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_HS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0,									/* bInterval */
	/* Bulk Out Endpoint */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	MSC_EP_OUT,							/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_HS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0,									/* bInterval */

	/* Interface 0, Alternate Setting 0, HID Class */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB_HID_IF_NUM,						/* bInterfaceNumber */
	0x00,								/* bAlternateSetting */
	0x01,								/* bNumEndpoints */
	USB_DEVICE_CLASS_HUMAN_INTERFACE,	/* bInterfaceClass */
	HID_SUBCLASS_BOOT,					/* bInterfaceSubClass */
	HID_PROTOCOL_MOUSE,					/* bInterfaceProtocol */
	0x07,								/* iInterface */
	/* HID Class Descriptor */
	/* HID_DESC_OFFSET = 0x0012 */
	HID_DESC_SIZE,						/* bLength */
	HID_HID_DESCRIPTOR_TYPE,			/* bDescriptorType */
	WBVAL(0x0111),						/* bcdHID : 1.11*/
	0x00,								/* bCountryCode */
	0x01,								/* bNumDescriptors */
	HID_REPORT_DESCRIPTOR_TYPE,			/* bDescriptorType */
	WBVAL(sizeof(Mouse_ReportDescriptor)),		/* wDescriptorLength */
	/* Endpoint, HID Interrupt In */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	HID_EP_IN,							/* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
	WBVAL(0x0008),						/* wMaxPacketSize */
	0x07,								/* bInterval : 8ms */

	/* Terminator */
	0									/* bLength */
};

/**
 * USB String Descriptor (optional)
 */
const uint8_t USB_StringDescriptor[] = {
	/* Index 0x00: LANGID Codes */
	0x04,								/* bLength */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	WBVAL(0x0409),						/* wLANGID  0x0409 = US English*/
	/* Index 0x01: Manufacturer */
	(18 * 2 + 2),						/* bLength (13 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'N', 0,
	'X', 0,
	'P', 0,
	' ', 0,
	'S', 0,
	'e', 0,
	'm', 0,
	'i', 0,
	'c', 0,
	'o', 0,
	'n', 0,
	'd', 0,
	'u', 0,
	'c', 0,
	't', 0,
	'o', 0,
	'r', 0,
	's', 0,
	/* Index 0x02: Product */
	(13 * 2 + 2),						/* bLength (13 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'L', 0,
	'P', 0,
	'C', 0,
	' ', 0,
	'C', 0,
	'o', 0,
	'm', 0,
	'p', 0,
	'o', 0,
	's', 0,
	'i', 0,
	't', 0,
	'e', 0,
	/* Index 0x03: Serial Number */
	(13 * 2 + 2),						/* bLength (13 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'A', 0,
	'B', 0,
	'C', 0,
	'D', 0,
	'1', 0,
	'2', 0,
	'3', 0,
	'4', 0,
	'5', 0,
	'6', 0,
	'7', 0,
	'8', 0,
	'9', 0,
	/* Index 0x04: Interface 0, Alternate Setting 0 */
	(3 * 2 + 2),						/* bLength (3 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'D', 0,
	'F', 0,
	'U', 0,
	/* Index 0x05 : Interface 1, Alternate Setting 0 */
	(3 * 2 + 2),						/* bLength (3 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'C', 0,
	'D', 0,
	'C', 0,
	/* Index 0x06 0x05: Interface 1, Alternate Setting 0 */
	(6 * 2 + 2),						/* bLength (13 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'M', 0,
	'e', 0,
	'm', 0,
	'o', 0,
	'r', 0,
	'y', 0,
	/* Index 0x06: Interface 2, Alternate Setting 0 */
	(3 * 2 + 2),						/* bLength (3 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'H', 0,
	'I', 0,
	'D', 0,
};

const uint8_t DFU_ConfigDescriptor[] = {
	/* Configuration 1 */
	USB_CONFIGURATION_DESC_SIZE,		/* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	WBVAL(								/* wTotalLength */
		1 * USB_CONFIGURATION_DESC_SIZE +
		1 * USB_INTERFACE_DESC_SIZE     +
		DFU_FUNC_DESC_SIZE
		),
	0x01,								/* bNumInterfaces */
	0x01,								/* bConfigurationValue */
	0x00,								/* iConfiguration */
	USB_CONFIG_SELF_POWERED,			/* bmAttributes */
	USB_CONFIG_POWER_MA(100),			/* bMaxPower */
	/* Interface 0, Alternate Setting 0, DFU Class */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	0x00,								/* bInterfaceNumber */
	0x00,								/* bAlternateSetting */
	0x00,								/* bNumEndpoints */
	USB_DEVICE_CLASS_APP,				/* bInterfaceClass */
	USB_DFU_SUBCLASS,					/* bInterfaceSubClass */
	0x02,								/* bInterfaceProtocol */
	0x04,								/* iInterface */
	/* DFU RunTime/DFU Mode Functional Descriptor */
	DFU_FUNC_DESC_SIZE,					/* bLength */
	USB_DFU_DESCRIPTOR_TYPE,			/* bDescriptorType */
	USB_DFU_CAN_DOWNLOAD | USB_DFU_CAN_UPLOAD | USB_DFU_MANIFEST_TOL,	/* bmAttributes */
	WBVAL(0x1000),						/* wDetachTimeout */
	WBVAL(USB_DFU_XFER_SIZE),			/* wTransferSize */
	WBVAL(0x100),						/* bcdDFUVersion */
	/* Terminator */
	0									/* bLength */
};






