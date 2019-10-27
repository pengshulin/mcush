# coding: utf8
__doc__ = 'MAX31865 SPI Thermometer'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils


REG_CONFIG       = 0
REG_RESULT_H     = 1
REG_RESULT_L     = 2
REG_HIGH_FAULT_H = 3
REG_HIGH_FAULT_L = 4
REG_LOW_FAULT_H  = 5
REG_LOW_FAULT_L  = 6
REG_FAULT        = 7

CONFIG_BIAS_ON   = 0x80
CONFIG_BIAS_OFF  = 0x00
CONFIG_CONV_AUTO = 0x40
CONFIG_CONV_OFF  = 0x00
CONFIG_1SHOT_ON  = 0x20
CONFIG_1SHOT_OFF = 0x00
CONFIG_3WIRE_ON  = 0x10
CONFIG_3WIRE_OFF = 0x00
CONFIG_FILTER_50 = 0x01
CONFIG_FILTER_60 = 0x00
CONFIG_FAULT_AUTO_CLEAR = 0x02



class Max31865():
    rref = 430
    bias = CONFIG_BIAS_ON
    conv = CONFIG_CONV_AUTO
    one_shot = CONFIG_1SHOT_ON
    three_wire = CONFIG_3WIRE_ON
    filter_sel = CONFIG_FILTER_50
        
    def __init__( self, controller, sdi=None, sdo=None, sck=None, cs=None ):
        self.controller = controller
        self.controller.spi_init( sdi, sdo, sck, cs, width=8, cpol=1, cpha=1 )
        self.config()

    def config( self ):
        cfg = self.bias | self.conv | self.one_shot | self.three_wire | self.filter_sel
        self.writeReg( REG_CONFIG, cfg )

    def writeReg( self, addr, dat ):
        self.controller.spiWrite([(addr&0x7)|0x80, dat]) 

    def readReg( self, addr ):
        return self.controller.spiRead([addr&0x7, 0xFF])[1]

    def readAllRegs( self ):
        ret = {}
        ret['config'] = self.readReg( REG_CONFIG )
        ret['result_h'] = self.readReg( REG_RESULT_H )
        ret['result_l'] = self.readReg( REG_RESULT_L )
        ret['high_fault_h'] = self.readReg( REG_HIGH_FAULT_H )
        ret['high_fault_l'] = self.readReg( REG_HIGH_FAULT_L )
        ret['low_fault_h'] = self.readReg( REG_LOW_FAULT_H )
        ret['low_fault_l'] = self.readReg( REG_LOW_FAULT_L )
        ret['fault'] = self.readReg( REG_FAULT )
        return ret

    def getTemperature( self ):
        ad = (self.readReg( REG_RESULT_H )<<8) | self.readReg( REG_RESULT_L )
        if ad & 0x0001:
            raise Exception("sensor fault")
        r = float(ad>>1) * self.rref / (2**15)
        # calibrate as linear: 203.11 ohm @ 275.0 degree
        # factor = 275.0/(203.11-100) = 2.667 deg/ohm
        t = (r-100)*2.667
        return t

