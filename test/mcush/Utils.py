# coding: utf8
__doc__ = 'utilities'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from os import system, remove
from sys import platform, stdout
from binascii import hexlify, unhexlify, crc32
from random import randint
from subprocess import Popen, PIPE
from struct import pack, unpack
from time import strftime, localtime
import json
from . import Env
import traceback


MAX_LINE_LEN = 80

def writeTitle( title='', mark='=', color=True ):
    if Env.PYTHON_V3:
        title = str(title)
    else:
        title = unicode(title)
    line = title.join( '  ' ) if title.strip() else ''
    marks = mark + mark
    while len(line) < MAX_LINE_LEN:
        line = line.join( marks )
    line = line[:MAX_LINE_LEN]
    if color:
        line = colored_string(line, 'black', 'white', style='bold')
    if Env.PYTHON_V3:
        stdout.write( str(line) + '\n' )
    else:
        stdout.write( unicode(line) + '\n' )
    stdout.flush()

def generateRandomFile( size ):
    FILE = open( Env.TEST_WR, 'w+' )
    counter = 0
    while counter < size:
        FILE.write( chr(randint(0, 255)) )
        counter += 1
    FILE.close()    

def diffFiles( file1, file2 ):
    if Env.platform != 'win32':
        ret = system( 'colordiffbin %s %s'% (file1, file2) )
        return not bool(ret)
    else:
        import filecmp
        return filecmp.cmp( file1, file2 )

def diffTestFiles():
    return diffFiles( Env.TEST_WR, Env.TEST_RD )

def testSingleRandomFile( device, targetfile, size ):
    generateRandomFile( size )
    device.putFile( targetfile, Env.TEST_WR )
    device.getFile( targetfile, Env.TEST_RD )
    return diffTestFiles()

def dumpFile( fname ):
    system( "hexdump -C %s"% fname )

def dumpMem( mem, method=2 ):
    if platform == 'win32':
        method = 1  # force
    if method == 1:
        # method 1: use binascii
        stdout.write( hexlify(mem) + '\n' )
        stdout.flush()
    elif method == 2:
        # method 2: use hexdump
        proc = Popen( ['hexdump -C'], stdin = PIPE, stdout = PIPE, shell=True )
        proc.stdin.write( mem )
        proc.stdin.flush()
        proc.stdin.close()
        read = proc.stdout.read() 
        stdout.write( read + '\n' )
        stdout.flush()


def parseKeyValueLines( lines ):
    # parse lines in format:
    #   key1: val1
    #   key2:       <-- treat as empty string 
    #   key3: val3
    #   ...
    # return k-v dictionary, evaluate k/v as trimmed case-sensitive string
    # if two or more keys are the same, preserve the last one
    var = {}
    for l in lines:
        idx = l.find(': ')
        if idx <= 0:
            continue
        a = l[:idx]
        b = l[idx+2:]
        var[a.strip()] = b.strip()
    return var


def enumPorts():
    ports = []
    if platform == 'win32':
        try:
            from serial.tools import list_ports
            for p in list_ports.comports():
                ports.append( p[0] )
        except ImportError:
            pass 
    else:
        from glob import glob
        ports += glob( '/dev/ttyACM*' )
        ports += glob( '/dev/ttyUSB*' )
    return ports


def enumPortsByClass( cls, callback=None ):
    from . import Instrument
    ret = []
    for port in enumPorts():
        try:
            obj = cls( port, check_idn=False )
            obj.scpiIdn()
            if callback:
                callback( obj )
            ret.append(port)
        except Instrument.PortNotFound:
            pass
        except Instrument.IDNMatchError:
            pass
        except Instrument.CommandTimeoutError:
            pass
        except Instrument.CommandSyntaxError:
            pass
        except Instrument.CommandExecuteError:
            pass
        except Instrument.ResponseError:
            pass
        except Instrument.UnknownPortError:
            pass
        finally:
            try:
                obj.disconnect()
                del obj
            except NameError:
                pass
    return ret


def espeak( contents ):
    if Env.platform != 'win32':
        system( 'espeak %s'% contents )


# sign convert
def i2I( val ):
    return unpack('I', pack('i', val)) [0]

def I2i( val ):
    return unpack('i', pack('I', val)) [0]

def h2H( val ):
    return unpack('H', pack('h', val)) [0]

def H2h( val ):
    return unpack('h', pack('H', val)) [0]

# float convert
def i2f( val ):
    return unpack('f', pack('i', val)) [0]

def I2f( val ):
    return unpack('f', pack('I', val)) [0]

def f2i( val ):
    return unpack('i', pack('f', val)) [0]
 
def f2I( val ):
    return unpack('I', pack('f', val)) [0]

# memory convert 
def s2f( val ):
    return unpack('f', val) [0]

def s2d( val ):
    return unpack('d', val) [0]

def s2b( val ):
    return unpack('B', val) [0]
 
def s2B( val ):
    return unpack('B', val) [0]
 
def s2h( val ):
    return unpack('h', val) [0]
 
def s2H( val ):
    return unpack('H', val) [0]
 
def s2i( val ):
    return unpack('i', val) [0]
 
def s2I( val ):
    return unpack('I', val) [0]
 
def s2q( val ):
    return unpack('q', val) [0]
 
def s2Q( val ):
    return unpack('Q', val) [0]
    
def f2s( val ):
    return pack('f', val)

def d2s( val ):
    return pack('d', val)

def i2s( val ):
    return pack('i', val)

def I2s( val ):
    return pack('I', val)

def h2s( val ):
    return pack('h', val)

def H2s( val ):
    return pack('H', val)

def b2s( val ):
    return pack('b', val)

def B2s( val ):
    return pack('B', val)

def q2s( val ):
    return pack('q', val)

def Q2s( val ):
    return pack('Q', val)

DATETIME_FORMAT = '%Y-%m-%d %H:%M:%S'
def DATETIME():
    return strftime(DATETIME_FORMAT)

def I2t( val, format=None ):
    if format is None:
        format = DATETIME_FORMAT
    return strftime(format, localtime(val))

def dt2s( dt, need_ms=False ):
    hour = int(dt / 3600)
    dt -= hour*3600
    minute = int(dt / 60)
    dt -= minute * 60
    sec = int(dt)
    if need_ms:
        ms = (dt - sec) * 1000
        return '%d:%02d:%02d.%03d'% (hour, minute, sec, ms)
    else:
        return '%d:%02d:%02d'% (hour, minute, sec)



def pyobfuscate( pyin, pyout, merged_modules=[], tmpfile=None, remove_tmp=True ):
    if Env.platform == 'win32':
        print( 'must run under linux' )
        return 
    # prepare source file with specified modules merged
    src = open(pyin,'r').read()
    for m in merged_modules:
        pattern = 'from %s import *'% m
        replace = open('%s.py'% m,'r').read()
        src = src.replace( pattern, replace )
    if tmpfile is None:
        tmpfile = Env.TEST_WR
    open(tmpfile,'w+').write(src)
    # obfuscate
    system( 'pyobfuscate %s > %s'% (tmpfile, pyout) )
    if remove_tmp:
        remove(tmpfile) 

def compact_cjson_str(obj):
    LINE_LIMIT = 128
    cfg = list(json.dumps(obj,separators=(',',':')))
    lst = []
    while cfg:
        if len(cfg) <= LINE_LIMIT:
            lst.append(''.join(cfg))
            break
        p = LINE_LIMIT - 1
        while cfg[p] != ',':
            p -= 1
        lst.append(''.join(cfg[:p+1]))
        cfg = cfg[p+1:]
    #print lst 
    return lst


def STR(s):
    try:
        s = _(s)
    except:
        return s
    try:
        return s.decode('utf8')
    except:
        return s


def crc( data, oldcrc=0 ):
    v = crc32( data, oldcrc )
    return v & 0xFFFFFFFF


class colored_string():
    COLOR = {
        'black': 30, 'red': 31, 'green': 32, 'yellow': 33, 'blue': 34,
        'magenta': 35, 'cyan': 36, 'white': 37, 'reset': 39, 
        'lightblack': 90, 'lightred': 91, 'lightgreen': 92,
        'lightyellow': 93, 'lightblue': 94, 'lightmagenta': 95,
        'lightcyan': 96, 'lightwhite': 97 }
    STYLE = {
        'bold': 1, 'faint': 2, 'italic': 3, 'underline': 4, 'blink': 5, 
        'reverse': 7, 'concealed': 8, 'crossed': 9, 'normal': 22, }
    def __init__( self, string, fgcolor=None, bgcolor=None, style=None ):
        self.string = string
        self.fgcolor = fgcolor
        self.bgcolor = bgcolor
        self.style = style

    def __str__( self ):
        head = ''
        if self.style is not None:
            head += '\x1b[%dm'% (self.STYLE[self.style])
        if self.bgcolor is not None:
            head += '\x1b[%dm'% (self.COLOR[self.bgcolor]+10)
        if self.fgcolor is not None:
            head += '\x1b[%dm'% self.COLOR[self.fgcolor]
        if head:
            return self.string.join([head, '\x1b[0m'])
        else:
            return self.string

def red_string(string):
    return colored_string(string, 'red')

def green_string(string):
    return colored_string(string, 'green')

def blue_string(string):
    return colored_string(string, 'blue')

def yellow_string(string):
    return colored_string(string, 'yellow')

def magenta_string(string):
    return colored_string(string, 'magenta')

def cyan_string(string):
    return colored_string(string, 'cyan')

def bold_string(string):
    return colored_string(string, style='bold')

def faint_string(string):
    return colored_string(string, style='faint')

RED = red_string
GREEN = green_string
BLUE = blue_string
YELLOW = yellow_string
MAGENTA = magenta_string
CYAN = cyan_string
BOLD = bold_string
FAINT = faint_string

def colored_log( log_string ):
    # parse log type and colorized it
    items = log_string.split(' ')
    items_len = len(items)
    if (items_len >= 4) and (len(items[2]) == 1):
        # format: 20YY-M-D HH:MM:SS T module: msg
        TYPE = items[2] 
    elif (items_len >= 3) and (len(items[1]) == 1):
        # format: HH:MM:SS.ms T module: msg
        TYPE = items[1] 
    else:
        return colored_string(log_string, 'white', 'red', 'bold')
    if TYPE in ['E']:
        return colored_string(log_string, 'lightred')
    elif TYPE in ['W']:
        return colored_string(log_string, 'lightyellow')
    elif TYPE in ['I']:
        return log_string
    elif TYPE in ['D']:
        return faint_string(log_string)
    else:
        return colored_string(log_string, 'white', 'red', 'bold')
        

def check_traceback_for_errline(e):
    if Env.DEBUG:
        traceback.print_exc(stdout)
    err = []
    for line in traceback.format_exc().splitlines():
        line = line.rstrip()
        if line:
            err.append(line)
    err_info = err.pop()
    err_file, err_line, err_module = None, None, None
    while err:
        l = err.pop()
        try:
            if not l.startswith('  File '):
                continue
        except UnicodeDecodeError:
            continue
        try:
            _file, _line, _module = l.split(', ')
        except ValueError:
            _file, _line = l.split(', ')
            _module = '<module>'
        err_module = _module.lstrip('in ')
        if err_module == '<module>':
            err_file = _file.lstrip('  File ')
            err_line = int(_line.lstrip('line '))
            break
    return err_line



class FCFS:
    class Signature():
        def __init__( self, info ):
            self.info = info
        def __len__( self ):
            return len(self.info)
        def __str__( self ):
            return self.info
    
    class Node():
        def __init__( self, file ):
            self.file = file
            self.offset = 0
            self.size = file.size
        def __len__( self ):
            return 4
        def __str__( self ):
            return pack('H', self.offset) + pack('H', self.size)
    
    class File():
        def __init__( self, fname, contents ):
            self.fname = fname
            self.contents = contents
            self.size = len(self.contents)
        def __len__( self ):
            return len(self.fname.encode('utf8')) + 1 + self.size
        def __str__( self ):
            return self.fname.encode('utf8') + '\00' + self.contents

    def __init__( self ):
        self.reset()

    def reset( self ):
        self.head = [self.Signature('FCFS')]
        uid = [chr(randint(0,255)) for i in range(12)]
        self.uid = self.Signature(''.join(uid))
        self.head.append( self.uid )
        self.contents = []

    def setUID( self, uid ):
        while len(uid) < 12:
            uid += '\x00'
        self.uid.info = uid[:12]

    def setIntegerUID( self, integer, msb=True ):
        uid = I2s(integer&0xFFFFFFFF)
        uid += I2s((integer>>32)&0xFFFFFFFF)
        uid += I2s((integer>>64)&0xFFFFFFFF)
        if msb:
            uid = list(uid)
            uid.reverse()
            self.setUID( ''.join(uid) )
        else:
            self.setUID( uid )
 
    def appendSignature( self, item ):
        self.contents.append( item )

    def appendFile( self, name, contents ):
        f = self.File(name, contents)
        self.contents.append( f )
        self.head.append( self.Node(f) )

    def generate( self ):
        self.head.append( self.Signature('\x00\x00\x00\x00') )  # end node
        self.head.append( self.Signature(strftime('<%y%m%d%H%M%S>')) )  # timestamp
        # re-calculate the offsets
        offset_counter = sum([len(h) for h in self.head])
        for idx in range(len(self.contents)):
            f = self.head[idx+2]
            f.offset = offset_counter
            offset_counter += len(f.file)
        self.contents.append( self.Signature('END') )  # end
        output = self.head + self.contents
        output = ''.join([str(o) for o in output])
        return output
 





