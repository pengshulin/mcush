# coding: utf8
__doc__ = 'SSD1306 controlled OLED12864 display'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush
from . import Bitmap
from . import Font
from . import Canvas as _Canvas
import time


class Ssd1306_Common():
    current_page = None
    current_x = None

    def cmd( self, b ):
        raise NotImplementedError

    def dat( self, b ):
        raise NotImplementedError
    
    def fill( self, pattern ):
        # fill full screen with fixed pattern (initialize)
        for p in range((self.height-1)/8+1):
            self.cmd(0xb0+p)
            self.cmd(0x01)
            self.cmd(0x10)
            for x in range(self.width):
                self.dat(pattern)
    
    def write_mem( self, page, x, mem ):
        # write display buffer at specified page/x
        # optimize: do not re-send position if it's continuous
        if page != self.current_page:
            self.cmd(0xb0+page)
            self.current_page = page 

        if x != self.current_x:
            self.cmd(((x&0xf0)>>4)|0x10)
            self.cmd((x&0x0f)|0x00)
            self.current_x = x
           
        self.dat( mem )
        self.current_x += 1

    def init(self):
        # registers initialize, refer to datasheet
        self.cmd(0xae)   # turn off oled panel
        self.cmd(0x00)   # set low column address
        self.cmd(0x10)   # set high column address
        self.cmd(0x40)   # set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
        self.cmd(0x81)   # set contrast control register
        self.cmd(0xcf)   # Set SEG Output Current Brightness
        self.cmd(0xa1)   # Set SEG/Column Mapping
        self.cmd(0xc8)   # Set COM/Row Scan Direction
        self.cmd(0xa6)   # set normal display
        self.cmd(0xa8)   # set multiplex ratio(1 to 64)
        self.cmd(0x3f)   # 1/64 duty
        self.cmd(0xd3)   # set display offset Shift Mapping RAM Counter (0x00~0x3F)
        self.cmd(0x00)   # not offset
        self.cmd(0xd5)   # set display clock divide ratio/oscillator frequency
        self.cmd(0x80)   # set divide ratio, Set Clock as 100 Frames/Sec
        self.cmd(0xd9)   # set pre-charge period
        self.cmd(0xf1)   # Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
        self.cmd(0xda)   # set com pins hardware configuration
        self.cmd(0x12)   # 
        self.cmd(0xdb)   # set vcomh
        self.cmd(0x40)   # Set VCOM Deselect Level
        self.cmd(0x20)   # Set Page Addressing Mode (0x00/0x01/0x02)
        self.cmd(0x02)   # 
        self.cmd(0x8d)   # set Charge Pump enable/disable
        self.cmd(0x14)   # set(0x10) disable
        self.cmd(0xa4)   # Disable Entire Display On (0xa4/0xa5)
        self.cmd(0xa6)   # Disable Inverse Display On (0xa6/a7) 
        self.cmd(0xaf)   # turn on oled panel


        
class Ssd1306_I2C(Ssd1306_Common):

    def __init__( self, controller, scl=None, sda=None ):
        self.controller = controller
        self.controller.i2c_init( 0x3C, scl, sda, delay=5 )
        self.init()

    def cmd( self, b ):
        self.controller.i2c( [0x00, b] )
 
    def dat( self, b ):
        self.controller.i2c( [0x40, b] )
    
    def fill( self, pattern ):
        # optimized code for i2c command: multi data write
        for p in range((self.height-1)/8+1):
            self.cmd(0xb0+p)
            self.cmd(0x01)
            self.cmd(0x10)
            for x in range(self.width/8):
                self.controller.i2c( [
                    0xC0, pattern, 0xC0, pattern, 0xC0, pattern, 0xC0, pattern, 
                    0xC0, pattern, 0xC0, pattern, 0xC0, pattern, 0xC0, pattern, 
                    ] )
 

class Ssd1306_SPI(Ssd1306_Common):

    def __init__( self, controller, rst=None, dc=None, sdo=None, sdi=None, sck=None, cs=None ):
        self.controller = controller
        self.controller.spi_init( sdo=sdo, sdi=sdi, sck=sck, cs=cs, delay=5 )
        self.rst_pin = rst
        self.dc_pin = dc
        self.dc = None
        self.controller.pinOutputHigh( [rst, dc] )
        self.reset()
        self.init()

    def reset( self ):
        self.controller.pinClr( self.rst_pin )
        time.sleep(0.1)
        self.controller.pinSet( self.rst_pin )
     
    def cmd( self, d ):
        if self.dc != 0:
            self.controller.pinClr( self.dc_pin )
            self.dc = 0
        self.controller.spi( [d] )
 
    def dat( self, d ):
        if self.dc != 1:
            self.controller.pinSet( self.dc_pin )
            self.dc = 1
        self.controller.spi( [d] )
    
   

class OLED128x64_I2C(Ssd1306_I2C):
    width, height = 128, 64


class OLED128x64_SPI(Ssd1306_SPI):
    width, height = 128, 64



# abstract canvas based on matrix led controller
class Canvas(_Canvas.Canvas):

    def __init__( self, display ):
        self.display = display
        self.width = display.width
        self.height = display.height
        self.pages = (display.height-1)/8+1
        self.buffer = [ [0] * self.width for i in range(self.pages) ]
        self.display.fill( 0 )
        self.dirty = [0] * self.pages

    def flush( self, force=False ):
        for p in range(self.pages):
            for x in range(self.width):
                if force or (self.dirty[p] & (1<<x)):
                    self.display.write_mem( p, x, self.buffer[p][x] )
                    self.dirty[p] &= ~(1<<x);

    def setPixel( self, x, y, color=None, flush=True ):
        # check if it's out of range
        if x < 0 or x >= self.width:
            return
        if y < 0 or y >= self.height:
            return
        page = y/8
        byte_mask = 1 << (y%8)
        byte_index = x
        if color is None:
            color = 1
        if color:
            self.buffer[page][byte_index] |= byte_mask
        else:
            self.buffer[page][byte_index] &= ~byte_mask
        self.dirty[page] |= 1<<x
        if flush:
            self.flush()
    

