# coding: utf8
__doc__ = 'products from Pingyang Miaoguan Tech.'
__author__ = 'Peng Shulin <trees_peng@163.com>'
import re
import time
from .. import Env, Instrument, Utils
import pymodbus
import pymodbus.client
import pymodbus.client.sync
from pymodbus.client.sync import ModbusSerialClient
from pymodbus.exceptions import ModbusIOException

DEFAULT_UNIT = 0x01  # default instrument address



class TH10S_B( Instrument.SerialInstrument ):
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
         
    def readADC( self, addr ):
        response = self.client.read_input_registers(addr, 1, unit=self.unit)
        if isinstance(response, ModbusIOException):
            raise Instrument.CommandTimeoutError
        val = response.registers[0]
        if val == 0x8000:
            raise Exception('Probe error')
        return val        
        
    def readTemperature( self ):
        val = self.readADC(0)
        val = Utils.s2i(Utils.I2s(val))
        return round(val*0.1, 1)

    def readHumidity( self,  ):
        return round(self.readADC(1)*0.1, 1)

