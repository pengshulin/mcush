/*
 * @brief USB Band Width and WCID example descriptors.
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

#include "app_usbd_cfg.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/**
 * USB Standard Device Descriptor
 */
ALIGNED(4) const uint8_t USB_DeviceDescriptor[] = {
	USB_DEVICE_DESC_SIZE,				/* bLength */
	USB_DEVICE_DESCRIPTOR_TYPE,			/* bDescriptorType */
	WBVAL(0x0200),						/* bcdUSB: 2.00 */
	0x00,								/* bDeviceClass */
	0x00,								/* bDeviceSubClass */
	0x00,								/* bDeviceProtocol */
	USB_MAX_PACKET0,					/* bMaxPacketSize0 */
	WBVAL(0x1FC9),						/* idVendor */
	WBVAL(WCID_VENDOR_CODE),			/* idProduct */
	WBVAL(0x0100),						/* bcdDevice: 1.00 */
	0x01,								/* iManufacturer */
	0x02,								/* iProduct */
	0x03,								/* iSerialNumber */
	0x01								/* bNumConfigurations */
};

/**
 * USB Device Qualifier
 */
ALIGNED(4) const uint8_t USB_DeviceQualifier[] = {
	USB_DEVICE_QUALI_SIZE,					/* bLength */
	USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE,	/* bDescriptorType */
	WBVAL(0x0200),							/* bcdUSB:  2.00 */
	0x00,									/* bDeviceClass */
	0x00,									/* bDeviceSubClass */
	0x00,									/* bDeviceProtocol */
	USB_MAX_PACKET0,						/* bMaxPacketSize0 */
	0x01,									/* bNumOtherSpeedConfigurations */
	0x00									/* bReserved */
};

/* USB FSConfiguration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
ALIGNED(4) uint8_t USB_FsConfigDescriptor[] = {
	/* Configuration 1 */
	USB_CONFIGURATION_DESC_SIZE,		/* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	WBVAL(								/* wTotalLength */
		USB_CONFIGURATION_DESC_SIZE +
		USB_INTERFACE_DESC_SIZE     +
		4 * USB_ENDPOINT_DESC_SIZE
		),
	0x01,							/* bNumInterfaces */
	0x01,							/* bConfigurationValue */
	0x00,							/* iConfiguration */
	USB_CONFIG_BUS_POWERED,			/* bmAttributes */
	USB_CONFIG_POWER_MA(100),		/* bMaxPower */
	/* Interface 0, Alternate Setting 0, Custom Class */
	USB_INTERFACE_DESC_SIZE,		/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,	/* bDescriptorType */
	0x00,							/* bInterfaceNumber */
	0x00,							/* bAlternateSetting */
	0x04,							/* bNumEndpoints */
	0xFF,							/* bInterfaceClass */
	0xF0,							/* bInterfaceSubClass */
	0x00,							/* bInterfaceProtocol */
	0x04,							/* iInterface */
	/* EP1 Bulk Out Endpoint */
	USB_ENDPOINT_DESC_SIZE,			/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB_ENDPOINT_OUT(1),			/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,			/* bmAttributes */
	WBVAL(64),						/* wMaxPacketSize */
	0,								/* bInterval */
	/* EP1 Bulk In Endpoint */
	USB_ENDPOINT_DESC_SIZE,			/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB_ENDPOINT_IN(1),				/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,			/* bmAttributes */
	WBVAL(64),						/* wMaxPacketSize */
	0,								/* bInterval */
	/* EP2 Bulk Out Endpoint */
	USB_ENDPOINT_DESC_SIZE,			/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB_ENDPOINT_OUT(2),			/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,			/* bmAttributes */
	WBVAL(64),						/* wMaxPacketSize */
	0,								/* bInterval */
	/* EP2 Bulk In Endpoint */
	USB_ENDPOINT_DESC_SIZE,			/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB_ENDPOINT_IN(2),				/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,			/* bmAttributes */
	WBVAL(64),						/* wMaxPacketSize */
	0,								/* bInterval */
	/* Terminator */
	0								/* bLength */
};

/* USB HSConfiguration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
ALIGNED(4) uint8_t USB_HsConfigDescriptor[] = {
	/* Configuration 1 */
	USB_CONFIGURATION_DESC_SIZE,		/* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	WBVAL(								/* wTotalLength */
		USB_CONFIGURATION_DESC_SIZE +
		USB_INTERFACE_DESC_SIZE     +
		4 * USB_ENDPOINT_DESC_SIZE
		),
	0x01,							/* bNumInterfaces */
	0x01,							/* bConfigurationValue */
	0x00,							/* iConfiguration */
	USB_CONFIG_BUS_POWERED,			/* bmAttributes */
	USB_CONFIG_POWER_MA(100),		/* bMaxPower */
	/* Interface 0, Alternate Setting 0, Custom Class */
	USB_INTERFACE_DESC_SIZE,		/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,	/* bDescriptorType */
	0x00,							/* bInterfaceNumber */
	0x00,							/* bAlternateSetting */
	0x04,							/* bNumEndpoints */
	0xFF,							/* bInterfaceClass */
	0xF0,							/* bInterfaceSubClass */
	0x00,							/* bInterfaceProtocol */
	0x04,							/* iInterface */
	/* EP1 Bulk In Endpoint */
	USB_ENDPOINT_DESC_SIZE,			/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB_ENDPOINT_OUT(1),			/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,			/* bmAttributes */
	WBVAL(512),						/* wMaxPacketSize */
	0,								/* bInterval */
	/* EP1 Bulk In Endpoint */
	USB_ENDPOINT_DESC_SIZE,			/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB_ENDPOINT_IN(1),				/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,			/* bmAttributes */
	WBVAL(512),						/* wMaxPacketSize */
	0,								/* bInterval */
	/* EP2 Bulk In Endpoint */
	USB_ENDPOINT_DESC_SIZE,			/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB_ENDPOINT_OUT(2),			/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,			/* bmAttributes */
	WBVAL(512),						/* wMaxPacketSize */
	0,								/* bInterval */
	/* EP2 Bulk In Endpoint */
	USB_ENDPOINT_DESC_SIZE,			/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB_ENDPOINT_IN(2),				/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,			/* bmAttributes */
	WBVAL(512),						/* wMaxPacketSize */
	0,								/* bInterval */
	/* Terminator */
	0								/* bLength */
};

/* USB String Descriptor (optional) */
ALIGNED(4) const uint8_t USB_StringDescriptor[] = {
	/* Index 0x00: LANGID Codes */
	0x04,							/* bLength */
	USB_STRING_DESCRIPTOR_TYPE,		/* bDescriptorType */
	WBVAL(0x0409),					/* wLANGID  0x0409 = US English*/
	/* Index 0x01: Manufacturer */
	(18 * 2 + 2),					/* bLength (18 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,		/* bDescriptorType */
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
	(23 * 2 + 2),					/* bLength (23 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,		/* bDescriptorType */
	'L', 0,
	'P', 0,
	'C', 0,
	'1', 0,
	'8', 0,
	'x', 0,
	'x', 0,
	' ', 0,
	'B', 0,
	'A', 0,
	'N', 0,
	'D', 0,
	' ', 0,
	'W', 0,
	'I', 0,
	'D', 0,
	'T', 0,
	'H', 0,
	' ', 0,
	'T', 0,
	'E', 0,
	'S', 0,
	'T', 0,
	/* Index 0x03: Serial Number */
	(13 * 2 + 2),					/* bLength (13 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,		/* bDescriptorType */
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
	(6 * 2 + 2),					/* bLength (6 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,		/* bDescriptorType */
	'S', 0,
	't', 0,
	'r', 0,
	'e', 0,
	'a', 0,
	'm', 0,
};

/* WCID USB: Microsoft String Descriptor */
ALIGNED(4) const uint8_t WCID_String_Descriptor[] = {
	(8 * 2 + 2),					/* bLength (8 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,		/* bDescriptorType */
	'M', 0,
	'S', 0,
	'F', 0,
	'T', 0,
	'1', 0,
	'0', 0,
	'0', 0,
	WCID_VENDOR_CODE, 0,
};

/* WCID USB: Microsoft Compatible ID Feature Descriptor */
ALIGNED(4) const uint8_t WCID_CompatID_Descriptor[] = {
	0x28, 0x00, 0x00, 0x00,						/* Length 40 bytes */
	0x00, 0x01,									/* Version */
	0x04, 0x00,									/* Compatibility ID Descriptor index  */
	0x01,										/* Number of sections */
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,							/* Reserved: 7 bytes */
	0x00,										/* Interface Number */
	0x01,										/* Reserved */
	'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,	/* Compatible ID: 8 bytes ASCII */
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,						/* Sub-Compatible ID: 8 bytes ASCII*/
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00,									/* Reserved: 6 bytes */
};

/* WCID USB: Microsoft Extended Properties Feature Descriptor */
ALIGNED(4) const uint8_t WCID_ExtProp_Descriptor[] = {
	0x8E, 0x00, 0x00, 0x00,						/* Length 142 bytes */
	0x00, 0x01,									/* Version */
	0x05, 0x00,									/* Extended Properties Feature Descriptor index  */
	0x01, 0x00,									/* Number of sections */
	0x84, 0x00, 0x00, 0x00,						/* Size of the property section  */
	0x01, 0x00, 0x00, 0x00,						/* Property data type (1 = Unicode REG_SZ, see table below)  */
	0x28, 0x00,									/* Property name length (40 bytes) */
	/* Property Name (L"DeviceInterfaceGUID")  */
	'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0, 'I', 0, 'n', 0,
	't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0, 'G', 0,
	'U', 0, 'I', 0, 'D', 0, 0, 0,
	0x4E, 0x00, 0x00, 0x00,						/* Property name length (80 bytes) */
	/* Property Name "{a01674b4-c5f6-485c-af94-3271701d57b4}" */
	'{', 0, 'a', 0, '0', 0, '1', 0, '6', 0, '7', 0, '4', 0, 'b', 0,
	'4', 0, '-', 0, 'c', 0, '5', 0, 'f', 0, '6', 0, '-', 0, '4', 0,
	'8', 0, '5', 0, 'c', 0, '-', 0, 'a', 0, 'f', 0, '9', 0, '4', 0,
	'-', 0, '3', 0, '2', 0, '7', 0, '1', 0, '7', 0, '0', 0, '1', 0,
	'd', 0, '5', 0, '7', 0, 'b', 0, '4', 0, '}', 0, 0, 0,
};






