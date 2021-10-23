# coding: utf8
__doc__ = 'products from Zhenzhou Feeltech'
__author__ = 'Peng Shulin <trees_peng@163.com>'
import re
import time
from .. import Env, Instrument

MODES_A = [
'sine',
'square',  # fixed 50% duty
'rectangle',  # adjustable duty
'trapezoid',
'cmos',
'adj-pulse',
'dc',
'triangle',
'ramp',
'neg ramp',
'stair triangle',
'stair step',
'neg stair step',
'pos exponent',
'neg exponent',
'pos fall exponent',
'neg fall exponent',
'pos logarithm',
'neg logarithm',
'pos fall logarithm',
'neg fall logarithm',
'pos full sine',
'neg full sine',
'pos half sine',
'neg half sine',
'lorentz pulse',
'multitone',
'random noise',
'ecg',
'trapezoid',
'sinc pulse',
'impulse',
'awgn',
'am',
'pm',
'chirp',
]

MODES_B = [
'sine',
'square',
'rectangle',
'trapezoid',
'cmos',
'dc',
'triangle',
'ramp',
'neg ramp',
'stair triangle',
'stair step',
'neg stair step',
'pos exponent',
'neg exponent',
'pos fall exponent',
'neg fall exponent',
'pos logarithm',
'neg logarithm',
'pos fall logarithm',
'neg fall logarithm',
'pos full sine',
'neg full sine',
'pos half sine',
'neg half sine',
'lorentz pulse',
'multitone',
'random noise',
'ecg',
'trapezoid',
'sinc pulse',
'impulse',
'awgn',
'am',
'pm',
'chirp',
]

for i in range(64):
    MODES_A.append('arb%d'%(i+1))
    MODES_B.append('arb%d'%(i+1))

KEYS = ['wave', 'meas', 'sweep', 'mod', 'sync', 'sys', 'more', 
'ch1', 'ch2', 'f1', 'f2', 'f3', 'f4', 'f5',
'left', 'right', 'ok', 'up', 'down']

MEASURE_GATE = [1, 10, 100]

SWEEP_MODE = ['freq', 'amp', 'offset', 'duty']


class FY6900( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'FY6900'
    DEFAULT_TERMINATOR_READ = '\x0A'
    DEFAULT_TERMINATOR_WRITE = '\x0A'
    DEFAULT_TERMINAL_RESET = False
    DEFAULT_CHECK_IDN = False
    DEFAULT_CHECK_RETURN_COMMAND = False

    def __init__( self, *args, **kwargs ):
        kwargs['baudrate'] = 115200  # this is fixed
        Instrument.SerialInstrument.__init__( self, *args, **kwargs ) 

    def connect( self ):
        Instrument.SerialInstrument.connect( self )
        # read model/id
        model = self.getModel()
        sn = self.getSerialNumber()
        self.idn = ','.join([model,sn])

    def readUntilPrompts( self, line_callback=None ):
        # this device response at most one line, but followed with multiple 0x0A 
        newline_str = ''
        while True:
            byte = self.port.read(1)
            if byte:
                if Env.PYTHON_V3:
                    byte = chr(ord(byte))
                if byte == self.DEFAULT_TERMINATOR_READ:
                    newline_str = newline_str.rstrip()
                    self.logger.debug( '[R] '+ newline_str )
                    return [newline_str]
                else:
                    newline_str += byte
            else:
                if newline_str:
                    raise Instrument.CommandTimeoutError( newline_str )
                else:
                    raise Instrument.CommandTimeoutError( 'No response' )

    def writeCommand( self, cmd ):
        # write command and wait for response
        cmd = cmd.strip()
        self.writeLine( cmd )
        ret = self.readUntilPrompts()
        return ret

    def getModel( self ):
        ret = self.writeCommand("UMO")
        self.readUntilPrompts()
        self.readUntilPrompts()
        self.readUntilPrompts()
        return ret[0]

    def getSerialNumber( self ):
        ret = self.writeCommand("UID")
        self.readUntilPrompts()
        return ret[0]
       
    def outputEnableA( self, enable=True ):
        self.writeCommand( 'WMN1' if enable else 'WMN0' )
    
    def outputEnableB( self, enable=True ):
        self.writeCommand( 'WFN1' if enable else 'WFN0' )

    def outputDisableA( self ):
        self.outputEnableA( False )
    
    def outputDisableB( self ):
        self.outputEnableB( False )

    def beepEnable( self, enable=True ):
        self.writeCommand( 'UBZ1' if enable else 'UBZ0' )
    
    def beepDisable( self ):
        self.beepEnable( False )

    def beepIsEnabled( self ):
        r = self.writeCommand( 'RBZ' )
        return bool(r > 0)

    def key( self, name ):
        key_id = KEYS.index(name)
        self.writeCommand( 'KEY%d'% key_id )     
    
    def outputA( self, mode, amp, freq, offset=0, duty=None, phase=0, enable=True ):
        if isinstance(mode, int):
            self.writeCommand( 'WMW%02d'% mode )
        else:
            self.writeCommand( 'WMW%02d'% MODES_A.index(mode) )
        self.writeCommand( 'WMA%f'% float(amp) )  # in V
        self.writeCommand( 'WMF%d'% int(freq*1E6) )  # in uHz
        self.writeCommand( 'WMO%f'% float(offset) )  # in V
        if duty is not None:
            self.writeCommand( 'WMD%.1f'% float(duty) )  # in %
        self.writeCommand( 'WMP%f'% float(phase) )  # in degree
        if enable:
            self.outputEnableA( True )

    def outputB( self, mode, amp, freq, offset=0, duty=None, phase=0, enable=True ):
        if isinstance(mode, int):
            self.writeCommand( 'WFW%02d'% mode )
        else:
            self.writeCommand( 'WFW%02d'% MODES_B.index(mode) )
        self.writeCommand( 'WFA%f'% float(amp) )  # in V
        self.writeCommand( 'WFF%d'% int(freq*1E6) )  # in uHz
        self.writeCommand( 'WFO%f'% float(offset) )  # in V
        if duty is not None:
            self.writeCommand( 'WFD%.1f'% float(duty) )  # in %
        self.writeCommand( 'WFP%f'% float(phase) )  # in degree
        if enable:
            self.outputEnableB( True )

    def measureConfig( self, dc=True, gate=1 ):
        self.meas_gate = MEASURE_GATE.index(gate)
        self.meas_dc = dc
        self.writeCommand( 'WCG%d'% self.meas_gate )
        self.writeCommand( 'WCC%d'% (1 if dc else 0) )
        self.writeCommand( 'WCZ' )

    def measureReadFreq( self ):
        freq = float(self.writeCommand( 'RCF' )[0])
        freq /= 10**self.meas_gate
        self.readUntilPrompts()
        return freq
    
    def measureReadCounter( self ):
        counter = float(self.writeCommand( 'RCC' )[0])
        self.readUntilPrompts()
        return counter
        
    def sweepConfig( self, mode, start, end, time, linear=True, auto_start=True ):
        self.sweep_mode = SWEEP_MODE.index(mode)
        self.sweep_start = start
        self.sweep_end = end
        self.sweep_time = time
        self.sweep_linear = linear
        self.writeCommand( 'SBE0' )
        self.writeCommand( 'SOB%d'% self.sweep_mode )
        self.writeCommand( 'SST%s'% self.sweep_start )
        self.writeCommand( 'SEN%s'% self.sweep_end )
        self.writeCommand( 'STI%s'% self.sweep_time )
        self.writeCommand( 'SMO%d'% (0 if linear else 1) )
        if auto_start:
            self.writeCommand( 'SBE1' )

    def sweepStart( self ):
        self.writeCommand( 'SBE1' )

    def sweepStop( self ):
        self.writeCommand( 'SBE0' )

