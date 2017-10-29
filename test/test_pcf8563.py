#!/usr/bin/env python
# coding: utf8
# test pcf8563 rtc module
# Peng Shulin <trees_peng@163.com>
import sys
import time
from mcush import *

PCF8563T_SYS1  = 0x00  # status reg1
PCF8563T_SYS2  = 0x01  # status reg2
PCF8563T_SEC   = 0x02  # second
PCF8563T_MIN   = 0x03  # minute
PCF8563T_HOUR  = 0x04  # hour
PCF8563T_DAY   = 0x05  # day
PCF8563T_WEEK  = 0x06  # week
PCF8563T_MONTH = 0x07  # month
PCF8563T_YEAR  = 0x08  # year
PCF8563T_BJ1   = 0x09  # alarm: minute
PCF8563T_BJ2   = 0x0A  # alarm: hour
PCF8563T_BJ3   = 0x0B  # alarm: day
PCF8563T_BJ4   = 0x0C  # alarm: week
PCF8563T_CLK   = 0x0D  # test
PCF8563T_CT    = 0x0E  # timer mode
PCF8563T_COUNT = 0x0F  # timer counter

WEEK={
0 : 'MON', 1 : 'TUE', 2 : 'WED', 3 : 'THU',
4 : 'FRI', 5 : 'SAT', 6 : 'SUN', }

def fromBCD(val):
    a = (val & 0xF0)>>4
    b = val & 0x0F
    return a*10+b
    
def toBCD(val):
    a = val / 10
    b = val % 10
    return a*16+b
 
def main(argv=None):
    s = Mcush.Mcush()
    s.i2c_init(0x51, sda='0.0', scl='0.1')
    print s.i2c( [0x0], 16 )  # print all registers
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
    sync()  # sync time
    counter = 1
    while True:
        a,b,c,d,e,f,g = s.i2c( [0x2], 7 )
        print '[%d]'% counter, '%X-%X-%X %s %02X:%02X:%02X'% (g,f,d,WEEK[e],c,b,a)
        counter += 1

if __name__ == '__main__':
    main(sys.argv)
    