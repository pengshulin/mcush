#!/usr/bin/env python
# coding: utf8
# test step motor
# Peng Shulin <trees_peng@163.com>
import sys
import time
from mcush import *

STEP = 1600
ENABLE = '0.0'
DIR = '0.1'
PULSE = '0.2'

def main(argv=None):
    s = Mcush.Mcush( check_idn=False )
    s.pinOutputLow( [ENABLE, DIR, PULSE] )
    s.pulseInit( pin=PULSE, delay_us=50 )

    direction = True

    try:
        s.pinClr( ENABLE )
        while True:
            # prepare direction pin
            print( "Direction: %d"% (int(direction)) )
            if direction:
                s.pinSet( DIR )
            else:
                s.pinClr( DIR )

            # rotate for one cycle
            s.pulse( STEP )
            time.sleep(1)

            direction = not direction

    finally:
        s.pinSet( ENABLE )


if __name__ == '__main__':
    main(sys.argv)
    
