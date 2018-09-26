# coding: utf8
__doc__ = 'max7219 matrix led controller module'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush

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


def B2I( bitmap, reverse ):
    assert len(bitmap) == 8
    ret = 0
    for i in range(8):
        if bitmap[i] == '*':
            ret |= 1<<(7-i)
    return (ret ^ 0xFF) if reverse else ret


class LED8x8():
    WIDTH, HEIGHT = 8, 8
 
    def __init__( self, controller ):
        self.controller = controller

    def init( self, sdi=None, sdo=None, sck=None, cs=None ):
        self.controller.spi_init( sdi, sdo, sck, cs, width=16, delay=100 )
        self.reset()

    def write(self, addr, dat):
        val = ((addr & 0x0F)<<8) + (dat & 0xFF)
        self.controller.spi( [val] )

    def reset(self):
        self.write( ADDR_SHUTDOWN, 1 )
        self.write( ADDR_SCAN_LIMIT, 7 )
        self.write( ADDR_DECODE_MODE, 0 )
        self.write( ADDR_INTENSITY, 0x0A )
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
        self.write( ADDR_INTENSITY, val )

    def test_all( self ):
        self.write( ADDR_D0, 0xFF )
        self.write( ADDR_D1, 0xFF )
        self.write( ADDR_D2, 0xFF )
        self.write( ADDR_D3, 0xFF )
        self.write( ADDR_D4, 0xFF )
        self.write( ADDR_D5, 0xFF )
        self.write( ADDR_D6, 0xFF )
        self.write( ADDR_D7, 0xFF )

    def write_line( self, index, line_buf ):
        self.write( ADDR_D0 + index, int(line_buf) )
    
    def draw_bitmap( self, bitmap, reverse=False ):
        assert len(bitmap) == 8
        self.write( ADDR_D0, B2I(bitmap[0], reverse))
        self.write( ADDR_D1, B2I(bitmap[1], reverse))
        self.write( ADDR_D2, B2I(bitmap[2], reverse))
        self.write( ADDR_D3, B2I(bitmap[3], reverse))
        self.write( ADDR_D4, B2I(bitmap[4], reverse))
        self.write( ADDR_D5, B2I(bitmap[5], reverse))
        self.write( ADDR_D6, B2I(bitmap[6], reverse))
        self.write( ADDR_D7, B2I(bitmap[7], reverse))


