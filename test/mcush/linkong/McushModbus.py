# coding:utf8
__doc__ = 'MCUSH controllers based on Modbus'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import pymodbus
import pymodbus.client
import pymodbus.client.sync


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

    def errno( self, errno ):
        self.client.write_registers(REG_ERRNO_VAL, [errno&0xFFFF, (errno>>16)&0xFFFF])


class McushModbusRTU(McushModbus):

    def connect( self ):
        raise NotImplementedError()


class McushModbusTCP(McushModbus):
    DEFAULT_NET_PORT = 502
    DEFAULT_TIMEOUT = 30  # longer for network
 
    def connect( self ):
        self.client = pymodbus.client.sync.ModbusTcpClient(
                        self.net_addr, port=self.net_port,
                        timeout=self.DEFAULT_TIMEOUT )
        self.client.connect()
        self.updateInfo()


