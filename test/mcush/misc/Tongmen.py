# coding: utf8
__doc__ = 'products from Dongguan Tongmen Electronics Tech.'
__author__ = 'Peng Shulin <trees_peng@163.com>'
from .. import Env, Utils, Modbus


REG_OUTPUT      = 0x0001
REG_PROTECT     = 0x0002
REG_MODEL       = 0x0003
REG_CLASS       = 0x0004
REG_DIGIT       = 0x0005
REG_VOLTAGE_DISP = 0x0010
REG_CURRENT_DISP = 0x0011
REG_POWER_DISP   = 0x0011
REG_VOLTAGE_SET = 0x0030
REG_CURRENT_SET = 0x0031
REG_OVP_SET = 0x0020
REG_OCP_SET = 0x0021
REG_OPP_SET = 0x0022


class ETM( Modbus.ModbusRTU ):
    DEFAULT_NAME = 'TH10S_B'
    
    def outputEnable( self ):
        self.writeReg(REG_OUTPUT, 1)

    def outputDisable( self ):
        self.writeReg(REG_OUTPUT, 0)

    def output( self, volt=None, amp=None, enable=True ):
        if volt is not None:
            volt = float(volt)
            if volt < 0 or volt > self.VMAX:
                raise Exception('Volt out of range')
            self.writeReg(REG_VOLTAGE_SET, int(volt*100))
        if amp is not None:
            amp = float(amp)
            if amp < 0 or amp > self.IMAX:
                raise Exception('Current out of range')
            self.writeReg(REG_CURRENT_SET, int(amp*1000))
        if enable is not None:
            if enable:
                self.outputEnable()
            else:
                self.outputDisable()
        
    def getSetting( self ):
        volt = self.readReg(REG_VOLTAGE_SET) / 100.0
        amp = self.readReg(REG_CURRENT_SET) / 1000.0
        return volt, amp

    def readVoltage( self ):
        return self.readReg(REG_VOLTAGE_DISP) / 100.0
         
    def readCurrent( self ):
        return self.readReg(REG_CURRENT_DISP) / 1000.0

    def readPower( self ):
        return self.readUInt32(REG_POWER_DISP) / 1000.0

    def read( self ):
        return (self.readVoltage(), self.readCurrent())


# low-power switch series

class ETM1520P( ETM ):
    DEFAULT_NAME = 'ETM1520P'
    VMAX = 15.0
    IMAX = 20.0

class ETM305P( ETM ):
    DEFAULT_NAME = 'ETM305P'
    VMAX = 30.0
    IMAX = 5.0

class ETM3010P( ETM ):
    DEFAULT_NAME = 'ETM3010P'
    VMAX = 30.0
    IMAX = 10.0

class ETM605P( ETM ):
    DEFAULT_NAME = 'ETM605P'
    VMAX = 60.0
    IMAX = 5.0

class ETM1003P( ETM ):
    DEFAULT_NAME = 'ETM1003P'
    VMAX = 100.0
    IMAX = 3.0

class ETM1502P( ETM ):
    DEFAULT_NAME = 'ETM1502P'
    VMAX = 150.0
    IMAX = 2.0


# low-power linear series

class ETM303DP( ETM ):
    DEFAULT_NAME = 'ETM303DP'
    VMAX = 30.0
    IMAX = 3.0

class ETM305DP( ETM ):
    DEFAULT_NAME = 'ETM305DP'
    VMAX = 30.0
    IMAX = 5.0

class ETM603DP( ETM ):
    DEFAULT_NAME = 'ETM603DP'
    VMAX = 60.0
    IMAX = 3.0

class ETM605DP( ETM ):
    DEFAULT_NAME = 'ETM605DP'
    VMAX = 60.0
    IMAX = 5.0

class ETM1501DP( ETM ):
    DEFAULT_NAME = 'ETM1501DP'
    VMAX = 150.0
    IMAX = 1.0


# mini series

class ETM305MP( ETM ):
    DEFAULT_NAME = 'ETM305MP'
    VMAX = 30.0
    IMAX = 5.0

class ETM3010MP( ETM ):
    DEFAULT_NAME = 'ETM3010MP'
    VMAX = 30.0
    IMAX = 10.0

class ETM605MP( ETM ):
    DEFAULT_NAME = 'ETM605MP'
    VMAX = 60.0
    IMAX = 5.0



