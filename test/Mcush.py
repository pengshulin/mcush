#!/usr/bin/env python
# coding:utf8
import sys
import os
import time
import Instrument
import Utils
import Env
import re
import logging
import binascii
#import ctypes
#from ctypes import c_char, c_byte, c_ubyte, c_short, c_ushort
#from ctypes import c_int, c_uint, c_long, c_ulong
#from ctypes import c_float, c_double, c_void_p
#from ctypes import sizeof, Structure, POINTER, pointer
#from ctypes import create_string_buffer, memmove



class Mcush( Instrument.SerialInstrument ):
    '''Mcush core'''
    DEFAULT_NAME = 'MCUSH'
    DEFAULT_IDN = re.compile( 'mcush,([0-9]+\.[0-9]+.*)' )
 
    def __init__( self, port=None, baudrate=None, connect=True ):
        Instrument.SerialInstrument.__init__( self, port, baudrate, connect=connect )
        self.files = None
 
    def connect( self ):
        Instrument.SerialInstrument.connect( self )
   
    def reset( self, delay=None ):
        '''reset'''
        sync = False
        retry = 0
        RETRY_TIME = 10
        try:
            Instrument.SerialInstrument.reset( self )
            Instrument.SerialInstrument.connect( self )
            sync = True
        except Instrument.ResponseError:
            pass
        except Instrument.CommandTimeoutError:
            pass
        while not sync: 
            try:
                Instrument.SerialInstrument.connect( self )
                sync = True
            except:
                retry = retry + 1
                if retry > RETRY_TIME:
                    raise Instrument.CommandTimeoutError()
        self.scpiIdn()
        if delay is None:
            time.sleep( Env.DELAY_AFTER_RESET )
        else:
            time.sleep( delay )

    def getModel( self ):
        if self.idn is None:
            self.scpiIdn()
        try:
            return self.idn.split(',')[0]
        except IndexError:
            return ''

    def getSerialNumber( self ):
        if self.idn is None:
            self.scpiIdn()
        try:
            return self.idn.split(',')[2]
        except IndexError:
            return ''

    def getVersion( self ):
        if self.idn is None:
            self.scpiIdn()
        try:
            return self.idn.split(',')[1]
        except IndexError:
            return ''

    def parseMemLine( self, line ):
        '''format:  XXXXXXXX: xx xx xx xx xx'''
        return ''.join([binascii.unhexlify(i) for i in line[10:].split()])
 
    def led( self, idx, on=None, toggle=None ):
        if on is None and toggle is None:
            # read
            r = self.writeCommand( 'led -i %d'% (idx) )
            return bool(r[0].strip() == '1')
        else:
            # set 
            cmd = 'led -i %d'% (idx)
            if toggle is not None:
                cmd += ' -t'
            else:
                cmd += ' -s' if on else ' -c'
            self.writeCommand( cmd )


 
    

if __name__ == '__main__':
    c = Mcush()
    print c.getModel(), c.getVersion() 
    c.disconnect()

