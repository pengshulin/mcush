#!/usr/bin/env python
# coding: utf8
# test DXW90 Micro Servo
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import math
import random
from mcush import *

    
RANGE = 10000  # pwm value 0 ~ 10000
FREQ = 50  # Hz
#VMIN, VMAX = 500, 1000  # 10000*1ms/20ms, 10000*2ms/20ms
VMIN, VMAX = 250, 1250  # 10000*0.5ms/20ms, 10000*2.5ms/20ms
DV = (VMAX-VMIN)/180.0  # per degree
 
def main(argv=None):
    s = Mcush.Mcush()
    s.pwm_init(freq=FREQ, range_value=RANGE, init_value=0)
    while True:
        for degree in [0, 30, 60, 90, 120, 150, 180, 150, 120, 90, 60, 30]:
        #for degree in [0, 90, 180, 90]:
            val = VMIN+int(DV*degree)
            s.pwm( index=0, value=val )
            print 'degree %d, value %d'% (degree, val)
            time.sleep(1)




if __name__ == '__main__':
    main(sys.argv)

