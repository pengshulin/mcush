/****************************** BEGIN OF FILE *******************************/
/* NOTES:                                                                   */
/* THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT MANUALLY !!!      */
/* Last modified at Tue Aug 21 08:32:01 2018                                */
/****************************************************************************/
#ifndef __MCUSH_LIB_CRC_H__
#define __MCUSH_LIB_CRC_H__
/****************************************************************************/
/* crc libs, generated from python crcmod module                            */
/* crcmod module is designed by Raymond L. Buvel and Craig McQueen          */
/* MCUSH designed by Peng Shulin, all rights reserved.                      */
/****************************************************************************/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


uint8_t _crc8(const uint8_t *data, int len, uint8_t crc, const uint8_t *table);
uint16_t _crc16(const uint8_t *data, int len, uint16_t crc, const uint16_t *table);
uint32_t _crc32(const uint8_t *data, int len, uint32_t crc, const uint32_t *table);

extern const uint8_t crc8_table[256];
uint8_t crc8( uint8_t *buf, int len );
extern const uint16_t crc16_table[256];
uint16_t crc16( uint8_t *buf, int len );
extern const uint16_t crc16_xmodem_table[256];
uint16_t crc16_xmodem( uint8_t *buf, int len );
extern const uint16_t crc16_modbus_table[256];
uint16_t crc16_modbus( uint8_t *buf, int len );
extern const uint16_t crc16_kermit_table[256];
uint16_t crc16_kermit( uint8_t *buf, int len );
extern const uint32_t crc24_table[256];
uint32_t crc24( uint8_t *buf, int len );
extern const uint32_t crc32_table[256];
uint32_t crc32( uint8_t *buf, int len );

#ifdef __cplusplus
}
#endif


#endif
/******************************* END OF FILE ********************************/
