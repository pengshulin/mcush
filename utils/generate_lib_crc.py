#!/usr/bin/env python
# generate ../mcush/mcush_lib_crc.c(.h) by crcmod module
# crcmod is a Python module for gererating objects that compute the Cyclic
# Redundancy Check.  Any 8, 16, 24, 32, or 64 bit polynomial can be used. 
# crcmod module is designed by Raymond L. Buvel and Craig McQueen
# thanks for their great work
# MCUSH designed by Peng Shulin, all rights reserved.
import os
import FileGenerator
import crcmod.predefined

OUTPUT_C_SRC = '../mcush/mcush_lib_crc.c'
OUTPUT_C_HEAD = '../mcush/mcush_lib_crc.h'

COMMON_C_SRC = '''\
#include <stdint.h>

uint8_t _crc8( const uint8_t *data, int len, uint8_t crc, const uint8_t *table )
{
    while (len > 0)
    {
        crc = table[*data ^ (uint8_t)crc];
        data++;
        len--;
    }
    return crc;
}

uint16_t _crc16( const uint8_t *data, int len, uint16_t crc, const uint16_t *table )
{
    while (len > 0)
    {
        crc = table[*data ^ (uint8_t)crc] ^ (crc >> 8);
        data++;
        len--;
    }
    return crc;
}

uint32_t _crc32( const uint8_t *data, int len, uint32_t crc, const uint32_t *table )
{
    crc = crc ^ 0xFFFFFFFFu;
    while (len > 0)
    {
        crc = table[*data ^ (uint8_t)crc] ^ (crc >> 8);
        data++;
        len--;
    }
    crc = crc ^ 0xFFFFFFFFU;
    return crc;
}
'''

COMMON_C_HEAD = '''\
#include <stdint.h>

uint8_t _crc8(const uint8_t *data, int len, uint8_t crc, const uint8_t *table);
uint16_t _crc16(const uint8_t *data, int len, uint16_t crc, const uint16_t *table);
uint32_t _crc32(const uint8_t *data, int len, uint32_t crc, const uint32_t *table);
'''



NOTES = '''\
crc libs, generated from python crcmod module
crcmod module is designed by Raymond L. Buvel and Craig McQueen
MCUSH designed by Peng Shulin, all rights reserved.'''

C = FileGenerator.CGenerator( OUTPUT_C_SRC, os.path.basename(OUTPUT_C_SRC) )
C.writeSeperator()
C.writeComment( NOTES )
C.writeSeperator()
C.writeLine( COMMON_C_SRC )

H = FileGenerator.HGenerator( OUTPUT_C_HEAD, os.path.basename(OUTPUT_C_HEAD) )
H.writeSeperator()
H.writeComment( NOTES )
H.writeSeperator()
H.writeLine( COMMON_C_HEAD )

INT_CTYPE = ['uint8_t', 'uint16_t', 'uint32_t', 'uint32_t']
INT_WIDTH = [8, 16, 32, 32]
INT_FORMAT = ['0x%02Xu', '0x%04Xu', '0x%08Xu', '0x%08Xu']

CRC_DEF = [
# mod_name, export_name
('crc-8',  'crc8'),
('crc-16', 'crc16'),
('xmodem', 'crc16_xmodem'),
('modbus', 'crc16_modbus'),
('kermit', 'crc16_kermit'),
('crc-24', 'crc24'),
('crc-32', 'crc32'),
]


for crc_type, export_name in CRC_DEF:
    print crc_type
    c = crcmod.predefined.PredefinedCrc(crc_type)
    #crc_name = crc_type.lower().replace('-','_')
    crc_name = export_name
    crc_ctype = INT_CTYPE[c.digest_size-1]
    crc_width = INT_WIDTH[c.digest_size-1]
    crc_format = INT_FORMAT[c.digest_size-1]
    crc_init = c.initCrc
    crc_poly = c.poly
    crc_reverse = c.reverse
    func_declare = '%s %s( uint8_t *buf, int len )'% (crc_ctype, crc_name)
    C.writeLine( '\n// poly=0x%X, init=0x%X, reverse=%d'% (crc_poly, crc_init, int(crc_reverse)) )
    C.writeLine( '%s\n{'% func_declare ); 
    C.writeLine( '    static const %s table[256] = {'% (crc_ctype) )
    line = ''
    for i in c.table:
        s = (crc_format% i) + ','
        if len(line) + len(s) + 8 > 79:
            C.writeLine( '        '+line )
            line = s
        else:
            line += s
    C.writeLine( '        '+line+'};\n' )
    C.writeLine( '    return _crc%d( buf, len, 0x%X, table );'% (crc_width, crc_init) )
    C.writeLine( '}' ); 
    H.writeLine( '%s;'% func_declare ); 

C.writeLine()
H.writeLine()
C.close()
H.close()
