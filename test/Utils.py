# coding: utf8
__doc__ = 'utilities'
__author__ = 'Peng Shulin <trees_peng@163.com>'
from os import system, remove
from sys import platform, stdout
from binascii import hexlify
from random import randint
from subprocess import Popen, PIPE
from ctypes import c_int, c_uint, c_long, c_ulong, c_int64, c_uint64
from ctypes import c_float, c_double, c_void_p
from ctypes import create_string_buffer
from ctypes import sizeof, Structure, POINTER, pointer, byref, memmove
import Env
import json


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


def enumPorts():
    ports = []
    if platform == 'linux2':
        from glob import glob
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

class int_val(Structure):
    _fields_ = [("val", c_uint)]

class float_val(Structure):
    _fields_ = [("val", c_float)]

def i2f( val ):
    ival, fval = int_val(), float_val()
    ival.val = val
    memmove( pointer(fval), pointer(ival), sizeof(float_val) )
    return fval.val
 
def f2i( val ):
    ival, fval = int_val(), float_val()
    fval.val = val
    memmove( pointer(ival), pointer(fval), sizeof(int_val) )
    return ival.val
 
def s2f( val ):
    fval = float_val()
    s = create_string_buffer(val)
    memmove( pointer(fval), pointer(s), sizeof(float_val) )
    return fval.val
 
def s2i( val ):
    ifval = int_val()
    s = create_string_buffer(val)
    memmove( pointer(ival), pointer(s), sizeof(int_val) )
    return ival.val
    
def f2s( val ):
    fval = float_val()
    fval.val = val
    s = create_string_buffer(sizeof(float_val))
    memmove( pointer(s), pointer(fval), sizeof(float_val) )
    return s.raw
 
def i2s( val ):
    ival = int_val()
    ival.val = val
    s = create_string_buffer(sizeof(int_val))
    memmove( pointer(s), pointer(ival), sizeof(int_val) )
    return s.raw


def pyobfuscate( pyin, pyout, merged_modules=[], tmpfile=None, remove_tmp=True ):
    if Env.platform != 'linux2':
        print 'must run under linux'
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

