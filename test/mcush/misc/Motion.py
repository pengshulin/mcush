# coding: utf8
__doc__ = 'motion sensors'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils


class Mpu6050():

    def __init__( self, controller ):
        self.controller = controller

    def init( self, scl=None, sda=None ):
        self.controller.i2c_init( 0x68, scl=scl, sda=sda )
        self.reset()

    def reset(self):
        self.controller.i2c( [0x6B, 0x00] )  # wake up
        self.controller.i2c( [0x19, 0x07] )  # sample rate: 0x07(125Hz)
        self.controller.i2c( [0x1A, 0x06] )  # filter: 0x06(5Hz)
        self.controller.i2c( [0x1B, 0x18] )  # gyro setting: 0x18(no self test, 2000deg/s)
        self.controller.i2c( [0x1C, 0x01] )  # accel setting: 0x01(no self test, 2G, 5Hz)
 
    def printAllReg( self ):
        print self.controller.i2c( [0x0], 118 )

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
 


