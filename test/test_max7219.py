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


def B2I( bitmap, reverse ):
    assert len(bitmap) == 8
    ret = 0
    for i in range(8):
        if bitmap[i] == '*':
            ret |= 1<<(7-i)
    return (ret ^ 0xFF) if reverse else ret


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

    def draw_bitmap( self, bitmap, reverse=False ):
        assert len(bitmap) == 8
        self.write( ADDR_D0, B2I(bitmap[0], reverse))
        self.write( ADDR_D1, B2I(bitmap[1], reverse))
        self.write( ADDR_D2, B2I(bitmap[2], reverse))
        self.write( ADDR_D3, B2I(bitmap[3], reverse))
        self.write( ADDR_D4, B2I(bitmap[4], reverse))
        self.write( ADDR_D5, B2I(bitmap[5], reverse))
        self.write( ADDR_D6, B2I(bitmap[6], reverse))
        self.write( ADDR_D7, B2I(bitmap[7], reverse))
 

EMPTY = [
    '--------',
    '--------',
    '--------',
    '--------',
    '--------',
    '--------',
    '--------',
    '--------', ]


HEART = [
    '-**--**-',
    '*--**--*',
    '*------*',
    '*------*',
    '-*----*-',
    '-*----*-',
    '--*--*--',
    '---**---', ]

HEART2 = [
    '-**--**-',
    '********',
    '********',
    '********',
    '-******-',
    '-******-',
    '--****--',
    '---**---', ]


FAN1 = [
    '----*---',
    '----*---',
    '----*---',
    '*****---',
    '---*****',
    '---*----',
    '---*----',
    '---*----', ]

FAN2 = [
    '--------',
    '-*----*-',
    '--*--*--',
    '---**---',
    '---**---',
    '--*--*--',
    '-*----*-',
    '--------', ]

DIGIT_0 = [
    '--------',
    '--****--',
    '-**--**-',
    '-**--**-',
    '-**--**-',
    '-**--**-',
    '-**--**-',
    '--****--', ]

DIGIT_1 = [
    '--------',
    '---**---',
    '--***---',
    '---**---',
    '---**---',
    '---**---',
    '---**---',
    '--****--', ]

DIGIT_2 = [
    '--------',
    '--****--',
    '-******-',
    '-**--**-',
    '----**--',
    '---**---',
    '-******-',
    '-******-', ]


BITMAPS = [FAN1, FAN2]
BITMAPS = [DIGIT_0, DIGIT_1, DIGIT_2]
BITMAPS = [HEART, HEART2]




 
def main(argv=None):
    s = MAX7219()
    s.init()
    s.reset()
    s.write( ADDR_INTENSITY, 2 )
    counter = 1
    while True:
        for img in BITMAPS:
            s.draw_bitmap( img )
            time.sleep(0.5)
        counter += 1


if __name__ == '__main__':
    main(sys.argv)

