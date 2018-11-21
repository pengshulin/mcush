# coding: utf8
__doc__ = 'ws2812 color led strap controller module'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush
from . import Bitmap
from . import Font



class LEDS():
    '''WS2812 controller'''
    
    def __init__( self, controller, length=None, swap_rg=None, pin=None ):
        if length is None:
            length = self.width * self.height
        self.length = length
        self.swap_rg = swap_rg
        self.controller = controller
        if pin is None:
            self.controller.writeCommand( "W -l%d -I"% length )
        else:
            self.controller.writeCommand( "W -l%d -I -p%s"% (length, pin) )

    MEM_BUF_LINE_LIMIT = 50
    def write( self, mem, offset=0 ):
        mem = mem[:self.length-offset]  # ignore the invalid portion
        if offset:
            membuf = 'W -o%d '% offset
        else:
            membuf = 'W '
        if self.swap_rg:
            membuf += '-g '
        count = 0
        while True:
            if len(mem) == 0:
                if count:
                    self.controller.writeCommand( membuf )
                break
            membuf += '%d '% (mem.pop(0) & 0xFFFFFF)
            count += 1
            if len(membuf) >= self.MEM_BUF_LINE_LIMIT:
                self.controller.writeCommand( membuf )
                offset += count
                count = 0
                membuf = 'W -o%d '% offset
                if self.swap_rg:
                    membuf += '-g '
        self.controller.writeCommand( "W -w" )



class LEDS8(LEDS):
    width, height = 8, 1

class LEDS16(LEDS):
    width, height = 16, 1

class LEDS32(LEDS):
    width, height = 32, 1

class LEDS64(LEDS):
    width, height = 64, 1

class LEDS128(LEDS):
    width, height = 128, 1

class LEDS256(LEDS):
    width, height = 256, 1


class LEDS8x8(LEDS):
    width, height = 8, 8

class LEDS16x8(LEDS):
    width, height = 16, 8

class LEDS24x8(LEDS):
    width, height = 24, 8

class LEDS32x8(LEDS):
    width, height = 32, 8
 
class LEDS8x16(LEDS):
    width, height = 8, 16

class LEDS16x16(LEDS):
    width, height = 16, 16

class LEDS24x16(LEDS):
    width, height = 24, 16

class LEDS32x16(LEDS):
    width, height = 32, 16

class LEDS8x24(LEDS):
    width, height = 8, 24

class LEDS16x24(LEDS):
    width, height = 16, 24

class LEDS24x24(LEDS):
    width, height = 24, 24

class LEDS32x24(LEDS):
    width, height = 32, 24

class LEDS8x32(LEDS):
    width, height = 8, 32

class LEDS16x32(LEDS):
    width, height = 16, 32

class LEDS24x32(LEDS):
    width, height = 24, 32

class LEDS32x32(LEDS):
    width, height = 32, 32




