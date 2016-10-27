# coding: utf-8
'''common instrument classes'''
import serial
import logging
import binascii
import re
import Env
import Utils

if Env.LOGGING_FORMAT:
    logging.BASIC_FORMAT = Env.LOGGING_FORMAT
else:
    logging.BASIC_FORMAT = '%(asctime)s ' + logging.BASIC_FORMAT

class SerialNotFound( Exception ):
    pass

class UnknownSerialError( Exception ):
    pass

class ResponseError( Exception ):
    pass

class CommandSyntaxError( Exception ):
    pass

class CommandSemanticsError( Exception ):
    pass

class CommandTimeoutError( Exception ):
    pass

class SerialInstrument:
    '''Instruments based on serial port'''
    DEFAULT_NAME = 'INST'
    DEFAULT_TERMINATOR_WRITE = '\x0A'  # '\n'
    DEFAULT_TERMINATOR_READ = '\x0A'  # '\n'
    DEFAULT_TERMINATOR_RESET = '\x03'  # Ctrl-C
    DEFAULT_TIMEOUT = 5
    DEFAULT_PROMPTS = re.compile( '[=#?!]>' )
    DEFAULT_IDN = None
    

    def __init__( self, port=None, baudrate=None, rtscts=False, connect=True ):
        '''init'''
        self.verbose = Env.VERBOSE
        self.debug = Env.DEBUG
        self.info = Env.INFO
        self.prompts = self.DEFAULT_PROMPTS
        if self.debug:
            logging.basicConfig( level=logging.DEBUG )
        elif self.info:
            logging.basicConfig( level=logging.INFO )
        else:
            logging.basicConfig( level=logging.FATAL )
        self.logger = logging.getLogger( self.DEFAULT_NAME )
        if port:
            self.port = port
        else:
            self.port = Env.PORT
        if baudrate:
            self.baudrate = baudrate
        else:
            self.baudrate = Env.BAUDRATE
        if rtscts:
            self.rtscts = rtscts
        else:
            self.rtscts = Env.RTSCTS
        
        self.idn = None
        self.open_port()
        if connect:
            self.connect()

    def open_port( self ):
        try:
            self.ser = serial.serial_for_url( self.port, do_not_open=True )
        except AttributeError:
            self.ser = serial.Serial()
 
    def setVerbose( self, verbose ):
        '''Set verbose mode'''
        self.verbose = verbose

    def setLoggingLevel( self, level ):
        '''set logging level'''
        self.debug, self.info = False, False
        if level in ['debug']:
            self.debug = True 
            logging.basicConfig( level=logging.DEBUG )
        elif level in ['info']:
            self.info = True 
            logging.basicConfig( level=logging.INFO )
        else:
            logging.basicConfig( level=logging.FATAL )
 
    def setInfo( self, debug ):
        '''set debug'''
        self.debug = debug
 
    def setPrompts( self, prompts ):
        '''dynamically modify the prompts'''
        self.prompts = prompts

    def connect( self ):
        '''connect'''
        if self.ser.isOpen():
            return
        self.ser.port = self.port
        self.ser.baudrate = self.baudrate
        self.ser.ttscts = self.rtscts
        self.ser.timeout = self.DEFAULT_TIMEOUT
 
        #self.ser.setPort( self.port )
        #self.ser.setBaudrate( self.baudrate )
        #self.ser.setRtsCts( self.rtscts )
        #self.ser.setTimeout( self.DEFAULT_TIMEOUT )
        try:
            self.ser.open()
        except Exception:
            raise SerialNotFound( self.port )
        try:
            self.ser.write( self.DEFAULT_TERMINATOR_RESET )
            self.ser.flush()
            self.readUntilPrompts()
            self.scpiIdn()
            #self.scpiRst()
        except serial.SerialException, e:
            raise UnknownSerialError( str(e) )
        #except Exception, e:
        #    print type(e), str(e)

    def disconnect( self ):
        '''disconnect'''
        if self.ser.isOpen():
            self.ser.close()

    def assertIsOpen( self ):
        '''assert serial is opened'''
        if not self.ser.isOpen():
            self.connect()
            if not self.ser.isOpen():
                raise Exception, "Fail to open serial port"
    
    def readUntilPrompts( self ):
        '''read until prompts'''
        contents, newline = [], ''
        while True:
            byte = self.ser.read(1)
            if byte:
                if byte == self.DEFAULT_TERMINATOR_READ:
                    contents.append( newline.rstrip() )
                    self.logger.debug( newline )
                    newline = ''
                else:
                    newline += byte
            else:
                contents.append( newline )
                raise CommandTimeoutError( ' | '.join(contents) )
            match = self.prompts.match( newline )
            if match:
                contents.append( newline )
                self.logger.debug( newline )
                return contents
   
    def writeCommand( self, cmd ):
        '''write command and wait for prompts'''
        self.assertIsOpen() 
        self.ser.flushInput()
        self.ser.write( cmd + self.DEFAULT_TERMINATOR_WRITE )
        self.ser.flush()
        self.logger.debug( cmd )
        ret = self.readUntilPrompts()
        for line in [i.strip() for i in ret]:
            if line:
                self.logger.debug( line )
        self.checkReturnedPrompt( ret )
        self.checkReturnedCommand( ret, cmd )
        return ret[1:-1] 
 
    def checkReturnedCommand( self, ret, cmd ):
        '''assert command returned is valid'''
        if Env.NO_ECHO_CHECK:
            return
        cmdret = ret[0].strip()
        if cmd and cmd != cmdret:
            raise ResponseError('Command %s, but returned %s'% \
                                                (cmd, cmdret))

    def checkReturnedPrompt( self, ret ):
        '''assert prompt is valid'''
        cmd = ret[0]
        prompt = ret[-1]
        if prompt in ['?>', '?']:
            raise CommandSyntaxError( cmd )
        elif prompt == '!>':
            result = ret[1:-1]
            err = cmd + ', returns: ' + ','.join(result)
            raise CommandSemanticsError( err )
 
    def scpiRst( self ):
        '''send *rst command'''
        self.writeCommand( '*rst' )

    def scpiIdn( self ):
        '''get identify name'''
        ret = self.writeCommand( '*idn?' )
        self.idn = ret[0].strip()
        self.logger.info( 'IDN:%s', str(self.idn) )
        if not self.DEFAULT_IDN.match( self.idn ):
            raise Exception, "IDN not match"

    def reset( self ):
        '''send reset command'''
        self.writeCommand( 'reset' )

    def gpio( self, port, i=None, o=None, s=None, c=None, t=None ):
        '''gpio command'''
        cmd = 'gpio -p %s'% str(port)
        if i is not None:
            if type(i) is int:
                cmd += ' -i 0x%x'% i
            elif type(i) is bool and i:
                cmd += ' -i'
        if o is not None:
            if type(o) is int:
                cmd += ' -o 0x%x'% o
            elif type(o) is bool and o:
                cmd += ' -o'
        if s is not None:
            if type(s) is int:
                cmd += ' -s 0x%x'% s
            elif type(s) is bool and s:
                cmd += ' -s'
        if c is not None:
            if type(c) is int:
                cmd += ' -c 0x%x'% c 
            elif type(c) is bool and c:
                cmd += ' -c'
        if t is not None:
            if type(t) is int:
                cmd += ' -t 0x%x'% t 
            elif type(t) is bool and t:
                cmd += ' -t'
        ret = self.writeCommand( cmd )
        if ret:
            return eval(ret[0])

    def parseMemLine( self, line ):
        '''format:  xx xx xx xx xx'''
        return ''.join([binascii.unhexlify(i) for i in line.split()])
 
    def fillMem( self, addr, pattern, width, length ):
        '''fill memory'''
        cmd = 'mfill -b 0x%X -w %d -p 0x%X -l %d'% ( addr, width, pattern, length )
        self.writeCommand( cmd )
         
    def readMem( self, addr, length=1 ):
        '''get memory'''
        cmd = 'x -b 0x%X -l %d'% ( addr, length )
        ret = self.writeCommand( cmd )
        assert 1 <= len(ret)
        for line in ret:
            self.logger.info( line )
        mem = ''.join([self.parseMemLine(line.strip()) for line in ret])
        return mem[:length]
      
    def writeMem( self, addr, data ):
        '''write memory'''
        length = len(data)
        written = 0
        while written < length:
            cmd = 'w -b 0x%X -w1 '% ( addr )
            left = min(length - written, 16)
            cmd += ' '.join([str(ord(c)) for c in data[written:written+left]])
            self.writeCommand( cmd )
            written += left
            addr += left

 
    def dumpMem( self, addr, length=1 ):
        '''dump memory'''
        Utils.dumpMem( self.readMem(addr, length) )
    
class SerialInstrumentTest( SerialInstrument ):
    '''for test'''
    def test( self ):
        self.dumpMem( 0, 128 )

 
if __name__ == '__main__':
    TEST = SerialInstrumentTest()
    TEST.test()
    TEST.disconnect()
