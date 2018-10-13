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
    m = Motion.Adxl345(Mcush.Mcush())
    counter = 1
    while True:
        ax = m.getAccelX()
        ay = m.getAccelY()
        az = m.getAccelZ()
        print '[%d]'% counter, '%9.1f %9.1f %9.1f'% (ax, ay, az)
        counter += 1


if __name__ == '__main__':
    main(sys.argv)

