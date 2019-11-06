# coding: utf8
__doc__ = 'TCS3472 color sensor from ams AG'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'


REG_ENABLE = 0x00
REG_ATIME = 0x01
REG_WTIME = 0x03
REG_AILT = 0x04
REG_AIHT = 0x06
REG_PERS = 0x0C
REG_CONFIG = 0x0D
REG_CONTROL = 0x0F
REG_ID = 0x12
REG_STATUS = 0x13
REG_CDATA = 0x14
REG_RDATA = 0x16
REG_GDATA = 0x18
REG_BDATA = 0x1A


class Tcs3472():
        
    def __init__( self, controller, scl=None, sda=None ):
        self.controller = controller
        self.controller.i2c_init( 0x29, scl, sda )
        self.init()

    def readReg( self, addr ):
        command = [ 0x80 | (1<<5) | (addr&0x1F) ]
        return self.controller.i2c( command, read=1 )[0]

    def readReg2( self, addr ):
        command = [ 0x80 | (1<<5) | (addr&0x1F) ]
        r = self.controller.i2c( command, read=2 )
        return (r[1]<<8)+r[0]

    def writeReg( self, addr, dat ):
        command = [ 0x80 | (1<<5) | (addr&0x1F), dat&0xFF]
        self.controller.i2c( command )

    def writeReg2( self, addr, dat ):
        command = [ 0x80 | (1<<5) | (addr&0x1F), dat&0xFF, (dat>>8)&0xFF]
        self.controller.i2c( command )

    def init( self ):
        self.id = self.readReg( REG_ID )  # chip id
        self.writeReg( REG_ENABLE, 0x03 )  # power on, adc enabled
        self.writeReg( REG_ATIME, 0x00 )  # integ_cycles:256, time 700ms
        self.writeReg( REG_WTIME, 0x00 )  # wait:256, time 614ms

    def readStatus( self ):
        return self.readReg( REG_STATUS )

    def readRawData( self ):
        c = self.readReg2( REG_CDATA )
        r = self.readReg2( REG_RDATA )
        g = self.readReg2( REG_GDATA )
        b = self.readReg2( REG_BDATA )
        return c, r, g, b

