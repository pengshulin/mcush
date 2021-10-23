# coding:utf8
__doc__ = 'DS1000B/E/U/Z Digital Oscilloscope'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Instrument
from .. import Env
import time

# Product lines:
#            Bandwidth   Channels  Samplerate    Depth
# DS1000E/U  50M ~ 100M     2       1G Sa/s      1M pts
# DS1000Z    50M ~ 100M     4       1G Sa/s     24M pts
# DS1000Z-E  200M           2       1G Sa/s     24M pts
# DS1000B    70M ~ 200M     4       2G Sa/s     16k pts 

B2S = {0:'OFF', 1:'ON', True:'ON', False:'OFF', None:'OFF'}

class DS1000( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'DS1000'
    DEFAULT_IDN = re_compile( 'Rigol Technologies,DS1.*' )
    DEFAULT_TERMINAL_RESET = False
    DEFAULT_READ_UNTIL_PROMPTS = False
    DEFAULT_CHECK_RETURN_COMMAND = False

    # eg: *IDN?
    #     Rigol Technologies,DS1102E,DS1ET162359072,00.04.01.00.02

    def writeCommand( self, cmd, need_response=False ):
        # write command and wait for response
        cmd = cmd.strip().upper()
        self.writeLine( cmd )
        time.sleep(0.2)
        if need_response:
            return self.readLine()

    def scpiIdn( self ):
        self.idn = self.writeCommand('*idn?', need_response=True)
        self.brand, self.model, self.sn, self.ver = self.idn.split(',')
        return self.idn

    def scpiRst( self ):
        Instrument.SerialInstrument.scpiRst(self)
        time.sleep(0.5)

    def getSerialNumber( self ):
        return self.scpiIdn().split(',')[-2]

    def run( self ):
        # this command does not work, use key('run') instead
        self.writeCommand(":SYST:RUN")

    def stop( self ):
        self.writeCommand(":SYST:STOP")

    def auto( self ):
        self.writeCommand(":SYST:AUTO")

    def snapshot( self ):
        self.writeCommand(":SYST:HARD")

    def beepEnable( self, enable=True ):
        self.writeCommand(":BEEP:ENABLE %s"% B2S[enable])
        
    def beep( self ):
        self.writeCommand(":BEEP:ACT")

    def key( self, key_name ):
        self.writeCommand(":KEY:%s"% key_name) 

    def display( self, disp_type='VECT', grid_type='HALF', persist=False ):
        self.writeCommand(":DISP:TYPE %s"% disp_type)
        self.writeCommand(":DISP:GRID %s"% grid_type)
        self.writeCommand(":DISP:PERS %s"% B2S[persist])
        self.writeCommand(":DISP:CLE")

    def timebase( self, format='YT', delay=False, offset=0, scale=1 ):
        self.writeCommand(":TIM:FORMAT %s"% format )
        self.writeCommand(":TIM:MODE %s"% ('del' if delay else'main' ))
        self.writeCommand(":TIM:OFFS %s"% offset )
        self.writeCommand(":TIM:SCAL %s"% scale )

    def trigger( self, mode, source=None, level=None, sweep=None ):
        # mode type: edge/pulse/video/slope/pattern/duration/alternation
        mode = mode.upper()[:4]
        self.writeCommand(":TRIG:MODE %s"% mode )
        if source is not None:
            self.writeCommand(":TRIG:%s:SOUR %s"% (mode, source) )
        if level is not None:
            self.writeCommand(":TRIG:%s:LEV %s"% (mode, level) )
        if sweep is not None:
            self.writeCommand(":TRIG:%s:SWE %s"% (mode, sweep) )
        
    def forceTrig( self ):
        self.writeCommand(":FORC" )

    def channel( self, channel_idx=1, display=True, coupling='DC',
                       scale=1, offset=0, bw_limit=False, invert=False, 
                       probe=1, filter_on=False ):
        prefix = ':CHAN%d:'% channel_idx
        self.writeCommand(prefix+'DISP %s'% B2S[display])
        self.writeCommand(prefix+'COUP %s'% coupling)
        self.writeCommand(prefix+'SCAL %s'% scale)
        self.writeCommand(prefix+'OFFS %s'% offset)
        self.writeCommand(prefix+'PROB %s'% probe)
        self.writeCommand(prefix+'INV %s'% B2S[invert])
        self.writeCommand(prefix+'BWL %s'% B2S[bw_limit])
        self.writeCommand(prefix+'FILT %s'% B2S[filter_on])

    def waveform( self, mode=None, channel_idx=1 ):
        if mode is not None:
            # mode: NORM/MAX/RAW
            self.writeCommand(':WAV:POIN:MODE %s'% mode)
        else:
            ret = self.writeCommand(':WAV:DATA? CHAN%d'% channel_idx, need_response=True)
     
