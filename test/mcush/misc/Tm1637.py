# coding: utf8
__doc__ = 'tm1637 led controller module'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush
from . import SegmentTable


BRIGHTNESS = 7

CMD_DATA = 0x40
CMD_DATA_READ_KEY = 0x42
CMD_DATA_FIXED_ADDR = 0x44
CMD_DATA_TEST_MODE = 0x48
CMD_CONTROL = 0x80
CMD_ADDRESS = 0xC0


class LED:
    DIGITS = 1
    
    def __init__( self, controller, scl=None, sda=None, brightness=BRIGHTNESS ):
        self.controller = controller
        self.segments = [0]*self.DIGITS
        self.controller.i2c_init( 0, scl=scl, sda=sda, lsb=True )
        self.brightness = brightness
        
    def clear( self ):
        self.display_segs( 0, " "*self.DIGITS, ascii_mode=True )

    def display_string( self, string, index=0 ):
        segments = []
        for c in string:
            if not c in ['.', ':']:
                segments.append( SegmentTable.SEGMENT_TABLE_ASCII[ord(c)] )
            elif len(segments) > 0:
                segments[-1] |= SegmentTable.SH
        self.controller.i2c( addr=CMD_DATA )
        self.controller.i2c( segments[:self.DIGITS], addr=CMD_ADDRESS+index )
        self.controller.i2c( addr=CMD_CONTROL+0x08+self.brightness ) 

    def display_integer( self, val, hex_mode=False ):
        if hex_mode:
            string = (('%%%dX'% self.DIGITS)% val)[-self.DIGITS:]
        else:
            string = (('%%%dd'% self.DIGITS)% val)[-self.DIGITS:]
        self.display_segs( 0, string[:self.DIGITS], ascii_mode=True )
    
    def display_segment( self, index, segment, ascii_mode=None, hex_mode=None ):
        self.controller.i2c( addr=CMD_DATA+index )
        if hex_mode:
            segment = SegmentTable.SEGMENT_TABLE_HEX[segment]
        elif ascii_mode:
            segment = SegmentTable.SEGMENT_TABLE_ASCII[ord(segment)]
        self.segments[index] = segment
        self.controller.i2c( [segment], addr=CMD_ADDRESS+index )
        self.controller.i2c( addr=CMD_CONTROL+0x08+self.brightness ) 

    def display_segments( self, index, segments, ascii_mode=None, hex_mode=None ):
        self.controller.i2c( addr=CMD_DATA )
        segments = segments[:self.DIGITS]
        if hex_mode:
            segments = [ SegmentTable.SEGMENT_TABLE_HEX[c] for c in segments ]
        elif ascii_mode:
            segments = [ SegmentTable.SEGMENT_TABLE_ASCII[ord(c)] for c in segments ]
        self.segments = segments
        self.controller.i2c( segments, addr=CMD_ADDRESS+index )
        self.controller.i2c( addr=CMD_CONTROL+0x08+self.brightness ) 

    clrscr = clear
    display_str = display_string
    display_int = display_integer
    display_seg = display_segment
    display_segs = display_segments


class LED2(LED):
    DIGITS = 2

class LED3(LED):
    DIGITS = 3
     
class LED4(LED):
    DIGITS = 4

class LED5(LED):
    DIGITS = 5

class LED6(LED):
    DIGITS = 6

# NOTE: TM1637 chip supports 6 digits at most

