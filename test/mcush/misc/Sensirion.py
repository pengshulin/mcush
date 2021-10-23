# coding: utf8
__doc__ = 'Sensirion sensors'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Instrument, Utils
import time



class Sht30():
        
    def __init__( self, controller, scl=None, sda=None, addr=None ):
        self.controller = controller
        self.controller.i2c_init( 0x45 if addr else 0x44, scl, sda )
        self.reset()

    def _writeCmd( self, cmd, read=None ):
        return self.controller.i2c( [(cmd>>8)&0xFF, cmd&0xFF], read=read )
   
    def _checkReg( self, regs ):
        m, l, c = regs
        # TODO: add crc check
        # crc=???
        #if crc != c:
        #    raise Exception('CRC error 0x%02X(should be 0x%02X)'% (c, crc))
        return (m<<8) + l

    def reset( self, delay=0.1 ):
        self._writeCmd( 0x30A2 )  # soft reset
        time.sleep(delay)

    def readStatus( self ):
        r = self._writeCmd( 0xF32D, read=3 )
        return self._checkReg( r[0:3] )
    
    def setHeater( self, on=True ):
        self._writeCmd( 0x306D if on else 0x3060 )

    def setART( self ):
        self._writeCmd( 0x30A2 )

    __cmd_period_measu = {
        (0.5, 'high') :     0x2032,
        (0.5, 'medium') :   0x2024,
        (0.5, 'low') :      0x202F,
        (1, 'high') :       0x2130,
        (1, 'medium') :     0x2126,
        (1, 'low') :        0x212D,
        (2, 'high') :       0x2236,
        (2, 'medium') :     0x2220,
        (2, 'low') :        0x222B,
        (4, 'high') :       0x2334,
        (4, 'medium') :     0x2322,
        (4, 'low') :        0x2329,
        (10, 'high') :      0x2737,
        (10, 'medium') :    0x2721,
        (10, 'low') :       0x272A,
        }
    def startPeriodicMeasurement( self, mps=0.5, repeatability='high' ):
        self._writeCmd( self.__cmd_period_measu[(mps,repeatability)] )

    def read( self ):
        while True:
            try:
                ret = self._writeCmd( 0xE000, read=6 )
                break
            except Instrument.CommandExecuteError as e:
                time.sleep(0.1)
                pass
        t = self._checkReg(ret[0:3])
        h = self._checkReg(ret[3:6])
        t = -45.0 + 175.0 * t / (2**16-1)
        h = 100.0 * h / (2**16-1)
        return t, h 

