# coding:utf8
__doc__ = 'MCUSH controllers based on Modbus'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import time
import logging
import pymodbus
import pymodbus.client
import pymodbus.client.sync
import pymodbus.exceptions
from pymodbus.client.sync import ModbusSerialClient, ModbusTcpClient
from pymodbus.exceptions import ModbusIOException
from . import Env, Utils


class ModbusCommon():
    EXCEPTION_CODE = {}
    DEFAULT_NAME = "Modbus"
    DEFAULT_AUTO_RECONNECT = False
    EXCEPTION_CODE = {
        1: 'illegal function code',
        2: 'illegal address',
        3: 'illegal data',
        4: 'server error',
        5: 'server confirm',
        6: 'server busy',
        }

    def initLogger(self):
        self.verbose = Env.VERBOSE
        self.debug = Env.DEBUG
        self.info = Env.INFO
        self.warning = Env.WARNING
        if self.debug:
            level = logging.DEBUG
        elif self.info:
            level = logging.INFO
        elif self.warning:
            level = logging.WARNING
        else:
            level = logging.ERROR
        logging.basicConfig( level=level, format=Env.LOG_FORMAT, datefmt=Env.LOG_DATEFMT )
        self.logger = logging.getLogger( self.DEFAULT_NAME )

    def disconnect( self ):
        self.client.close()

    def checkRet( self, ret ):
        if not ret.isError():
            return ret
        try:
            code = ret.exception_code
            err = "error_code=%d"% code
            if code in self.EXCEPTION_CODE:
                err += ' (%s)'% self.EXCEPTION_CODE[code]
        except AttributeError:
            err = ret.message
        raise Exception(err)

    def writeReg( self, addr, val ):
        if self.unit is None:
            return self.checkRet( self.client.write_register(addr, int(val) ) )
        else:
            return self.checkRet( self.client.write_register(addr, int(val), unit=self.unit ) )

    def writeRegs( self, addr, vals ):
        if self.unit is None:
            return self.checkRet( self.client.write_registers(addr, vals ) )
        else:
            return self.checkRet( self.client.write_registers(addr, vals, unit=self.unit ) )

    def readInputReg( self, addr ):
        if self.unit is None:
            ret = self.checkRet( self.client.read_input_registers(addr, 1) )
        else:
            ret = self.checkRet( self.client.read_input_registers(addr, 1, unit=self.unit) )
        return ret.registers[0]

    def readInputRegs( self, addr, length=1 ):
        if self.unit is None:
            ret = self.checkRet( self.client.read_input_registers(addr, length) )
        else:
            ret = self.checkRet( self.client.read_input_registers(addr, length, unit=self.unit) )
        return ret.registers

    def readReg( self, addr ):
        if self.unit is None:
            ret = self.checkRet( self.client.read_holding_registers(addr, 1) )
        else:
            ret = self.checkRet( self.client.read_holding_registers(addr, 1, unit=self.unit) )
        return ret.registers[0]

    def readRegs( self, addr, length=1 ):
        if self.unit is None:
            ret = self.checkRet( self.client.read_holding_registers( addr, length) )
        else:
            ret = self.checkRet( self.client.read_holding_registers( addr, length, unit=self.unit) )
        return ret.registers

    def readUInt32( self, addr ):
        l, h = self.readRegs(addr, 2)
        return (h<<16) + l
    
    def writeUInt32( self, addr, value ):
        self.writeRegs( addr, [value&0xFFFF, (value>>16)&0xFFFF])

    def readInt32( self, addr ):
        return Utils.I2i( self.readUInt32(addr) )

    def readFloat32( self, addr ):
        return Utils.I2f(self.readUInt32(addr))

    def writeFloat32( self, addr, val ):
        self.writeUInt32( addr, Utils.f2I(val) )

    def readString( self, addr, length ):
        return ''.join(map(chr, self.readRegs(addr, length)))

  
class ModbusRTU( ModbusCommon ):
    DEFAULT_NAME = 'ModbusRTU'
    DEFAULT_BAUDRATE = 9600
    DEFAULT_TIMEOUT = 5
    DEFAULT_UNIT = 1

    def __init__( self, port, *args, **kwargs ):
        self.port = port
        self.baudrate = kwargs.get('baudrate', self.DEFAULT_BAUDRATE)
        self.timeout = kwargs.get('timeout', self.DEFAULT_TIMEOUT)
        self.unit = kwargs.get('unit', self.DEFAULT_UNIT)
        self.auto_reconnect = kwargs.get('auto_reconnect', self.DEFAULT_AUTO_RECONNECT)
        self.initLogger()
        if kwargs.get('connect', True):
            self.connect()

    def connect( self ):
        self.client = ModbusSerialClient('rtu', port=self.port, baudrate=self.baudrate, timeout=self.timeout)
        self.client.logger = self.logger
        self.client.connect()



class ModbusTCP( ModbusCommon ):
    DEFAULT_TIMEOUT = 30  # longer for network
    DEFAULT_NET_PORT = 502

    def __init__( self, net_addr, *args, **kwargs ):
        self.net_addr = net_addr
        self.net_port = kwargs.get('net_port', self.DEFAULT_NET_PORT)
        self.timeout = kwargs.get('timeout', self.DEFAULT_TIMEOUT)
        self.unit = None
        self.auto_reconnect = kwargs.get('auto_reconnect', self.DEFAULT_AUTO_RECONNECT)
        self.initLogger()
        if kwargs.get('connect', True):
            self.connect()

    def connect( self ):
        self.client = ModbusTcpClient( self.net_addr, port=self.net_port, timeout=self.timeout )
        self.client.logger = self.logger
        self.client.connect()



#############################################################################
# MCUSH platform general purpose registers
REG_MODEL_STR       = 60000
REG_VERSION_STR     = 60020
REG_SN_STR          = 60040
REG_ERRNO           = 60070  # 2 words, int32_t
REG_UPTIME          = 60072  # 2 words, uint32_t
REG_REBOOT_COUNTER  = 60074  # 2 words, uint32_t
REG_REBOOT_COMMAND  = 60076
REG_RTC_SYNCED      = 60080
REG_RTC_UPDATE      = 60081
REG_RTC_YEAR        = 60082
REG_RTC_MONTH       = 60083
REG_RTC_DAY         = 60084
REG_RTC_HOUR        = 60085
REG_RTC_MINUTE      = 60086
REG_RTC_SECOND      = 60087



class McushModbus():

    def updateInfo( self ):
        self.model = self.readString(REG_MODEL_STR+1, self.readReg(REG_MODEL_STR))
        self.version = self.readString(REG_VERSION_STR+1, self.readReg(REG_VERSION_STR))
        self.sn = self.readString(REG_SN_STR+1, self.readReg(REG_SN_STR))

    def errno( self, errno=None ):
        if errno is None:
            return self.readInt32(REG_ERRNO)
        else:
            self.writeUInt32(REG_ERRNO, errno&0xFFFFFFFF)

    def uptime( self ):
        return self.readInt32(REG_UPTIME)

    def getRebootCounter( self ):
        return self.readInt32(REG_REBOOT_COUNTER)

    def reboot( self, delay=None ):
        self.writeReg(REG_REBOOT_COMMAND, 1)
      
    def rtcSync( self ):
        self.writeRegs(REG_RTC_YEAR, time.localtime()[:6])
        self.writeReg(REG_RTC_UPDATE, 1)
     
    def rtcRead( self ): 
        if self.readReg(REG_RTC_SYNCED) == 0:
            return None  # not synced, rtc invalid
        year, mon, day, hour, minute, sec = self.readRegs(REG_RTC_YEAR, 6)
        return (year, mon, day, hour, minute, sec, 0, 1, -1)



class McushModbusRTU(McushModbus, ModbusRTU):
    def connect( self ):
        ModbusRTU.connect(self)
        self.updateInfo()


class McushModbusTCP(McushModbus, ModbusTCP):
    def connect( self ):
        ModbusTCP.connect(self)
        self.updateInfo()

