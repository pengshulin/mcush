# coding: utf8
__doc__ = 'ws2812 color led strap controller module'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush
from . import Bitmap
from . import Font



class LEDS():
    '''WS2812 controller'''
    
    def __init__( self, controller, length=None, group_length=None, swap_rg=None, pin=None ):
        if length is None:
            if group_length is None:
                length = self.width * self.height
            else:
                length = 1  # whole strap share the same color
        self.length = length
        self.swap_rg = swap_rg
        self.controller = controller
        cmd = "W -I -l%d"% length
        if group_length:
            cmd += " -G%d"% group_length
        if pin is not None:
            cmd += " -p%s"% pin
        self.controller.writeCommand( cmd )

    DEFAULT_CMD_LINE_LIMIT = 110

    def write( self, mem, offset=0, push=None, fill=None, length=None, flush=True ):
        #mem = mem[:self.length-offset]  # ignore the invalid portion
        cmd = 'W'
        if offset:
            cmd += ' -o%d'% offset
        if self.swap_rg:
            cmd += ' -g'
        if push is not None:
            if push == 'f':
                cmd += ' -f'
            elif push == 'b':
                cmd += ' -b'
        if fill is not None:
            cmd += ' -F'
        if length is not None:
            cmd += ' -l%d'% length
        count_data = 0
        count_argv = len(cmd.split())
        while True:
            if len(mem) == 0:
                if count_data:
                    if not flush:
                        self.controller.writeCommand( cmd )
                    else:
                        # the final line left
                        if ((len(cmd)+3) >= self.DEFAULT_CMD_LINE_LIMIT) or ((count_argv+1) >= self.controller.DEFAULT_CMD_ARGV_LIMIT):
                            # too full to add -w option
                            self.controller.writeCommand( cmd )
                            self.controller.writeCommand( 'W -w')
                        else:
                            # add -w option
                            self.controller.writeCommand( cmd.replace('W', 'W -w') )
                elif flush:
                    # all data written, but not flushed
                    self.controller.writeCommand( 'W -w')
                break
            cmd += ' %d'% (int(mem.pop(0)) & 0xFFFFFF)
            count_data += 1
            count_argv += 1
            if (len(cmd) >= self.DEFAULT_CMD_LINE_LIMIT) or (count_argv >= self.controller.DEFAULT_CMD_ARGV_LIMIT):
                # split data into multiple lines
                self.controller.writeCommand( cmd )
                if not push:
                    offset += count_data
                cmd = 'W'
                if offset:
                    cmd += ' -o%d'% offset
                if self.swap_rg:
                    cmd += ' -g'
                if push is not None:
                    if push == 'f':
                        cmd += ' -f'
                    elif push == 'b':
                        cmd += ' -b'
                count_data = 0
                count_argv = len(cmd.split())

    def flush( self, flush=True ):
        if flush:
            self.controller.writeCommand( 'W -w')

    def pushf( self, mem, offset=None, length=None, flush=True ):
        self.write( mem, offset=offset, push='f', length=length, flush=flush )

    def pushb( self, mem, offset=None, length=None, flush=True ):
        self.write( mem, offset=offset, push='b', length=length, flush=flush )

    def fill( self, mem, offset=None, length=None, flush=True ):
        self.write( mem, offset=offset, fill=True, length=length, flush=flush )


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




