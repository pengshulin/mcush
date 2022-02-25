# coding: utf8
__doc__ = 'Bh1750 Ambient Light Sensor'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Instrument, Utils
import time


OPCODE_POWER_DOWN = 0x00
OPCODE_POWER_ON   = 0x01
OPCODE_RESET      = 0x07
OPCODE_CONT_HRES_MODE  = 0x10
OPCODE_CONT_HRES_MODE2 = 0x11
OPCODE_CONT_LRES_MODE  = 0x13
OPCODE_SINGLE_HRES_MODE  = 0x20
OPCODE_SINGLE_HRES_MODE2 = 0x21
OPCODE_SINGLE_LRES_MODE  = 0x23



class Bh1750():

    def __init__( self, controller, scl=None, sda=None, addr_h=False, delay=10 ):
        self.controller = controller
        self.controller.i2c_init( 0x5C if addr_h else 0x23, scl=scl, sda=sda, delay=delay )
        self.init()

    def init( self ):
        self.controller.i2c( [OPCODE_POWER_ON] )
        self.controller.i2c( [OPCODE_CONT_HRES_MODE] )
        self.t0 = time.time()

    def read( self ):
        while time.time() < self.t0 + 0.12:
            time.sleep(0.01)
        h, l = self.controller.i2c( read=2 )
        self.t0 = time.time()
        return ((h<<8)+l) / 1.2  # unit: lx

