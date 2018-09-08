#!/usr/bin/env python
# coding: utf8
# test mpu6050 module
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *


def main(argv=None):
    s = Mcush.Mcush()
    s.i2c_init(0x68, scl='1.6', sda='1.7', delay=50 )
    #s.i2c_init(0x68, scl='1.10', sda='1.11' )
    #s.i2c_init(0x68)
    s.i2c( [0x6B, 0x00] )  # 解除休眠状态
    s.i2c( [0x19, 0x07] )  # 陀螺仪采样率，典型值：0x07(125Hz)
    s.i2c( [0x1A, 0x06] )  # 低通滤波频率，典型值：0x06(5Hz)
    s.i2c( [0x1B, 0x18] )  # 陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
    s.i2c( [0x1C, 0x01] )  # 加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
    print s.i2c( [0x0], 118 )  # 打印全体寄存器列表
    #time.sleep(1)
    counter = 1
    while True:
        a,b = s.i2c( [0x41], 2 )
        #temp = 35 + (Utils.s2h( chr(b) + chr(a) ) + 521)/340.0
        #print temp
        temp = Utils.s2h( chr(b) + chr(a) ) 
        a,b = s.i2c( [0x3B], 2 )
        ax = Utils.s2h( chr(b) + chr(a) )
        a,b = s.i2c( [0x3D], 2 )
        ay = Utils.s2h( chr(b) + chr(a) )
        a,b = s.i2c( [0x3F], 2 )
        az = Utils.s2h( chr(b) + chr(a) )
        a,b = s.i2c( [0x43], 2 )
        gx = Utils.s2h( chr(b) + chr(a) )
        a,b = s.i2c( [0x45], 2 )
        gy = Utils.s2h( chr(b) + chr(a) )
        a,b = s.i2c( [0x47], 2 )
        gz = Utils.s2h( chr(b) + chr(a) )
        print '[%d]'% counter, '%.1f     '%temp, '%6d %6d %6d    %6d %6d %6d'% (ax, ay, az, gx, gy, gz)
        F_MG = 1
        #print '[%d]'% counter, '%9.1f %9.1f %9.1f'% (ax*F_MG, ay*F_MG, az*F_MG)
        counter += 1




if __name__ == '__main__':
    main(sys.argv)

