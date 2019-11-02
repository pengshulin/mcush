# coding: utf8
__doc__ = 'Aosong sensors'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils
import time


class Dht11():
        
    def __init__( self, controller, pin=None ):
        self.controller = controller
        if pin:
            self.controller.writeCommand('dht11 -p%s'% pin)

    def read( self ):
        ret = self.controller.writeCommand('dht11')
        return float(ret[0]), float(ret[1])


CMD_INIT = 0xE1
CMD_RESET = 0xBA
CMD_CONVERT = 0xAC
CMD_READ = 0x71

class Aht10():
        
    def __init__( self, controller, scl=None, sda=None ):
        self.controller = controller
        self.controller.i2c_init( 0x38, scl, sda )
        self.reset()
        self.calib()

    def calib( self ):
        while not self.readStatus() & 0x08:
            self.controller.i2c( [CMD_INIT, 0x08, 0x00] )
            time.sleep(1)

    def reset( self ):
        self.controller.i2c( [CMD_RESET] )
        time.sleep(0.1)

    def convert( self ):
        self.controller.i2c( [CMD_CONVERT, 0x33, 0x00] )
        time.sleep(0.1)
        while self.readStatus() & 0x80:
            time.sleep(0.1)

    def readStatus( self ):
        ret = self.controller.i2c( [CMD_READ], read=1 )
        return ret[0]

    def read( self ):
        ret = self.controller.i2c( [CMD_READ], read=6 )
        humidity = float((ret[1]<<12)|(ret[2]<<4)|(ret[3]>>4))
        temperature = float(((ret[3]&0xF)<<16)|(ret[4]<<8)|(ret[5]))
        humidity = humidity/(2**20)*100
        temperature = (temperature/(2**20)*200)-50.0
        return humidity, temperature

