# coding: utf8
__doc__ = 'products from Dongguan Tongmen Electronics Tech.'
__author__ = 'Peng Shulin <trees_peng@163.com>'
import re
import time
from .. import Env, Instrument
import pymodbus
import pymodbus.client
import pymodbus.client.sync
from pymodbus.client.sync import ModbusSerialClient
from pymodbus.exceptions import ModbusIOException

DEFAULT_UNIT = 0x01  # default instrument address

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


class ETM( Instrument.SerialInstrument ):
    DEFAULT_CHECK_RETURN_COMMAND = False
    DEFAULT_IDN = re.compile( '.*' )

    def __init__( self, *args, **kwargs ):
        kwargs['baudrate'] = 9600  # this is fixed
        if not 'unit' in kwargs:
            kwargs['unit'] = DEFAULT_UNIT
        Instrument.SerialInstrument.__init__( self, *args, **kwargs ) 

    def connect( self ):
        self.client = ModbusSerialClient('rtu', port=self.port.port, baudrate=self.port.baudrate, timeout=self.port.timeout)
        self.client.connect()
         
    def outputEnable( self ):
        self.client.write_register(REG_OUTPUT, 1, unit=self.unit)

    def outputDisable( self ):
        self.client.write_register(REG_OUTPUT, 0, unit=self.unit)
 
    def output( self, volt=None, amp=None, enable=True ):
        if volt is not None:
            volt = float(volt)
            if volt < 0 or volt > self.VMAX:
                raise Exception('Volt out of range')
            self.client.write_register(REG_VOLTAGE_SET, int(volt*100), unit=self.unit)
        if amp is not None:
            amp = float(amp)
            if amp < 0 or amp > self.IMAX:
                raise Exception('Current out of range')
            self.client.write_register(REG_CURRENT_SET, int(amp*1000), unit=self.unit)
        if enable is not None:
            if enable:
                self.outputEnable()
            else:
                self.outputDisable()
        
    def getSetting( self ):
        response = self.client.read_holding_registers(REG_VOLTAGE_SET, 1, unit=self.unit)
        if isinstance(response, ModbusIOException):
            raise Instrument.CommandTimeoutError
        regs = response.registers
        volt = regs[0] / 100.0
        response = self.client.read_holding_registers(REG_CURRENT_SET, 1, unit=self.unit)
        if isinstance(response, ModbusIOException):
            raise Instrument.CommandTimeoutError
        regs = response.registers
        amp = regs[0] / 1000.0
        return float(volt), float(amp)

    def readVoltage( self ):
        response = self.client.read_holding_registers(REG_VOLTAGE_DISP, 1, unit=self.unit)
        if isinstance(response, ModbusIOException):
            raise Instrument.CommandTimeoutError
        regs = response.registers
        return regs[0] / 100.0
         
    def readCurrent( self ):
        response = self.client.read_holding_registers(REG_CURRENT_DISP, 1, unit=self.unit)
        if isinstance(response, ModbusIOException):
            raise Instrument.CommandTimeoutError
        regs = response.registers
        return regs[0] / 1000.0

    def readPower( self ):
        response = self.client.read_holding_registers(REG_CURRENT_DISP, 2, unit=self.unit)
        if isinstance(response, ModbusIOException):
            raise Instrument.CommandTimeoutError
        regs = response.registers
        return ((regs[0]<<16)+regs[1]) / 1000.0

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



