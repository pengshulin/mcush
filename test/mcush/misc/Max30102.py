# coding: utf8
__doc__ = 'MAX30102 pulse oximeter and hart-rate sensor'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'


REG_INTR_STATUS_1 = 0x00
REG_INTR_STATUS_2 = 0x01
REG_INTR_ENABLE_1 = 0x02
REG_INTR_ENABLE_2 = 0x03
REG_FIFO_WR_PTR = 0x04
REG_OVF_COUNTER = 0x05
REG_FIFO_RD_PTR = 0x06
REG_FIFO_DATA = 0x07
REG_FIFO_CONFIG = 0x08
REG_MODE_CONFIG = 0x09
REG_SPO2_CONFIG = 0x0A
REG_LED1_PA = 0x0C
REG_LED2_PA = 0x0D
REG_MULTI_LED_CTRL1 = 0x11
REG_MULTI_LED_CTRL2 = 0x12
REG_TEMP_INTR = 0x1F
REG_TEMP_FRAC = 0x20
REG_TEMP_CONFIG = 0x21
REG_REV_ID = 0xFE
REG_PART_ID = 0xFF



class Max30102():
        
    def __init__( self, controller, scl=None, sda=None ):
        self.controller = controller
        self.controller.i2c_init( 0x57, scl, sda )
        self.reset()
        self.init()

    def readReg( self, addr ):
        return self.controller.i2c( [addr], read=1 )[0]

    def writeReg( self, addr, dat ):
        self.controller.i2c( [addr, dat] )

    def reset( self ):
        self.writeReg( REG_MODE_CONFIG, 0x40 )

    def init( self ):
        self.writeReg( REG_INTR_ENABLE_1, 0xc0 ) # INTR setting
        self.writeReg( REG_INTR_ENABLE_2, 0x00 )
        self.writeReg( REG_FIFO_WR_PTR, 0x00 )  # FIFO_WR_PTR[4:0]
        self.writeReg( REG_OVF_COUNTER, 0x00 )  # OVF_COUNTER[4:0]
        self.writeReg( REG_FIFO_RD_PTR, 0x00 )  # FIFO_RD_PTR[4:0]
        self.writeReg( REG_FIFO_CONFIG, 0X0F )  # sample avg: 1, fifo rollover: 0, fifo almost full: 17
        self.writeReg( REG_MODE_CONFIG, 0x03 )  # 0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
        self.writeReg( REG_SPO2_CONFIG, 0x27 )  # SPO2_ADC range: 4096nA,  SPO2 sample rate (100 Hz), LED pulseWidth (400uS)
        self.writeReg( REG_LED1_PA, 0x24 )  # 7mA for LED1(RED)
        self.writeReg( REG_LED2_PA, 0x24 )  # 7mA for LED2(IRED)

    def readStatus( self ):
        s1 = self.readReg( REG_INTR_STATUS_1 )
        s2 = self.readReg( REG_INTR_STATUS_2 )
        return s1, s2

    def readFifo( self ):
        self.readStatus()
        r = self.controller.i2c( [REG_FIFO_DATA], read=6 )
        red = (r[0]<<16) + (r[1]<<8) + r[2]
        ir = (r[3]<<16) + (r[4]<<8) + r[5]
        return red, ir

