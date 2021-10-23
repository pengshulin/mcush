# coding: utf8
__doc__ = 'max7219 matrix led controller module'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush
from . import Bitmap
from . import Font
from . import Canvas as _Canvas
from . import SegmentTable

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

class LED64x8(LED):
    width, height = 64, 8

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
class Canvas(_Canvas.Canvas):

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

    def setPixel( self, x, y, color=None, flush=True ):
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
        if color is None:
            color = 1
        if color:
            self.buffer[y][byte_index] |= byte_mask
        else:
            self.buffer[y][byte_index] &= ~byte_mask
        self.dirty |= 1<<y
        if flush:
            self.flush()
    
    def clrScr( self, flush=True ):
        # optimized api
        for i in range(8):
            for j in range(self.controller_num):
                self.buffer[i][j] = 0 
        self.dirty = 0xFF
        if flush:
            self.flush()


#  
#      a
#    -----
#  f|     | b
#   |  g  |
#    ----- 
#  e|     | c
#   |     |
#    ----- .h
#      d
# D7 D6 D5 D4 D3 D2 D1 D0
# DP A  B  C  D  E  F  G 
SA = 0x40
SB = 0x20
SC = 0x10
SD = 0x08
SE = 0x04
SF = 0x02
SG = 0x01
SH = 0x80

SEGMENT_TABLE_HEX = SegmentTable.generateSegmentTableHEX(SA,SB,SC,SD,SE,SF,SG,SH)
SEGMENT_TABLE_ASCII = SegmentTable.generateSegmentTableASCII(SA,SB,SC,SD,SE,SF,SG,SH)

class LED8(LED):
    DIGITS = 8
       
    def __init__( self, controller, sdi=None, sdo=None, sck=None, cs=None, intensity=None ):
        self.controller = controller
        self.controller.spi_init( sdi, sdo, sck, cs, width=16, delay=10 )
        self.controller_num = int((self.DIGITS-1)/8)+1
        if intensity is None:
            intensity = 0x08
        self.intensity = intensity & 0x0F  # range: 0 ~ 0x0F
        self.reset()

    def clear( self ):
        self.display_segs( 0, "    ", ascii_mode=True )

    def display_string( self, string, index=0 ):
        segments = []
        for c in string:
            if not c in ['.', ':']:
                segments.append( SEGMENT_TABLE_ASCII[ord(c)] )
            elif len(segments) > 0:
                segments[-1] |= SegmentTable.SH
        for i, s in enumerate(segments[:self.DIGITS], index):
            self.write_line( i, s )

    def display_integer( self, val, hex_mode=False ):
        if hex_mode:
            string = (('%%%dX'% self.DIGITS)% val)[-self.DIGITS:]
        else:
            string = (('%%%dd'% self.DIGITS)% val)[-self.DIGITS:]
        self.display_segs( 0, string[:self.DIGITS], ascii_mode=True )
    
    def display_segment( self, index, segment, ascii_mode=None, hex_mode=None ):
        if hex_mode:
            segment = SEGMENT_TABLE_HEX[segment]
        elif ascii_mode:
            segment = SEGMENT_TABLE_ASCII[ord(segment)]
        self.segments[index] = segment
        self.write_line( index, segment )

    def display_segments( self, index, segments, ascii_mode=None, hex_mode=None ):
        segments = segments[:self.DIGITS]
        if hex_mode:
            segments = [ SEGMENT_TABLE_HEX[c] for c in segments ]
        elif ascii_mode:
            segments = [ SEGMENT_TABLE_ASCII[ord(c)] for c in segments ]
        self.segments = segments
        for i,s in enumerate(segments,index):
            self.write_line( i, s )

    clrscr = clear
    display_str = display_string
    display_int = display_integer
    display_seg = display_segment
    display_segs = display_segments



