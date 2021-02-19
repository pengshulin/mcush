/*
 * @brief Configuration file needed for USB ROM stack based applications.
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
#include "lpc_types.h"
#include "error.h"
#include "usbd_rom_api.h"

#ifndef __APP_USB_CFG_H_
#define __APP_USB_CFG_H_

#ifdef __cplusplus
extern "C"
{
#endif

/** @ingroup EXAMPLES_USBDROM_18XX43XX_COMPOSITE
 * @{
 */

/* This example uses 5 endpoints hence will work With USB0 only. */
#define USE_USB0

/* Manifest constants used by USBD ROM stack. These values SHOULD NOT BE CHANGED
   for advance features which require usage of USB_CORE_CTRL_T structure.
   Since these are the values used for compiling USB stack.
 */
#define USB_MAX_IF_NUM          8		/*!< Max interface number used for building USBD ROM. DON'T CHANGE. */
#define USB_MAX_EP_NUM          6		/*!< Max number of EP used for building USBD ROM. DON'T CHANGE. */
#define USB_MAX_PACKET0         64		/*!< Max EP0 packet size used for building USBD ROM. DON'T CHANGE. */
#define USB_FS_MAX_BULK_PACKET  64		/*!< MAXP for FS bulk EPs used for building USBD ROM. DON'T CHANGE. */
#define USB_HS_MAX_BULK_PACKET  512		/*!< MAXP for HS bulk EPs used for building USBD ROM. DON'T CHANGE. */
#define USB_DFU_XFER_SIZE       2048	/*!< Max DFU transfer size used for building USBD ROM. DON'T CHANGE. */

/* DFU boot definitions */
#define DFU_DEST_BASE         0x20004000
#define DFU_MAX_IMAGE_LEN     (32 * 1024)
#define DFU_XFER_BLOCK_SZ     (USB_DFU_XFER_SIZE)
#define DFU_MAX_BLOCKS        (DFU_MAX_IMAGE_LEN / DFU_XFER_BLOCK_SZ)

/* Manifest constants to select appropriate USB instance */
#ifdef USE_USB0
#define LPC_USB_BASE            LPC_USB0_BASE
#define LPC_USB                 LPC_USB0
#define LPC_USB_IRQ             USB0_IRQn
#define USB_IRQHandler          USB0_IRQHandler
#define USB_init_pin_clk        Chip_USB0_Init
#else
#undef USB_IRQHandler
#define LPC_USB_BASE            LPC_USB1_BASE
#define LPC_USB                 LPC_USB1
#define LPC_USB_IRQ             USB1_IRQn
#define USB_IRQHandler          USB1_IRQHandler
#define USB_init_pin_clk        Chip_USB1_Init
#endif

/* Manifest constants defining interface numbers and endpoints used by a
   particular interface in this application.
 */
#define USB_HID_IF_NUM      0x04
#define USB_CDC_CIF_NUM     0x01
#define USB_CDC_DIF_NUM     0x02
#define USB_MSC_IF_NUM      0x03
#define USB_DFUAPP_IF_NUM   0x00

#define USB_CDC_IN_EP       0x81
#define USB_CDC_OUT_EP      0x01
#define USB_CDC_CIF_IN_EP   0x82
#define MSC_EP_IN           0x83
#define MSC_EP_OUT          0x02
#define HID_EP_IN           0x84

/* On LPC18xx/43xx the USB controller requires endpoint queue heads to start on
   a 4KB aligned memory. Hence the mem_base value passed to USB stack init should
   be 4KB aligned. The following manifest constants are used to define this memory.
 */
#define USB_STACK_MEM_BASE      0x20000000
#define USB_STACK_MEM_SIZE      0x00002000

/* USB descriptor arrays defined *_desc.c file */
extern uint8_t USB_DeviceDescriptor[];
extern uint8_t USB_HsConfigDescriptor[];
extern uint8_t USB_FsConfigDescriptor[];
extern const uint8_t USB_StringDescriptor[];
extern const uint8_t USB_DeviceQualifier[];
extern const uint8_t DFU_DeviceDescriptor[];
extern const uint8_t DFU_ConfigDescriptor[];

/**
 * @brief	Find the address of interface descriptor for given class type.
 * @param	pDesc		: Pointer to configuration descriptor in which the desired class
 *			interface descriptor to be found.
 * @param	intfClass	: Interface class type to be searched.
 * @return	If found returns the address of requested interface else returns NULL.
 */
extern USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __APP_USB_CFG_H_ */






