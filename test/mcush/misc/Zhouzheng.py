# coding: utf8
__doc__ = 'products from Xian Zhouzhen Electronics Tech.'
__author__ = 'Peng Shulin <trees_peng@163.com>'
from .. import Env, Utils, Modbus


class DAQM9004( Modbus.ModbusRTU ):
    DEFAULT_NAME = 'DAQM9004'
    
    def readADC( self ):
        return self.readInputRegs(0x0000, 8)

    def readVolt( self ):
        return [ val/4095.0*5.0 for val in self.readADC() ]

    def readAmp( self ):
        return [ val/4095.0*0.02 for val in self.readADC() ]

