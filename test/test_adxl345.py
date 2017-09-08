#!/usr/bin/env python
# coding: utf8
# test adxl345 accelerator module
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
import Env
import Mcush
import Utils


def main(argv=None):
    s = Mcush.Mcush()
    s.i2c_init(0x53)
    s.i2c( [0x31, 0x0B] )  # 13位，16g
    s.i2c( [0x2C, 0x08] )  # 速率设定为12.5
    s.i2c( [0x2D, 0x08] )  # 选择电源模式
    s.i2c( [0x2E, 0x80] )  # 使能DATA_READY中断
    s.i2c( [0x1E, 0x00] )  # X 偏移量
    s.i2c( [0x1F, 0x00] )  # Y 偏移量
    s.i2c( [0x20, 0x00] )  # Z 偏移量
    #print s.i2c( [0x0], 57 )  # 打印全体寄存器列表
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

