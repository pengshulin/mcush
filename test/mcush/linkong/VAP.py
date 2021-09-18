# coding:utf8
__doc__ = 'Vibration Analysis Platform Series'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Mcush, Utils, Env
#from . import McushModbus
from .McushModbus import *
import time
import logging
import math

class _vap( Mcush.Mcush ):
    DEFAULT_NAME = 'VAP'
    DEFAULT_IDN = re_compile( 'VAP.*,([0-9]+\.[0-9]+.*)' )

    def getDeviceName( self ):
        ret = self.writeCommand( '*idn?' )
        try:
            self.device_name = ret[2].strip()
            self.logger.info( 'DeviceName:%s', str(self.device_name) )
            return self.device_name
        except IndexError:
            self.device_name = None
            return ''

    def parseEnv( self, info ):
        var = {}
        for l in info:
            ab = l.split(':')
            if len(ab) == 2:
                a, b = ab
            elif len(ab) == 1:
                a, b = ab[0], ''
            var[a.strip()] = b.strip()
        return var
        
    def env( self ):
        return self.parseEnv( self.writeCommand('env') )
 
    def daq( self, command, index=None, value=None ):
        cmd = 'daq -c %s'% command
        if index is not None:
            cmd += ' -i %d'% index
        if value is not None:
            cmd += ' -v 0x%X'% value
        return self.writeCommand( cmd )


class VAP100( _vap):
    '''VAP 1xx Series'''
    # low cost MEMS sensor
    DEFAULT_NAME = 'VAP100'
    DEFAULT_IDN = re_compile( 'VAP1[0-9]*.*,([0-9]+\.[0-9]+.*)' )

# NOTE: this process the original ADC value read from SRAM, 
# head flags (channel index/filter settled/...) can be checked in detail
# kept here just for debugging, this will be obselected in the future
# data will be locally converted and read out only in float mode
__CHANNEL_REMAP = [7,6,5,4,0,1,2,3]
__CHANNEL_POLARITY_INV = [0,0,0,0,1,1,1,1]
def _process_vap200_adc_val( obj, channel, index, adc, float_mode ):
    # check head index
    head = adc >> 24
    adc_idx = head & 0x7
    if adc_idx != __CHANNEL_REMAP[channel]:
        info = 'data index not match %d(%d required)'% (adc_idx, __CHANNEL_REMAP[channel])
        obj.logger.error(info)
        raise Exception(info)
    # check value
    adc_val = Utils.I2i((adc&0xFFFFFF)<<8)/256
    if __CHANNEL_POLARITY_INV[channel]:
        adc_val = -adc_val
    if float_mode:
        adc_fval = adc_val * 5.0 / (2**23) * 5.0
        #obj.logger.debug('%d 0x%08X -> %d -> %f'% (index, adc, adc_val, adc_fval))
        return adc_fval
    else:
        #obj.logger.debug('%d 0x%08X -> %d'% (index, adc, adc_val))
        return adc_val




class VAP200( _vap ):
    '''VAP 2xx Series'''
    # 8 synchronous channels with external trigger
    # 24-bit signal-delta ADC, synchronous sampling
    # sample rate up to 204ksps
    # hardware low-pass filter at 45kHz for accelerator channel
    # hardware integral circuit for speed measurement
    DEFAULT_NAME = 'VAP200'
    DEFAULT_IDN = re_compile( 'VAP2[0-9]*.*,([0-9]+\.[0-9]+.*)' )
       
    def ad7768( self, command, index=None, value=None ):
        cmd = 'ad7768 -c %s'% command
        if index is not None:
            cmd += ' -i %d'% index
        if value is not None:
            cmd += ' -v 0x%X'% value
        return self.writeCommand( cmd )
   
    def recordReset( self ):
        self.ad7768( 'record_reset' )

    def recordStart( self ):
        self.ad7768( 'record_start' )

    def recordStop( self ):
        self.ad7768( 'record_stop' )

    def recordWaitForDone( self, wait_tick=0.1 ):
        while True:
            if int(self.ad7768('record_done')[0]):
                break
            time.sleep(wait_tick)

    def recordFloatConvert( self, index=None ):
        self.ad7768( 'record_fconv', index=index )

    def setSampleRate( self, sample_rate=25600 ):
        self.ad7768( 'record_sample_rate', value=sample_rate )

    def getSampleRate( self ):
        return float(self.ad7768('record_sample_rate')[0])

    def setChannelBitmap( self, channel=0xFF ):
        self.ad7768( 'record_channel', value=channel )

    def getChannelBitmap( self ):
        return int(self.ad7768('record_channel')[0], 16)

    def setSampleLength( self, length=1024 ):
        self.ad7768( 'record_length', value=length )

    def getSampleLength( self ):
        return int(self.ad7768('record_length')[0],16)

    def getRecordBuffers( self ):
        ret = []
        for line in self.ad7768('record_buffer'):
            line = line.strip()
            if len(line) >= 12:
                a, b = line.split(' ')
                ret.append( [int(a), int(b, 16)] ) 
        return ret

    def readBufferMem( self, channel, float_mode=True ):
        dat, dat_addr = [], None
        for ch, addr in self.getRecordBuffers():
            if ch == channel:
                dat_addr = addr
        if dat_addr is None:
            raise Exception("channel %d is not in buffer list"% channel)
        sample_length = self.getSampleLength()
        self.logger.info( "read memory from 0x%08X, length %d"% (dat_addr,sample_length*4) )
        mem = self.readMem( dat_addr, length=sample_length*4, width=4, compact_mode=True)
        for i in range(sample_length):
            adc = Utils.s2I(mem[i*4:i*4+4])
            dat.append( _process_vap200_adc_val(self, channel, i, adc, float_mode) )
        return dat

    def enterRemoteMode( self ):
        self.daq( 'remote', value=1 )

    def exitRemoteMode( self ):
        self.daq( 'remote', value=0 )

    def setAlarmOutput( self, on=True ):
        self.daq( 'pwr_r', value=int(bool(on)) )
 
    def getAlarmOutput( self ):
        return int(self.daq( 'pwr_r' )[0])

    def setSensorPower( self, on=True ):
        self.daq( 'pwr_s', value=int(bool(on)) )
 
    def getSensorPower( self ):
        return int(self.daq( 'pwr_s' )[0])

    # to be obseleted, only for debug
    def setExtendPower( self, on=True ):
        self.daq( 'pwr_e', value=int(bool(on)) )

    def setSpeedBitmap( self, bitmap=0x00 ):
        self.daq( 'speed', value=bitmap )
    
    def getSpeedBitmap( self ):
        return int(self.daq( 'speed' )[0])
 
    def setMCLK( self, mclk_idx=0 ):
        self.daq( 'mclk', value=mclk_idx )

    def setTachoLEDEnable( self, enable=True ):
        self.daq( 'tacho_led', value=int(bool(enable)) )

    def setTachoMeasEnable( self, enable=True ):
        self.daq( 'tacho_meas', value=int(bool(enable)) )

    def setTachoTrigEnable( self, enable=True ):
        self.daq( 'tacho_trig', value=int(bool(enable)) )

    def isTachoWaiting( self ):
        return bool(int(self.daq( 'tacho_waiting' )[0]))

    def isTachoTriggered( self ):
        return bool(int(self.daq( 'tacho_triggered' )[0]))

    def tachoStart( self ):
        self.daq( 'tacho_start' )
    
    def tachoStop( self ):
        self.daq( 'tacho_stop' )

    def getTachoFreq( self ):
        val = self.daq('tacho')[0]
        if val == 'disable':
            return None
        return float(val)

    def readSwitchConfig( self ):
        return int(self.daq('config')[0])
 
    def daqLock( self ):
        self.daq('lock')

    def daqUnlock( self ):
        self.daq('unlock')

    def daqEnable( self ):
        self.daq('enable')

    def daqDisable( self ):
        self.daq('disable')

    def daqIsBusy( self ):
        return bool(int(self.daq('busy')[0]))
   
    def daqChannelInfo( self, info=None ):
        ret = {}
        if info is None:
            info = self.daq('info')
        for l in info:
            if l.startswith('#'):
                a, b, c = l.split('  ')
                idx = int(a.lstrip('#'))
                avg = float(b.split(': ')[-1]) 
                rms = float(c.split(': ')[-1]) 
                ret[idx] = {'offset': avg, 'rms': rms}
        return ret
                 

# general group
REG_REMOTE_MODE         = 10
REG_POWER_SENSOR        = 11
REG_ALARM_OUTPUT        = 12
REG_CHANNEL_BITMAP      = 13
REG_SPEED_BITMAP        = 14
REG_MCLK_MODE           = 15
REG_SAMPLE_RATE_X10     = 16
REG_LOCAL_COUNTER       = 17
# tacho group
REG_TACHO_LED_EN        = 20
REG_TACHO_MEAS_EN       = 21
REG_TACHO_TRIG_EN       = 22
REG_TACHO_START         = 23
REG_TACHO_RPM           = 24
REG_TACHO_WAITING       = 25
REG_TACHO_TRIGGERED     = 26
# record group
REG_RECORD_LENGTH       = 30
REG_RECORD_RUN          = 31
REG_RECORD_DONE         = 32
REG_RECORD_FLOAT_CONV   = 33
REG_BUFFER_MAP_SEL      = 34
# local group
REG_LOCAL_OFFSET_MV_0       = 40
REG_LOCAL_OFFSET_MV_1       = 41
REG_LOCAL_OFFSET_MV_2       = 42
REG_LOCAL_OFFSET_MV_3       = 43
REG_LOCAL_OFFSET_MV_4       = 44
REG_LOCAL_OFFSET_MV_5       = 45
REG_LOCAL_OFFSET_MV_6       = 46
REG_LOCAL_OFFSET_MV_7       = 47
REG_LOCAL_RMS_MV_0          = 50
REG_LOCAL_RMS_MV_1          = 51
REG_LOCAL_RMS_MV_2          = 52
REG_LOCAL_RMS_MV_3          = 53
REG_LOCAL_RMS_MV_4          = 54
REG_LOCAL_RMS_MV_5          = 55
REG_LOCAL_RMS_MV_6          = 56
REG_LOCAL_RMS_MV_7          = 57
REG_LOCAL_THRESHOLD_MV_0    = 60
REG_LOCAL_THRESHOLD_MV_1    = 61
REG_LOCAL_THRESHOLD_MV_2    = 62
REG_LOCAL_THRESHOLD_MV_3    = 63
REG_LOCAL_THRESHOLD_MV_4    = 64
REG_LOCAL_THRESHOLD_MV_5    = 65
REG_LOCAL_THRESHOLD_MV_6    = 66
REG_LOCAL_THRESHOLD_MV_7    = 67

# waveform buffer mapping
REG_BUFFER_BEGIN        = 10000
REG_BUFFER_END          = 59999

REG_RECORD_LENGTH_MAX = int((REG_BUFFER_END-REG_BUFFER_BEGIN+1)/2)

 

class VAP200_ModbusTCP( McushModbusTCP ):
    '''VAP 2xx Series over ModbusTCP'''
    DEFAULT_NAME = 'VAP_Modbus'

    def __init__( self, net_addr, *args, **kwargs ):
        # logging level 
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

        self.net_addr = net_addr
        self.net_port = self.DEFAULT_NET_PORT
        self.model = None
        self.version = None
        self.sn = None
        self.sample_length = None
        self.sample_rate = None
        self.auto_reconnect = None

        if 'timeout' in kwargs:
            self.DEFAUT_TIMEOUT = kwargs['timeout']
        if kwargs.get('auto_reconnect', None):
            self.auto_reconnect = True
        if kwargs.get('connect', True):
            self.connect()

    def enterRemoteMode( self ):
        self.client.write_registers(REG_REMOTE_MODE, [1])
    
    def exitRemoteMode( self ):
        self.client.write_registers(REG_REMOTE_MODE, [0])
       
    def setAlarmOutput( self, on=True ):
        self.client.write_registers(REG_ALARM_OUTPUT, [int(bool(on))])
 
    def getAlarmOutput( self ):
        return bool(self.client.read_holding_registers(REG_ALARM_OUTPUT, 1).registers[0])

    def setSensorPower( self, on=True ):
        self.client.write_registers(REG_POWER_SENSOR, [int(bool(on))])
    
    def getSensorPower( self ):
        return bool(self.client.read_holding_registers(REG_POWER_SENSOR, 1).registers[0])
 
    def setSpeedBitmap( self, bitmap=0x00 ):
        self.client.write_registers(REG_SPEED_BITMAP, [bitmap] )
 
    def getSpeedBitmap( self ):
        return self.client.read_holding_registers(REG_SPEED_BITMAP, 1).registers[0]

    def setTachoLEDEnable( self, enable=True ):
        self.client.write_registers(REG_TACHO_LED_EN, [int(bool(enable))])
 
    def setTachoMeasEnable( self, enable=True ):
        self.client.write_registers(REG_TACHO_MEAS_EN, [int(bool(enable))])
    
    def setTachoTrigEnable( self, enable=True ):
        self.client.write_registers(REG_TACHO_TRIG_EN, [int(bool(enable))])

    def isTachoWaiting( self ):
        return bool(self.client.read_holding_registers(REG_TACHO_WAITING, 1).registers[0])

    def isTachoTriggered( self ):
        return bool(self.client.read_holding_registers(REG_TACHO_TRIGGERED, 1).registers[0])

    def tachoStart( self ):
        self.client.write_registers(REG_TACHO_START, [1])
    
    def tachoStop( self ):
        self.client.write_registers(REG_TACHO_START, [0])

    def getTachoFreq( self ):
        val = self.client.read_holding_registers(REG_TACHO_RPM, 1).registers[0]
        if val == 65535:
            return None
        return float(val)/60.0

    def recordStart( self ):
        self.client.write_registers(REG_RECORD_RUN, [1])

    def recordStop( self ):
        self.client.write_registers(REG_RECORD_RUN, [0])

    def setMCLK( self, mclk_idx=0 ):
        self.client.write_registers(REG_MCLK_MODE, [mclk_idx])

    def setSampleRate( self, sample_rate=25600 ):
        self.client.write_registers(REG_SAMPLE_RATE_X10, [int(sample_rate/10.0)])
        self.sample_rate = sample_rate

    def getSampleRate( self ):
        self.sample_rate = self.client.read_holding_registers(REG_SAMPLE_RATE_X10, 1).registers[0]*10.0
        return self.sample_rate

    def setSampleLength( self, length=1024 ):
        if length > REG_RECORD_LENGTH_MAX:
            # due to modbus reg width, waveform buffer is mapped at limited area
            raise Exception('sample lenght no more than %d'% REG_RECORD_LENGTH_MAX)
        self.client.write_registers(REG_RECORD_LENGTH, [length])
        self.sample_length = length

    def getSampleLength( self ):
        self.sample_length = self.client.read_holding_registers(REG_RECORD_LENGTH, 1).registers[0]
        return self.sample_length

    def setChannelBitmap( self, bitmap ):
        self.client.write_registers(REG_CHANNEL_BITMAP, [bitmap])

    def getChannelBitmap( self ):
        return self.client.read_holding_registers(REG_CHANNEL_BITMAP, 1).registers[0]
 
    def recordWaitForDone( self, wait_tick=0.1 ):
        while True:
            if self.client.read_holding_registers(REG_RECORD_DONE, 1).registers[0]: 
                break
            time.sleep(wait_tick)

    def recordFloatConvert( self, index=None ):
        self.client.write_registers(REG_RECORD_FLOAT_CONV, [0xFFFF if index is None else index])

    def readBufferMem( self, channel, float_mode=True ):
        if float_mode:
            self.recordFloatConvert( channel )
        self.client.write_registers(REG_BUFFER_MAP_SEL, [channel])
        mem, read = [], 0
        while read < self.sample_length*2:
            required = self.sample_length*2 - read
            if required > 125:
                required = 125
            mem.extend( self.client.read_holding_registers(REG_BUFFER_BEGIN+read, required).registers )
            read += required 
        dat = []
        for i in range(self.sample_length):
            if float_mode:
                dat.append( Utils.I2f((mem[i*2+1]<<16)+mem[i*2]) )
            else:
                # NOTE: raw int24 processing, will be obsoleted
                adc = (mem[i*2+1]<<16)+mem[i*2]
                dat.append( _process_vap200_adc_val(self, channel, i, adc, False) )
        return dat

   
