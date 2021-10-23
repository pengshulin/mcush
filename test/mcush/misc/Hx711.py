# coding: utf8
__doc__ = 'HX711 24-bit ADC'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils
import time


MODE_A128 = 0
MODE_B32 = 1
MODE_A64 = 2


class Hx711():
    ref = 5.0
    mode = MODE_A128
    clocks = 25

    def __init__( self, controller, sdi=None, sdo=None, sck=None, cs=None, mode='a128', ref=5.0 ):
        self.controller = controller
        self.sdi = sdi if sdi is not None else '0.0'
        self.sdo = sdo if sdo is not None else '0.1'
        self.sck = sck if sck is not None else '0.2'
        self.cs  = cs  if cs  is not None else '0.3'
        self.ref = ref
        self._setMode( mode )
        self.controller.spiInit(self.sdi, self.sdo, self.sck, self.cs, width=self.clocks, cpha=True)

    def _setMode( self, mode ):
        if mode in [0,'a128','A128','a','A']:
            self.mode = MODE_A128
        elif mode in [1,'b32','B32','b','B']:
            self.mode = MODE_B128
        elif mode in [0,'a64','A64']:
            self.mode = MODE_A64
        else:
            raise Exception('invalid mode')
        self.clocks = self.mode + 25

    def setMode( self, mode ):
        self._setMode( mode )
        self.controller.spiUpdate(self.sdi, self.sdo, self.sck, self.cs, width=self.clocks, cpha=True)

    def dataIsReady( self ):
        return self.controller.pinIsLow(self.sdi)

    def readData( self, mode=None ):
        a = self.controller.spiRead([0])[0]
        if mode == 'hex':
            return a>>(self.mode+1)
        else:
            val = Utils.I2i(a<<(7-self.mode))/256
            if mode == 'int':
                return val
            v = self.ref * val / (2**24)
            # return voltage from gain setting
            if self.mode == MODE_A128:
                return v / 128
            elif self.mode == MODE_B32:
                return v / 32
            elif self.mode == MODE_A64:
                return v / 64

