#!/usr/bin/env python
# coding: utf8
# test max7219 module
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *
from mcush.misc import Max7219, Font


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
BITMAPS = [Font.DEFAULT_FONT['0'], Font.DEFAULT_FONT['1']]




 
def main(argv=None):
    disp = Max7219.LED32x8(Mcush.Mcush())
    disp.init()
    counter = 1
    cv = Max7219.Canvas( disp )
    #disp.test_all()
    #time.sleep(1)
    while True:
        for img in BITMAPS:
            cv.drawBitmap( 0, 0, img )
            time.sleep(0.5)
        counter += 1


if __name__ == '__main__':
    main(sys.argv)

