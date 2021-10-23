# coding: utf8
__doc__ = 'MLX90614 infra-red temperature sensor'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils, Env
import time


# RAM
MLX90614_RAWIR1  = 0x04
MLX90614_RAWIR2  = 0x05
MLX90614_TA      = 0x06
MLX90614_TOBJ1   = 0x07
MLX90614_TOBJ2   = 0x08
# EEPROM
MLX90614_TOMAX   = 0x00
MLX90614_TOMIN   = 0x01
MLX90614_PWMCTRL = 0x02
MLX90614_TARANGE = 0x03
MLX90614_EMISS   = 0x04
MLX90614_CONFIG  = 0x05
MLX90614_ADDR    = 0x0E
MLX90614_ID1     = 0x1C
MLX90614_ID2     = 0x1D
MLX90614_ID3     = 0x1E
MLX90614_ID4     = 0x1F


class Mlx90614():

    def __init__( self, controller, scl=None, sda=None ):
        self.controller = controller
        self.controller.i2c_init( 0x5A, scl, sda )
       
    def readRam( self, addr ):
        datl, dath, pec = self.controller.i2c( [addr & 0x1F], 3 )
        return datl + (dath<<8)
    
    def readRamInt16( self, addr ):
        datl, dath, pec = self.controller.i2c( [addr & 0x1F], 3 )
        if Env.PYTHON_V3:
            return Utils.s2h( bytes([datl, dath]) )
        else:
            return Utils.s2h( chr(datl) + chr(dath) )
        
    def readE2prom( self, addr ):
        datl, dath, pec = self.controller.i2c( [(addr & 0x1F) | 0x20], 3 )
        return datl + (dath<<8)
    
    def readID( self ):
        id1 = self.readE2prom( MLX90614_ID1 )
        id2 = self.readE2prom( MLX90614_ID2 )
        id3 = self.readE2prom( MLX90614_ID3 )
        id4 = self.readE2prom( MLX90614_ID4 )
        return (id1, id2, id3, id4)

    def readFlags( self ):
        datl, dath, pec = self.controller.i2c( [0xF0], 3 )
        return datl + (dath<<8)
   
    def readTempAmb( self ):
        t = self.readRamInt16( MLX90614_TA ) * 0.02 - 273.15
        return t

    def readTempObj( self ):
        t = self.readRamInt16( MLX90614_TOBJ1 ) * 0.02 - 273.15
        return t

    def readTemp( self ):
        return (self.readTempAmb(), self.readTempObj()) 

