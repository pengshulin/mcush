# coding: utf8
__doc__ = 'products from Pingyang Miaoguan Tech.'
__author__ = 'Peng Shulin <trees_peng@163.com>'
from .. import Env, Utils, Modbus



class TH10S_B( Modbus.ModbusRTU ):
    DEFAULT_NAME = 'TH10S_B'

    def readADC( self, addr ):
        val = self.readInputReg( addr )
        if val == 0x8000:
            raise Exception('Probe error')
        return val        
        
    def readTemperature( self ):
        val = Utils.I2i(self.readADC(0))
        return round(val*0.1, 1)

    def readHumidity( self ):
        return round(self.readADC(1)*0.1, 1)

