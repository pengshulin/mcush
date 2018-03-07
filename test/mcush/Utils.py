# coding: utf8
__doc__ = 'utilities'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from os import system, remove
from sys import platform, stdout
from binascii import hexlify
from random import randint
from subprocess import Popen, PIPE
from struct import pack, unpack
from time import strftime, localtime
import json
from . import Env


MAX_LINE_LEN = 80

def writeTitle( title='', mark='=' ):
    title = unicode(title)
    line = title.join( '  ' ) if title.strip() else ''
    marks = mark + mark
    while len(line) < MAX_LINE_LEN:
        line = line.join( marks )
    stdout.write( line[:MAX_LINE_LEN] + '\n' )
    stdout.flush()

def generateRandomFile( size ):
    FILE = open( Env.TEST_WR, 'wb+' )
    counter = 0
    while counter < size:
        FILE.write( chr(randint(0, 255)) )
        counter += 1
    FILE.close()    

def diffFiles( file1, file2 ):
    if Env.platform == 'linux2':
        ret = system( 'colordiffbin %s %s'% (file1, file2) )
        return not bool(ret)
    else:
        import filecmp
        return filecmp.cmp( file1, file2 )

def diffTestFiles():
    return diffFiles( Env.TEST_WR, Env.TEST_RD )

def testSingleRandomFile( device, targetfile, size ):
    generateRandomFile( size )
    buf = open(Env.TEST_WR, 'rb').read()
    device.cat( targetfile, True, True, False, buf )
    ret = device.cat( targetfile, True, False, False )
    open( Env.TEST_RD, 'wb+' ).write( ret )
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


def enumPorts():
    ports = []
    if platform == 'linux2':
        from glob import glob
        ports += glob( '/dev/ttyACM*' )
        ports += glob( '/dev/ttyUSB*' )
    elif platform == 'win32':
        try:
            from serial.tools import list_ports
            for p in list_ports.comports():
                ports.append( p[0] )
        except ImportError:
            pass 
    return ports


def espeak( contents ):
    if Env.platform == 'linux2':
        system( 'espeak %s'% contents )


def i2f( val ):
    return unpack('f', pack('i', val)) [0]

def I2f( val ):
    return unpack('f', pack('I', val)) [0]

def f2i( val ):
    return unpack('i', pack('f', val)) [0]
 
def f2I( val ):
    return unpack('I', pack('f', val)) [0]
 
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


def I2t( val, format=None ):
    if format is None:
        format = '%y-%m-%d %H:%M:%S'
    return strftime(format, localtime(val))



def pyobfuscate( pyin, pyout, merged_modules=[], tmpfile=None, remove_tmp=True ):
    if Env.platform != 'linux2':
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
