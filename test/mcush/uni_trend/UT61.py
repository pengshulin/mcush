# coding:utf8
__doc__ = 'UT61B/C/D/E digital multimeter'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Instrument
from .. import Env



class UT61( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'UT61'
    DEFAULT_TIMEOUT = 10
    DEFAULT_PROMPTS = re_compile( '(\x03)?[=#?!]>' )
    DEFAULT_IDN = re_compile( '.*' )

    def writeCommand( self, cmd ):
        # UT61X only receives serial packets
        pass

    def getReading( self ):
        # read measure value, function, range, flags
        # TODO: read from serial input stream 
        pass

