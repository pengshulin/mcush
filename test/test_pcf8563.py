#!/usr/bin/env python
# coding: utf8
# test pcf8563 rtc module
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *


PCF8563T_SYS1  = 0x00  # PCF8563状态寄存器地址1
PCF8563T_SYS2  = 0x01  # PCF8563状态寄存器地址2
PCF8563T_SEC   = 0x02  # 秒
PCF8563T_MIN   = 0x03  # 分
PCF8563T_HOUR  = 0x04  # 时
PCF8563T_DAY   = 0x05  # 日
PCF8563T_WEEK  = 0x06  # 星期
PCF8563T_MONTH = 0x07  # 月
PCF8563T_YEAR  = 0x08  # 年
PCF8563T_BJ1   = 0x09  # 分钟报警
PCF8563T_BJ2   = 0x0A  # 小时报警
PCF8563T_BJ3   = 0x0B  # 日报警
PCF8563T_BJ4   = 0x0C  # 星期报警
PCF8563T_CLK   = 0x0D  # 时钟测试
PCF8563T_CT    = 0x0E  # 定时器工作方式
PCF8563T_COUNT = 0x0F  # 定时器工作方式

WEEK={
0:'MON',
1:'TUE',
2:'WED',
3:'THU',
4:'FRI',
5:'SAT',
6:'SUN', 
}

def fromBCD(val):
    a=(val&0xF0)>>4
    b=val&0x0F
    return a*10+b
    
def toBCD(val):
    a=val/10
    b=val%10
    return a*16+b
 
def main(argv=None):
    s = Mcush.Mcush()
    s.i2c_init(0x51, sda='0.0', scl='0.1')
    print s.i2c( [0x0], 16 )  # 打印全体寄存器列表
    #time.sleep(1)
    def sync():
        tm = time.localtime()
        year, mon, mday, hour, minute, sec, wday, yday, isdst = tm
        s.i2c( [PCF8563T_YEAR, toBCD(year%100)] )
        s.i2c( [PCF8563T_MONTH, toBCD(mon)] )
        s.i2c( [PCF8563T_DAY, toBCD(mday)] )
        s.i2c( [PCF8563T_WEEK, wday] )
        s.i2c( [PCF8563T_HOUR, toBCD(hour)] )
        s.i2c( [PCF8563T_MIN, toBCD(minute)] )
        s.i2c( [PCF8563T_SEC, toBCD(sec)] )

    sync()  # 同步时间
    counter = 1
    while True:
        a,b,c,d,e,f,g = s.i2c( [0x2], 7 )
        print '[%d]'% counter, '%X-%X-%X %s %02X:%02X:%02X'% (g,f,d,WEEK[e],c,b,a)
        counter += 1




if __name__ == '__main__':
    main(sys.argv)

