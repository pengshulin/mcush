# coding: utf8
__doc__ = 'max7219 matrix led controller module'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush
from . import Bitmap
from . import Font

ADDR_NOP = 0
ADDR_D0 = 1
ADDR_D1 = 2
ADDR_D2 = 3
ADDR_D3 = 4
ADDR_D4 = 5
ADDR_D5 = 6
ADDR_D6 = 7
ADDR_D7 = 8
ADDR_DECODE_MODE = 9
ADDR_INTENSITY = 10
ADDR_SCAN_LIMIT = 11
ADDR_SHUTDOWN = 12
ADDR_DISP_TEST = 15



class LED():

    def __init__( self, controller, sdi=None, sdo=None, sck=None, cs=None, intensity=None ):
        self.controller = controller
        self.controller.spi_init( sdi, sdo, sck, cs, width=16, delay=10 )
        self.controller_num = (int((self.width-1)/8)+1) * (int((self.height-1)/8)+1)
        if intensity is None:
            intensity = 0x08
        self.intensity = intensity & 0x0F  # range: 0 ~ 0x0F
        self.reset()

    def write(self, addr, dat):
        if isinstance(dat, list):
            # multiple controllers, different value
            val = [((addr & 0x0F)<<8) + (d & 0xFF) for d in dat]
            val.reverse()
            self.controller.spi( val )
        elif self.controller_num > 1 :
            # multiple controllers, same value
            val = ((addr & 0x0F)<<8) + (dat & 0xFF)
            self.controller.spi( [val] * self.controller_num )
        else:
            # single controller
            val = ((addr & 0x0F)<<8) + (dat & 0xFF)
            self.controller.spi( [val] )

    def reset(self):
        self.write( ADDR_SHUTDOWN, 1 )
        self.write( ADDR_SCAN_LIMIT, 7 )
        self.write( ADDR_DECODE_MODE, 0 )
        self.write( ADDR_INTENSITY, self.intensity )
        self.write( ADDR_D0, 0 )
        self.write( ADDR_D1, 0 )
        self.write( ADDR_D2, 0 )
        self.write( ADDR_D3, 0 )
        self.write( ADDR_D4, 0 )
        self.write( ADDR_D5, 0 )
        self.write( ADDR_D6, 0 )
        self.write( ADDR_D7, 0 )
        self.write( ADDR_DISP_TEST, 0 )

    def set_intensity( self, val ):
        self.write( ADDR_INTENSITY, val & 0x0F )
        self.intensity = val

    def test_all( self ):
        self.write( ADDR_D0, 0xFF )
        self.write( ADDR_D1, 0xFF )
        self.write( ADDR_D2, 0xFF )
        self.write( ADDR_D3, 0xFF )
        self.write( ADDR_D4, 0xFF )
        self.write( ADDR_D5, 0xFF )
        self.write( ADDR_D6, 0xFF )
        self.write( ADDR_D7, 0xFF )

    def write_line( self, index, line ):
        self.write( ADDR_D0 + 7 - index, line )

    

class LED8x8(LED):
    width, height = 8, 8

class LED16x8(LED):
    width, height = 16, 8

class LED24x8(LED):
    width, height = 24, 8

class LED32x8(LED):
    width, height = 32, 8
 
class LED8x16(LED):
    width, height = 8, 16

class LED16x16(LED):
    width, height = 16, 16

class LED24x16(LED):
    width, height = 24, 16

class LED32x16(LED):
    width, height = 32, 16

class LED8x24(LED):
    width, height = 8, 24

class LED16x24(LED):
    width, height = 16, 24

class LED24x24(LED):
    width, height = 24, 24

class LED32x24(LED):
    width, height = 32, 24

class LED8x32(LED):
    width, height = 8, 32

class LED16x32(LED):
    width, height = 16, 32

class LED24x32(LED):
    width, height = 24, 32

class LED32x32(LED):
    width, height = 32, 32



# abstract canvas based on matrix led controller
class Canvas():

    def __init__( self, display ):
        if not isinstance(display, LED):
            raise Exception("display type error")
        self.display = display
        self.width = display.width
        self.height = display.height
        self.controller_num = display.controller_num
        self.buffer = [ [0] * self.controller_num for i in range(8) ]
        self.dirty = 0xFF 

    def flush( self, force=False ):
        for i in range(8):
            if force or (self.dirty & (1<<i)):
                self.display.write_line( i, self.buffer[i] )
                self.dirty &= ~(1<<i)

    def clrScr( self, flush=True ):
        # clear screen
        for i in range(8):
            for j in range(self.controller_num):
                self.buffer[i][j] = 0 
        self.dirty = 0xFF
        if flush:
            self.flush()
 
    def setPixel( self, x, y, color=1, flush=True ):
        # check if it's out of range
        if x < 0 or x >= self.width:
            return
        if y < 0 or y >= self.height:
            return
        # transform postion to chain mode
        while y >= 8:
            y -= 8 
            x += self.width
        byte_mask = 1 << (x%8)
        byte_index = int(x/8)
        if color:
            self.buffer[y][byte_index] |= byte_mask
        else:
            self.buffer[y][byte_index] &= ~byte_mask
        self.dirty |= 1<<y
        if flush:
            self.flush()
 
    def drawVLine( self, x, y0, y1, color=1, flush=True ):
        # draw vertical line
        if y1 < y0:
            y0, y1 = y1, y0
        for y in range(y0, y1+1):
            self.setPixel( x, y, color, flush=False )
        if flush:
            self.flush()

    def drawHLine( self, y, x0, x1, color=1, flush=True ):
        # draw horizontal line
        if x1 < x0:
            x0, x1 = x1, x0
        for x in range(x0, x1+1):
            self.setPixel( x, y, color, flush=False )
        if flush:
            self.flush()
 
    def drawRectangle( self, x0, y0, x1, y1, color=1, fill=True, flush=True ):
        # draw rectangle 
        if x0 == x1:
            self.drawVLine( x0, y0, y1, color, flush=False )
        elif y0 == y1:
            self.drawHLine( y0, x0, x1, color, flush=False )
        else:
            self.drawHLine( y0, x0, x1, color, flush=False )
            self.drawHLine( y1, x0, x1, color, flush=False )
            if fill:
                for y in range(y0+1, y1):
                    self.drawHLine( y, x0, x1, color, flush=False )
            else:
                self.drawVLine( x0, y0, y1, color, flush=False )
                self.drawVLine( x1, y0, y1, color, flush=False )
        if flush:
            self.flush()
    
    def drawLine( self, x0, y0, x1, y1, color=1, flush=True ):
        # draw normal line
        if x1 < x0:
            x0, y0, x1, y1 = x1, y1, x0, y0
        if x0 == x1:
            self.drawVLine( x0, y0, y1, color, flush=False )
        elif y0 == y1:
            self.drawHLine( y0, x0, x1, color, flush=False )
        else:
            self.setPixel( x0, y0, color, flush=False )
            self.setPixel( x1, y1, color, flush=False )
            k = float(y1-y0)/(x1-x0)
            if abs(k) < 1.0:
                for x in range(x0+1, x1):
                    y = int(y0+(x-x0)*k+0.5)
                    self.setPixel( x, y, color, flush=False )
            else:
                if k > 0:
                    for y in range(y0+1, y1):
                        x = int(x0+(y-y0)/k+0.5)
                        self.setPixel( x, y, color, flush=False )
                else:
                    for y in range(y0-1, y1, -1):
                        x = int(x0+(y-y0)/k+0.5)
                        self.setPixel( x, y, color, flush=False )
        if flush:
            self.flush()
 
    def drawBitmap( self, x, y, bitmap, mode='normal', flush=True ):
        for i in range(bitmap.height):
            for j in range(bitmap.width):
                c = bitmap.getPixel(j, i)
                if mode == 'normal':
                    self.setPixel( x+j, y+i, int(c), flush=False )
                elif mode == 'transparent':
                    if c:
                        self.setPixel( x+j, x+i, flush=False )
        if flush: 
            self.flush() 

    def drawString( self, x, y, string, mode='normal', font=None, flush=True ):
        if font is None:
            font = Font.DEFAULT_FONT
        for s in string:
            if not s in font:
                s = None
            elif font[s] is None:
                s = '?'
            bitmap = font[s]
            self.drawBitmap( x, y, bitmap, mode, flush=False )
            x += bitmap.width
            if x >= self.width:
                break
        if flush: 
            self.flush() 

    def getStringRenderSize( self, string, font=None ):
        width = 0
        height = 0
        if font is None:
            font = Font.DEFAULT_FONT
        for s in string:
            if not s in font:
                s = None
            elif font[s] is None:
                s = '?'
            bitmap = font[s]
            w, h = bitmap.width, bitmap.height
            width += w
            if h > height:
                height = h
        return (width, height) 

