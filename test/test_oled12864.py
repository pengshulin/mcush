#!/usr/bin/env python
# coding: utf8
# test oled12864 lcm with spi/i2c ports
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *


def main(argv=None):
    s = Mcush.Mcush(check_idn=False)
    oled = Ssd1306.OLED128x64_SPI(s, rst='0.4', dc='0.5')
    #oled = Ssd1306.OLED128x64_I2C(s)
    cv = Ssd1306.Canvas(oled)

    BITMAPS = [Bitmap.HEART, Bitmap.HEART2]

    x, right = 0, True
            
    cv.drawRectangle( 0, 0, cv.width-1, cv.height-1, color=1, fill=False )

    line1 = 'Hardware control'
    line2 = 'with Python'
    w1, h = cv.getStringRenderSize(line1)
    w2, h = cv.getStringRenderSize(line2)
    cv.drawString( (cv.width-w1)/2, (cv.height-h)/2-2, line1 )
    cv.drawString( (cv.width-w2)/2, (cv.height+h)/2+2, line2 )

    while True:
        #for img in BITMAPS:
        #    cv.drawBitmap( 0, 0, img )
        #    time.sleep(0.5)

        for i in range(7):
            #cv.clrScr( flush=False )
            x, y = 10, 10
            cv.drawRectangle( x, y, x+7, y+7, color=0, fill=True, flush=False )
            cv.drawLine( x+i, y, x+7-i, y+7, flush=False )
            cv.drawLine( x+i+1, y, x+7-i-1, y+7, flush=False )
            cv.drawLine( x+7, y+i, x+0, y+7-i, flush=False )
            cv.drawLine( x+7, y+i+1, x+0, y+7-i-1, flush=False )
            cv.flush()
            #time.sleep(0.1)

        #for img in BITMAPS:
        #    cv.clrScr( flush=False )
        #    cv.drawBitmap( x, 0, img )
        #    time.sleep(0.5)
        #    # check for hitting wall
        #    if right:
        #        x += 1
        #        if x + img.width > cv.width:
        #            x = cv.width - img.width
        #            right = False
        #    else:
        #        x -= 1
        #        if x < 0:
        #            x = 0
        #            right = True




if __name__ == '__main__':
    main(sys.argv)

