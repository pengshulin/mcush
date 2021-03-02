# coding: utf8
__doc__ = 'MCUSH module'
__version__ = '1.5.0'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__website__ = 'https://github.com/pengshulin/mcush/'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'

from . import Env
from . import Utils
from . import Instrument
from . import Register
from . import Mcush
from . import AppUtils

try:
    from .android import *
    from .linkong import *
    from .fluke import *
    from .uni_trend import *
    from .rigol import *
    from .misc import *  # require pymodbus
except ImportError as e:
    print( 'import error:', e )
