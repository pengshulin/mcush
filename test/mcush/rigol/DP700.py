# coding:utf8
__doc__ = 'DP700 Series Single-channel Programmable Power'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Instrument
from .. import Env

# Product lines:
#        Channel  Power  Output(per channel)
# DP711  1        150W   30V/5A    
# DP712  1        150W   50V/3A    
#

class DP700( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'DP700'
    DEFAULT_IDN = re_compile( '.*' )

    def __init__( self, *args, **kwargs ):
        Instrument.SerialInstrument.__init__( self, *args, **kwargs ) 
        raise NotImplementedError

        # TODO: append the instruments

