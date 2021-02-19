/*
 * @brief Protocol definitions for LPCUSBSIO's I2C interface
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

#include <stdint.h>

#ifndef __LPCUSBSIO_I2C_H_
#define __LPCUSBSIO_I2C_H_

#ifdef __cplusplus
extern "C"
{
#endif

#pragma pack(1)

/** @defgroup LPCUSBSIO_I2C LPCUSBSIO protocol definitions for I2C interface
 * @ingroup LPCUSBSIO
 * This package defines the LPCUSBSIO packet structure for I2C interface.
 * Both the firmware and PC (host) libraries use this definitions to construct
 * the I2C payloads.
 * @{
 */

#define HID_I2C_MAX_PACKETS         4		/*!< Maximum packets allowed in processing queue */
#define HID_I2C_PACKET_SZ           64		/*!< Packet size of each I2C command packet */
#define HID_I2C_HEADER_SZ           4		/*!< Size of the header in I2C command packet */

/**********************************************************************
	HID_I2C Requests 
 **********************************************************************/
/** Request to abort and flush all pending requests 
 * Request Packet layout:
 *		No parameters
 * Response Packet layout:
 *		No response data
 */
#define HID_I2C_REQ_RESET           0x00	

/** Request to initialize the I2C port.
 * Request Packet layout:
 *		byte[0 -  3] 		: I2C bus speed
 *		byte[4 -  7]		: Configuration options
 *		byte[8 - 12]		: I2C port number
 * Response Packet layout:
 *		No response data
 * sesID field in header contains the I2C port number.
 */
#define HID_I2C_REQ_INIT_PORT       0x01	

/** Request to de-initialize the I2C port. 
 * Request Packet layout:
 *		No parameters
 * Response Packet layout:
 *		No response data
 * sesID field in header contains the I2C port number.
 */
#define HID_I2C_REQ_DEINIT_PORT     0x02	

/** Request to write data to the I2C port. 
 * Request Packet layout:
 *		byte[0 -  1] 		: Length of the transfer.
 *		byte[2]				: options.
 *		byte[3]				: I2C slave address
 *		byte[4 - ..] 		: Write data.
 * Response Packet layout:
 *		No response data
 * sesID field in header contains the I2C port number.
 */
#define HID_I2C_REQ_DEVICE_WRITE    0x03	

/** Request to read data from the I2C port. 
 * Request Packet layout:
 *		byte[0 -  1] 		: Length of the transfer.
 *		byte[2]				: options.
 *		byte[3]				: I2C slave address
 *		byte[4 - ..] 		: reserved.
 * Response Packet layout:
 *		byte[0 -  1] 		: Length of the transfer.
 *		byte[2]				: options.
 *		byte[3]				: I2C slave address
 *		byte[4 - ..] 		: read data.
 * sesID field in header contains the I2C port number.
 */
#define HID_I2C_REQ_DEVICE_READ     0x04	

/**  Request to write and then read data from the I2C port. 
 * Request Packet layout:
 *		byte[0 -  1] 		: Length of the transfer.
 *		byte[2]				: options.
 *		byte[3]				: I2C slave address
 *		byte[4 - ..] 		: Write data.
 * Response Packet layout:
 *		byte[0 -  1] 		: Length of the transfer.
 *		byte[2]				: options.
 *		byte[3]				: I2C slave address
 *		byte[4 - ..] 		: Read data.
 * sesID field in header contains the I2C port number.
 */
#define HID_I2C_REQ_DEVICE_XFER     0x05	

/** Last I2C specific request */
#define HID_I2C_REQ_MAX			 	0x0F	

/**********************************************************************
	HID_SPI Requests
**********************************************************************/
/**  Request to reset and abort the SPI controller. 
 * Request Packet layout:
 *		No parameters
 * Response Packet layout:
 *		No response data
 */
#define HID_SPI_REQ_RESET           0x10	/*!< Request to abort and flush all pending requests */
/**  Request to initialize SPI port. 
 * Request Packet layout:
 *		No parameters
 * Response Packet layout:
 *		No response data
 */
#define HID_SPI_REQ_INIT_PORT       0x11	/*!< Request to initialize the SPI port */
/**  Request to deinitializ SPI port. 
 * Request Packet layout:
 *		No parameters
 * Response Packet layout:
 *		No response data
 */
#define HID_SPI_REQ_DEINIT_PORT     0x12	/*!< Request to de-initialize the SPI port */
/**  Request to write read data from the SPI port. 
 * Request Packet layout:
 *		No parameters
 * Response Packet layout:
 *		No response data
 */
#define HID_SPI_REQ_DEVICE_XFER     0x13	/*!< Request to write and then read data from the SPI port */

/** Last SPI specific request */
#define HID_SPI_REQ_MAX			 	0x1F	

/**********************************************************************
	HID_GPIO Requests
**********************************************************************/

/**  Request to write GPIO port state. 
 * Request Packet layout:
 *		byte[0 -  3]	: Bits/PINs to be set
 *		byte[4 -  7]	: Bits/PINs to be cleared
 * Response Packet layout:
 *		byte[0 -  3] 	: GPIO port PIN status
 * sesID field in header contains the GPIO port number.
 */
#define HID_GPIO_REQ_PORT_VALUE      0x20	

/**  Request to set GPIO port pin direction state. 
 * Request Packet layout:
 *		byte[0 -  3]	: Bits/PINs to be set as outputs
 *		byte[4 -  7]	: Bits/PINs to be set as inputs
 * Response Packet layout:
 *		byte[0 -  3] 	: GPIO port DIR status
 * sesID field in header contains the GPIO port number.
 */
#define HID_GPIO_REQ_PORT_DIR	 0x21	

/**  Request to set GPIO pin state. 
 * Request Packet layout:
 *		byte[0]			: GPIO pin number
 *		byte[1 - 3] 	: Reserved
 * Response Packet layout:
 *		No response data
 * sesID field in header contains the GPIO port number.
 */
#define HID_GPIO_REQ_TOGGLE_PIN		 0x23	

/** Last GPIO specific request */
#define HID_GPIO_REQ_MAX			 0x2F	

/**********************************************************************
	HID_SIO Requests
**********************************************************************/

/**  Request to get LPCUSBSIO information. 
 * Request Packet layout:
 *		No parameters
 * Response Packet layout:
 *		byte[0] 		: Number of I2C ports available on this instance
 *		byte[1]			: Number of SPI ports available on this instance
 *		byte[2]			: Number of GPIO ports available on this instance
 *		byte[3]			: Reserved
 *		byte[4 -  5]	: Firmware Minor version number
 *		byte[6 -  7]	: Firmware Major version number
  *		byte[8 -...]	: Firmware version string
 */
#define HID_SIO_REQ_DEV_INFO         0xF0	

/** Last SIO specific request */
#define HID_SIO_REQ_MAX			 	0xFF	

/**
 * @brief	HID to I2C bridge Request structure.
 *  Defines the structure of HID to I2C Request packet. This is same as
 *  HID OUT report.
 */
typedef struct __HIDI2C_OUT_REPORT {
	uint8_t length;		/*!< Length of the HID_I2C Request structure including cmd, transId and length fields. */
	uint8_t transId;	/*!< I2C_HID transaction identifier. Rolls over after 255. */
	uint8_t sesId;		/*!< I2C_HID session identifier. */
	uint8_t req;		/*!< I2C_HID Request */
	uint8_t data[];		/*!< Data corresponding to the Request */

} HID_I2C_OUT_REPORT_T;

/** HID_I2C responses. The response code below 0x10 should match with I2CM_STATUS codes. */
#define HID_I2C_RES_OK              0x00		/*!< Requested Request was executed successfully. */
#define HID_I2C_RES_ERROR           0x01		/*!< Unknown error condition. */
#define HID_I2C_RES_NAK             0x02		/*!< No device responded for given slave address. */
#define HID_I2C_RES_BUS_ERROR       0x03		/*!< I2C bus error */
#define HID_I2C_RES_SLAVE_NAK       0x04		/*!< NAK received after SLA+W or SLA+R */
#define HID_I2C_RES_ARBLOST         0x05		/*!< Arbitration lost */

#define HID_I2C_RES_TIMEOUT         0x10		/*!< Transaction timed out. */
#define HID_I2C_RES_INVALID_CMD     0x11		/*!< Invalid HID_I2C Request or Request not supported in this version. */
#define HID_I2C_RES_INVALID_PARAM   0x12		/*!< Invalid parameters are provided for the given Request. */
#define HID_I2C_RES_PARTIAL_DATA    0x13		/*!< Partial transfer completed. */

/**
 * @brief	HID to I2C bridge response structure.
 *  Defines the structure of HID to I2C Request packet. This is same as
 *  HID OUT report.
 */
typedef struct __HIDI2C_IN_REPORT {
	uint8_t length;		/*!< Length of the HID_I2C response structure including resp, transId and length fields.*/
	uint8_t transId;	/*!< I2C_HID transaction identifier. */
	uint8_t sesId;		/*!< I2C_HID session identifier. */
	uint8_t resp;		/*!< I2C_HID response */
	uint8_t data[];		/*!< Data corresponding to the response */

} HID_I2C_IN_REPORT_T;

/**
 * @brief Port configuration information
 */
typedef struct __HIDI2C_PortConfig_t {
	uint32_t busSpeed;	/*!< I2C bus speed */
	uint32_t Options;	/*!< Configuration options */
} HID_I2C_PORTCONFIG_T;

/** SPI_PORT_CFG_OPTIONS Port Configuration options
 * @{
 */
/** SPI Data Size is 8 Bits */
#define HID_SPI_CONFIG_OPTION_DATA_SIZE_8     (0x0007)
/** SPI Data Size is 16 Bits */
#define HID_SPI_CONFIG_OPTION_DATA_SIZE_16    (0x000F)
/** SPI Clock Default Polarity is Low */
#define HID_SPI_CONFIG_OPTION_POL_0			      ((uint32_t) 0 << 6)
/** SPI Clock Default Polarity is High */
#define HID_SPI_CONFIG_OPTION_POL_1			      ((uint32_t) 1 << 6)
/** SPI Data is captured on the first clock transition of the frame */
#define HID_SPI_CONFIG_OPTION_PHA_0			      ((uint32_t) 0 << 7)
/** SPI Data is captured on the second clock transition of the frame */
#define HID_SPI_CONFIG_OPTION_PHA_1			      ((uint32_t) 1 << 7)

/** @} */

/**
* @brief Port configuration information
*/
typedef struct __HIDSPI_PortConfig_t {
	uint32_t busSpeed;	/*!< SPI bus speed */
	uint32_t Options;	/*!< Configuration options */
} HID_SPI_PORTCONFIG_T;

/** I2C_IO_OPTIONS Options to I2C_DeviceWrite & I2C_DeviceRead routines
 * @{
 */
/** Generate start condition before transmitting */
#define HID_I2C_TRANSFER_OPTIONS_START_BIT      0x0001

/** Generate stop condition at the end of transfer */
#define HID_I2C_TRANSFER_OPTIONS_STOP_BIT       0x0002

/** Continue transmitting data in bulk without caring about Ack or nAck from device if this bit is
   not set. If this bit is set then stop transmitting the data in the buffer when the device nAcks*/
#define HID_I2C_TRANSFER_OPTIONS_BREAK_ON_NACK  0x0004

/** lpcusbsio-I2C generates an ACKs for every byte read. Some I2C slaves require the I2C
   master to generate a nACK for the last data byte read. Setting this bit enables working with such
   I2C slaves */
#define HID_I2C_TRANSFER_OPTIONS_NACK_LAST_BYTE 0x0008

/* Setting this bit would mean that the address field should be ignored.
    The address is either a part of the data or this is a special I2C
    frame that doesn't require an address. For example when transferring a
    frame greater than the USB_HID packet this option can be used. */
#define HID_I2C_TRANSFER_OPTIONS_NO_ADDRESS     0x00000040

/** @} */

/**
 * @brief	HID to I2C bridge read and write transfer parameters structure.
 *  Defines the structure of HID to I2C read-write transfer parameters.
 */
typedef struct __HIDI2C_RW_PARAMS {
	uint16_t length;		/*!< Length of the transfer.*/
	uint8_t options;	/*!< check @ref I2C_IO_OPTIONS. */
	uint8_t slaveAddr;	/*!< I2C slave device address. */
	uint8_t data[];		/*!< Data corresponding to the response */

} HID_I2C_RW_PARAMS_T;

/** I2C_FAST_XFER_OPTIONS I2C master faster transfer options
 * @{
 */

/** Ignore NACK during data transfer. By default transfer is aborted. */
#define I2C_FAST_XFER_OPTION_IGNORE_NACK     0x01
/** ACK last byte received. By default we NACK last byte we receive per I2C spec. */
#define I2C_FAST_XFER_OPTION_LAST_RX_ACK     0x02

/**
 * @}
 */

/**
 * @brief	HID to I2C bridge fast transfer parameters structure.
 *  Defines the parameters structure for HID_I2C_REQ_DEVICE_XFER command.
 */
typedef struct __HIDI2C_XFER_PARAMS {
	uint8_t txLength;	/*!< Length of the Tx transfer.*/
	uint8_t rxLength;	/*!< Length of the Rx transfer. */
	uint8_t options;	/*!< check @ref I2C_FAST_XFER_OPTIONS. */
	uint8_t slaveAddr;	/*!< I2C slave device address. */
	uint8_t data[];		/*!< Data corresponding to the response */

} HID_I2C_XFER_PARAMS_T;

/**
* @brief	HID to SPI bridge transfer parameters structure.
*  Defines the parameters structure for HID_SPI_REQ_DEVICE_XFER command.
*/
typedef struct __HIDSPI_XFER_PARAMS {
	uint8_t txLength;	/*!< Length of the Tx transfer.*/
	uint8_t rxLength;	/*!< Length of the Rx transfer. */
	uint8_t options;	/*!< SPI transfer options. */
	uint8_t device; 	/*!< SPI slave device number i.e. SSELn. */
	uint8_t data[];		/*!< Data corresponding to the response */

} HID_SPI_XFER_PARAMS_T;

/**
 * @}
 */

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif /* __LPCUSBSIO_I2C_H_ */






