#!/usr/bin/env python
# coding: utf8
# test adxl345 accelerator module
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *


def main(argv=None):
    s = Mcush.Mcush()
    s.i2c_init(0x53)
    s.i2c( [0x31, 0x0B] )  # DATA_FORMAT: full resolution, 16g range
    s.i2c( [0x2C, 0x08] )  # BW_RATE: 12.5Hz bandwidth, 25Hz output rate
    s.i2c( [0x2D, 0x08] )  # POWER_CTRL: Measure 
    s.i2c( [0x2E, 0x80] )  # INT_ENABLE: DATA_READY 
    s.i2c( [0x1E, 0x00] )  # OFSX: X-axis offset
    s.i2c( [0x1F, 0x00] )  # OFSY: Y-axis offset
    s.i2c( [0x20, 0x00] )  # OFSZ: Z-axis offset
    #print s.i2c( [0x0], 57 )  # print all registers
    #time.sleep(1)
    counter = 1
    while True:
        a,b,c,d,e,f = s.i2c( [0x32], 6 )
        x = Utils.s2h( chr(a) + chr(b) )
        y = Utils.s2h( chr(c) + chr(d) )
        z = Utils.s2h( chr(e) + chr(f) )
        #print '%6d %6d %6d'% (x, y, z)
        F_MG = 3.9
        print '[%d]'% counter, '%9.1f %9.1f %9.1f'% (x*F_MG, y*F_MG, z*F_MG)
        counter += 1


if __name__ == '__main__':
    main(sys.argv)

