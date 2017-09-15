#!/usr/bin/env python
# coding: utf8
# test RFIC_MFRC522 chip with spi ports
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
import Env
import Mcush
import Utils


MAXRLEN        = 18
MIN_STRENGTH   = 228
# RC522 FIFO长度定义 
DEF_FIFO_LENGTH      = 64             # FIFO size=64byte

# RC522命令字
PCD_IDLE             = 0x00           # 取消当前命令
PCD_AUTHENT          = 0x0E           # 验证密钥
PCD_RECEIVE          = 0x08           # 接收数据
PCD_TRANSMIT         = 0x04           # 发送数据
PCD_TRANSCEIVE       = 0x0C           # 发送并接收数据
PCD_RESETPHASE       = 0x0F           # 复位
PCD_CALCCRC          = 0x03           # CRC计算

# Mifare_One卡片命令字
PICC_REQIDL          = 0x26           # 寻天线区内未进入休眠状态
PICC_REQALL          = 0x52           # 寻天线区内全部卡
PICC_ANTICOLL1       = 0x93           # 防冲撞
PICC_ANTICOLL2       = 0x95           # 防冲撞
PICC_AUTHENT1A       = 0x60           # 验证A密钥
PICC_AUTHENT1B       = 0x61           # 验证B密钥
PICC_READ            = 0x30           # 读块
PICC_WRITE           = 0xA0           # 写块
PICC_DECREMENT       = 0xC0           # 扣款
PICC_INCREMENT       = 0xC1           # 充值
PICC_RESTORE         = 0xC2           # 调块数据到缓冲区
PICC_TRANSFER        = 0xB0           # 保存缓冲区中数据
PICC_HALT            = 0x50           # 休眠

# MF522寄存器定义
# PAGE 0
RFU00                = 0x00    
CommandReg           = 0x01    
ComIEnReg            = 0x02    
DivlEnReg            = 0x03    
ComIrqReg            = 0x04    
DivIrqReg            = 0x05
ErrorReg             = 0x06    
Status1Reg           = 0x07    
Status2Reg           = 0x08    
FIFODataReg          = 0x09
FIFOLevelReg         = 0x0A
WaterLevelReg        = 0x0B
ControlReg           = 0x0C
BitFramingReg        = 0x0D
CollReg              = 0x0E
RFU0F                = 0x0F
# PAGE 1     
RFU10                = 0x10
ModeReg              = 0x11
TxModeReg            = 0x12
RxModeReg            = 0x13
TxControlReg         = 0x14
TxASKReg             = 0x15
TxSelReg             = 0x16
RxSelReg             = 0x17
RxThresholdReg       = 0x18
DemodReg             = 0x19
RFU1A                = 0x1A
RFU1B                = 0x1B
MifareReg            = 0x1C
RFU1D                = 0x1D
RFU1E                = 0x1E
SerialSpeedReg       = 0x1F
# PAGE 2    
RFU20                = 0x20  
CRCResultRegM        = 0x21
CRCResultRegL        = 0x22
RFU23                = 0x23
ModWidthReg          = 0x24
RFU25                = 0x25
RFCfgReg             = 0x26
GsNReg               = 0x27
CWGsCfgReg           = 0x28
ModGsCfgReg          = 0x29
TModeReg             = 0x2A
TPrescalerReg        = 0x2B
TReloadRegH          = 0x2C
TReloadRegL          = 0x2D
TCounterValueRegH    = 0x2E
TCounterValueRegL    = 0x2F
# PAGE 3      
RFU30                = 0x30
TestSel1Reg          = 0x31
TestSel2Reg          = 0x32
TestPinEnReg         = 0x33
TestPinValueReg      = 0x34
TestBusReg           = 0x35
AutoTestReg          = 0x36
VersionReg           = 0x37
AnalogTestReg        = 0x38
TestDAC1Reg          = 0x39  
TestDAC2Reg          = 0x3A   
TestADCReg           = 0x3B   
RFU3C                = 0x3C   
RFU3D                = 0x3D   
RFU3E                = 0x3E   
RFU3F                = 0x3F



def main(argv=None):
    s = Mcush.Mcush()
    s.spi_init()

    RST = '0.4'
    s.gpio( RST, o=True, s=True )
    
    def rst():
        s.gpio( RST, c=True )
        time.sleep(0.1)
        s.gpio( RST, s=True )
      
    def read(addr, len=1):
        byte0 = 0x80 + ((addr&0x3F)<<1)
        r = s.spi( [byte0] + [0]*len, True )
        return r[1:] if len > 1 else r[1]
      
    def write(addr, dat):
        byte0 = ((addr&0x3F)<<1)
        if isinstance(dat, list):
            s.spi( [byte0] + dat )
        else:
            s.spi( [byte0, dat] )

    def setbits(addr, bits):
        write(addr, [read(addr) | bits])

    def clrbits(addr, bits):
        write(addr, [read(addr) & ~bits])

    def print_regs():
        info = []
        for page in range(4):
            info.append( 'P%d %s'% (page, ['%02X'% read(addr+page*16) for addr in range(16)]) )
        print '\n'.join(info)


    def trans(buf):
        ret = None
        irqEn   = 0x77
        waitFor = 0x30
        write( ComIEnReg, irqEn|0x80 )
        clrbits( ComIrqReg, 0x80 )
        write( CommandReg, PCD_IDLE )
        setbits( FIFOLevelReg, 0x80 )  # 清空FIFO 
        if isinstance(buf, list):
            for d in buf:
                write( FIFODataReg, d )  # 数据写入FIFO
        else:
            write( FIFODataReg, buf )  # 数据写入FIFO
        write( CommandReg, PCD_TRANSCEIVE )  # 命令写入命令寄存器
        setbits( BitFramingReg, 0x80 )  # 开始发送     
        t0 = time.time()
        timeout = False
        while True:
            if time.time() > t0 + 5:
                #raise Exception, "Read timeout"
                timeout = True
                break
            n = read( ComIrqReg )
            if n & 0x01:
                break
            if n & 0x30:
                break
        clrbits( BitFramingReg, 0x80 )
        if not timeout and not read(ErrorReg) & 0x1B:
            if n & irqEn & 0x01:
                print 'No card'
            else:
                n = read( FIFOLevelReg )
                lastBits = read(ControlReg) & 0x07
                if lastBits:
                    OutLenBit = (n-1)*8 + lastBits
                else:
                    OutLenBit = n*8
                if n==0:
                    n = 1
                if n>MAXRLEN:
                    n = MAXRLEN
                outdata = [read(FIFODataReg) for i in range(n)]
                #print 'OUTDATA', outdata
                ret = outdata
        setbits( ControlReg, 0x80 )
        write( CommandReg, PCD_IDLE )
        return ret
 
    rst()

    write( CommandReg, 0x0F )  # reset command
    write( CommandReg, 0x0 )  # power on
    write( ModeReg, 0x3D )
    write( TReloadRegL, 30 )
    write( TReloadRegH, 0 )
    write( TModeReg, 0x8D )
    write( TPrescalerReg, 0x3E )

    write( TxASKReg, 0x40 )
    if not read( TxControlReg ) & 0x03:
        setbits( TxControlReg, 0x03 )
    read( TxASKReg )

    print_regs()      
        
    while True:
        #print_regs()

        clrbits( Status2Reg, 0x08 )
        write( BitFramingReg, 0x07 )
        setbits( TxControlReg, 0x03 )
                
        _type = trans( [0x52] )
        if isinstance(_type, list) and len(_type) == 2:
            if _type == [4, 0]:
                print 'Card type: MFOne-S50'
            elif _type == [2, 0]:
                print 'Card type: MFOne-S70'
            elif _type == [44, 0]:
                print 'Card type: MF-UltraLight'
            elif _type == [8, 0]:
                print 'Card type: MF-Pro'
            elif _type == [44, 3]:
                print 'Card type: MF Desire'
            else:
                print 'Unknown card'
                _type = None

            if _type: 
                clrbits( Status2Reg, 0x08 )
                write( BitFramingReg, 0x00 )
                clrbits( CollReg, 0x80 )
 
                _sn = trans( [PICC_ANTICOLL1, 0x20] )
                if isinstance(_sn, list):
                    print 'Card SN: %02X%02X%02X%02X'% (_sn[0], _sn[1], _sn[2], _sn[3])
                    s.beep()

                
        #time.sleep(0.5)


if __name__ == '__main__':
    main(sys.argv)

