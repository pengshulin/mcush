#!/usr/bin/env python
# coding: utf8
__doc__ = 'test suites'
__author__ = 'Peng Shulin <trees_peng@163.com>'
import os
import os.path
from os import system, remove
import sys
from sys import platform, stdout
from binascii import hexlify
from random import randint
from subprocess import Popen, PIPE
import time
import Env
from Mcush import *
import tempfile
import base64

def halt(message=None):
    print message
    raise Exception(message)

#############################################################################
# GPIO
def gpio_read( argv=None ):
    port_pin = argv[0]
    print '%s: 0x%08X'% (port_pin, Mcush().gpio(port_pin))
 
def gpio_set_high( argv=None ):
    # set gpio high for 100ms(default) and back
    port_pin = argv[0]
    try:
        ms = int(argv[1])
    except:
        ms = 100
    try:
        set_back = Env.getenv_bool(argv[2])
    except:
        set_back = True
    s = Mcush()
    s.gpio( port_pin, o=True )
    s.gpio( port_pin, s=True )
    if ms:
        time.sleep( ms/1000.0 )
    if set_back:
        s.gpio( port_pin, c=True )


#############################################################################
# LED
LED_CMD = {
'-1': -1, '1': 1, '0': 0,
's': 1, 'S': 1, 'set': 1, 'SET': 1,
'on': 1, 'ON': 1, 'c': 0,
'C': 0, 'clr': 0, 'CLR': 0,
'off': 0, 'OFF': 0, 't': -1,
'T': -1, 'toggle': -1, 'TOGGLE': -1, }

def led( argv=None ):
    if len(argv) == 1: # read status
        cmd = None
        print Mcush().led( int(argv[0]) )
    elif len(argv) == 2: # set/clr/toggle
        cmd = LED_CMD[argv[1].strip()]
        if cmd == -1:
            Mcush().led( int(argv[0]), toggle=True )
        else:
            Mcush().led( int(argv[0]), on=bool(cmd==1) )
    else:
        halt("led <led_id> [s|c|t]")
   
#############################################################################
# MISC
def info( argv=None ):
    m = Mcush()
    print m.getModel(), m.getVersion()
    m.disconnect()
 
def reset( argv=None ):
    Mcush().reset()

def regs_test( argv=None ):
    s = Mcush()
    s.addReg( reg( 'PORTA_CRL', 0x40010800, 'PORTA control (low)' ) )
    s.addReg( reg( 'PORTA_CRH', 0x40010804, 'PORTA control (high)' ) )
    s.addReg( reg( 'PORTA_IDR', 0x40010808, 'PORTA input data' ) )
    s.addReg( reg( 'PORTA_ODR', 0x40010808, 'PORTA output data' ) )
    print hex(s.getReg('PORTA_CRL'))
    print hex(s.getReg('PORTA_CRH'))
    print hex(s.getReg('PORTA_IDR'))
    print hex(s.getReg('PORTA_ODR'))
    s.setReg('PORTA_ODR', 0x12345678)

def beep( argv=None ):
    freq, duration = None, None 
    try:
        freq = int(argv[1])
        duration = float(argv[2])
    except:
        pass
    Mcush().beep( freq, duration )

def disp( argv=None ):
    if len(argv) > 0:
        Mcush().disp( buf=argv[0] )

def uptime( argv=None ):
    print Mcush().uptime()
        
def uptime_mon( argv=None ):
    s = Mcush()
    while True:
        print s.uptime()


#############################################################################
# MEMORY
def memory_read( argv=None ):
    try:
        assert 2 <= len(argv) <= 3
        addr = int(eval(argv[0]))
        length = int(eval(argv[1]))
    except:
        halt( 'parms: addr length <file_name>' )
        return
    try:
        fname = argv[2]
    except IndexError:
        fname = None
    if Env.VERBOSE:
        print 'addr: 0x%08X, length: %d'% (addr, length)
        print 'output: %s'% ('stdout' if fname is None else fname )
    if length <= 0:
        return
    mem = Mcush().readMem( addr, length, compact_mode=Env.COMPACT_MODE )
    if fname is None:
        #Utils.dumpMem( mem, method=1 ) 
        open(Env.TEST_WR, 'w+').write(mem)
        #Utils.dumpFile( Env.TEST_WR )
        sys.stdout.write( mem )  # write binary directly to console
        os.remove( Env.TEST_WR )
    else:
        open(fname, 'w+').write(mem)


def memory_read_loop( argv=None ):
    s = Mcush()
    count = 1
    while True:
        print time.strftime('%H:%M:%S'), count
        mem = s.readMem( 0x20000000, 20*1024 )
        #Utils.dumpMem( mem ) 
        count += 1

def memory_read_speed_test( argv=None ):
    s = Mcush()
    SIZE = 20*1024
    while True:
        t0 = time.time()
        mem = s.readMem( 0x20000000, SIZE, compact_mode=Env.COMPACT_MODE )
        t1 = time.time()
        speed = len(mem)/(t1-t0)
        print 'read %d bytes, %.3f kB/s'% (SIZE, speed/1000.0)

#############################################################################
# FILESYSTEM
def spiffs_info( argv=None ):
    print Mcush().spiffs( 'info' )

def spiffs_format( argv=None ):
    Mcush().spiffs( 'format' )

def spiffs_ls( argv=None ):
    for f, s in Mcush().list('/s'):
        print '%s %s'% (f,s)

def spiffs_remove( argv=None ):
    f = argv[0]
    if not f.startswith('/s/'):
        f = os.path.join('/s', f)
    Mcush().remove( f )
 
def spiffs_removeall( argv=None ):
    m = Mcush()
    for f, s in m.list('/s'):
        print 'remove %s'% f
        m.remove( f )
   
def spiffs_loop_append( argv=None ):
    buf = open('/proc/misc','r').read()
    s = Mcush()
    while True:
        print s.list()
        for i in range(10):
            print '/s/test.%d'% i
            s.cat( '/s/test.%d'% i, append=True, buf=buf )
 
def spiffs_fill_small_files( argv=None ):
    buf = open('/proc/misc','r').read()
    s = Mcush()
    print s.list()
    while True:
        f = tempfile.mktemp(prefix='test', dir='/s')
        print f
        s.cat( f, append=True, buf=buf )
 
def spiffs_get( argv=None ):
    if len(argv) < 1:
        print 'syntax: spiffs_get <remote_pathname> [local_pathname]'
        return
    r = Mcush().cat( argv[0], b64=True )
    r = base64.b64decode('\n'.join(r))
    if len(argv) > 1:
        open(argv[1], 'w+').write(r)
    else:
        print r






#############################################################################
if __name__ == '__main__':
    app_name = os.path.basename(sys.argv[0])
    if app_name == 'Tests.py':
        if len(sys.argv) < 2:
            print 'Syntax: Test.py test_command <args>'
            sys.exit(1)
        test_command = sys.argv[1]
        test_argv = sys.argv[2:] 
    else:
        # symbolic linked commands
        test_command = sys.argv[0]
        test_argv = sys.argv[1:]
    if test_command.endswith('.py'):
        test_command = test_command[:-3]
    test_command = test_command.split('.')[-1]
    try:
        command = eval(test_command)
        # TODO: verify command
    except:
        print 'invalid command %s'% test_command
        sys.exit(1)
    #print test_command, test_argv
    command(test_argv) 
    
    

