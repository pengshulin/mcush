# coding: utf8
__doc__ = 'products from Shenzhen Korad Tech.'
__author__ = 'Peng Shulin <trees_peng@163.com>'
import re
import time
from .. import Env, Instrument


class KA3000( Instrument.SerialInstrument ):
    DEFAULT_CHECK_RETURN_COMMAND = False
    DEFAULT_IDN = re.compile( 'KORAD KA3[0-9]+P V[0-9]+.[0-9]+ SN:[0-9]+' )

    # eg: *IDN?
    #     KORAD KA3005P V5.5 SN:16072670
    
    def __init__( self, *args, **kwargs ):
        kwargs['baudrate'] = 9600  # this is fixed
        Instrument.SerialInstrument.__init__( self, *args, **kwargs ) 

    def readUntilPrompts( self, line_callback=None ):
        pass

    def connect( self, check_idn=True ):
        Instrument.SerialInstrument.connect( self, check_idn=False )
        idn = self.writeCommand('*IDN?', need_response=True)
        brand, model, ver, sn = idn.split(' ')
        sn = sn.split(':')[1]
        self.model = model
        self.ver = ver
        self.sn = sn
        self.idn = ','.join([model,ver])

    def getSerialNumber( self ):
        return self.sn 

    def writeCommand( self, cmd, need_response=False ):
        # write command and wait for response
        cmd = cmd.strip().upper()
        self.writeLine( cmd )
        self.logger.debug( cmd )
        if need_response:
            line = self.readLine()
            self.logger.debug( line )
            return line

    def output( self, volt=None, amp=None, channel=1, enable=True ):
        if volt is not None:
            volt = float(volt)
            if volt < 0 or volt > self.VMAX:
                raise Exception('Volt out of range')
            self.writeCommand('VSET%d:%.2f'% (channel, volt))
        if amp is not None:
            amp = float(amp)
            if amp < 0 or amp > self.IMAX:
                raise Exception('Current out of range')
            self.writeCommand('ISET%d:%.2f'% (channel, amp))
        if enable is not None:
            self.writeCommand('OUT1' if enable else 'OUT0')
        
    def outputEnable( self ):
        self.output( enable=True )

    def outputDisable( self ):
        self.output( enable=False )

    def getSetting( self, channel=1 ):
        volt = self.writeCommand('VSET%d?'% channel, need_response=True)
        amp = self.writeCommand('ISET%d?'% channel, need_response=True)
        return float(volt), float(amp)
 
    def read( self, channel=1 ):
        volt = self.writeCommand('VOUT%d?'% channel, need_response=True)
        amp = self.writeCommand('IOUT%d?'% channel, need_response=True)
        return float(volt), float(amp)
        
    def beep( self, on=False ):
        self.writeCommand('BEEP%d'% int(on))
        
    def status( self ):
        s = self.writeCommand('STATUS?', need_response=True)
        return ord(s)
        



class KA3005P( KA3000 ):
    DEFAULT_NAME = 'KA3005P'
    VMAX = 30.0
    IMAX = 5.0


