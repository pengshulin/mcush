# coding: utf8
__doc__ = 'Generic GCode Controller'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import re
from .. import Instrument, Utils


class Marlin( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'Marlin'
    DEFAULT_TERMINATOR_WRITE = '\x0A'  # '\n'
    DEFAULT_TERMINATOR_READ = '\x0A'  # '\n'
    DEFAULT_PROMPTS = re.compile( 'ok' )
    DEFAULT_CHECK_IDN = False
    DEFAULT_CHECK_RETURN_COMMAND = False
   
    def connect( self ):
        '''connect'''
        self.port.connect()
        if not self.port.connected:
            return
        init_prompt = re.compile( 'init valid:' )
        old = self.setPrompts( init_prompt )
        info = self.readUntilPrompts()
        # TODO: parse and record init info for future purpose
        self.setPrompts( old )
 
    def _code( self, prefix, function_code, *args, **kwargs ):
        command = ['%s%u'% (prefix, function_code)]
        command.extend( [str(arg).strip().upper() for arg in args] )
        for k,v in kwargs.items():
            if isinstance(v, float):
                arg = '%s%.3f'% (k, v)
            elif isinstance(v, int):
                arg = '%s%d'% (k, v)
            else:
                arg = '%s%s'% (k, v)
            command.append( arg.upper() )
        return self.writeCommand( ' '.join(command) )

    def G( self, function_code, *args, **kwargs ):
        return self._code( 'G', function_code, *args, **kwargs )

    def M( self, function_code, *args, **kwargs ):
        return self._code( 'M', function_code, *args, **kwargs )

    def G0( self, *args, **kwargs ):
        return self.G( 0, *args, **kwargs )

    def G1( self, *args, **kwargs ):
        return self.G( 1, *args, **kwargs )

    def gotoX( self, x ):
        return self.G1( x=x )

    def gotoY( self, y ):
        return self.G1( y=y )

    def gotoZ( self, z ):
        return self.G1( z=z )

    def gotoE( self, e ):
        return self.G1( e=e )

    def gotoXY( self, x, y ):
        return self.G1( x=x, y=y )

    def gotoXYs( self, points_list ):
        for x,y in points_list:
            self.G1( x=x, y=y )

    def gotoXYE( self, x, y, e ):
        return self.G1( x=x, y=y, e=e )

    def gotoXYZ( self, x, y, z ):
        return self.G1( x=x, y=y, z=z )

    def getPos( self ):
        ret = {}
        a, b = self.M( 114 )[-1].split(' Count ')
        for k,v in Utils.parseKeyValueLines(a.split(' '),splitter=':').items():
            ret[k] = float(v)
        for k,v in Utils.parseKeyValueLines(b.split(' '),splitter=':').items():
            ret['Count '+k] = v
        return ret

    def setMaxAcc( self, x=1000, y=1000, z=None ):
        self.G( 201, x=x, y=y )

    def resetAxesPos( self, x=None, y=None, z=None, e=None ):
        args = []
        if x is not None:
            args.append( "X0" )
        if y is not None:
            args.append( "Y0" )
        if z is not None:
            args.append( "Z0" )
        if e is not None:
            args.append( "E0" )
        self.G( 92, *args )

    def gotoHome( self ):
        self.G( 28 )

    def waitForReady( self ):
        self.M( 400 )

    def setRelativeZero( self ):
        self.G( 92 )
        
    def setRelativeMode( self, enable=True ):
        self.G( 91 if enable else 90 )

    def setFan( self, on=True ):
        self.M( 106 if on else 107 )

    def beep( self, freq=1000, ms=50 ):
        self.M( 300, S=freq, P=ms )
       
    def motorEnable( self, enable=True ):
        self.M( 17 if enable else 18 )

    def setExtruderTemp( self, temperature=25 ):
        self.M( 104, temperature )

    def setBedTemp( self, temperature=25 ):
        pass

    def getTemp( self ):
        # FIXME: this command behaviors abnormally:
        # data is printed after ok prompt
        # M105
        # ok T:15.08 /0.00 B:15.39 /0.00 @:0 B@:0
        ret = self.M( 105 )
        return ret

    def getSD( self, init=False ):
        if init:
            self.M( 21 )
        flist, begin = [], None
        for line in self.M(20):
            if not begin:
                if line == 'Begin file list':
                    begin = True
            elif line == 'End file list':
                break
            else:
                a, b = line.split()
                flist.append( (a, int(b)) )
        return flist


class Grbl( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'GRBL'
    DEFAULT_TERMINATOR_WRITE = '\x0A'  # '\n'
    DEFAULT_TERMINATOR_READ = '\x0A'  # '\n'
    DEFAULT_PROMPTS = re.compile( '$' )
    DEFAULT_CHECK_IDN = False
    DEFAULT_CHECK_RETURN_COMMAND = False
 
