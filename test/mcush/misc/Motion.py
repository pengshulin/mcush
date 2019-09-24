# coding: utf8
__doc__ = 'motion sensors'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils

MPU6050_REGS_BY_IDX = {
    1: 'AUX_VDDIO',
    25: 'SMPLRT_DIV',
    26: 'CONFIG',
    27: 'GYRO_CONFIG',
    28: 'ACCEL_CONFIG',
    29: 'FF_THR',
    30: 'FF_DUR',
    31: 'MOT_THR',
    32: 'MOT_DUR',
    33: 'ZRMOT_THR',
    34: 'ZRMOT_DUR',
    35: 'FIFO_EN',
    36: 'I2C_MST_CTRL',
    37: 'I2C_SLV0_ADDR',
    38: 'I2C_SLV0_REG',
    39: 'I2C_SLV0_CTRL',
    40: 'I2C_SLV1_ADDR',
    41: 'I2C_SLV1_REG',
    42: 'I2C_SLV1_CTRL',
    43: 'I2C_SLV2_ADDR',
    44: 'I2C_SLV2_REG',
    45: 'I2C_SLV2_CTRL',
    46: 'I2C_SLV3_ADDR',
    47: 'I2C_SLV3_REG',
    48: 'I2C_SLV3_CTRL',
    49: 'I2C_SLV4_ADDR',
    50: 'I2C_SLV4_REG',
    51: 'I2C_SLV4_DO',
    52: 'I2C_SLV4_CTRL',
    53: 'I2C_SLV4_DI',
    54: 'I2C_MST_STATUS',
    55: 'INT_PIN_CFG',
    56: 'INT_ENABLE',
    58: 'INT_STATUS',
    59: 'ACCEL_XOUT_H',
    60: 'ACCEL_XOUT_L',
    61: 'ACCEL_YOUT_H',
    62: 'ACCEL_YOUT_L',
    63: 'ACCEL_ZOUT_H',
    64: 'ACCEL_ZOUT_L',
    65: 'TEMP_OUT_H',
    66: 'TEMP_OUT_L',
    67: 'GYRO_XOUT_H',
    68: 'GYRO_XOUT_L',
    69: 'GYRO_YOUT_H',
    70: 'GYRO_YOUT_L',
    71: 'GYRO_ZOUT_H',
    72: 'GYRO_ZOUT_L',
    73: 'EXT_SENS_DATA_00',
    74: 'EXT_SENS_DATA_01',
    75: 'EXT_SENS_DATA_02',
    76: 'EXT_SENS_DATA_03',
    77: 'EXT_SENS_DATA_04',
    78: 'EXT_SENS_DATA_05',
    79: 'EXT_SENS_DATA_06',
    80: 'EXT_SENS_DATA_07',
    81: 'EXT_SENS_DATA_08',
    82: 'EXT_SENS_DATA_09',
    83: 'EXT_SENS_DATA_10',
    84: 'EXT_SENS_DATA_11',
    85: 'EXT_SENS_DATA_12',
    86: 'EXT_SENS_DATA_13',
    87: 'EXT_SENS_DATA_14',
    88: 'EXT_SENS_DATA_15',
    89: 'EXT_SENS_DATA_16',
    90: 'EXT_SENS_DATA_17',
    91: 'EXT_SENS_DATA_18',
    92: 'EXT_SENS_DATA_19',
    93: 'EXT_SENS_DATA_20',
    94: 'EXT_SENS_DATA_21',
    95: 'EXT_SENS_DATA_22',
    96: 'EXT_SENS_DATA_23',
    97: 'MOT_DETECT_STATUS',
    99: 'I2C_SLV0_DO',
    100: 'I2C_SLV1_DO',
    101: 'I2C_SLV2_DO',
    102: 'I2C_SLV3_DO',
    103: 'I2C_MST_DELAY_CTRL',
    104: 'SIGNAL_PATH_RESET',
    105: 'MOT_DETECT_CTRL',
    106: 'USER_CTRL',
    107: 'PWR_MGMT_1',
    108: 'PWR_MGMT_2',
    114: 'FIFO_COUNTH',
    115: 'FIFO_COUNTL',
    116: 'FIFO_R_W',
    117: 'WHO_AM_I',
    }

MPU6050_REGS_BY_NAME = {}
for k, v in MPU6050_REGS_BY_IDX.items():
    MPU6050_REGS_BY_NAME[v] = k

class Mpu6050():

    def __init__( self, controller, scl=None, sda=None ):
        self.controller = controller
        self.controller.i2c_init( 0x68, scl=scl, sda=sda )
        self.reset()

    def reset(self):
        self.writeByName( "PWR_MGMT_1", 0x80 )  # device reset
        while True:
            if not self.readByName( "PWR_MGMT_1" ) & 0x80:
                break
        self.writeByName( "PWR_MGMT_1", 0x00 )  # internal 8M CLK
        self.writeByName( "PWR_MGMT_2", 0x00 )  # no standby mode
        self.writeByName( "AUX_VDDIO", 0x80 )  # AUX_VDDIO=VDD
        self.writeByName( "SMPLRT_DIV", 0x07 )  # sample rate: 0x07(125Hz)
        self.writeByName( "CONFIG", 0x06 )  # filter: 0x06(5Hz)
        self.writeByName( "GYRO_CONFIG", 0x18 )  # gyro setting: 0x18(no self test, 2000deg/s)
        self.writeByName( "ACCEL_CONFIG", 0x01 )  # accel setting: 0x01(no self test, 2G, 5Hz)
        #self.writeByName( "PWR_MGMT_1", 0x01 )  # user X/PLL Clk 
 
    def readByIdx( self, idx ):
        return self.controller.i2c( [idx], 1 )[0]

    def readByName( self, name ):
        return self.controller.i2c( [MPU6050_REGS_BY_NAME[name]], 1 )[0]

    def writeByIdx( self, idx, val ):
        self.controller.i2c( [idx, val] )

    def writeByName( self, name, val ):
        self.controller.i2c( [MPU6050_REGS_BY_NAME[name], val] )

    def printAllReg( self ):
        #regs = self.controller.i2c( [0x0], 120 )
        #for i,name in MPU6050_REGS_BY_IDX.items():
        #    print( '[%d / 0x%02X] 0x%02X (%s)'% (i, i, regs[i], name) )
        for i,name in MPU6050_REGS_BY_IDX.items():
            print( '[%d / 0x%02X] 0x%02X (%s)'% (i, i, self.readByName(name), name) )

    def getTemperature( self ):
        a,b = self.controller.i2c( [0x41], 2 )
        return 35 + (Utils.s2h( chr(b) + chr(a) ) + 521)/340.0

    def getAccelX( self ):
        a,b = self.controller.i2c( [0x3B], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 16384.0 * 9.8
        
    def getAccelY( self ):
        a,b = self.controller.i2c( [0x3D], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 16384.0 * 9.8
        
    def getAccelZ( self ):
        a,b = self.controller.i2c( [0x3F], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 16384.0 * 9.8
 
    def getGyroX( self ):
        a,b = self.controller.i2c( [0x43], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 16384.0 * 16.4
        
    def getGyroY( self ):
        a,b = self.controller.i2c( [0x45], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 16384.0 * 16.4
        
    def getGyroZ( self ):
        a,b = self.controller.i2c( [0x47], 2 )
        return Utils.s2h( chr(b) + chr(a) ) / 16384.0 * 16.4
 



class Adxl345():

    def __init__( self, controller, scl=None, sda=None ):
        self.controller = controller
        self.controller.i2c_init( 0x53, scl=scl, sda=sda )
        self.reset()

    def reset(self):
        self.controller.i2c( [0x31, 0x0B] )  # DATA_FORMAT: full resolution, 16g range
        self.controller.i2c( [0x2C, 0x08] )  # BW_RATE: 12.5Hz bandwidth, 25Hz output rate
        self.controller.i2c( [0x2D, 0x08] )  # POWER_CTRL: Measure 
        self.controller.i2c( [0x2E, 0x80] )  # INT_ENABLE: DATA_READY 
        self.controller.i2c( [0x1E, 0x00] )  # OFSX: X-axis offset
        self.controller.i2c( [0x1F, 0x00] )  # OFSY: Y-axis offset
        self.controller.i2c( [0x20, 0x00] )  # OFSZ: Z-axis offset

    def printAllReg( self ):
        print( self.controller.i2c( [0x0], 57 ) )

    def getAccelX( self ):
        a,b = self.controller.i2c( [0x32], 2 )
        return Utils.s2h( chr(a) + chr(b) ) / 25.0
 
    def getAccelY( self ):
        a,b = self.controller.i2c( [0x34], 2 )
        return Utils.s2h( chr(a) + chr(b) ) / 25.0
 
    def getAccelZ( self ):
        a,b = self.controller.i2c( [0x36], 2 )
        return Utils.s2h( chr(a) + chr(b) ) / 25.0
 
