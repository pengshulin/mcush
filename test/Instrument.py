# coding: utf-8
'''common instrument classes'''
import re
import time
import serial
import logging
import binascii
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
_W2L = { 8:1, 16:2, 32:4 }

class reg:
    '''mcu register'''
    def __init__( self, name, address, description, width=32 ):
        self.name = str(name)
        self.address = int(address)
        self.description = str(description)
        self.width = int(width)
        self.length = _W2L[self.width]

    def __str__( self ):
        return self.name


class SerialInstrument:
    '''Instruments based on serial port'''
    DEFAULT_NAME = 'INST'
    DEFAULT_TERMINATOR_WRITE = '\x0A'  # '\n'
    DEFAULT_TERMINATOR_READ = '\x0A'  # '\n'
    DEFAULT_TERMINATOR_RESET = '\x03'  # Ctrl-C
    DEFAULT_TIMEOUT = 5
    DEFAULT_PROMPTS = re.compile( '[=#?!]>' )
    DEFAULT_IDN = None
    DEFAULT_RESET_RETRY = 10
    

    def __init__( self, port=None, baudrate=None, rtscts=False, connect=True ):
        '''init'''
        self.verbose = Env.VERBOSE
        self.debug = Env.DEBUG
        self.info = Env.INFO
        self.prompts = self.DEFAULT_PROMPTS
        self.regs = {}

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
        try:
            self.ser = serial.serial_for_url( self.port, do_not_open=True )
        except AttributeError:
            self.ser = serial.Serial()
        if connect:
            self.connect()
        
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
        try:
            self.ser.open()
        except Exception:
            raise SerialNotFound( self.port )
        try:
            self.ser.write( self.DEFAULT_TERMINATOR_RESET )
            self.ser.flush()
            self.readUntilPrompts()
            self.scpiIdn()
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
            raise ResponseError('Command %s, but returned %s'% (cmd, cmdret))

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
        '''scpi reset'''
        self.writeCommand( '*rst' )

    def scpiIdn( self ):
        '''get identify name'''
        ret = self.writeCommand( '*idn?' )
        self.idn = ret[0].strip()
        self.logger.info( 'IDN:%s', str(self.idn) )
        if not self.DEFAULT_IDN.match( self.idn ):
            raise Exception, "IDN not match"

    def reset( self, delay=None ):
        '''reset command'''
        sync = False
        retry = 0
        try:
            self.writeCommand( 'reset' )
            self.connect()
            sync = True
        except ResponseError:
            pass
        except CommandTimeoutError:
            pass
        while not sync: 
            try:
                SerialInstrument.connect( self )
                sync = True
            except:
                retry = retry + 1
                if retry > self.DEFAULT_RESET_RETRY:
                    raise CommandTimeoutError()
        self.scpiIdn()
        if delay is None:
            time.sleep( Env.DELAY_AFTER_RESET )
        else:
            time.sleep( delay )

    def getModel( self ):
        if self.idn is None:
            self.scpiIdn()
        try:
            return self.idn.split(',')[0]
        except IndexError:
            return ''

    def getSerialNumber( self ):
        if self.idn is None:
            self.scpiIdn()
        try:
            return self.idn.split(',')[2]
        except IndexError:
            return ''

    def getVersion( self ):
        if self.idn is None:
            self.scpiIdn()
        try:
            return self.idn.split(',')[1]
        except IndexError:
            return ''

    def led( self, idx, on=None, toggle=None ):
        '''led control'''
        if on is None and toggle is None:
            # read
            r = self.writeCommand( 'led -i %d'% (idx) )
            return bool(r[0].strip() == '1')
        else:
            # set 
            cmd = 'led -i %d'% (idx)
            if toggle is not None:
                cmd += ' -t'
            else:
                cmd += ' -s' if on else ' -c'
            self.writeCommand( cmd )

    def gpio( self, port, i=None, o=None, s=None, c=None, t=None ):
        '''gpio control'''
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
        '''parse memory line'''
        # format:  XXXXXXXX: xx xx xx xx xx'''
        return ''.join([binascii.unhexlify(i) for i in line[10:].split()])
 
    def fillMem( self, addr, pattern, width, length ):
        '''fill memory with specific pattern'''
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

    def addReg( self, r ):
        '''add register'''
        self.regs[r.name] = r
        self.regs[r.address] = r
        
    def setReg( self, regname, value ):
        '''set register'''
        r = self.regs[regname]
        ml = []
        for i in range(r.length):
            ml.append( chr((value/(256**i))%256) )
        self.writeMem( r.address, ''.join(ml) )
    
    def getReg( self, regname ):
        '''get register'''
        r = self.regs[regname]
        m = self.readMem( r.address, length=r.length )
        ret = 0
        for i in range(r.length):
            ret += ord(m[i]) * (256**i)
        return ret


    
class SerialInstrumentTest( SerialInstrument ):
    '''for test'''
    def test( self ):
        self.dumpMem( 0, 128 )

 
if __name__ == '__main__':
    TEST = SerialInstrumentTest()
    TEST.test()
    TEST.disconnect()

