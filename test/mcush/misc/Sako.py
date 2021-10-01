# coding: utf8
__doc__ = 'products from Hangzhou Sako Frequency Tech.'
__author__ = 'Peng Shulin <trees_peng@163.com>'
from .. import Env, Utils, Modbus


REG_CONTROL           = 0x2000
REG_PARM_READ         = 0xD000
REG_FREQ_SET          = 0x2001
REG_TORQUE_SET        = 0x2002
REG_PID_FREQ_SET      = 0x2003
REG_PID_FEEDBACK_SET  = 0x2004
REG_AO1_SET           = 0x2005
REG_AO2_SET           = 0x2006
REG_PUL_SET           = 0x2007
REG_DO_SET            = 0x2008

REG_STATUS            = 0xA000
REG_ERROR_CODE        = 0xE000
REG_WARNING_CODE      = 0xE001
REG_PASSWD_CHECK      = 0xAD00
REG_PASSWD_CHECK      = 0xAD00




class SKI600( Modbus.ModbusRTU ):
    # Frequency Converter
    DEFAULT_NAME = 'SKI600'

    DEFAULT_BAUDRATE = 19200  # 2400/4800/9600/19200/38400/115200

    EXCEPTION_CODE = {
        1: 'illegal function code',
        2: 'illegal address',
        3: 'illegal data',
        4: 'illegal reg length',
        5: 'crc error',
        6: 'write disabled while running',
        7: 'write disabled',
        8: 'control command invalid',
        9: 'password protected',
        10: 'password error',
        }

    def setFunctionCode( self, code, val ):
        return self.writeReg( code, val )
            
    def getFunctionCode( self, code ):
        return self.readReg( code )
    
    def getErrorCode( self ):
        return self.readReg(REG_ERROR_CODE)

    def getWarningCode( self ):
        return self.readReg(REG_WARNING_CODE)
         
    def enterRemoteMode( self ):
        self.old_control_mode = self.getFunctionCode( 0x0006 )
        self.setFunctionCode( 0x0006, 2 )
        self.old_freqa_mode = self.getFunctionCode( 0x0007 )
        self.setFunctionCode( 0x0007, 2 )
        self.old_freqb_mode = self.getFunctionCode( 0x0008 )
        self.setFunctionCode( 0x0008, 2 )
        
    def exitRemoteMode( self ):
        self.setFunctionCode( 0x0006, self.old_control_mode )
        self.setFunctionCode( 0x0007, self.old_freqa_mode )
        self.setFunctionCode( 0x0008, self.old_freqb_mode )
  
    def setFreq( self, freq=50.0 ):
        self.writeReg( REG_FREQ_SET, int(freq*100) )

    def start( self, forward=True ):
        self.writeReg( REG_CONTROL, 1 if forward else 9 )
   
    def stop( self, free=True ):
        self.writeReg( REG_CONTROL, 4 if free else 3 )



