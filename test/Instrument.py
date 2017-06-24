# coding: utf8
__doc__ = 'instrument class with basic port communication'
__author__ = 'Peng Shulin <trees_peng@163.com>'
from re import compile as re_compile
from time import sleep
from binascii import unhexlify
import logging
import serial
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

class CommandExecuteError( Exception ):
    pass

class CommandTimeoutError( Exception ):
    pass

    

class reg:
    '''mcu register'''
    def __init__( self, name, address, description, reg_type=int, width=32 ):
        self.name = str(name)
        self.address = int(address)
        self.description = str(description)
        self.width = width
        self.length = (width+7)/8
        self.reg_type = reg_type
        assert 0 < self.length < 32
        assert reg_type in [int, float, str]

    def __str__( self ):
        return self.name


class SerialInstrument:
    '''Instruments based on serial port'''
    DEFAULT_NAME = 'INST'
    DEFAULT_TERMINATOR_WRITE = '\x0A'  # '\n'
    DEFAULT_TERMINATOR_READ = '\x0A'  # '\n'
    DEFAULT_TERMINATOR_RESET = '\x03'  # Ctrl-C
    DEFAULT_TIMEOUT = 5
    DEFAULT_PROMPTS = re_compile( '[=#?!]>' )
    DEFAULT_PROMPTS_MULTILINE = re_compile( '>' )
    DEFAULT_IDN = None
    DEFAULT_RESET_RETRY = 10
    DEFAULT_LINE_LIMIT = 128
    

    def __init__( self, port=None, baudrate=None, rtscts=False, connect=True ):
        '''init'''
        self.verbose = Env.VERBOSE
        self.debug = Env.DEBUG
        self.info = Env.INFO
        self.prompts = self.DEFAULT_PROMPTS
        self.regs_by_name = {}
        self.regs_by_addr = {}

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

    def serialOutput( self, cmd ):
        self.assertIsOpen() 
        self.ser.flushInput()
        self.ser.write( cmd + self.DEFAULT_TERMINATOR_WRITE )
        self.ser.flush()
   
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
    
    def writeCommandRetry( self, cmd, retry=None ):
        '''write command with retry '''
        if retry is None:
            retry = Env.COMMAND_FAIL_RETRY
        assert retry > 1
        for r in range(retry-1):
            try:
                ret = self.writeCommand( cmd )
                return ret
            except Exception, e:  # capture ANY exception
                if Env.VERBOSE:
                    print e
        return self.writeCommand( cmd )
  
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
            raise CommandExecuteError( err )
 
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
            sleep( Env.DELAY_AFTER_RESET )
        else:
            sleep( delay )

    def getModel( self ):
        if self.idn is None:
            self.scpiIdn()
        try:
            return self.idn.split(',')[0]
        except IndexError:
            return ''

    def getSerialNumber( self ):
        ret = self.writeCommand( '*idn?' )
        try:
            self.serial_number = ret[1].strip()
            self.logger.info( 'SN:%s', str(self.serial_number) )
            return self.serial_number
        except IndexError:
            self.serial_number = None
            return ''

    def getVersion( self ):
        if self.idn is None:
            self.scpiIdn()
        try:
            return self.idn.split(',')[1]
        except IndexError:
            return ''

    def printInfo( self ):
        print '%s, %s'% (self.getModel(), self.getVersion())
    
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

    def parseMemLine( self, line, compact_mode=False ):
        '''parse memory line which has been stripped'''
        # format(standard):  XXXXXXXX: xx xx xx xx xx ... xx    (no ascii mode, final space stripped)
        # format(compact):   xxxxxxxxxxxx...xx
        line_len = len(line)
        line_len_err = bool(line_len % 2) if compact_mode else bool((line_len-9) % 3)
        if line_len_err:
            raise CommandExecuteError('memory line width error, length=%s'% line_len)
        if compact_mode:
            return ''.join([unhexlify(line[i:i+2]) for i in range(0,line_len,2)])
        else:
            return ''.join([unhexlify(line[i:i+2]) for i in range(10,line_len,3)])
       
    def fillMem( self, addr, pattern, width, length ):
        '''fill memory with specific pattern'''
        cmd = 'mfill -b 0x%X -w %d -p 0x%X -l %d'% ( addr, width, pattern, length )
        self.writeCommand( cmd )
 
    def testMem( self, addr, pattern, width, length ):
        '''test filled memory with specific pattern'''
        cmd = 'mfill -t -b 0x%X -w %d -p 0x%X -l %d'% ( addr, width, pattern, length )
        try:
            self.writeCommand( cmd )
            return True
        except CommandExecuteError:
            return False
        
    def malloc( self, length ):
        cmd = 'mapi -m -l %d'% length
        ret = self.writeCommand(cmd)
        addr = eval(ret[0])
        return addr

    def realloc( self, addr, length ):
        cmd = 'mapi -r -b 0x%08X -l %d'% (addr, length)
        ret = self.writeCommand(cmd)
        addr = eval(ret[0])
        return addr

    def free( self, addr ):
        cmd = 'mapi -f -b 0x%08X'% addr
        self.writeCommand( cmd )
 
    def readMem( self, addr, length=1, compact_mode=False, retry=None ):
        '''get memory'''
        cmd = 'x -b 0x%X -l %d'% ( addr, length )
        if compact_mode:
            cmd += ' -c'
        if retry:
            ret = self.writeCommandRetry( cmd, retry )
        else:
            ret = self.writeCommand( cmd )
        assert 1 <= len(ret)
        for line in ret:
            self.logger.info( line )
        mem = ''.join([self.parseMemLine(line.strip(), compact_mode) for line in ret])
        return mem[:length]

    MAX_ITEMS_PER_WRITE = { 1: 16, 2: 8, 4: 4  }

    def writeMem( self, addr, data, width=1 ):
        '''write memory'''
        length = len(data)
        written = 0
        while written < length:
            cmd = 'w -b 0x%X -w%d '% ( addr, width )
            left = min(length-written, self.MAX_ITEMS_PER_WRITE[width])
            if width == 1:
                cmd += ' '.join([str(ord(c)) for c in data[written:written+left]])
            elif width == 2:
                cmd += ' '.join([hex(ord(data[i])+(ord(data[i+1])<<8)) for i in range(written, written+left*2, 2)])
            elif width == 4:
                cmd += ' '.join([hex(ord(data[i])+(ord(data[i+1])<<8)+(ord(data[i+2])<<16)+(ord(data[i+3])<<24)) for i in range(written, written+left*4, 4)])
            self.writeCommand( cmd )
            written += left*width
            addr += left*width

    def dumpMem( self, addr, length=1 ):
        '''dump memory'''
        Utils.dumpMem( self.readMem(addr, length) )

    def addReg( self, r ):
        '''add register'''
        self.regs_by_name[r.name] = r
        self.regs_by_addr[r.address] = r
        
    def setReg( self, regname, value ):
        '''set register'''
        r = self.regs_by_name[regname]
        if r.reg_type == float:
            m = Utils.f2s(value)
        elif r.reg_type == int:
            m = Utils.i2s(value)
        else:
            m = str(value)
        #print 'setReg', regname, value, binascii.hexlify(m)
        self.writeMem( r.address, m )
    
    def getReg( self, regname ):
        '''get register'''
        r = self.regs_by_name[regname]
        m = self.readMem( r.address, length=r.length )
        if r.reg_type == float:
            return Utils.s2f(m)
        elif r.reg_type == int:
            return Utils.s2i(m)
        else:
            return m

    def uptime( self ):
        return self.writeCommand('uptime')[0].strip()

    def sysTask( self ):
        ret = self.writeCommand('sys t')
        return ret

    def sysQueue( self ):
        ret = self.writeCommand('sys q')
        return ret

    def beep( self, freq=None, duration=0.05 ):
        cmd = 'beep'
        if freq:
            cmd += ' -f %d'% freq
        if duration:
            cmd += ' %d'% (int(duration*1000))   
        self.writeCommand(cmd)


 
class SerialInstrumentTest( SerialInstrument ):
    '''for test'''
    def test( self ):
        self.dumpMem( 0, 128 )

 
if __name__ == '__main__':
    TEST = SerialInstrumentTest()
    TEST.test()
    TEST.disconnect()

