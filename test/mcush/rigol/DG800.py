# coding:utf8
__doc__ = 'DG811/812/821/822/831/832 Programmable Signal Generator'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Instrument
from .. import Env


class DG800( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'DG800'
    DEFAULT_IDN = re_compile( '.*' )

    def __init__( self, *args, **kwargs ):
        Instrument.SerialInstrument.__init__( self, *args, **kwargs ) 
        raise NotImplementedError

        # TODO: append the instruments

