# coding: utf8
__doc__ = 'products from Xian Zhouzhen Electronics Tech.'
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



class DAQM9004( Instrument.SerialInstrument ):
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
         
    def readADC( self ):
        response = self.client.read_input_registers(0x0000, 8, unit=self.unit)
        if isinstance(response, ModbusIOException):
            raise Instrument.CommandTimeoutError
        return response.registers

    def readVolt( self,  ):
        return [ val/4095.0*5.0 for val in self.readADC() ]

    def readAmp( self ):
        return [ val/4095.0*0.02 for val in self.readADC() ]




