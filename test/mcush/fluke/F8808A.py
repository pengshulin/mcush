# coding:utf8
__doc__ = 'Fluke 8808A 5-1/2 digit bench-top multimeter'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Instrument
from .. import Env


FUNCTION = ['AAC', 'AACDC', 'ADC', 'CONT', 'DIODE', 'FREQ', 'OHMS', 'VAC', 'VACDC', 'VDC', ]
FUNCTION_WITH_SECONDARY = ['AAC', 'ADC', 'FREQ', 'OHMS', 'VAC', 'VDC', ]


class F8808A( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'F8808A'
    DEFAULT_TIMEOUT = 10
    DEFAULT_PROMPTS = re_compile( '(\x03)?[=#?!]>' )
    DEFAULT_IDN = re_compile( 'FLUKE, (8808A), .*' )

    def writeCommand( self, cmd ):
        # write command and wait for prompts
        cmd = cmd.upper()
        self.assertIsOpen() 
        self.ser.flushInput()
        self.ser.write( cmd + self.DEFAULT_TERMINATOR_WRITE )
        self.ser.flush()
        self.logger.debug( cmd )
        ret = self.readUntilPrompts()
        for line in [i.strip() for i in ret]:
            if line:
                self.logger.debug( line )
        if not ret[0]:
            ret = ret[1:]
        self.checkReturnedPrompt( ret )
        self.checkReturnedCommand( ret, cmd )
        return ret[1:-1] 

    def function( self, mode, secondary=False, wire=None, check=False ):
        # set function
        mode = mode.upper()
        assert mode in FUNCTION
        self.secondary = ( secondary and mode in FUNCTION_WITH_SECONDARY )
        self.mode = mode + ( '2' if self.secondary else '' )
        self.writeCommand( self.mode )
        self.writeCommand( 'format 1' ) 
        self.setRange(auto=True)
  
    def getReading( self ):
        # read measure value
        try:
            ret = self.writeCommand( "MEAS?" )[0].split(',')
            return [float(f) for f in ret]
        except IndexError:
            return []
    
    def setRange( self, auto=True, value=None ):
        # set range
        self.writeCommand( 'AUTO' if auto else 'FIXED' )
        if not auto:
            self.writeCommand( 'RANGE %d'%value )
        
