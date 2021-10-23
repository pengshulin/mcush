# coding: utf8
__doc__ = 'basic environment'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from os import getenv
from os.path import isdir, join
from sys import platform, version_info
from binascii import unhexlify
from tempfile import mktemp
from subprocess import check_output


_bool_true_list = ['1', 'Y', 'y', 'T', 't', 'yes', 'Yes', 'YES', 'true', 'True', 'TRUE']


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
TEST_MINID = getenv_int('TEST_MINID', 0)
TEST_MAXID = getenv_int('TEST_MAXID', 100)
TEST_MAXSIZE = getenv_int('TEST_MAXSIZE', 10*1024)


if platform == 'win32':
    PORT = getenv('PORT', 'COM1')
else:
    PORT = getenv('PORT', '/dev/ttyUSB0')
    try:
        PORTS = check_output(['allports']).strip().decode(encoding='utf8')
        PORTS_LIST = PORTS.split(',')
    except:
        PORTS_LIST = []
BAUDRATE = getenv_int( 'BAUDRATE', 0 )
RTSCTS = getenv_bool( 'RTSCTS' )
PARITY = getenv( 'PARITY', 'N' )
TIMEOUT = getenv_int('TIMEOUT', 5)
COMMAND_FAIL_RETRY = getenv_int( 'COMMAND_FAIL_RETRY', 3 )

DELAY = getenv_float( 'DELAY', 1 )
RETRY = getenv_int( 'RETRY', 1000 )
NO_ECHO_CHECK = getenv_bool( 'NO_ECHO_CHECK' )
NO_IDN_CHECK = getenv_bool( 'NO_IDN_CHECK' )
COMPACT_MODE = getenv_bool( 'COMPACT_MODE' )

DEVELOPMENT = getenv_bool( 'DEVELOPMENT' )
PROFILE = getenv_bool( 'PROFILE' )
LOGGING_FORMAT = getenv('LOGGING_FORMAT')

DEBUG = getenv_bool( 'DEBUG' )
VERBOSE = getenv_bool( 'VERBOSE' )
INFO = getenv_bool( 'INFO' )
WARNING = getenv_bool( 'WARNING' )


ESPEAK = getenv_bool( 'ESPEAK' )

PDF_READER = None
SOFFICE_BIN = None 
if platform == 'win32':
    PDF_READER = 'AcroRd32.exe'
    SOFFICE_BIN = 'soffice.exe'
    MPLAYER_BIN = 'mplayer.exe'
    NOTEPAD_BIN = 'notepad.exe'
elif platform == 'linux2':
    PDF_READER = '/usr/bin/evince'
    SOFFICE_BIN = '/usr/bin/soffice'
    MPLAYER_BIN = '/usr/bin/mplayer'
    NOTEPAD_BIN = '/usr/bin/mousepad'

REV = getenv_bool( 'REV' )
SIM = getenv_bool( 'SIM' )

LANGUAGES = {'en', 'zh_cn'}
LANGUAGE = getenv( 'LANGUAGE', 'en' ).split(':')[0].lower()

PYTHON_V3 = bool(version_info > (3, 0))
if PYTHON_V3:
    EMPTY_BYTE = b''
    ZERO_BYTE = b'\x00'
else:
    EMPTY_BYTE = ''
    ZERO_BYTE = '\x00'

LOG_DATEFMT = '%Y-%m-%d %H:%M:%S'
LOG_FORMAT = '%(asctime)s %(name)s %(levelname)s %(message)s'

try:
    if PYTHON_V3:
        from mcush.EnvExtra import *
    else:
        from EnvExtra import *
except ImportError:
    pass

