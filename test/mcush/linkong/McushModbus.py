# coding:utf8
__doc__ = 'MCUSH controllers based on Modbus'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import time
import pymodbus
import pymodbus.client
import pymodbus.client.sync
import pymodbus.exceptions
from .. import Utils


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
    DEFAULT_TIMEOUT = 5

    def disconnect( self ):
        self.client.close()
 
    def updateInfo( self ):
        # read model string 
        _len = self.client.read_holding_registers(REG_MODEL_STR, 1).registers[0]
        model_str = ''.join(map(chr, self.client.read_holding_registers(REG_MODEL_STR+1, _len).registers))
        # read version string 
        _len = self.client.read_holding_registers(REG_VERSION_STR, 1).registers[0]
        version_str = ''.join(map(chr, self.client.read_holding_registers(REG_VERSION_STR+1, _len).registers))
          
        self.model = model_str
        self.version = version_str

    def errno( self, errno=None ):
        if errno is None:
            l, h = self.client.read_holding_registers(REG_ERRNO, 2).registers
            return Utils.I2i((h<<16)+l)
        else:
            self.client.write_registers(REG_ERRNO, [errno&0xFFFF, (errno>>16)&0xFFFF])

    def uptime( self ):
        l, h = self.client.read_holding_registers(REG_ERRNO, 2).registers
        return Utils.I2i((h<<16)+l)

    def getRebootCounter( self ):
        l, h = self.client.read_holding_registers(REG_REBOOT_COUNTER, 2).registers
        return Utils.I2i((h<<16)+l)

    def reboot( self, delay=None ):
        self.client.write_registers(REG_REBOOT_COMMAND, [1])
      
    def rtcSync( self ):
        self.client.write_registers(REG_RTC_YEAR, time.localtime()[:6])
        self.client.write_registers(REG_RTC_UPDATE, [1])
     
    def rtcRead( self ): 
        if int(self.client.read_holding_registers(REG_RTC_SYNCED, 1).registers[0]) == 0:
            return None  # not synced, rtc invalid
        year = int(self.client.read_holding_registers(REG_RTC_YEAR, 1).registers[0])
        mon = int(self.client.read_holding_registers(REG_RTC_MONTH, 1).registers[0])
        day = int(self.client.read_holding_registers(REG_RTC_DAY, 1).registers[0])
        hour = int(self.client.read_holding_registers(REG_RTC_HOUR, 1).registers[0])
        minute = int(self.client.read_holding_registers(REG_RTC_MINUTE, 1).registers[0])
        second = int(self.client.read_holding_registers(REG_RTC_SECOND, 1).registers[0])
        return (year, mon, day, hour, minute, second, 0, 1, -1)

 

class McushModbusRTU(McushModbus):

    def connect( self ):
        raise NotImplementedError()


class _ModbusTcpClient( pymodbus.client.sync.ModbusTcpClient ):
    # override reading_holding_registers that may take long time, reconnect after disconnected
    def read_holding_registers( self, address, count=1, **kwargs ):
        reconnect = 0
        while True:
            ret = pymodbus.client.sync.ModbusTcpClient.read_holding_registers(
                                        self, address, count, **kwargs ) 
            if isinstance(ret, pymodbus.exceptions.ModbusIOException):
                if self.auto_reconnect:
                    reconnect += 1
                    self.logger.info( 'reconnect... %d'% reconnect )
                    self.connect()
                    continue
                else:
                    raise Exception("Disconnected");
            return ret



class McushModbusTCP(McushModbus):
    DEFAULT_NET_PORT = 502
    DEFAULT_TIMEOUT = 30  # longer for network

    def connect( self ):
        self.client = _ModbusTcpClient( self.net_addr, port=self.net_port,
                                        timeout=self.DEFAULT_TIMEOUT )
        self.client.logger = self.logger
        self.client.auto_reconnect = self.auto_reconnect
        self.client.connect()
        self.updateInfo()


