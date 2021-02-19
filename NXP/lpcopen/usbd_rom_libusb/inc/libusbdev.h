/*
 * @brief Configuration file needed for libUSB applications.
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
#include "lpc_types.h"
#include "error.h"
#include "usbd_rom_api.h"

#ifndef __LIBUSBDEV_H_
#define __LIBUSBDEV_H_

#ifdef __cplusplus
extern "C"
{
#endif

/** @ingroup EXAMPLES_USBDROM_18XX43XX_LUSBK
 * @{
 */

/**
 * @brief	Initialize USB interface.
 * @param	mem_base	: Pointer to memory address which can be used by libusbdev driver
 * @param	mem_size	: Size of the memory passed
 * @return	If found returns the address of requested interface else returns NULL.
 */
extern ErrorCode_t libusbdev_init(uint32_t memBase, uint32_t memSize);

/**
 * @brief	Check if libusbdev is connected USB host application.
 * @return	Returns non-zero value if connected.
 */
extern bool libusbdev_Connected(void);

/**
 * @brief	Queue the read buffer to USB DMA
 * @param	pBuf	: Pointer to buffer where read data should be copied
 * @param	buf_len	: Length of the buffer passed
 * @return	Returns LPC_OK on success.
 */
extern ErrorCode_t libusbdev_QueueReadReq(uint8_t *pBuf, uint32_t buf_len);

/**
 * @brief	Check if queued read buffer got any data
 * @return	Returns length of data received. Returns -1 if read is still pending.
 * @note	Since on USB, zero length packets are transferred -1 is used for
 *			Rx pending indication.
 */
extern int32_t libusbdev_QueueReadDone (void);

/**
 * @brief	A blocking read call
 * @param	pBuf	: Pointer to buffer where read data should be copied
 * @param	buf_len	: Length of the buffer passed
 * @return	Return number of bytes read. Returns -1 if previous read is pending.
 */
extern int32_t libusbdev_Read(uint8_t *pBuf, uint32_t buf_len);

/**
 * @brief	Queue the given buffer for transmission to USB host application.
 * @param	pBuf	: Pointer to buffer to be written
 * @param	buf_len	: Length of the buffer passed
 * @return	Returns LPC_OK on success.
 */
extern ErrorCode_t libusbdev_QueueSendReq(uint8_t *pBuf, uint32_t buf_len);

/**
 * @brief	Check if queued send is done.
 * @return	Returns length of remaining data to be sent.
 *			0 indicates transfer done.
 */
extern int32_t libusbdev_QueueSendDone (void);

/**
 * @brief	Send the given buffer to USB host application.
 * @param	pBuf	: Pointer to buffer to be written
 * @param	buf_len	: Length of the buffer passed
 * @return	Returns LPC_OK on success.
 * @note	This is a BLOCKING call until Tx is done.
 */
extern ErrorCode_t libusbdev_Send(uint8_t *pBuf, uint32_t buf_len);

/**
 * @brief	Send interrupt signal to USB host application.
 * @param	status	: A 32 bit status sent to host at next interrupt
 * @return	Returns LPC_OK on success.
 * @note	USB host polls our device at interval specified
 *			by bInterval field of LUSB_INT_EP endpoint's descriptor.
 *			This routine sends the specified status on next polling opportunity.
 *			Use the routine to tell host we have data to send. So that it can
 *			start putting IN tokens.

 */
extern ErrorCode_t libusbdev_SendInterrupt(uint32_t status);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __LIBUSBDEV_H_ */






