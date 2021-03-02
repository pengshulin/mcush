# coding:utf8
__doc__ = 'Vibration Analysis Platform Series'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Mcush, Utils
import time


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
 
    def setSampleRate( self, sample_rate=25600 ):
        self.ad7768( 'record_sample_rate', value=sample_rate )

    def getSampleRate( self ):
        return float(self.ad7768('record_sample_rate')[0])

    def setChannel( self, channel=0xFF ):
        self.ad7768( 'record_channel', value=channel )

    def getChannel( self ):
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

    CHANNEL_REMAP = [7,6,5,4,0,1,2,3]
    CHANNEL_POLARITY_INV = [0,0,0,0,1,1,1,1]

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
        inv = self.CHANNEL_POLARITY_INV[channel]
        for i in range(sample_length):
            offset=i*4
            adc = Utils.s2I(mem[offset:offset+4])
            head = adc>>24
            # check head index
            adc_idx = head&0x7
            if adc_idx != self.CHANNEL_REMAP[channel]:
                info = 'data index not match %d(%d required) @+0x%08X'% (adc_idx, self.CHANNEL_REMAP[channel],i*4)
                self.logger.error(info)
                raise Exception(info)
            # check value
            adc_val = Utils.I2i((adc&0xFFFFFF)<<8)/256
            if inv:
                adc_val = -adc_val
            if float_mode:
                adc_fval = adc_val * 5.0 / (2**23) * 5.0
                self.logger.debug('%d 0x%08X -> %d -> %f'% (i, adc, adc_val, adc_fval))
                dat.append(adc_fval)
            else:
                self.logger.debug('%d 0x%08X -> %d'% (i, adc, adc_val))
                dat.append(adc_val)
        return dat

    def enterRemoteMode( self ):
        self.daq( 'remote', value=1 )

    def exitRemoteMode( self ):
        self.daq( 'remote', value=0 )

    def setSensorPower( self, on=True ):
        self.daq( 'pwr_s', value=int(on) )
 
    def setSensorSpeedMode( self, mode=0x00 ):
        self.daq( 'speed_mode', value=mode )
 
    def daq_lock( self ):
        self.daq('lock')

    def daq_unlock( self ):
        self.daq('unlock')

    def daq_enable( self ):
        self.daq('enable')

    def daq_disable( self ):
        self.daq('disable')

    def daq_busy( self ):
        return bool(int(self.daq('busy')[0]))

    def daq_tacho( self, info=None ):
        val = None
        if info is None:
            info = self.daq('info')
        for l in info:
            if l.startswith('tac:'):
                val = float(l.split(': ')[1].strip())
                break
        return val
    
    def daq_channel_info( self, info=None ):
        ret = {}
        if info is None:
            info = self.daq('info')
        for l in info:
            if l.startswith('#'):
                a, b, c, d = l.split('  ')
                idx = int(a.lstrip('#'))
                cnt = int(b.split(': ')[-1]) 
                avg = float(c.split(': ')[-1]) 
                rms = float(d.split(': ')[-1]) 
                ret[idx] = {'cnt': cnt, 'avg': avg, 'rms': rms}
        return ret
                 
