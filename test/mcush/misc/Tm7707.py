# coding: utf8
__doc__ = 'TM7707 24-bit ADC'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils
import time


# Communication register
REG_COMM     = 0x00  # Communication register
REG_SETUP    = 0x10  # Setting register
REG_FILTER_H = 0x20  # Filter(high) register
REG_DATA     = 0x30  # Data register
REG_TEST     = 0x40  # Test register
REG_FILTER_L = 0x50  # Filter(low) register
REG_ZERO_CAL = 0x60  # Zero cal register 
REG_FULL_CAL = 0x70  # Full cal register
WRITE        = 0x00  # Write register
READ         = 0x08  # Read register
CH_1         = 0x00  # AIN1+ AIN1-
CH_2         = 0x01  # AIN2+ AIN2-
CH_3         = 0x02  # AIN1- AIN1-
CH_4         = 0x03  # AIN1- AIN2-
CH_MAX       = 0x04  # Cannot write to device register.

# Setting register
MD_NORMAL   = (0<<6)  # Normal
MD_CAL_SELF = (1<<6)  # Self calibrate 
MD_CAL_ZERO = (2<<6)  # Calibrate Zero
MD_CAL_FULL = (3<<6)  # Calibrate Full 
GAIN_1      = (0<<3)  # Gain x1 
GAIN_2      = (1<<3)  # Gain x2 
GAIN_4      = (2<<3)  # Gain x4
GAIN_8      = (3<<3)  # Gain x8
GAIN_16     = (4<<3)  # Gain x16
GAIN_32     = (5<<3)  # Gain x32
GAIN_64     = (6<<3)  # Gain x64
GAIN_128    = (7<<3)  # Gain x128
BIPOLAR     = (0<<2)  # Bipolar input
UNIPOLAR    = (1<<2)  # Unipolar input
BUF_NO      = (0<<1)  # Non-buffered input
BUF_EN      = (1<<1)  # Buffered input
FSYNC_0     = 0
FSYNC_1     = 1

# Filter low register
CLKDIS_0    = 0x00  # Clock enable
CLKDIS_1    = 0x10  # Clock disable (safe power)
CLK_4_9152M = 0x08  # 4.9152M
CLK_2_4576M = 0x00  # 2.4576M
CLK_1M      = 0x04  # 1M
CLK_2M      = 0x0C  # 2M
ZERO_0      = 0x00
ZERO_1      = 0x80
FS_50HZ     = 0x000
FS_60HZ     = 0x100
FS_250HZ    = 0x200
FS_500HZ    = 0x400
FS_DEFAULT  = 0xF00


class Tm7707():
    ref = 2.5
    channel = CH_1
    gain = GAIN_1
    polar = BIPOLAR
    buffered = BUF_NO
    clock_dis = CLKDIS_0
    clock_sel = CLK_4_9152M
    zero = ZERO_0
    fs = FS_500HZ

    def __init__( self, controller, sdi=None, sdo=None, sck=None, cs=None, rst=None, drdy=None, init=True ):
        self.controller = controller
        self.controller.spiInit(sdi, sdo, sck, cs, cpol=True, cpha=True)
        self.rst = rst
        self.drdy = drdy
        self.controller.pinInput(drdy)
        self.controller.pinOutputHigh(rst)
        self.reset()
        if init:
            self.init()
   
    def reset( self ):
        self.controller.pinClr(self.rst)
        time.sleep(0.1)
        self.controller.pinSet(self.rst)
        time.sleep(0.1)

    def init( self ):
        self.controller.spi( [ REG_SETUP | WRITE | self.channel,
                MD_NORMAL | self.gain | self.polar | self.buffered ] )
        self.controller.spi( [ REG_FILTER_H | WRITE | self.channel,
                self.clock_dis | self.clock_sel | self.zero | ((self.fs>>8)&0xF) ] )
        self.controller.spi( [ REG_FILTER_L | WRITE | self.channel,
                self.fs&0xFF ] )
        # write default calib value (useless for following self-calibrating command)
        #self.controller.spi( [ REG_ZERO_CAL | WRITE | self.channel,
        #        0x1F, 0x40, 0x00 ] )
        #self.controller.spi( [ REG_FULL_CAL | WRITE | self.channel,
        #        0x57, 0x61, 0xAB ] )

    def readAllRegs( self ):
        val = {}
        _, a = self.controller.spiRead( [ REG_COMM | READ | self.channel, 0xFF ] )
        val['comm'] = a
        _, a = self.controller.spiRead( [ REG_SETUP | READ | self.channel, 0xFF ] )
        val['setup'] = a
        _, a = self.controller.spiRead( [ REG_FILTER_H | READ | self.channel, 0xFF ] )
        val['filter_h'] = a
        _, a = self.controller.spiRead( [ REG_FILTER_L | READ | self.channel, 0xFF ] )
        val['filter_l'] = a
        _, a, b, c = self.controller.spiRead( [ REG_ZERO_CAL | READ | self.channel, 0xFF, 0xFF, 0xFF ] )
        val['cal_zero'] = (a<<16)+(b<<8)+c
        _, a, b, c = self.controller.spiRead( [ REG_FULL_CAL | READ | self.channel, 0xFF, 0xFF, 0xFF ] )
        val['cal_full'] = (a<<16)+(b<<8)+c
        return val
   
    def sync( self ):
        self.controller.spi( [ REG_SETUP | WRITE | self.channel,
                MD_NORMAL | self.gain | self.polar | self.buffered | FSYNC_1 ] )
        self.controller.spi( [ REG_SETUP | WRITE | self.channel,
                MD_NORMAL | self.gain | self.polar | self.buffered | FSYNC_0 ] )

    def dataIsReady( self ):
        return self.controller.pinIsLow(self.drdy)

    def readData( self, mode=None ):
        _, a, b, c = self.controller.spiRead( [
                REG_DATA | READ | self.channel, 0xFF, 0xFF, 0xFF ] )
        val = (a<<16)|(b<<8)|c
        if mode in ['hex', 'int']:
            return val
        else:
            if self.polar == BIPOLAR:
                val -= 0x800000
                return self.ref * val / (2**23)
            else:
                return self.ref * val / (2**24)

    def calibSelf( self ):
        self.controller.spi( [ REG_SETUP | WRITE | self.channel,
                MD_CAL_SELF | self.gain | self.polar | self.buffered ] )
        while not self.dataIsReady():
            time.sleep(0.1)
        
    def calibZero( self ):
        self.controller.spi( [ REG_SETUP | WRITE | self.channel,
                MD_CAL_ZERO | self.gain | self.polar | self.buffered] )
        while not self.dataIsReady():
            time.sleep(0.1)

    def calibFull( self ):
        self.controller.spi( [ REG_SETUP | WRITE | self.channel,
                MD_CAL_FULL | self.gain | self.polar | self.buffered] )
        while not self.dataIsReady():
            time.sleep(0.1)

