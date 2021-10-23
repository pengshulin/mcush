# coding:utf8
__doc__ = 'DG1000Z Series Programmable Signal Generator'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Instrument
from .. import Env


class DG1022Z( Instrument.SocketInstrument ):
    DEFAULT_NAME = 'DG1022Z'
    # *idn? --> Rigol Technologies,DG1022Z,DG1ZA231101158,03.01.12
    DEFAULT_IDN = re_compile( 'Rigol Technologies,DG1022Z,DG[0-9A-Z]+,.*' )  
    DEFAULT_TERMINAL_RESET = False
    DEFAULT_CHECK_RETURN_COMMAND = False

    def __init__( self, *args, **kwargs ):
        kwargs['net_addr'] = args[0]
        kwargs['net_port'] = 5555
        Instrument.SocketInstrument.__init__( self, *args, **kwargs ) 

    def writeCommand( self, cmd ):
        cmd = cmd.strip()
        self.writeLine( cmd )
        return []

    def scpiIdn( self, check=True ):
        self.writeLine( '*idn?' )
        ret = self.readLine()
        self.logger.debug( 'IDN:%s', ret )
        if not self.DEFAULT_IDN.match( ret ):
            raise IDNMatchError(ret)
        _, self.model, self.serial_number, self.version = ret.split(',')
        self.idn = ','.join([self.model, self.version])  # cached

    def beepEnable( self, enable=True ):
        self.writeCommand( 'syst:beep:stat %s'% ('on' if enable else 'off') )

    def beepDisable( self ):
        self.beepEnable( False )

    def outputEnableA( self, enable=True ):
        self.writeCommand( 'outp %s'% ('on' if enable else 'off') )
    
    def outputEnableB( self, enable=True ):
        self.writeCommand( 'outp2 %s'% ('on' if enable else 'off') )

    def outputDisableA( self ):
        self.outputEnableA( False )
    
    def outputDisableB( self ):
        self.outputEnableB( False )

    def outputA( self, mode, amp, freq, offset=0, duty=None, phase=0, enable=True ):
        if mode not in ['sin','squ','ramp','puls','nois','dc','user']:
            raise Exception('mode error')
        self.writeCommand( 'appl:%s %s,%s,%s'% (mode,freq,amp,offset) )
        if enable:
            self.outputEnableA( True )

    def outputB( self, mode, amp, freq, offset=0, duty=None, phase=0, enable=True ):
        if mode not in ['sin','squ','ramp','puls','nois','dc','user']:
            raise Exception('mode error')
        self.writeCommand( 'sour2:appl:%s %s,%s,%s'% (mode,freq,amp,offset) )
        if enable:
            self.outputEnableB( True )


