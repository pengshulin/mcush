# coding:utf8
__doc__ = 'DP800 Multi-channels Programmable Power'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Instrument
from .. import Env

# Product lines:
#        Channel  Power  Output(per channel)
# DP811  1        200W   50V/3A    
# DP821  2        140W   8V10A||60V/1A    
# DP831  3        160W   8V5A||30V/2A,-30V/2A
# DP832  3        195W   30V/3A||30V/3A,5V/3A
#
class DP800( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'DP800'
    DEFAULT_IDN = re_compile( '.*' )

    def __init__( self, *args, **kwargs ):
        Instrument.SerialInstrument.__init__( self, *args, **kwargs ) 
        raise NotImplementedError

        # TODO: append the instruments

