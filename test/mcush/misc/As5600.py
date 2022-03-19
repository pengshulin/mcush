# coding: utf8
__doc__ = 'AS5600 Magnetic Rotary Position Sensor'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Instrument, Utils
import time

REG_ZMCO        = 0x00
REG_ZPOS        = 0x01
REG_MPOS        = 0x03
REG_MANG        = 0x05
REG_CONF        = 0x07
REG_STATUS      = 0x0B
REG_RAW_ANGLE   = 0x0C
REG_ANGLE       = 0x0E
REG_AGC         = 0x1A
REG_MAGNITUDE   = 0x1B
REG_BURN        = 0xFF



class As5600():

    def __init__( self, controller, scl=None, sda=None, delay=5 ):
        self.controller = controller
        self.controller.i2c_init( 0x36, scl=scl, sda=sda, delay=delay )

    def readByte( self, addr ):
        return self.controller.i2c( [addr], read=1 )[0]

    def readWord( self, addr ):
        h, l = self.controller.i2c( [addr], read=2 )
        return (h<<8)+l

    def checkStatus( self ):
        s = self.readByte( REG_STATUS )
        if s & 0x10:
            raise Exception('Magnetic too weak')
        if s & 0x08:
            raise Exception('Magnetic too strong')
        if not s & 0x20:
            raise Exception('Magnetic not detected')

    def readMagnitude( self ):
        return self.readWord( REG_MAGNITUDE )
 
    def readRawAngle( self ):
        return self.readWord( REG_RAW_ANGLE )

    def readAngle( self ):
        return self.readWord( REG_ANGLE ) * 360 / 4096

