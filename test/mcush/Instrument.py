# coding: utf8
__doc__ = 'instrument class with basic port communication'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
import time
import logging
from . import Env, Utils

if Env.LOGGING_FORMAT:
    logging.BASIC_FORMAT = Env.LOGGING_FORMAT
else:
    logging.BASIC_FORMAT = '%(asctime)s ' + logging.BASIC_FORMAT

class PortNotFound( Exception ):
    pass

class UnknownPortError( Exception ):
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

class IDNMatchError( Exception ):
    pass




class Instrument:
    '''abstract instrument class'''
    
    DEFAULT_NAME = 'INST'
    DEFAULT_TERMINATOR_WRITE = '\x0A'  # '\n'
    DEFAULT_TERMINATOR_READ = '\x0A'  # '\n'
    DEFAULT_TERMINATOR_RESET = '\x03'  # Ctrl-C
    DEFAULT_BAUDRATE = 9600
    DEFAULT_TIMEOUT = 5
    DEFAULT_PROMPTS = re_compile( '[=#?!]>' )
    DEFAULT_PROMPTS_MULTILINE = re_compile( '[=#?!]?>' )
    DEFAULT_IDN = re_compile( '.*' )

    DEFAULT_TERMINAL_RESET = True
    DEFAULT_CHECK_IDN = True
    DEFAULT_READ_UNTIL_PROMPTS = True
    DEFAULT_CHECK_RETURN_COMMAND = True
    DEFAULT_LINE_PROMPT_MODE = False
   

    def __init__( self, *args, **kwargs ):
        '''init'''
        # logging level 
        self.verbose = Env.VERBOSE
        self.debug = Env.DEBUG
        self.info = Env.INFO
        self.warning = Env.WARNING
        if self.debug:
            level = logging.DEBUG
        elif self.info:
            level = logging.INFO
        elif self.warning:
            level = logging.WARNING
        else:
            level = logging.ERROR
        logging.basicConfig( level=level, format=Env.LOG_FORMAT, datefmt=Env.LOG_DATEFMT )
        self.logger = logging.getLogger( self.DEFAULT_NAME )
        self.returned_cmd = None
        self.returned_prompt = None

        # load from parms/env/default and saved as attributes
        self.logger.debug( '__init__: args: %s'% str(args) )
        try:
            kwargs['port'] = args[0]
        except:
            pass
        if not 'port' in kwargs:
            kwargs['port'] = Env.PORT
        if not 'baudrate' in kwargs:
            if Env.BAUDRATE:
                kwargs['baudrate'] = Env.BAUDRATE
            else:
                kwargs['baudrate'] = self.DEFAULT_BAUDRATE
        if not 'rtscts' in kwargs:
            kwargs['rtscts'] = Env.RTSCTS
        if not 'parity' in kwargs:
            kwargs['parity'] = Env.PARITY
        if not 'connect' in kwargs:
            kwargs['connect'] = True
        if not 'prompts' in kwargs:
            kwargs['prompts'] = self.DEFAULT_PROMPTS
        if not 'timeout' in kwargs:
            kwargs['timeout'] = self.DEFAULT_TIMEOUT
        if 'terminal_reset' in kwargs:
            self.DEFAULT_TERMINAL_RESET = bool(kwargs['terminal_reset'])
        if 'check_idn' in kwargs:
            self.DEFAULT_CHECK_IDN = bool(kwargs['check_idn'])
        # some attributes 'connect', ...  need to be renamed for method conflict
        for n in ['connect', 'baudrate', 'timeout']:
            kwargs['_'+n] = kwargs.pop(n)
        # attached as attributes
        for k, v in kwargs.items():
            self.__dict__[k] = v
        self.logger.debug( '__init__: %s'% str(self.__dict__) )

        # init/connect
        self.idn = None
        self.serial_number = None
        self.port = self.PORT_TYPE(self, *args, **kwargs)
        if self._connect:
            self.connect()

    @property        
    def connected( self ):
        return self.port.connected

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

    def setPrompts( self, prompts=None ):
        '''dynamically modify the prompts'''
        old_prompts = self.prompts
        if prompts is None:
            prompts = self.DEFAULT_PROMPTS
        self.prompts = prompts
        return old_prompts

    def setTimeout( self, new=None ):
        if new is None:
            new = self.DEFAULT_TIMEOUT
        old = self.port.timeout
        self.port.timeout = new
        return old

    def connect( self ):
        '''connect'''
        self.port.connect()
        if not self.port.connected:
            return
        if self.DEFAULT_TERMINAL_RESET:
            self.logger.debug( '[RST]' )
            self.port.write( self.DEFAULT_TERMINATOR_RESET )
            self.port.flush()
            if self.DEFAULT_READ_UNTIL_PROMPTS:
                self.readUntilPrompts()
        if self.DEFAULT_CHECK_IDN:
            self.scpiIdn()

    def disconnect( self ):
        '''disconnect'''
        if self.port.connected:
            self.port.disconnect()

    def assertIsOpen( self ):
        '''assert port is opened'''
        if not self.port.connected:
            self.port.connect()
            if not self.port.connect:
                raise Exception("Fail to open port")
  
    def readUntilPrompts( self, line_callback=None ):
        '''read until prompts'''
        if self.DEFAULT_LINE_PROMPT_MODE:
            return self._readUntilPromptsByLine( line_callback )
        else:
            return self._readUntilPromptsByChar( line_callback )

    def _readUntilPromptsByChar( self, line_callback=None ):
        contents, newline_lst, newline_str = [], [], ''
        while True:
            byte = self.port.read(1)
            if byte:
                if Env.PYTHON_V3:
                    byte = chr(ord(byte))
                if byte == self.DEFAULT_TERMINATOR_READ:
                    newline_str = newline_str.rstrip()
                    contents.append( newline_str )
                    self.logger.debug( '[R] '+ newline_str )
                    if line_callback is not None:
                        line_callback( newline_str )
                    newline_lst, newline_str = [], ''
                else:
                    newline_lst.append( byte )
                    newline_str += byte
            else:
                contents.append( newline_str )
                self.logger.debug( '[R] '+ newline_str )
                if contents:
                    raise CommandTimeoutError( ' | '.join(contents) )
                else:
                    raise CommandTimeoutError( 'No response' )
            match = self.prompts.match( newline_str )
            if match:
                contents.append( newline_str )
                #self.logger.debug( '[P] '+ newline_str )
                return contents

    def _readUntilPromptsByLine( self, line_callback=None ):
        contents = []
        while True:
            newline_str = self.readLine( strip=False )
            if newline_str is None:
                if contents:
                    raise Instrument.CommandTimeoutError( ' | '.join(contents) )
                else:
                    raise Instrument.CommandTimeoutError( 'No response' )
            match = self.prompts.match( newline_str )
            if match:
                contents.append( newline_str )
                #self.logger.debug( '[P] '+ newline_str )
                return contents
            else:
                contents.append( newline_str )
                self.logger.debug( '[R] '+ newline_str )
                if line_callback is not None:
                    line_callback( newline_str )
                
    def readLine( self, eol=None, timeout=None, decode_utf8=True, strip=True ):
        chars = []
        if eol is None:
            eol = self.DEFAULT_TERMINATOR_READ
        if Env.PYTHON_V3:
            eol = bytes(eol, encoding='utf8')
        if timeout is not None:
            old_timeout = self.port.timeout
            self.port.timeout = timeout
        while True:
            char = self.port.read(1) 
            if char:
                if char == eol:
                    break
                chars.append( char )
            else:
                if chars:
                    ret = Env.EMPTY_BYTE.join(chars)
                    self.logger.debug( '[E] '+ str(ret) )
                return None
                #raise CommandTimeoutError( 'readLine timeout' )
                #break
        if timeout is not None:
            self.port.timeout = old_timeout
        if chars:
            ret = Env.EMPTY_BYTE.join(chars)
            if strip:
                ret = ret.strip()
            if decode_utf8:
                ret = ret.decode('utf8')
                self.logger.debug( '[R] '+ ret )
            else:
                self.logger.debug( '[R] '+ str(ret) )
            return ret
        else:
            return ''

    def writeLine( self, dat, encode_utf8=True ):
        if isinstance( dat, bytes ):
            self.logger.debug( '[T] '+str(dat) )
        else:
            self.logger.debug( '[T] '+dat )
        if encode_utf8:
            if Env.PYTHON_V3:
                if isinstance( dat, str ):
                    dat = dat.encode('utf8')
            else:
                if isinstance( dat, unicode ):
                    dat = dat.encode('utf8')
        self.port.write( dat )
        self.port.write( self.DEFAULT_TERMINATOR_WRITE )
        self.port.flush()
   
    def writeCommand( self, cmd, strip=True ):
        '''write command and wait for prompts'''
        if strip:
            cmd = cmd.strip()
        self.writeLine( cmd )
        if self.DEFAULT_READ_UNTIL_PROMPTS:
            ret = self.readUntilPrompts()
            self.checkReturnedPrompt( ret )
            if cmd and self.DEFAULT_CHECK_RETURN_COMMAND and not Env.NO_ECHO_CHECK:
                self.checkReturnedCommand( ret, cmd )
                return ret[1:-1] 
            else:
                return ret[:-1] 
        else:
            return []
    
    def writeCommandRetry( self, cmd, retry=None ):
        '''write command with retry '''
        if retry is None:
            retry = Env.COMMAND_FAIL_RETRY
        assert retry > 1
        for r in range(retry-1):
            try:
                ret = self.writeCommand( cmd )
                return ret
            except Exception as e:
                if Env.VERBOSE:
                    print( e )
        return self.writeCommand( cmd )
  
    def checkReturnedCommand( self, ret, cmd ):
        '''assert command returned is valid'''
        cmdret = ret[0]
        if Env.PYTHON_V3 and isinstance(cmdret, bytes):
            cmdret = cmdret.encode('utf8')
        if cmd != cmdret:
            raise ResponseError('Command %s, but returned %s'% (cmd, cmdret))

    def checkReturnedPrompt( self, ret ):
        '''assert prompt is valid'''
        self.returned_cmd = cmd = ret[0].strip()
        self.returned_prompt = prompt = ret[-1].strip()
        if prompt in ['?>', '?']:
            result = ret[1:-1]
            if result:
                raise CommandSyntaxError( cmd + ', returns: ' + ','.join(result) )
            else:
                raise CommandSyntaxError( cmd )
        elif prompt == '!>':
            result = ret[1:-1]
            if result:
                raise CommandExecuteError( cmd + ', returns: ' + ','.join(result) )
            else:
                raise CommandExecuteError( cmd )

    # Instrument class only supports basic commands:
    # 
    # 1. scpi identify
    #    =>*idn?
    #    mcush,1.1          --- model, version
    #    NNNNNNNNNNNNNNNN   --- serial number (if exists)
    # 
    # 2. scpi reset
    #    =>*rst
    # 

    def scpiRst( self ):
        '''scpi reset'''
        self.writeCommand( '*rst' )

    def scpiIdn( self, check=True ):
        '''get identify name'''
        ret = self.writeCommand( '*idn?' )
        self.idn = ret[0].strip()
        if len(ret)>1:
            self.serial_number = ret[1].strip()
        self.logger.debug( 'IDN:%s', str(self.idn) )
        if check and (not Env.NO_IDN_CHECK):
            if not self.DEFAULT_IDN.match( self.idn ):
                raise IDNMatchError(self.idn.split(',')[0])
        return self.idn

    def getModel( self ):
        if self.idn is None:
            self.scpiIdn()
        try:
            return self.idn.split(',')[0]
        except IndexError:
            return ''

    def getVersion( self ):
        if self.idn is None:
            self.scpiIdn()
        try:
            return self.idn.split(',')[1]
        except IndexError:
            return ''

    def getSerialNumber( self ):
        if self.serial_number is None:
            ret = self.writeCommand( '*idn?' )
            if len(ret) > 1:
                self.serial_number = ret[1].strip()
        self.logger.info( 'SN:%s', str(self.serial_number) )
        if self.serial_number is None:
            return ''
        return self.serial_number

    def getIntegerSerialNumber( self, msb=True ):
        _sn = Utils.unhexlify(self.getSerialNumber())
        if Env.PYTHON_V3:
            sn = list(_sn)
        else:
            sn = [Utils.s2B(b) for b in _sn]
        ret = 0 
        while sn:
            if msb:
                ret = (ret<<8) | sn.pop(0)
            else:
                ret = (ret<<8) | sn.pop()
        return ret

    def printInfo( self ):
        print( '%s, %s'% (self.getModel(), self.getVersion()) )


class Port(object):
    
    def __init__( self, parent, *args, **kwargs ):
        self.parent = parent
        for k, v in kwargs.items():
            self.__dict__[k] = v
        self._connected = False

    def __del__( self ):
        self.disconnect()

    def connect( self ):
        raise NotImplementedError

    def disconnect( self ):
        pass
    
    @property        
    def connected( self ):
        return self._connected

    def read( self, read_bytes=1 ):
        raise NotImplementedError

    def write( self, buf ):
        raise NotImplementedError

    def flush( self ):
        pass

    def readall( self ):
        raise

    def update_baudrate( self, baudrate ):
        pass

    def update_timeout( self, timeout ):
        pass 

    @property
    def baudrate( self ):
        return self._baudrate

    @baudrate.setter
    def baudrate( self, val ):
        self._baudrate = val
        self.update_baudrate( val )

    @property
    def timeout( self ):
        return self._timeout

    @timeout.setter
    def timeout( self, val ):
        self._timeout = val
        self.update_timeout( val )


class SerialPort(Port):
    
    def __init__( self, parent, *args, **kwargs ):
        import serial
        self.serial_exception = serial.SerialException 
        try:
            self.ser = serial.serial_for_url( self.port, do_not_open=True )
        except AttributeError:
            self.ser = serial.Serial()
        Port.__init__( self, parent, *args, **kwargs )
   
    def connect( self ):
        if self._connected:
            return
        self.ser.port = self.port
        self.ser.baudrate = self.baudrate
        self.ser.rtscts = self.rtscts
        self.ser.parity = self.parity
        self.ser.timeout = self.timeout
        try:
            self.ser.open()
            self._connected = True
            self.ser.reset_output_buffer()
            self.ser.reset_input_buffer()
        except IOError:
            raise PortNotFound( self.port )
        except Exception as e:
            #print( e )
            raise UnknownPortError( e )

    def disconnect( self ):
        if self._connected:
            self.ser.close()
            self._connected = False

    def update_baudrate( self, baudrate ):
        self.ser.baudrate = baudrate

    def update_timeout( self, timeout ):
        self.ser.timeout = timeout
        
    def read( self, read_bytes=1 ):
        try:
            return self.ser.read(read_bytes)
        except self.serial_exception as e:
            raise UnknownPortError( str(e) )
    
    def readall( self ):
        return self.read( self.ser.in_waiting )

    def write( self, buf ):
        try:
            #print(type(buf), buf)
            if Env.PYTHON_V3 and isinstance(buf, str):
                buf = buf.encode("utf8")
            self.ser.write( buf )
        except self.serial_exception as e:
            raise UnknownPortError( str(e) )

    def flush( self ):
        try:
            self.ser.flush()
        except self.serial_exception as e:
            raise UnknownPortError( str(e) )


class SocketPort(Port):
        
    def __init__( self, parent, *args, **kwargs ):
        import socket
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        Port.__init__( self, parent, *args, **kwargs )
        
    def connect( self ):
        if self._connected:
            return
        self.s.connect( (self.net_addr, int(self.net_port)) )
        self.s.settimeout( self.timeout )
        self._connected = True

    def disconnect( self ):
        self._connected = False
        self.s.close()
    
    def read( self, read_bytes=1 ):
        if self._connected:
            ret = []
            for i in range(read_bytes):
                r = self.s.recv(1)
                if r:
                    ret.append( r )
                else:
                    break
            #print( 'read', ret )
            return Env.EMPTY_BYTE.join(ret)

    def write( self, buf ):
        if self._connected:
            if Env.PYTHON_V3:
                if isinstance(buf, str):
                    buf = buf.encode('utf8')
            self.s.sendall(buf)



class SerialInstrument(Instrument):
    '''Serial port based instruments'''
    PORT_TYPE = SerialPort


class SocketInstrument(Instrument):
    '''Socket port based instruments'''
    PORT_TYPE = SocketPort


