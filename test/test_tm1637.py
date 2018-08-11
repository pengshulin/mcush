#!/usr/bin/env python
# coding: utf8
# test tm1637 module
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *
from SegmentTable import *


BRIGHTNESS = 7

CMD_DATA = 0x40
CMD_DATA_READ_KEY = 0x42
CMD_DATA_FIXED_ADDR = 0x44
CMD_DATA_TEST_MODE = 0x48
CMD_CONTROL = 0x80
CMD_ADDRESS = 0xC0


 
def main(argv=None):
    s = Mcush.Mcush()

    def display_seg( index, segment, ascii_mode=None, hex_mode=None ):
        s.i2c( addr=CMD_DATA+index )
        if hex_mode:
            segment = SEGMENT_TABLE_HEX[segment]
        elif ascii_mode:
            segment = SEGMENT_TABLE_ASCII[ord(segment)]
        s.i2c( [segment], addr=CMD_ADDRESS+index )
        s.i2c( addr=CMD_CONTROL+0x08+BRIGHTNESS ) 

    def display_segs( segments, ascii_mode=None, hex_mode=None ):
        s.i2c( addr=CMD_DATA )
        if hex_mode:
            segments = [ SEGMENT_TABLE_HEX[c] for c in segments ]
        elif ascii_mode:
            segments = [ SEGMENT_TABLE_ASCII[ord(c)] for c in segments ]
        s.i2c( segments, addr=CMD_ADDRESS )
        s.i2c( addr=CMD_CONTROL+0x08+BRIGHTNESS ) 


    s.i2c_init( 0, lsb=True )
    counter = 0
    while True:
        print counter
        disp = ('%4X'% counter)[-4:]
        display_segs( disp, ascii_mode=True )
        counter += 1
        #time.sleep(0.1)
     
	


if __name__ == '__main__':
    main(sys.argv)

