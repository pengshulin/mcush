/*
 * @brief HID Serial IO example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
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

/** @defgroup EXAMPLES_USBDROM_18XX43XX_HID_SIO LPC18xx/43xx HID to Serial IO bridge
 * @ingroup EXAMPLES_USBDROM_18XX43XX
 * <b>Example description</b><br>
 * The example shows how to use USBD ROM stack to creates USB to Serial IO bridge using HID class.
 * The micro-controller shows up as a HID device on the PC and it needs the LPCUSBSIO v2.0 based application
 * to talk to the bridge. Devices connected on the board to the micro-controller through I2C or SPI can be 
 * accesses through the bridge. Also the GPIO lines on the micro-controller can be controlled through the bridge.
 * <br>
 *
 * <b>Special connection requirements</b><br>
 * Connect the USB cable between micro connector on board and to a host. On host
 * user should run a LPCUSBSIO v2.0 based application to talk to the bridge. If external I2C/SPI devices are connected
 * then the pin muxing in the example should be modified accordingly.
 * <br>
 *
 * <b>Build procedures:</b><br>
 * Visit the <a href="http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides">LPCOpen quickstart guides</a>
 * to get started building LPCOpen projects.
 *
 * <b>Supported boards and board setup:</b><br>
 * @ref LPCOPEN_18XX_BOARD_HITEX1850<br>
 * @ref LPCOPEN_43XX_BOARD_HITEX4350<br>
 * @ref LPCOPEN_18XX_BOARD_KEIL1857<br>
 * @ref LPCOPEN_43XX_BOARD_KEIL4357<br>
 * @ref LPCOPEN_18XX_BOARD_NGX1830<br>
 * @ref LPCOPEN_43XX_BOARD_NGX4330<br>
 * @ref LPCOPEN_43XX_BOARD_LPCLINK2_4370<br>
 *
 * <b>Submitting LPCOpen issues:</b><br>
 * @ref LPCOPEN_COMMUNITY
 * @{
 */

/**
 * @}
 */






