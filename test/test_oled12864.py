#!/usr/bin/env python
# coding: utf8
# test oled12864 lcm with spi ports
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *


def main(argv=None):
    s = Mcush.Mcush()
    s.spi_init()
    RST = '0.4'
    DC = '0.5'
    s.gpio( RST, o=True, s=True )
    s.gpio( DC, o=True, s=True )
    
    def rst():
        s.gpio( RST, c=True )
        time.sleep(0.1)
        s.gpio( RST, s=True )
      
    def dat(d):
        s.gpio( DC, s=True )
        s.spi( [d] )

    def cmd(d):
        s.gpio( DC, c=True )
        s.spi( [d] )
    
    def fill(bmp):
        for y in range(8):
            cmd(0xb0+y)
            cmd(0x01)
            cmd(0x10)
            for x in range(128):
                dat(bmp)
                
    def pos(x,y):
        cmd(0xb0+(y>>3))
        cmd(((x&0xf0)>>4)|0x10)
        cmd((x&0x0f)|0x01)

    def pixel(x,y):
        data1 = 0x01<<(y%8);    
        cmd(0xb0+(y>>3));
        cmd(((x&0xf0)>>4)|0x10);
        cmd((x&0x0f)|0x00);
        dat(data1); 

    rst()
    cmd(0xae)   # turn off oled panel
    cmd(0x00)   # set low column address
    cmd(0x10)   # set high column address
    cmd(0x40)   # set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    cmd(0x81)   # set contrast control register
    cmd(0xcf)   # Set SEG Output Current Brightness
    cmd(0xa1)   # Set SEG/Column Mapping     0xa0ṪóÓÒṖṀÖÃ 0xa1Õýġ£
    cmd(0xc8)   # Set COM/Row Scan Direction   0xc0ÉÏÏÂṖṀÖÃ 0xc8Õýġ£
    cmd(0xa6)   # set normal display
    cmd(0xa8)   # set multiplex ratio(1 to 64)
    cmd(0x3f)   # 1/64 duty
    cmd(0xd3)   # set display offset Shift Mapping RAM Counter (0x00~0x3F)
    cmd(0x00)   # not offset
    cmd(0xd5)   # set display clock divide ratio/oscillator frequency
    cmd(0x80)   # set divide ratio, Set Clock as 100 Frames/Sec
    cmd(0xd9)   # set pre-charge period
    cmd(0xf1)   # Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    cmd(0xda)   # set com pins hardware configuration
    cmd(0x12)   # 
    cmd(0xdb)   # set vcomh
    cmd(0x40)   # Set VCOM Deselect Level
    cmd(0x20)   # Set Page Addressing Mode (0x00/0x01/0x02)
    cmd(0x02)   # 
    cmd(0x8d)   # set Charge Pump enable/disable
    cmd(0x14)   # set(0x10) disable
    cmd(0xa4)   # Disable Entire Display On (0xa4/0xa5)
    cmd(0xa6)   # Disable Inverse Display On (0xa6/a7) 
    cmd(0xaf)   # turn on oled panel
    #fill(0x00)
    #pos(0,0)
    #pixel(10, 10) 

    right=True
    v=0x80
    while True:
        #time.sleep(1)
        for y in range(8):
            cmd(0xb0+y)
            cmd(0x01)
            cmd(0x10)
            for x in range(128):
                #dat(random.randint(0,255))
                dat(v)
                if right:
                    v=v>>1
                    if v == 0:
                        v = 1
                        right = False
                else:
                    v=v<<1
                    if v == 0x100:
                        v = 0x80
                        right = True
        right = not right
        v = 0x80 if right else 1
 



if __name__ == '__main__':
    main(sys.argv)

