# coding: utf8
__doc__ = 'Generic GCode Controller'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import re
from .. import Instrument, Utils


class _GCodeCommon(Instrument.SerialInstrument):
    DEFAULT_NAME = 'GCode'
    DEFAULT_BAUDRATE = 115200
    DEFAULT_TERMINATOR_WRITE = '\x0A'  # '\n'
    DEFAULT_TERMINATOR_READ = '\x0A'  # '\n'
    DEFAULT_TERMINATOR_RESET = '\x18'  # Ctrl-X
    DEFAULT_CHECK_IDN = False
    DEFAULT_CHECK_RETURN_COMMAND = False

    def connect( self ):
        '''connect'''
        old = self.setPrompts( self.CONNECT_PROMPT )
        self.setTimeout( 90 )
        Instrument.Instrument.connect(self)
        self.setPrompts( old )
        self.homed = False

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

    def gotoX( self, x, feedrate=None ):
        if feedrate:
            return self.G1( x=x, f=feedrate )
        else:
            return self.G0( x=x )

    def gotoY( self, y, feedrate=None ):
        if feedrate:
            return self.G1( y=y, f=feedrate )
        else:
            return self.G0( y=y )

    def gotoZ( self, z, feedrate=None ):
        if feedrate:
            return self.G1( z=z, f=feedrate )
        else:
            return self.G0( z=z )

    def gotoE( self, e, feedrate=None ):
        return self.G1( e=e )

    def gotoXY( self, x, y, feedrate=None ):
        if feedrate:
            return self.G1( x=x, y=y, f=feedrate )
        else:
            return self.G0( x=x, y=y )

    def gotoXYZ( self, x, y, z, feedrate=None ):
        if feedrate:
            return self.G1( x=x, y=y, z=z, f=feedrate )
        else:
            return self.G0( x=x, y=y, z=z )

    def setOrigin( self, x=0, y=0, z=0 ):
        self.G( 92, x=x, y=y, z=z )

    def gotoHome( self ):
        self.G( 28 )

    def setRelativeZero( self ):
        self.G( 92 )
        
    def setRelativeMode( self, enable=True ):
        self.G( 91 if enable else 90 )



class Marlin( _GCodeCommon ):
    DEFAULT_NAME = 'MARLIN'
    DEFAULT_PROMPTS = re.compile( 'ok' )
    CONNECT_PROMPT = re.compile( 'init valid:' )
  

    def gotoXYE( self, x, y, e ):
        return self.G1( x=x, y=y, e=e )

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

    def waitForReady( self ):
        self.M( 400 )

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

#############################################################################
# GRBL V1.1


ERROR_CODE = {
1 : "Expected command letter",
2 : "Bad number format",
3 : "Invalid statement",
4 : "Value < 0",
5 : "Setting disabled",
6 : "Value < 3 usec",
7 : "EEPROM read fail. Using defaults",
8 : "Not idle",
9 : "G-code lock",
10: "Homing not enabled",
11: "Line overflow",
12: "Step rate > 30kHz",
13: "Check Door",
14: "Line length exceeded",
15: "Travel exceeded",
16: "Invalid jog command",
17: "Setting disabled",
20: "Unsupported command",
21: "Modal group violation",
22: "Undefined feed rate",
23: "Invalid gcode ID:23",
24: "Invalid gcode ID:24",
25: "Invalid gcode ID:25",
26: "Invalid gcode ID:26",
27: "Invalid gcode ID:27",
28: "Invalid gcode ID:28",
29: "Invalid gcode ID:29",
30: "Invalid gcode ID:30",
31: "Invalid gcode ID:31",
32: "Invalid gcode ID:32",
33: "Invalid gcode ID:33",
34: "Invalid gcode ID:34",
35: "Invalid gcode ID:35",
36: "Invalid gcode ID:36",
37: "Invalid gcode ID:37",
38: "Invalid gcode ID:38",
}

SET_CODE = {
0  : "Step pulse time",
1  : "Step idle delay",
2  : "Step pulse invert",
3  : "Step direction invert",
4  : "Invert step enable pin",
5  : "Invert limit pins",
6  : "Invert probe pin",
10 : "Status report options",
11 : "Junction deviation",
12 : "Arc tolerance",
13 : "Report in inches",
20 : "Soft limits enable",
21 : "Hard limits enable",
22 : "Homing cycle enable",
23 : "Homing direction invert",
24 : "Homing locate feed rate",
25 : "Homing search seek rate",
26 : "Homing switch debounce delay",
27 : "Homing switch pull-off distance",
30 : "Maximum spindle speed",
31 : "Minimum spindle speed",
32 : "Laser-mode enable",
100: "X-axis travel resolution",
101: "Y-axis travel resolution",
102: "Z-axis travel resolution",
110: "X-axis maximum rate",
111: "Y-axis maximum rate",
112: "Z-axis maximum rate",
120: "X-axis acceleration",
121: "Y-axis acceleration",
122: "Z-axis acceleration",
130: "X-axis maximum travel",
131: "Y-axis maximum travel",
132: "Z-axis maximum travel",
}

ALARM_CODE = {
1 : "Hard limit",
2 : "Soft limit",
3 : "Abort during cycle",
4 : "Probe fail",
5 : "Probe fail",
6 : "Homing fail",
7 : "Homing fail",
8 : "Homing fail",
9 : "Homing fail",
10: "Homing fail",
}

class Grbl( _GCodeCommon ):
    DEFAULT_NAME = 'GRBL'
    DEFAULT_PROMPTS = re.compile( '(ok|(error|ALARM):[0-9]+)' )
    #DEFAULT_PROMPTS = re.compile( '(ok|(error|ALARM):[0-9]+|\[MSG:.*\])' )
    CONNECT_PROMPT = re.compile( 'Grbl [0-9]\.[0-9][a-z].* \[.*\]' )
    DEFAULT_LINE_PROMPT_MODE = True
    settings = {} 

    def checkReturnedPrompt( self, ret ):
        self.returned_prompt = prompt = ret[-1].strip()
        if prompt.startswith('error:'):
            error_code = int(prompt[6:])
            raise Instrument.CommandExecuteError( 'error %d (%s)'% 
                                (error_code, ERROR_CODE[error_code]) )
        elif prompt.startswith('ALARM:'):
            alarm_code = int(prompt[6:])
            raise Instrument.CommandExecuteError( 'alarm %d (%s)'% 
                                (alarm_code, ALARM_CODE[alarm_code]) )
        elif prompt.startswith('[MSG:'):
            raise Instrument.CommandExecuteError( 'msg (%s)'% prompt[5:-1] )
  
    def unlock(self):
        self.writeCommand( '$X' )

    def writeSetting( self, index, value ):
        cmd = '$%d=%s'% (index, value)
        self.writeCommand( cmd )

    def updateSettings( self ):
        settings = self.writeCommand("$$")
        for k,v in Utils.parseKeyValueLines(settings,splitter='=').items():
            self.settings[int(k[1:])] = float(v)
        
    def printSettings( self ):
        for k,v in self.settings.items():
            print( '$%d = %f (%s)'% (k,v,SET_CODE.get(k)) )

    def waitForReady( self ):
        while True:
            status = self.updateStatus().split('|')[0]
            if status == 'Idle':
                break

    def updateStatus( self ):
        line = self.writeCommand("?")[0].strip()
        if line[0] == '<' and line[-1] == '>':
            self.status = line[1:-1]
            return self.status
        else:
            raise Instrument.CommandExecuteError( 'Status format error' )

    def getPos( self ):
        # eg:
        # <Idle|MPos:0.000,0.000,0.000|FS:0,0|WCO:0.000,0.000,0.000>
        # <Idle|MPos:0.000,0.000,0.000|FS:0,0|Ov:100,100,100>
        # <Idle|MPos:0.000,0.000,0.000|FS:0,0>
        ret = {}
        status = self.updateStatus().split('|')[1]
        if not status.startswith('MPos:'):
            raise Exception("format err")
        x,y,z = status[5:].split(',')
        ret['X'] = float(x)
        ret['Y'] = float(y)
        ret['Z'] = float(z)
        return ret

    def calibHome( self ):
        self.writeCommand( '$H' )
        self.homed = True
         

