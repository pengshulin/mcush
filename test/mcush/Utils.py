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

printTitle = writeTitle

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


def parseKeyValueLines( lines, splitter=': ' ):
    # parse lines in format:
    #   key1: val1
    #   key2:       <-- treat as empty string 
    #   key3: val3
    #   ...
    # return k-v dictionary, evaluate k/v as trimmed case-sensitive string
    # if two or more keys are the same, preserve the last one
    var = {}
    splitter_len = len(splitter)
    for l in lines:
        idx = l.find(splitter)
        if idx <= 0:
            continue
        a = l[:idx]
        b = l[idx+splitter_len:]
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
def s2f( val, big_endian=False ):
    return unpack('>f' if big_endian else '<f', val) [0]

def s2d( val ):
    return unpack('d', val) [0]

def s2b( val ):
    if Env.PYTHON_V3 and isinstance(val,int):
        return unpack('b', bytes([val])) [0]
    return unpack('b', val) [0]

def s2B( val ):
    if Env.PYTHON_V3 and isinstance(val,int):
        return unpack('B', bytes([val])) [0]
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

def s2dt( ts ):
    # format 0:00:00.000 -> float
    h, m, s = ts.split(':')
    h, m, s = int(h), int(m), float(s)
    return h*3600+m*60+s

# array convert 
def s2fa( val ):
    length = int(len(val)/4)
    return [unpack('f', val[i*4:(i+1)*4]) for i in range(length)]

def s2da( val ):
    length = int(len(val)/8)
    return [unpack('d', val[i*8:(i+1)*8]) for i in range(length)]

def s2Ia( val ):
    length = int(len(val)/4)
    return [unpack('I', val[i*4:(i+1)*4]) for i in range(length)]

def s2ia( val ):
    length = int(len(val)/4)
    return [unpack('i', val[i*4:(i+1)*4]) for i in range(length)]

def s2Ha( val ):
    length = int(len(val)/2)
    return [unpack('H', val[i*2:(i+1)*2]) for i in range(length)]

def s2ha( val ):
    length = int(len(val)/2)
    return [unpack('h', val[i*2:(i+1)*2]) for i in range(length)]

def s2Ba( val ):
    length = len(val)
    return [unpack('B', val[i]) for i in range(length)]

def s2ba( val ):
    length = len(val)
    return [unpack('b', val[i]) for i in range(length)]

BITS_REV_TAB = [
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
    0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
    0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
    0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
    0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
    0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
    0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
    0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
    0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
    0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
    0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
    0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
    0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
    0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
    0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
    0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
    0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF ]
   

def rev32b(val):
    a,b,c,d = (val>>24)&0xFF, (val>>16)&0xFF, (val>>8)&0xFF, val&0xFF
    return (BITS_REV_TAB[d]<<24)+(BITS_REV_TAB[c]<<16)+(BITS_REV_TAB[b]<<8)+BITS_REV_TAB[a]

def rev24b(val):
    a,b,c = (val>>16)&0xFF, (val>>8)&0xFF, val&0xFF
    return (BITS_REV_TAB[c]<<16)+(BITS_REV_TAB[b]<<8)+BITS_REV_TAB[a]

def rev16b(val):
    a,b = (val>>8)&0xFF, val&0xFF
    return (BITS_REV_TAB[b]<<8)+BITS_REV_TAB[a]

def rev8b(val):
    return BITS_REV_TAB[val&0xFF]



def checksum( string ):
    if Env.PYTHON_V3:
        if isinstance( string, str ):
            string = string.encode('utf8')
    else:
        if isinstance( string, unicode ):
            string = string.encode('utf8')
        if isinstance( string, str ):
            string = map(ord, list(string))
    return sum(string)


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

if Env.PYTHON_V3:
    def cmp(a,b):
        return 0 if a==b else (1 if a>b else -1) 
else:
    cmp = cmp


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
        traceback.print_exc(file=stdout)
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
            if isinstance(info, bytes):
                self.info = info
            else:
                self.info = info.encode('utf8')
        def __len__( self ):
            return len(self.info)
        def __bytes__( self ):
            return self.info
     
    class Node():
        def __init__( self, file ):
            self.file = file
            self.size = len(file.contents)
            self.offset = 0
        def __len__( self ):
            return 4
        def __bytes__( self ):
            return pack('H', self.offset) + pack('H', self.size)

    class File():
        def __init__( self, fname, contents ):
            self.fname = fname.encode('utf8')
            if isinstance( contents, bytes ):
                self.contents = contents
            else:
                self.contents = contents.encode('utf8')
        def __len__( self ):
            return len(self.fname) + len(self.contents) + 2  # two \x00 seperators
        def __bytes__( self ):
            return self.fname + b'\x00' + self.contents + b'\x00'

    def __init__( self ):
        self.reset()

    def reset( self ):
        self.head = [self.Signature('FCFS')]
        if Env.PYTHON_V3:
            uid = bytes([randint(0,255) for i in range(12)])
        else:
            uid = ''.join([chr(randint(0,255)) for i in range(12)])
        self.uid = self.Signature(uid)
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
            if Env.PYTHON_V3:
                self.setUID( bytes(uid) )
            else:
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
        newlst = [i for i in self.head]
        newlst.append( self.Signature('\x00\x00\x00\x00') )  # end node
        newlst.append( self.Signature(strftime('<%y%m%d%H%M%S>')) )  # timestamp
        # re-calculate the offsets
        offset_counter = sum([len(h) for h in newlst])
        for idx in range(len(self.contents)):
            f = self.head[idx+2]
            f.offset = offset_counter
            offset_counter += len(f.file)
        newlst += self.contents
        newlst.append( self.Signature('END') )  # end
        if Env.PYTHON_V3:
            output = Env.EMPTY_BYTE.join([bytes(o) for o in newlst])
        else:
            output = Env.EMPTY_BYTE.join([o.__bytes__() for o in newlst])
        return output






