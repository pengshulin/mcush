#!/usr/bin/env python
# coding: utf8
# test mpu6050 module
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *
from mcush.misc import Motion


def main(argv=None):
    m = Motion.Mpu6050(Mcush.Mcush(),scl='1.6', sda='1.7')
    m.printAllReg()
    counter = 1
    while True:
        #print( 'WHO_AM_I', hex(m.readByName('WHO_AM_I')) )
        #m.printAllReg()
        temp = m.getTemperature()
        ax = m.getAccelX()
        ay = m.getAccelY()
        az = m.getAccelZ()
        gx = m.getGyroX()
        gy = m.getGyroY()
        gz = m.getGyroZ()
        print 'T: %.1f  AX: %.3f  AY: %.3f  AZ: %.3f  GX: %.3f  GY: %.3f  GZ: %.3f'% (temp, ax, ay, az, gx, gy, gz)
        counter += 1


if __name__ == '__main__':
    main(sys.argv)

