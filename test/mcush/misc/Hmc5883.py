# coding: utf8
__doc__ = 'Honeywell HMC5583 3-Axis Compass'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Instrument, Utils
import time


REG_CONFIG_A  = 0
REG_CONFIG_B  = 1
REG_MODE      = 2
REG_DAT_X_H   = 3
REG_DAT_X_L   = 4
REG_DAT_Z_H   = 5
REG_DAT_Z_L   = 6
REG_DAT_Y_H   = 7
REG_DAT_Y_L   = 8
REG_STATUS    = 9
REG_ID_A      = 10
REG_ID_B      = 11
REG_ID_C      = 12

LSB_PER_GAUSS_GAIN = [1370, 1090, 820, 660, 440, 390, 330, 230]

class Hmc5883():

    def __init__( self, controller, scl=None, sda=None ):
        self.controller = controller
        self.controller.i2c_init( 0x1E, scl, sda )
        self.config()
        self.mode()
        
    def config( self, ma=3, do=4, ms=0, gain=1 ):
        a = (ma<<5)+(do<<2)+(ms)
        b = (gain<<5)
        self.controller.i2c( [REG_CONFIG_A, a, b] )
        self.gain = gain

    def mode( self, continuous=True ):
        m = 0 if continuous else 1
        self.controller.i2c( [REG_MODE, m] )

    def readID( self ):
        a, b, c = self.controller.i2c( [REG_ID_A], read=3 )
        return (a<<16)+(b<<8)+c

    def readStatus( self ):
        s = self.controller.i2c( [REG_STATUS], read=1 )[0]
        return s

    def readValue( self ):
        while True:
            if self.readStatus() & 0x01:
                break
            time.sleep(0.01)
        xh, xl, zh, zl, yh, yl = self.controller.i2c( [REG_DAT_X_H], read=6 )
        x = Utils.H2h((xh<<8)+xl)
        z = Utils.H2h((zh<<8)+zl)
        y = Utils.H2h((yh<<8)+yl)
        lpg = float(LSB_PER_GAUSS_GAIN[self.gain])
        return (x/lpg, y/lpg, z/lpg)

