# coding: utf8
# control GRATTEN Instrument with serial port 
# manufactured by Nanjing GLARUN-ATTEN Technology Co. Ltd
# Peng Shulin <trees_peng@163.com> 2018
import os
import re
import sys
import time
from .. import Env, Instrument

WAVEFORM = {
0: 'Sine',
1: "Square",
2: "Triangle",
3: "Up ramp",
4: "Down ramp",
5: "Pos-pulse",
6: "Neg-pulse",
7: "Tri-pulse",
8: "Up-stair",
9: "Pos-DC",
10: "Neg-DC",
11: "All sine",
12: "Half sine",
13: "Limit sine",
14: "Gate sine",
15: "Squar-root",
16: "Exponent",
17: "Logarithm",
18: "Half round",
19: "Tangent",
20: "Sine(x)/x",
21: "Noise",
22: "Duty 10%",
23: "Duty 90%",
24: "Down stair",
25: "Po-bipulse",
26: "Ne-bipulse",
27: "Trapezia",
28: "Cosine",
29: "Bidir-SCR",
30: "Cardiogram",
31: "Earthquake",
}

WAVEFORM_REV = {}
for k, v in WAVEFORM.items():
    WAVEFORM_REV[v] = k
    WAVEFORM_REV[v.lower()] = k
    WAVEFORM_REV[v.upper()] = k


def convertUnit( value ):
    leading = ''
    value_abs = abs(value)
    if value_abs > 1000000.0:
        value, leading = value/1000000.0, 'M'
    elif value_abs > 1000.0:
        value, leading = value/1000.0, 'k'
    elif value_abs < 0.001:
        value, leading = value*1000.0, 'm'
    elif value_abs < 0.000001:
        value, leading = value*1000000.0, 'u'
    return value, leading
    
    
def getWaveformIndex( name ):
    if isinstance( name, int ):
        return name
    elif isinstance( name, str ):
        return WAVEFORM_REV[name]


class ATF20E( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'ATF20E'
    DEFAULT_TERMINAL_RESET = False
    DEFAULT_CHECK_IDN = False

    def __init__( self, *args, **kwargs ):
        if not 'address' in kwargs:
            kwargs['address'] = Env.getenv_int('ADDRESS', 88) 
        Instrument.SerialInstrument.__init__( self, *args, **kwargs ) 
        
    def connect( self ):
        '''connect'''
        self.port.parity = 'M'
        self.port.timeout = 1
        self.port.connect()
        if not self.port.connected:
            return
        self.command( 'rs232' )

    def disconnect( self ):
        self.command( 'local' )

    def writeCommand( self, cmd ):
        self.port.write( chr(self.address) )
        self.port.flush()
        self.port.disconnect()
        self.port.parity = 'S'
        self.port.connect()
        self.logger.debug( cmd )
        self.port.write(cmd + '\n')
        self.port.flush()
        ret = self.readLine()
        ret_addr = ord(ret[0])
        assert ret_addr == self.address
        ret_contents = ret[1:]
        self.logger.debug( ret_contents )
        if ret_contents == 'Receive error':
            raise Instrument.CommandSyntaxError()
        #time.sleep(5)
        return ret_contents

    def readLine( self ):
        newline_str = ''
        while True:
            byte = self.port.read(1)
            if byte:
                if Env.PYTHON_V3:
                    byte = chr(ord(byte))
                if byte == self.DEFAULT_TERMINATOR_READ:
                    return newline_str
                else:
                    newline_str += byte
            else:
                raise Instrument.CommandTimeoutError( 'No respone: ' + newline_str )

    def command( self, function, option=None, data=None, unit=None ):
        cmd = str(function).upper()
        if option is not None:
            cmd += ':%s'% option.upper()
            if data is not None:
                cmd += (':%s'% data).upper()
                if unit is not None:
                    cmd += ':%s'% unit
        return self.writeCommand( cmd ) 

    def reset(self):
        try:
            self.command( 'sys', 'reset' )
        except Instrument.CommandTimeoutError as e:
            pass
        time.sleep(5)
        while 1:
            try:
                self.command( 'rs232' )
                break
            except Instrument.CommandSyntaxError as e:
                pass
            
            
    def getVersion(self):
        return self.command( 'sys', '?ver' )

    def disableA( self ):
        self.command( 'cha', 'switch', 0, 'No.' )

    def disableB( self ):
        self.command( 'chb', 'switch', 0, 'No.' )

    def singleA( self, mode, freq, vpp, offset=0 ):
        switch = self.command( 'cha', '?switch' ).split()[-1]
        if switch == 'ON':
            self.command( 'cha', 'switch', 0, 'No.' )
        self.command( 'cha', 'awave', getWaveformIndex(mode), 'No.' )
        val, lu = convertUnit(vpp)
        self.command( 'cha', 'vpp', val, lu+'Vpp' )
        val, lu = convertUnit(freq)
        self.command( 'cha', 'afreq', val, lu+'Hz' )
        val, lu = convertUnit(offset)
        self.command( 'cha', 'aoffs', val, lu+'Vdc' )
        self.command( 'cha', 'switch', 1, 'No.' )
        #print self.command( 'cha', '?aoffs' )
        #print self.command( 'cha', '?aatte' )
        #print self.command( 'cha', '?afreq' )
    
    def singleB( self, mode, freq, vpp ):
        switch = self.command( 'chb', '?switch' ).split()[-1]
        if switch == 'ON':
            self.command( 'chb', 'switch', 0, 'No.' )
        self.command( 'chb', 'bwave', getWaveformIndex(mode), 'No.' )
        val, lu = convertUnit(freq)
        self.command( 'chb', 'bfreq', val, lu+'Hz' )
        val, lu = convertUnit(vpp)
        self.command( 'chb', 'vpp', val, lu+'Vpp' )
        self.command( 'chb', 'switch', 1, 'No.' )




