# coding: utf8
__doc__ = 'CS1237/1238 24-bit ADC'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils
import time


CMD_WRITE_CONFIG = 0x65
CMD_READ_CONFIG = 0x56

# config
REF_OUT_ON  = 0<<6
REF_OUT_OFF = 1<<6
SPEED_10    = 0<<4
SPEED_40    = 1<<4
SPEED_640   = 2<<4
SPEED_1280  = 3<<4
GAIN_1      = 0<<2
GAIN_2      = 1<<2
GAIN_64     = 2<<2
GAIN_128    = 3<<2
CH_A        = 0
CH_B        = 1  # CS1238 only
CH_T        = 2
CH_SHORT    = 3


class Cs1238():
    ref = 3.3
    channel = CH_A
    gain = GAIN_1
    speed = SPEED_10
    ref_out = REF_OUT_ON

    def __init__( self, controller, sdi=None, sdo=None, sck=None, cs=None, channel='a', gain=1, speed=10, ref_out=True, ref=3.3, init=True ):
        self.controller = controller
        self.sdi = sdi if sdi is not None else '0.0'
        self.sdo = sdo if sdo is not None else '0.1'
        self.sck = sck if sck is not None else '0.2'
        self.cs  = cs  if cs  is not None else '0.3'
        self.controller.spiInit(self.sdi, self.sdo, self.sck, self.cs, width=27, cpha=True)
        if channel in [0,'a','A']:
            self.channel = CH_A
        elif channel in [1,'b','B']:
            self.channel = CH_B
        elif channel in [2,'t','T']:
            self.channel = CH_T
        elif channel in [3,'s','S']:
            self.channel = CH_SHORT
        if gain == 1:
            self.gain = GAIN_1
        elif gain == 2:
            self.gain = GAIN_2
        elif gain == 64:
            self.gain = GAIN_64
        elif gain == 128:
            self.gain = GAIN_128
        if speed == 10:
            self.speed = SPEED_10
        elif speed == 40:
            self.speed = SPEED_40
        elif speed == 640:
            self.speed = SPEED_640
        elif speed == 1280:
            self.speed = SPEED_1280
        self.ref_out = REF_OUT_ON if ref_out else REF_OUT_OFF
        self.ref = ref
        if init:
            self.writeConfig()

    def writeConfig( self, read=False ):
        # wait for drdy
        while not self.dataIsReady():
            time.sleep(0.1)
        # CLK 1~27: read adc
        # CLK 28~29: read 2 dummy clocks
        self.controller.spiUpdate(self.sdi, self.sdo, self.sck, self.cs, width=27+2, cpha=True)
        self.controller.spiRead( [0] )
        # CLK 30~36: output command
        self.controller.spiUpdate(self.sdi, self.sdi, self.sck, self.cs, width=7, cpha=True)
        self.controller.spiWrite( [CMD_READ_CONFIG if read else CMD_WRITE_CONFIG] )
        # CLK 37: switch sdi as input for read mode
        # CLK 38~45: read/write config
        if read:
            self.controller.spiUpdate(self.sdi, self.sdo, self.sck, self.cs, width=9, cpha=True)
            ret = self.controller.spiRead([0])[0] & 0xFF
        else:
            self.controller.spiUpdate(self.sdi, self.sdi, self.sck, self.cs, width=9, cpha=True)
            self.controller.spiWrite( [self.ref_out | self.speed | self.gain | self.channel] )
        # CLK 46: switch back
        self.controller.spiUpdate(self.sdi, self.sdo, self.sck, self.cs, width=1, cpha=True)
        self.controller.spiWrite( [0] )
        self.controller.spiUpdate(self.sdi, self.sdo, self.sck, self.cs, width=27, cpha=True)
        if read:
            return ret

    def readConfig( self ):
        return self.writeConfig(True)

    def dataIsReady( self ):
        return self.controller.pinIsLow(self.sdi)

    def hex2volt( self, dat ):
        val = Utils.I2i(dat<<8)/256
        return self.ref * val / (2**24)
        
    def readData( self, mode=None ):
        a = self.controller.spiRead([0])[0]
        if mode == 'hex':
            return a>>3
        else:
            val = Utils.I2i(a<<5)/256
            if mode == 'int':
                return val
            return self.ref * val / (2**24)


