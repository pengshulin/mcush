'''utilities'''
import sys
import os
import subprocess
import binascii
import Env
import random

MAX_LINE_LEN = 79

def writeTitle( title='' ):
    line = title.join( '  ' ) if title.strip() else ''
    while (len(line) + 2) <= MAX_LINE_LEN:
        line = line.join( '==' )
    if len(line) < MAX_LINE_LEN:
        line += '='
    sys.stdout.write( line + '\n' )
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


 
