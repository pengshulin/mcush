/*
 * @brief LPC43XX dual core common functions and defines
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
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

#ifndef __DUALCORE_COMMON_H_
#define __DUALCORE_COMMON_H_

#include "board.h"

/** @defgroup EXAMPLES_DUALCORE_43XX_COMMON LPC43xx dual core common code
 * @ingroup EXAMPLES_DUALCORE_43XX
 * Common code shared among dual-core examples
 *
 * @{
 */

/*
 * The Stack sizes are currently used by Code Red LPCXpresso tool chain
 * only. In future, this will be used by Keil & IAR tool chains also
 */
#ifndef STACK_SIZE
#define STACK_SIZE  0x800
#endif

#ifndef HEAP_SIZE
#define HEAP_SIZE   0x6000
#endif

/*
 * Allocated indexes for some shared global variables
 */
#define SHGBL_USBDISKADDR  0
#define SHGBL_IPADDR       1
#define SHGBL_HOSTADDR     2


/* Priority of various tasks in dual-core examples */
/* LWIP thread priority should always be greater than the
 * MAC priority ie., greater than TASK_PRIO_ETHERNET
 */
#ifdef OS_FREE_RTOS
/* higher the number higher the priority */
#define TASK_PRIO_LCD             (tskIDLE_PRIORITY + 0UL)
#define TASK_PRIO_TOUCHSCREEN     (tskIDLE_PRIORITY + 1UL)
#define TASK_PRIO_BLINKY_EVENT    (tskIDLE_PRIORITY + 1UL)
#define TASK_PRIO_ETHERNET        (tskIDLE_PRIORITY + 2UL)
#define TASK_PRIO_IPC_DISPATCH    (tskIDLE_PRIORITY + 3UL)
#define TASK_PRIO_USBDEVICE       (tskIDLE_PRIORITY + 4UL)
#define TASK_PRIO_LWIP_THREAD     (tskIDLE_PRIORITY + 5UL)

#elif defined(OS_UCOS_III)
/* lower the number higher the priority */
#define TASK_PRIO_BLINKY_EVENT    14
#define TASK_PRIO_LCD             14
#define TASK_PRIO_TOUCHSCREEN     13
#define TASK_PRIO_ETHERNET        13
#define TASK_PRIO_IPC_DISPATCH    12
#define TASK_PRIO_USBDEVICE       11
#define TASK_PRIO_LWIP_THREAD     10
#endif

/* Priority of various IRQs used in dual-core examples */
/* lower the number higher the priority */
#define IRQ_PRIO_IPC              7
#define IRQ_PRIO_ETHERNET         6
#define IRQ_PRIO_USBDEV           5

/* Minimum stack size for UCOS-III Tasks */
#define UCOS_MIN_STACK_SZ         128

/*
 * Offset of M0 image from the starting of M4 image
 * Usually the size allocated for M0 image in scatter
 * file/ Linker Definition file / Memory configuration
 * in the IDE.
 * NOTE: Make sure when this macro is overridden, change
 * the link address of M0 in such way that the CODE section
 * starts at TARGET_XXXX address + M0_IMAGE_OFFSET
 */
#ifndef M0_IMAGE_OFFSET
#define M0_IMAGE_OFFSET      0x40000
#endif

/*
 * Absolute addresses used by both cores. Make sure these memory regions are not
 * used in DATA/CODE regions of the generated image (check the map file to verify)
 * Currently the shared memory buffers are located at the end of the SRAM area
 * 0x1008000. Last 64 bytes of the 40K area are used for M4 queue header and
 * the 64 bytes above that is used by M0 queue. This value need not be changed
 * for normal circumstances.
 */
#ifndef SHARED_MEM_M0
#define SHARED_MEM_M0          0x10089F80
#endif
#ifndef SHARED_MEM_M4
#define SHARED_MEM_M4          0x10089FC0
#endif

/* Size RAM DISK image used by FAT Filesystem */
#ifndef RAMDISK_SIZE
#define RAMDISK_SIZE           0x2000
#endif

/* Buffer address used by USB example */
#ifndef USB_DESC_BUFFER
#define USB_DESC_BUFFER        0x10088000 /* Must be 2K aligned */
#endif
#ifndef USB_DESC_BUFFER_SZ
#define USB_DESC_BUFFER_SZ     0x400      /* 1K Should suffice */
#endif

#ifdef CORE_M4
/* Delay and LED to be blinked by M4 Core */
#define BLINKY_DEFAULT_DELAY         1000
#define BLINK_LED                    1
#endif /* CORE_M4 */

#ifdef CORE_M0
/* Delay and LED to be blinked by M4 Core */
#define BLINK_LED                    0
#define BLINKY_DEFAULT_DELAY         500
#endif /* CORE_M0 */

/* Base address of various flashes */
#define SPIFI_BASE_ADDR      0x14000000
#define XFLASH_BASE_ADDR     0x1C000000
#define IFLASH_BANKA_ADDR    0x1A000000
#define IFLASH_BANKB_ADDR    0x1B000000

/*
 * Select the default device to which the image
 * be loaded, based on the board. It could be overridden
 * by the user by defining one of the TARGET_XXXXX in the
 * compiler defines.
 */
#if (!defined(TARGET_SPIFI) && !defined(TARGET_IFLASH) && !defined(TARGET_XFLASH))
	#if defined(BOARD_HITEX_EVA_4350)
		#define TARGET_SPIFI
	#elif defined(BOARD_KEIL_MCB_4357)
		#define TARGET_IFLASH
	#elif defined(BOARD_NGX_XPLORER_4330)
		#define TARGET_SPIFI
	#elif defined(BOARD_NXP_LPCXPRESSO_4337)
		#define TARGET_SPIFI
	#else
		#error "Unsupported chip or Unknown load target!"
	#endif
#endif

/* Selecting base address based on Target */
#ifdef TARGET_SPIFI
	#define M0_IMAGE_ADDR  (SPIFI_BASE_ADDR + M0_IMAGE_OFFSET)
#elif defined(TARGET_XFLASH)
	#define M0_IMAGE_ADDR  (XFLASH_BASE_ADDR + M0_IMAGE_OFFSET)
#elif defined(TARGET_IFLASH)
	#define M0_IMAGE_ADDR  IFLASH_BANKB_ADDR
#else
	#error "Unknown load target!"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	IPC Example initialization
 *
 * This function initializes the IPC Example implementation. It calls
 * IPC_initMsgQueue() to initialize the IPC queue of the calling core,
 * by allocating and passing a queue array. It will be called by the
 * @link EXAMPLE_DUALCORE_CMN_MAIN main()@endlink of both core-M0 and
 * core-M4 code for all the dual core examples.
 *
 * @return	None
 */
extern void IPCEX_Init(void);

/**
 * @brief	USB Host example initialization
 *
 * Function that initializes usb host stack by calling USB_Init() and
 * the dual core USB host example. *This function is called by
 * @link EXAMPLE_DUALCORE_CMN_MAIN main()@endlink only when EXAMPLE_USB_HOST
 * is defined*.
 *
 *@return	None
 */
extern void USBHOST_Init(void);

/**
 * @brief	USB Device example initialization
 *
 * Function that initializes usb device stack by calling USB_Init() and
 * the dual core USB device example. *This function is called by
 * @link EXAMPLE_DUALCORE_CMN_MAIN main()@endlink only when EXAMPLE_USB_DEVICE
 * is defined*.
 *
 *@return	None
 */
extern void USBDEV_Init(void);

/**
 * @brief	lwIP dual core example initialisation function
 *
 * This function initializes the lwIP interface (ethernet etc.). In
 * stand-alone configuration, this function will enable the tick
 * functionality. It will initialises the LWIP stack, initialises
 * the network interface, initialises the DHCP & HTTPD functions.
 * In FreeRTOS/uCOS-III configurations, it does nothing as the
 * initialization is handed by the corresponding tasks.  *This function
 * is called by @link EXAMPLE_DUALCORE_CMN_MAIN main()@endlink only
 * when EXAMPLE_LWIP is defined*.
 *
 * @return None
 */
extern void LWIP_Init(void);

/**
 * @brief	emWin dual core example initialisation function
 *
 * The function initialises the LCD controller & Touch screen controller.
 * It will register the IPC callbacks to receive graphics information from
 * other processor core. In stand-alone configuration, it will also create
 * the emWin Widget framewindow of the graphics application. *This function
 * is called by @link EXAMPLE_DUALCORE_CMN_MAIN main()@endlink only
 * when EXAMPLE_EMWIN is defined*.
 *
 * @return None
 */
extern void EMWIN_Init(void);

/**
 * @brief	Dual core Blinky example initialization
 *
 * This function will register the blink function to the event queue of
 * a Core (M0/M4). Whenever a blink event is received from the other core
 * the registered call-back function will be called-back by the ipcex_task(),
 * *This function is called by @link EXAMPLE_DUALCORE_CMN_MAIN main()@endlink
 * only when EXAMPLE_BLINKY is defined*. In all the dual core examples
 * EXAMPLE_BLINKY is defined by default.
 *
 * @return	None
 */
extern void BLINKY_Init(void);

/**
 * @brief	Dual Core IPC example implementation task
 *
 * This task receives the message from the other core and will call-back the
 * function (if registered via ipcex_register_callback()) corresponding to
 * the received message. This function is called by @link
 * EXAMPLE_DUALCORE_CMN_MAIN main()@endlink.
 *
 * @return	None
 */
extern void ipcex_tasks(void);

/**
 * @brief	Dual core blinky task
 *
 * Calling this function calls the blinky task in which will blink the
 * LEDs. When no OS is specified this will be repeatedly *called from
 * dual core @link EXAMPLE_DUALCORE_CMN_MAIN main()@endlink or once
 * if any OS is defined, only when EXAMPLE_BLINKY is defined*. In all
 * the dual core examples EXAMPLE_BLINKY is defined by default.
 *
 * @return	None
 */
extern void blinky_tasks(void);

/**
 * @brief	Dual Core USB host task
 *
 * This function creates the task that invokes the host tasks
 * and the USB_USBTask() provided by the USB library. *This
 * function is called by @link EXAMPLE_DUALCORE_CMN_MAIN main()@endlink
 * only when EXAMPLE_USB_HOST is defined*.
 *
 * @return None
 */
extern void usb_host_tasks(void);

/**
 * @brief	Dual Core USB device task
 *
 * This function creates the task that invokes the class driver
 * task and the USB_USBTask() provided by the USB library. *This
 * function is called by @link EXAMPLE_DUALCORE_CMN_MAIN main()@endlink
 * only when EXAMPLE_USB_DEVICE is defined*.
 *
 * @return None
 */
extern void usb_device_tasks(void);

/**
 * @brief	LWIP Task function
 *
 * Function that creates/performs the lwIP stack functionality.
 * In stand-alone configuration, this function will monitor the link
 * status and handles the packets. In FreeRTOS/uCOS-III configurations,
 * it will create the network interface task. *Called by dual core
 * @link EXAMPLE_DUALCORE_CMN_MAIN main()@endlink only when
 * EXAMPLE_LWIP is defined*.
 *
 * @return None
 */
extern void lwip_tasks(void);

/**
 * @brief	emWin dual core example task
 *
 * In stand-alone configuration, it will update the fields on LCD screen. In
 * FreeRTOS/uCOS-III configurations, this will create the LCD & Touch Screen
 * application tasks. *Called by dual core @link EXAMPLE_DUALCORE_CMN_MAIN
 * main()@endlink only when EXAMPLE_EMWIN is defined*.
 *
 * @return None
 */
extern void emwin_tasks(void);

/**
 * @brief	Millisecond sleep
 *
 * Calling this function will sleep for \a msec number of milli seconds
 * by a busy wait loop. This function uses the RITIMER in LPC43XX to calculate
 * the time lapsed.
 *
 * @param	msecs	: Number of milli seconds to sleep
 * @return	None
 */
void MSleep(int32_t msecs);

/**
 * @brief	Initialize M0 and boot the core with image at \a m0_image_addr
 *
 * This function is avilable only for code that runs on M4 core, and defined in
 * m0_ImageLoader.c
 *
 * @param	m0_image_addr	: uint32_t converted pointer to M0 image address
 * @return	0 on success -1 on error
 */
int M0Image_Boot(uint32_t m0_image_addr);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif






