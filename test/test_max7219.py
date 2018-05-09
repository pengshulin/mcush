#!/usr/bin/env python
# coding: utf8
# test max7219 module
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *


ADDR_NOP = 0
ADDR_D0 = 1
ADDR_D1 = 2
ADDR_D2 = 3
ADDR_D3 = 4
ADDR_D4 = 5
ADDR_D5 = 6
ADDR_D6 = 7
ADDR_D7 = 8
ADDR_DECODE_MODE = 9
ADDR_INTENSITY = 10
ADDR_SCAN_LIMIT = 11
ADDR_SHUTDOWN = 12
ADDR_DISP_TEST = 15


class MAX7219(Mcush.Mcush):
    def init(self):
        self.spi_init(width=16, delay=100)

    def write(self, addr, dat):
        val = ((addr & 0x0F)<<8) + (dat & 0xFF)
        self.spi( [val] )

    def reset(self):
        self.write( ADDR_SHUTDOWN, 1 )
        self.write( ADDR_SCAN_LIMIT, 7 )
        self.write( ADDR_DECODE_MODE, 0 )
        self.write( ADDR_INTENSITY, 0x0A )
        self.write( ADDR_D0, 0x55 )
        self.write( ADDR_D1, 0xAA )
        self.write( ADDR_D2, 0x55 )
        self.write( ADDR_D3, 0xAA )
        self.write( ADDR_D4, 0x55 )
        self.write( ADDR_D5, 0xAA )
        self.write( ADDR_D6, 0x55 )
        self.write( ADDR_D7, 0xAA )
        self.write( ADDR_DISP_TEST, 0 )
        
 
def main(argv=None):
    s = MAX7219()
    s.init()
    s.reset()
    for i in range(16):
        s.write( ADDR_INTENSITY, i )
        time.sleep(0.2)
    counter = 1
    while True:
        s.write( ADDR_D0, counter & 0xFF )
        time.sleep(0.1)
        counter += 1


if __name__ == '__main__':
    main(sys.argv)

