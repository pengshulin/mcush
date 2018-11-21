# coding: utf8
__doc__ = 'motion sensors'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils


class Mpu6050():

    def __init__( self, controller, scl=None, sda=None ):
        self.controller = controller
        self.controller.i2c_init( 0x68, scl=scl, sda=sda )
        self.reset()

    def reset(self):
        self.controller.i2c( [0x6B, 0x00] )  # wake up
        self.controller.i2c( [0x19, 0x07] )  # sample rate: 0x07(125Hz)
        self.controller.i2c( [0x1A, 0x06] )  # filter: 0x06(5Hz)
        self.controller.i2c( [0x1B, 0x18] )  # gyro setting: 0x18(no self test, 2000deg/s)
        self.controller.i2c( [0x1C, 0x01] )  # accel setting: 0x01(no self test, 2G, 5Hz)
 
    def printAllReg( self ):
        print( self.controller.i2c( [0x0], 118 ) )

    def getTemperature( self ):
        a,b = self.controller.i2c( [0x41], 2 )
        return 35 + (Utils.s2h( chr(b) + chr(a) ) + 521)/340.0

    def getAccelX( self ):
        a,b = self.controller.i2c( [0x3B], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 16384.0 * 9.8
        
    def getAccelY( self ):
        a,b = self.controller.i2c( [0x3D], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 16384.0 * 9.8
        
    def getAccelZ( self ):
        a,b = self.controller.i2c( [0x3F], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 16384.0 * 9.8
 
    def getGyroX( self ):
        a,b = self.controller.i2c( [0x43], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 16384.0 * 16.4
        
    def getGyroY( self ):
        a,b = self.controller.i2c( [0x45], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 16384.0 * 16.4
        
    def getGyroZ( self ):
        a,b = self.controller.i2c( [0x47], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 16384.0 * 16.4
 



class Adxl345():

    def __init__( self, controller, scl=None, sda=None ):
        self.controller = controller
        self.controller.i2c_init( 0x53, scl=scl, sda=sda )
        self.reset()

    def reset(self):
        self.controller.i2c( [0x31, 0x0B] )  # DATA_FORMAT: full resolution, 16g range
        self.controller.i2c( [0x2C, 0x08] )  # BW_RATE: 12.5Hz bandwidth, 25Hz output rate
        self.controller.i2c( [0x2D, 0x08] )  # POWER_CTRL: Measure 
        self.controller.i2c( [0x2E, 0x80] )  # INT_ENABLE: DATA_READY 
        self.controller.i2c( [0x1E, 0x00] )  # OFSX: X-axis offset
        self.controller.i2c( [0x1F, 0x00] )  # OFSY: Y-axis offset
        self.controller.i2c( [0x20, 0x00] )  # OFSZ: Z-axis offset

    def printAllReg( self ):
        print( self.controller.i2c( [0x0], 57 ) )

    def getAccelX( self ):
        a,b = self.controller.i2c( [0x32], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 8192.0
 
    def getAccelY( self ):
        a,b = self.controller.i2c( [0x34], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 8192.0
 
    def getAccelZ( self ):
        a,b = self.controller.i2c( [0x36], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 8192.0
 
