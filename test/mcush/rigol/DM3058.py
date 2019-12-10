# coding:utf8
__doc__ = 'DM3058/E 5-1/2 Digital Multimeter'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Instrument
from .. import Env


class DM3058( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'DM3058'
    DEFAULT_IDN = re_compile( '.*' )

    def __init__( self, *args, **kwargs ):
        Instrument.SerialInstrument.__init__( self, *args, **kwargs ) 
        raise NotImplementedError

        # TODO: append the instruments

