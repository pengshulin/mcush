/*
 * @brief Example implementation of IPC using IPC library
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

#ifndef _IPC_EXAMPLE_H_
#define _IPC_EXAMPLE_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup EXAMPLE_DUALCORE_CMN_IPC_EX LPC43xx Inter Processor Communication(IPC) example
 * @ingroup EXAMPLE_DUALCORE_CMN_IPC
 *
 * @{
 */

/**
 * \def IPCEX_QUEUE_SZ
 * Size of the IPC Queue used by this example implimentation, the current
 * implementation uses same queue size for both M0 and M4, application
 * developer can override it.
 */
#define IPCEX_QUEUE_SZ        64

/**
 * \def IPCEX_MAX_IDS
 * Maximum number of message IDs that this IPC examples uses
 */
#define IPCEX_MAX_IDS         20

/**
 * \def IPCEX_MAX_GBLCFG
 * Maximum number of synchronized global variables
 */
#define IPCEX_MAX_GBLCFG      8

#define IPCEX_ID_FREEMEM       1  /*!< Frees memory allocated by other core */
#define IPCEX_ID_GBLUPDATE     2  /*!< Update global variable or other core */
#define IPCEX_ID_BLINKY        4  /*!< Blinky IPC event ID */
#define IPCEX_ID_USER1         10 /*!< Used by Example IPC code */
#define IPCEX_ID_USER2         11 /*!< IPC ID that can be used by other user examples */

typedef struct __ipcex_msg {
	uint32_t id;
	uint32_t data;
} ipcex_msg_t;

/**
 * @brief M0-M4 Shared variable update notification function type
 */
typedef void (*ipcex_gblupdatefn_t)(int);

/**
 * @brief	IPC register callback function pointer
 *
 * This function registers a callback function pointer to a
 * message \a id, whenever a message with \a id is received the
 * register call-back function will be invoked.
 *
 * @param	id		: ID of message to which the callback \a func be associated
 * @param	func	: pointer to callback function
 * @return	0 on failure [given \a id is greater than 
 * @note	#IPCEX_MAX_IDS], !0 on success
 */
int ipcex_register_callback(uint32_t id, void (*func)(uint32_t));

/**
 * @brief	Push data on the queue
 * @param	id		: Task ID of the destination task
 * @param	data	: Data containing the message
 * @return	#QUEUE_ERROR or #QUEUE_FULL on error, #QUEUE_INSERT on success
 */
int ipcex_msgPush(uint32_t id, uint32_t data);

/**
 * @brief	Sets the value of a shared global variable
 * @param	index	: index of the shared variable
 * @param	val		: Value of the shared global variable
 * @return	0 on success 1 on error
 */
int ipcex_setGblVal(int index, uint32_t val);

/**
 * @brief	Read a shared global variable
 * @param	index	: Index of the shared variable
 * @return	Value of the shared variable
 */
uint32_t ipcex_getGblVal(int index);

/**
 * @brief		Register notification function for shared global value update
 * @param		newfn		: New function pointer that will handle notification
 * @return		Old function pointer that was handling the notfication
 * @note		The new function, after completing the handling of shared global
 * variable change, **MUST** call the old function handler if it is not NULL.
 */
ipcex_gblupdatefn_t ipcex_gblRegisterFn(ipcex_gblupdatefn_t newfn);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _IPC_EXAMPLE_H_ */






