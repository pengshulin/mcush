#!/usr/bin/env python
# coding: utf8
# test mlx90614 infra red thermometer
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *


MLX90614_I2CADDR = 0x5A
# RAM
MLX90614_RAWIR1  = 0x04
MLX90614_RAWIR2  = 0x05
MLX90614_TA      = 0x06
MLX90614_TOBJ1   = 0x07
MLX90614_TOBJ2   = 0x08
# EEPROM
MLX90614_TOMAX   = 0x00
MLX90614_TOMIN   = 0x01
MLX90614_PWMCTRL = 0x02
MLX90614_TARANGE = 0x03
MLX90614_EMISS   = 0x04
MLX90614_CONFIG  = 0x05
MLX90614_ADDR    = 0x0E
MLX90614_ID1     = 0x1C
MLX90614_ID2     = 0x1D
MLX90614_ID3     = 0x1E
MLX90614_ID4     = 0x1F


def main(argv=None):
    s = Mcush.Mcush( check_idn=False )
    s.i2c_init(MLX90614_I2CADDR, delay=5)
    dp = Max7219.LED32x8(s, sdi='0.4',sdo='0.5', sck='0.6', cs='0.7')
    cv = Max7219.Canvas(dp)

    def readRam( addr ):
        datl, dath, pec = s.i2c( [addr & 0x1F], 3 )
        return datl + (dath<<8)
 
    def readRamInt16( addr ):
        datl, dath, pec = s.i2c( [addr & 0x1F], 3 )
        if Env.PYTHON_V3:
            return Utils.s2h( bytes([datl, dath]) )
        else:
            return Utils.s2h( chr(datl) + chr(dath) )
        
    def readE2prom( addr ):
        datl, dath, pec = s.i2c( [(addr & 0x1F) | 0x20], 3 )
        return datl + (dath<<8)

    def writeE2prom( addr, dat ):
        # TODO: split into datl, dath
        s.i2c( [(addr & 0x1F) | 0x20, dat[0], dat[1]] )

    def readFlags():
        # TODO: parse read flags
        datl, dath, pec = s.i2c( [0xF0], 3 )
        return datl + (dath<<8)

    def enterSleep( ):
        s.i2c( [0xFF] )
       
    def readTemp( addr ):
        return readRamInt16( addr ) * 0.02 - 273.15

    # 检查ID
    id1 = readE2prom( MLX90614_ID1 )
    id2 = readE2prom( MLX90614_ID2 )
    id3 = readE2prom( MLX90614_ID3 )
    id4 = readE2prom( MLX90614_ID4 )
    print( 'ID: %04X-%04X-%-04X-%04X'% (id1, id2, id3, id4) )
    print( 'FLAG: 0x%04X'% readFlags() )
    #print s.i2c( [0x7], 3 )
    time.sleep(1)
    counter = 1
    while True:
        # 读温度值
        ta = readTemp( MLX90614_TA )
        tobj1 = readTemp( MLX90614_TOBJ1 )
        print( '[%d] %9.1f %9.1f'% (counter, ta, tobj1) )
        cv.clrScr( flush=False )
        cv.drawString( 0, 0, '%.1f'% tobj1 )
        counter += 1
        #time.sleep(0.1)


if __name__ == '__main__':
    main(sys.argv)
