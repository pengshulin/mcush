import os
from sys import platform
import subprocess
from os.path import isdir, join
from tempfile import mktemp
from binascii import unhexlify

getenv = os.getenv

_bool_true_list = ['1', 't', 'T', 'y', 'Y']
 
def getenv_bool( key, default=None ):
    ret = getenv(key, default)
    return False if ret is None else bool(ret in _bool_true_list)
 
def getenv_float( key, default=None ):
    ret = getenv(key, default)
    return None if ret is None else float(ret)
    
def getenv_int( key, default=None ):
    ret = getenv(key, default)
    try:
        if isinstance(ret, str):
            return int(eval(ret))
        else:
            return int(ret)
    except:
        return None
    #return None if ret is None else int(ret)

TEST_DIR = getenv('TEST_DIR')
if not TEST_DIR:
    if platform == 'linux2' and isdir( '/dev/shm' ):
        TEST_DIR = '/dev/shm'
    else:
        from tempfile import gettempdir
        TEST_DIR = gettempdir()

TEST_PREFIX = getenv('TEST_PREFIX', 'mcush_test' )
TEST_WR = mktemp( prefix=TEST_PREFIX + "_wr_", dir=TEST_DIR )
TEST_RD = mktemp( prefix=TEST_PREFIX + "_rd_", dir=TEST_DIR )

if platform == 'win32':
    PORT = getenv('PORT', 'COM1')
else:
    PORT = getenv('PORT', '/dev/ttyUSB0')
PORTS = getenv( 'PORTS', '' )
BAUDRATE = getenv_int( 'BAUDRATE', 9600 )
RTSCTS = getenv_bool( 'RTSCTS' )

DELAY = getenv_float( 'DELAY', 1 )
DELAY_AFTER_RESET = getenv_float( 'DELAY_AFTER_RESET', 1 )
RETRY = getenv_int( 'RETRY', 1000 )
NO_ECHO_CHECK = getenv_bool( 'NO_ECHO_CHECK' )

DEBUG = getenv_bool( 'DEBUG' )
VERBOSE = getenv_bool( 'VERBOSE' )
INFO = getenv_bool( 'INFO' )
WARNING = getenv_bool( 'WARNING' )

PORTS = subprocess.check_output(['allports']).strip()
PORTS_LIST = PORTS.split(',')

LOGGING_FORMAT = getenv('LOGGING_FORMAT')

DATA_MODE = getenv_bool( 'DATA_MODE' )

DATA_SIZE = getenv_int( 'DATA_SIZE', 4096 )

DIRECTION = getenv_int( 'DIRECTION', 0 )

ESPEAK = getenv_bool( 'ESPEAK' )

TEST_ADDR = getenv_int( 'TEST_ADDR', 0x28000000 )
TEST_ADDR_INC = getenv_int( 'TEST_ADDR_INC', DATA_SIZE )
TEST_PATTERN = getenv_int( 'TEST_PATTERN', 0x55AA )

