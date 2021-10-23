# coding: utf8
__doc__ = 'OMRON sensors'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import time


REG_ADDR = 0x0  # 0/1
REG_CTRL = 0x2
REG_WR = 0x3  # 3/4/5/6
REG_RD = 0x7  # 7/8/9/A
REG_INIT = 0xB
REG_PWR = 0xD


# Digital flow sensor
class D6F_PH():
    ADDR = 0x6C
    RANGE = 50  # Pa

    def __init__( self, controller, scl=None, sda=None ):
        self.controller = controller
        self.controller.i2c_init( self.ADDR, scl, sda )
        self.reset()

    def reset( self, delay=0.1 ):
        # hardware reset
        self.controller.i2c( [REG_PWR, 0x80] )
        time.sleep(delay)
        # load from NVM
        self.controller.i2c( [REG_INIT, 0x00] ) 

        # [D042] <= 0C35h 50msec wait@4MHz */
        self.controller.i2c( [REG_ADDR, 0xD0, 0x42, 0x28, 0x0C, 0x35] )

        # [D041] <= 88h Heater supply voltage is 1.3V */
        self.controller.i2c( [REG_ADDR, 0xD0, 0x41, 0x18, 0x88] )

        # [D044] <= 01h 16bit Resolution & Gain = 2 */
        self.controller.i2c( [REG_ADDR, 0xD0, 0x44, 0x18, 0x01] )

        # [D047] <= 0Ch:set MCU mode 31 */
        self.controller.i2c( [REG_ADDR, 0xD0, 0x47, 0x18, 0x1F] )

        # [D048] <= E6h */
        self.controller.i2c( [REG_ADDR, 0xD0, 0x48, 0x18, 0xE6] )
   
    def startMeasure( self ):
        # MCU power on, start
        self.controller.i2c( [REG_ADDR, 0xD0, 0x40, 0x18, 0x06] )

    def readSensorStatus( self ):
        self.controller.i2c( [REG_ADDR, 0xD0, 0x40, 0x1C] )
        ret = self.controller.i2c( [REG_RD], read=1 )
        return ret[0]

    def readPowerStatus( self ):
        ret = self.controller.i2c( [REG_PWR], read=1 )
        return ret[0]
        
    def readFlags( self ):
        self.controller.i2c( [REG_ADDR, 0xD0, 0x46, 0x1C] )
        ret = self.controller.i2c( [REG_RD], read=1 )
        return ret[0]
    
    def readSensorReference( self ):
        self.controller.i2c( [REG_ADDR, 0xD0, 0x65, 0x2C] )
        ret = self.controller.i2c( [REG_RD], read=2 )
        val = (ret[0]<<8)+ret[1]
        return val
  
    def readSensorThreshold( self ):
        self.controller.i2c( [REG_ADDR, 0xD0, 0x67, 0x2C] )
        ret = self.controller.i2c( [REG_RD], read=2 )
        val = (ret[0]<<8)+ret[1]
        return val
  
    def readPressure( self ):
        self.controller.i2c( [REG_ADDR, 0xD0, 0x51, 0x2C] )
        ret = self.controller.i2c( [REG_RD], read=2 )
        val = (ret[0]<<8)+ret[1]
        if self.RANGE == 50:
            return float(val-1024)/60000.0*100-100/2.0
        elif self.RANGE == 500:
            return float(val-1024)/60000.0*1000-1000/2.0
        elif self.RANGE == 250:
            return float(val-1024)/60000.0*250

    def readTemperature( self ):
        self.controller.i2c( [REG_ADDR, 0xD0, 0x61, 0x2C] )
        ret = self.controller.i2c( [REG_RD], read=2 )
        val = (ret[0]<<8)+ret[1]
        return float(val-10214)/37.39


class D6F_PH0025AD1(D6F_PH):
    ADDR = 0x6C
    RANGE = 250

class D6F_PH5050AD3(D6F_PH):
    ADDR = 0x6C
    RANGE = 500

