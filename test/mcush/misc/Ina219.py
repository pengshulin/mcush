# coding: utf8
__doc__ = 'INA219 Current/Power Monitor'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils


REG_CONFIG  = 0
REG_SHUNT   = 1
REG_BUS     = 2
REG_POWER   = 3
REG_CURRENT = 4
REG_CALIB   = 5

ADC_9B      = 0
ADC_10B     = 1
ADC_11B     = 2
ADC_12B     = 8
ADC_12B_2   = 9
ADC_12B_4   = 10
ADC_12B_8   = 11
ADC_12B_16  = 12
ADC_12B_32  = 13
ADC_12B_64  = 14
ADC_12B_128 = 15

BRNG_16 = 0x0000
BRNG_32 = 0x2000

PG_1 = (0<<11) 
PG_2 = (1<<11) 
PG_4 = (2<<11) 
PG_8 = (3<<11) 

MODE_POWER_DOWN     = 0
MODE_SHUNT_TRIG     = 1
MODE_BUS_TRIG       = 2
MODE_SHUNT_BUS_TRIG = 3
MODE_ADC_OFF        = 4
MODE_SHUNT_CONT     = 5
MODE_BUS_CONT       = 6
MODE_SHUNT_BUS_CONT = 7



class Ina219:
 
    mode = MODE_SHUNT_BUS_CONT
    badc = ADC_12B_128  # 12 bit, slow mode
    sadc = ADC_12B_128  # 12 bit, slow mode
   
    rshunt = 0.1  # in ohms
   
    # ref: https://github.com/adafruit/Adafruit_INA219/blob/master/Adafruit_INA219.cpp
    # Adafruit_INA219::setCalibration_16V_400mA 
    brng = BRNG_16
    gain = PG_1
    vshunt_max = 0.32
    current_lsb = 1/20000.0
    power_lsb = 0.001
    volt_lsb = 0.001
    calib = 8192
     
    def __init__( self, controller, scl=None, sda=None, addr=0 ):
        self.controller = controller
        self.controller.i2c_init( 0x40+(addr&0xF), scl=scl, sda=sda )
        self.current_reg = None
        self.config()
       
    def writeReg( self, reg, dat ):
        self.controller.i2c( [reg, dat>>8, dat&0xFF] )
        self.current_reg = reg
  
    def readReg( self, reg ):
        if self.current_reg != reg:
            r = self.controller.i2c( [reg], read=2 )
            self.current_reg = reg
        else:
            r = self.controller.i2c( read=2 )
        return (r[0]<<8)+r[1]

    def config( self ):
        cfg = self.brng + self.gain + (self.badc<<7) + (self.sadc<<3) + self.mode
        self.writeReg( REG_CONFIG, cfg )
        self.writeReg( REG_CALIB, self.calib )

    def readAllRegs( self ):
        ret = {}
        ret['config'] = self.readReg( REG_CONFIG )
        ret['shunt'] = self.readReg( REG_SHUNT )
        ret['bus'] = self.readReg( REG_BUS )
        ret['power'] = self.readReg( REG_POWER )
        ret['current'] = self.readReg( REG_CURRENT )
        ret['calib'] = self.readReg( REG_CALIB )
        return ret

    def readVoltage( self ):
        v = self.readReg( REG_BUS )
        if v & 1:
            raise 'volt overflow'
        return (v>>1)*self.volt_lsb

    def readCurrent( self ):
        v = Utils.H2h(self.readReg( REG_CURRENT ))
        return v*self.current_lsb

    def readPower( self ):
        v = self.readReg( REG_POWER )
        return v*self.power_lsb

