# coding:utf8
__doc__ = 'Fluke 8808A 5-1/2 digit bench-top multimeter'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Instrument
from .. import Env


FUNCTION = ['AAC', 'AACDC', 'ADC', 'CONT', 'DIODE', 'FREQ', 'OHMS', 'VAC', 'VACDC', 'VDC' ]
FUNCTION2 = ['AAC', 'ADC', 'FREQ', 'OHMS', 'VAC', 'VDC' ]
RANGE = {
'VAC':   [0.2, 2, 20, 200, 1000],
'VACDC': [0.2, 2, 20, 200, 1000],
'VDC':   [0.2, 2, 20, 200, 1000],
'AAC':   [0.02, 0.2, 2, 10],
'AACDC': [0.02, 0.2, 2, 10],
'ADC':   [0.02, 0.2, 2, 10],
'OHMS':  [200, 2000, 20000, 200000, 2000000, 20000000, 100000000],
'FREQ':  [2000, 20000, 200000, 1000000],
}


class F8808A( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'F8808A'
    DEFAULT_TIMEOUT = 10
    DEFAULT_PROMPTS = re_compile( '(\x03)?[=#?!]>' )
    DEFAULT_IDN = re_compile( 'FLUKE, 8808A, .*' )

    function = None
    function2 = None

    def writeCommand( self, cmd ):
        # write command and wait for prompts
        cmd = cmd.upper()
        self.writeLine( cmd )
        ret = self.readUntilPrompts()
        if not ret[0]:
            ret = ret[1:]
        self.checkReturnedPrompt( ret )
        self.checkReturnedCommand( ret, cmd )
        return ret[1:-1] 

    def setFormat( self, sel=1 ):
        # format 1: 1.2345E+6
        # format 2: +12.345E+6 OHM
        self.writeCommand( 'format %d'% sel ) 
       
    def getFormat( self ):
        return int(self.writeCommand( 'format?' )[0])
         
    def setRate( self, rate='m' ):
        # rate: S/M/F
        self.writeCommand( 'rate %s'% rate ) 

    def function( self, mode, wire=None, range=None ):
        # set main function
        mode = mode.strip().upper()
        if not mode in FUNCTION:
            raise Exception("invalid mode %s"% mode)
        self.function = mode
        self.writeCommand( self.function )
        if mode not in FUNCTION2:
            self.function2
        if self.getFormat() != 2:
            self.setFormat(2)
        if mode not in ['CONT', 'DIODE']:
            self.setRange(range)
  
    def function2( self, mode, wire=None ):
        # set secondary function
        mode = mode.strip().upper()
        if not mode in FUNCTION2:
            raise Exception("invalid mode %s"% mode)
        self.function2 = mode+'2'
        self.writeCommand( self.function2 )
  
    def setRange( self, value=None ):
        # set range
        if value is None:
            self.writeCommand( 'AUTO' )
        else:
            self.writeCommand( 'FIXED' )
            idx = RANGE[self.function].index(value)+1
            self.writeCommand( 'RANGE %d'% idx )
     
    def getReading( self ):
        # read the first display reading
        r, u = self.writeCommand( "MEAS1?" )[0].split()
        return (float(r), u)

    def getReading2( self ):
        # read the second display reading
        try:
            r, u = self.writeCommand( "MEAS2?" )[0].split()
            return (float(r), u)
        except Instrument.CommandExecuteError:
            return (None, None)
    
    def hold( self, on=True ):
        self.writeCommand( 'HOLD' if on else 'HOLDCLR' )
         
