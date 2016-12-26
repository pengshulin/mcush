'''utilities'''
import os
import sys
import binascii
import subprocess
import Env
import random
import ctypes
from ctypes import c_int, c_uint, c_long, c_ulong, c_int64, c_uint64
from ctypes import c_float, c_double, c_void_p
from ctypes import create_string_buffer
from ctypes import sizeof, Structure, POINTER, pointer, byref, memmove


MAX_LINE_LEN = 80

def writeTitle( title='', mark='=' ):
    line = title.join( '  ' ) if title.strip() else ''
    marks = mark + mark
    while len(line) < MAX_LINE_LEN:
        line = line.join( marks )
    sys.stdout.write( line[:MAX_LINE_LEN] + '\n' )
    sys.stdout.flush()

def generateRandomFile( size ):
    FILE = open( Env.TEST_WR, 'wb+' )
    counter = 0
    while counter < size:
        FILE.write( chr(random.randint(0, 255)) )
        counter += 1
    FILE.close()    

def diffFiles( file1, file2 ):
    if Env.platform == 'linux2':
        ret = os.system( 'colordiffbin %s %s'% (file1, file2) )
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
    os.system( "hexdump -C %s"% fname )

def dumpMem( mem, method=2 ):
    if sys.platform == 'win32':
        method = 1  # force
    if method == 1:
        # method 1: use binascii
        sys.stdout.write( binascii.hexlify(mem) + '\n' )
        sys.stdout.flush()
    elif method == 2:
        # method 2: use hexdump
        proc = subprocess.Popen( ['hexdump -C'], 
                                 stdin = subprocess.PIPE,
                                 stdout = subprocess.PIPE,
                                 shell=True )
        proc.stdin.write( mem )
        proc.stdin.flush()
        proc.stdin.close()
        read = proc.stdout.read() 
        sys.stdout.write( read + '\n' )
        sys.stdout.flush()


def enumPorts():
    ports = []
    if sys.platform == 'linux2':
        from glob import glob
        ports += glob( '/dev/ttyUSB*' )
    elif sys.platform == 'win32':
        try:
            from serial.tools import list_ports
            for p in list_ports.comports():
                ports.append( p[0] )
        except ImportError:
            pass 
    return ports


def espeak( contents ):
    if Env.platform == 'linux2':
        os.system( 'espeak %s'% contents )

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
 
