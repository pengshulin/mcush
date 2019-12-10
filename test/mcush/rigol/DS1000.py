# coding:utf8
__doc__ = 'DS1000B/E/U/Z Digital Oscilloscope'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Instrument
from .. import Env

# Product lines:
#            Bandwidth   Channels  Samplerate    Depth
# DS1000E/U  50M ~ 100M     2       1G Sa/s      1M pts
# DS1000Z    50M ~ 100M     4       1G Sa/s     24M pts
# DS1000Z-E  200M           2       1G Sa/s     24M pts
# DS1000B    70M ~ 200M     4       2G Sa/s     16k pts 

class DS1000( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'DS1000'
    DEFAULT_IDN = re_compile( '.*' )

    def __init__( self, *args, **kwargs ):
        Instrument.SerialInstrument.__init__( self, *args, **kwargs ) 
        raise NotImplementedError

        # TODO: append the instruments

