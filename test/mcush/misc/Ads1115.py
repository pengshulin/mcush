# coding: utf8
__doc__ = 'ADS1113/4/5 16-bit ADC'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Instrument, Utils
import time


REG_CONV     = 0  # Conversion
REG_CONFIG   = 1  # Config
REG_THRES_L  = 2  # Lo_thres
REG_THRES_H  = 3  # Hi_thres

ADDR = [0x48, 0x49, 0x4A, 0x4B]
PGA_RANGE = [6.144, 4.096, 2.048, 1.024, 0.512, 0.256, 0.256, 0.256]

class Ads1115():

    def __init__( self, controller, scl=None, sda=None, addr_mode=0 ):
        self.controller = controller
        self.controller.i2c_init( ADDR[addr_mode], scl, sda )
        self.reset()
        self.config(mode=0)  # continuous measure mode

    def readReg( self, addr ):
        h, l = self.controller.i2c( [addr], read=2 )
        return (h<<8)+l

    def writeReg( self, addr, dat ):
        self.controller.i2c( [addr&0x03, dat>>8, dat&0xFF] )

    def reset( self ):
        # reset all chips on the bus that response to common address
        try:
            self.controller.i2c( [0x06], addr=0x0 )
        except Instrument.CommandExecuteError:
            pass  # device has no ACK response

    def config( self, os=0, mux=0, pga=2, mode=1, dr=4, comp_mode=0, comp_pol=0, comp_lat=0, comp_que=3 ):
        config = (os<<15)+(mux<<12)+(pga<<9)+(mode<<8)+(dr<<5)+ \
                 (comp_mode<<4)+(comp_pol<<3)+(comp_lat<<2)+(comp_que)
        self.writeReg( REG_CONFIG, config )
        self.pga_range = PGA_RANGE[pga]

    def setThreshold( self, lo=0x8000, hi=0x7FFF ):
        self.writeReg( REG_THRES_L, lo )
        self.writeReg( REG_THRES_H, hi )

    def readConv( self ):
        val = Utils.H2h( self.readReg( REG_CONV ) )
        return val * self.pga_range / (2**15)

