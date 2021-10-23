# coding: utf8
__doc__ = 'PCA9685 16-channel 12-bit I2C-bus PWM LED controller'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'


REG_MODE1       = 0
REG_MODE2       = 1
REG_SUBADR1     = 2
REG_SUBADR2     = 3
REG_SUBADR3     = 4
REG_ALLCALLADR  = 5
REG_LED0        = 6
REG_LED1        = 10
REG_LED2        = 14
REG_LED3        = 18
REG_LED4        = 22
REG_LED4        = 26
REG_LED5        = 30
REG_LED7        = 34
REG_LED8        = 38
REG_LED9        = 42
REG_LED10       = 46
REG_LED11       = 50
REG_LED12       = 54
REG_LED13       = 58
REG_LED14       = 62
REG_LED15       = 66
REG_ALL_LED_ON  = 250
REG_ALL_LED_OFF = 252
REG_PRESCALE    = 254
REG_TESTMODE    = 255


class Pca9685():

    def __init__( self, controller, addr=0x0, scl=None, sda=None, ext_clk=None ):
        self.controller = controller
        self.controller.i2c_init( (addr&0x3F)|0x40, scl, sda )
        if ext_clk is None:
            self.clk, self.ext_clk = 25000000.0, False
        else:
            self.clk, self.ext_clk = float(self.ext_clk), True
        self.reset()

    def readReg( self, addr ):
        return self.controller.i2c( [addr], read=1 )[0]

    def writeReg( self, addr, dat ):
        self.controller.i2c( [addr, dat] )

    def writeRegs( self, addr, dat_list ):
        self.controller.i2c( [addr]+dat_list )

    def reset( self ):
        self.writeReg( REG_MODE1, 0x60 if self.ext_clk else 0x20 )
        self.writeReg( REG_MODE2, 0x04 )

    def setFreq( self, freq=1000.0 ):
        prescale = int(self.clk/4096.0/float(freq)) - 1
        prescale = max(0, min(prescale, 255))  # reg is 8bits limited
        mode1 = self.readReg( REG_MODE1 ) & 0x7F | 0x20 
        self.writeReg( REG_MODE1, mode1 | 0x10 )  # enter sleep mode
        self.writeReg( REG_PRESCALE, prescale )  # update in sleep mode
        self.writeReg( REG_MODE1, mode1 )  # exit sleep mode
  
    def setDuty( self, channel, duty ):
        val = int(duty * 4095)
        if val <= 0:
            on, off = 0, 4096  # fully off
        elif val >= 4095:
            on, off = 4096, 0  # fully on
        else:
            on, off = 0, val
        self.writeRegs( REG_LED0+channel*4, [on&0xFF, on>>8, off&0xFF, off>>8] )

