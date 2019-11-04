# coding: utf8
__doc__ = 'Bosch sensors'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils
import time


REG_AC1 = 0xAA
REG_AC2 = 0xAC
REG_AC3 = 0xAE
REG_AC4 = 0xB0
REG_AC5 = 0xB2
REG_AC6 = 0xB4
REG_B1 = 0xB6
REG_B2 = 0xB8
REG_MB = 0xBA
REG_MC = 0xBC
REG_MD = 0xBE

REG_CTRL_MEAS = 0xF4
REG_OUT = 0xF6
REG_OUT_XLSB = 0xF8
REG_SOFT_RESET = 0xE0
REG_ID = 0xD0


class Bmp180():
        
    def __init__( self, controller, scl=None, sda=None ):
        self.controller = controller
        self.controller.i2c_init( 0x77, scl, sda )
        self.reset()
        self.init()

    def readReg( self, addr, signed=False ):
        h, l = self.controller.i2c( [addr], read=2 )
        r = (h<<8) + l
        return Utils.H2h(r) if signed else r

    def readReg3( self, addr ):
        h, l, x = self.controller.i2c( [addr], read=3 )
        r = (h<<16) + (l<<8) + x
        return r

    def init( self ):
        # read calibration parameters
        self.ac1 = self.readReg( REG_AC1, True )
        self.ac2 = self.readReg( REG_AC2, True )
        self.ac3 = self.readReg( REG_AC3, True )
        self.ac4 = self.readReg( REG_AC4 )
        self.ac5 = self.readReg( REG_AC5 )
        self.ac6 = self.readReg( REG_AC6 )
        self.b1 = self.readReg( REG_B1, True )
        self.b2 = self.readReg( REG_B2, True )
        self.mb = self.readReg( REG_MB, True )
        self.mc = self.readReg( REG_MC, True )
        self.md = self.readReg( REG_MD, True )
        self.printCalibRegs()

    def printCalibRegs( self ):
        print( 'AC1: %d'% (self.ac1) )
        print( 'AC2: %d'% (self.ac2) )
        print( 'AC3: %d'% (self.ac3) )
        print( 'AC4: %d'% (self.ac4) )
        print( 'AC5: %d'% (self.ac5) )
        print( 'AC6: %d'% (self.ac6) )
        print( 'B1:  %d'% (self.b1) )
        print( 'B2:  %d'% (self.b2) )
        print( 'MB:  %d'% (self.mb) )
        print( 'MC:  %d'% (self.mc) )
        print( 'MD:  %d'% (self.md) )

    def reset( self ):
        self.controller.i2c( [REG_SOFT_RESET, 0xB6] )
        time.sleep(0.1)

    def read( self, oss=3 ):
        # read temperature
        self.controller.i2c( [REG_CTRL_MEAS, 0x2E] )
        time.sleep(0.005)
        ut = self.readReg( REG_OUT )
        # read pressure
        mode = 0x34 | (oss<<6)
        self.controller.i2c( [REG_CTRL_MEAS, mode] )
        time.sleep(0.005+0.003*(2**oss))
        up = self.readReg3( REG_OUT ) >> (8-oss)
        # calculate temperature
        x1 = ((ut - self.ac6) * self.ac5) / (2**15)
        x2 = (self.mc * (2**11) ) / (x1 + self.md)
        b5 = x1 + x2
        t = (b5 + 8) / (2**4)
        # calculate pressure
        b6 = b5 - 4000
        x1 = (self.b2 * (b6 * b6 / (2**12)))/(2**11)
        x2 = self.ac2 * b6 / (2**11)
        x3 = x1 + x2
        b3 = (((self.ac1*4 + x3)<<oss) + 2) / 4
        x1 = self.ac3 * b6 / (2**13)
        x2 = (self.b1 * (b6 * b6 / (2**12))) / (2**16)
        x3 = ((x1 + x2) + 2) / (2**2)
        b4 = self.ac4 * (x3 + 32768) / (2**15)
        b7 = (up - b3) * (50000>>oss)
        if b7 < 0x80000000:
            p = (b7 * 2) / b4
        else:
            p = (b7 / b4) * 2 
        x1 = (p / (2**8))**2
        x1 = (x1 * 3038) / (2**16)
        x2 = (-7357 * p) / (2**16)
        p = p + (x1 + x2 + 3791) / (2**4)
        return float(p), float(t)/10

    def convertPressure2Altitude( self, p ):
        a = 44330 * (1-((p/101325)**(1/5.255)))
        return a

