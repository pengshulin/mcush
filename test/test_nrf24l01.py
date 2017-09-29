#!/usr/bin/env python
# coding: utf8
# test NRF24L01 chip with spi ports
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *


WR = Env.getenv_bool('WR')

#####################################################
# NRF24L01寄存器操作命令
READ_REG       = 0x00  # 读配置寄存器,低5位为寄存器地址
WRITE_REG      = 0x20  # 写配置寄存器,低5位为寄存器地址
RD_RX_PLOAD    = 0x61  # 读RX有效数据,1~32字节
WR_TX_PLOAD    = 0xA0  # 写TX有效数据,1~32字节
FLUSH_TX       = 0xE1  # 清除TX FIFO寄存器.发射模式下用
FLUSH_RX       = 0xE2  # 清除RX FIFO寄存器.接收模式下用
REUSE_TX_PL    = 0xE3  # 重新使用上一包数据,CE为高,数据包被不断发送.
NOP            = 0xFF  # 空操作,可以用来读状态寄存器  
# SPI(NRF24L01)寄存器地址 
CONFIG         = 0x00  # 配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
                       # bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
EN_AA          = 0x01  # 使能自动应答功能  bit0~5,对应通道0~5
EN_RXADDR      = 0x02  # 接收地址允许,bit0~5,对应通道0~5
SETUP_AW       = 0x03  # 设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
SETUP_RETR     = 0x04  # 建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
RF_CH          = 0x05  # RF通道,bit6:0,工作通道频率;
RF_SETUP       = 0x06  # RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
STATUS         = 0x07  # 状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
                       # bit5:数据发送完成中断;bit6:接收数据中断;
MAX_TX         = 0x10  # 达到最大发送次数中断
TX_OK          = 0x20  # TX发送完成中断
RX_OK          = 0x40  # 接收到数据中断

OBSERVE_TX     = 0x08  # 发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
CD             = 0x09  # 载波检测寄存器,bit0,载波检测;
RX_ADDR_P0     = 0x0A  # 数据通道0接收地址,最大长度5个字节,低字节在前
RX_ADDR_P1     = 0x0B  # 数据通道1接收地址,最大长度5个字节,低字节在前
RX_ADDR_P2     = 0x0C  # 数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
RX_ADDR_P3     = 0x0D  # 数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
RX_ADDR_P4     = 0x0E  # 数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
RX_ADDR_P5     = 0x0F  # 数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
TX_ADDR        = 0x10  # 发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
RX_PW_P0       = 0x11  # 接收数据通道0有效数据宽度(1~32字节),设置为0则非法
RX_PW_P1       = 0x12  # 接收数据通道1有效数据宽度(1~32字节),设置为0则非法
RX_PW_P2       = 0x13  # 接收数据通道2有效数据宽度(1~32字节),设置为0则非法
RX_PW_P3       = 0x14  # 接收数据通道3有效数据宽度(1~32字节),设置为0则非法
RX_PW_P4       = 0x15  # 接收数据通道4有效数据宽度(1~32字节),设置为0则非法
RX_PW_P5       = 0x16  # 接收数据通道5有效数据宽度(1~32字节),设置为0则非法
FIFO_STATUS    = 0x17  # FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
                       # bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;

STA_MARK_RX    = 0X40
STA_MARK_TX    = 0X20
STA_MARK_MX    = 0X10

#####################################################
# 24L01发送接收数据宽度定义
TX_ADR_WIDTH    = 5  # 地址宽度
RX_ADR_WIDTH    = 5  # 地址宽度
TX_PLOAD_WIDTH  = 10  # 用户数据宽度
RX_PLOAD_WIDTH  = 10  # 用户数据宽度

    
if WR:
    ADDR_T  = [0xA0, 0x5A, 0x5A, 0x5A, 0x5A]
    ADDR_R0 = [0xA0, 0xA5, 0xA5, 0xA5, 0xA5]
    ADDR_R1 = [0xA1, 0xA5, 0xA5, 0xA5, 0xA5]
    ADDR_R2 = [0xA2, 0xA5, 0xA5, 0xA5, 0xA5]
    ADDR_R3 = [0xA3, 0xA5, 0xA5, 0xA5, 0xA5]
    ADDR_R4 = [0xA4, 0xA5, 0xA5, 0xA5, 0xA5]
    ADDR_R5 = [0xA5, 0xA5, 0xA5, 0xA5, 0xA5]
else:
    ADDR_T  = [0xA0, 0xA5, 0xA5, 0xA5, 0xA5]
    ADDR_R0 = [0xA0, 0x5A, 0x5A, 0x5A, 0x5A]
    ADDR_R1 = [0xA1, 0x5A, 0x5A, 0x5A, 0x5A]
    ADDR_R2 = [0xA2, 0x5A, 0x5A, 0x5A, 0x5A]
    ADDR_R3 = [0xA3, 0x5A, 0x5A, 0x5A, 0x5A]
    ADDR_R4 = [0xA4, 0x5A, 0x5A, 0x5A, 0x5A]
    ADDR_R5 = [0xA5, 0x5A, 0x5A, 0x5A, 0x5A]

    
ADDR_T = ADDR_R0 = [0xA5, 0xA5, 0xA5, 0xA5, 0xA5]


def main(argv=None):
    s = Mcush.Mcush()
    s.spi_init(sdi='0.5', delay=20)  # PA.0上拉电阻太小，信号失真

    EN = '0.4'
    s.gpio( EN, o=True )
    
    def chip_enable(enable=True):
        if enable:
            s.gpio( EN, s=True )
        else:
            s.gpio( EN, c=True )
      
    def write_buf(cmd, dat):
        s.spi( [cmd] + dat )

    def read_buf(cmd, len):
        return s.spi( [cmd] + [0xff]*len, True )[1:]

    def write_reg(reg, dat):
        reg = 0x20 | (reg & 0x1F)
        if isinstance(dat, list):
            r = s.spi( [reg] + dat, True )
        else:
            r = s.spi( [reg, dat], True )
        return r[0]

    def read_reg(reg, len=1):
        reg = reg & 0x1F
        if len:
            r = s.spi( [reg] + [0xFF]*len, True )
            return r[0], r[1:]
        else:
            r = s.spi( [reg], True )
            return r[0]

    def read_status():
        return s.spi( [NOP], True )[0]

    def print_regs():
        regs = ['%02X'% read_reg(reg)[1][0] for reg in range(24)]
        print '  '.join([' '.join(regs[i:i+4]) for i in range(0,24,4)])

    def power_off():
        chip_enable( False )
        write_reg( CONFIG, 0x0D )
        chip_enable( True )

    def clr_reg(t=True):
        s.spi( [FLUSH_TX if t else FLUSH_RX] )

    def clr_all():
        s.spi( [FLUSH_RX] )
        s.spi( [FLUSH_TX] )
        write_reg( STATUS, 0xFF )
       
    def set_tx_mode(dat):
        power_off()
        chip_enable( False )
        s.spi( [FLUSH_TX] )
        write_reg( STATUS, 0xFF )
        write_reg( SETUP_AW, 0x03 )
        write_reg( TX_ADDR, ADDR_T )
        write_reg( RX_ADDR_P0, ADDR_R0 )
        write_reg( RX_ADDR_P1, ADDR_R1 )
        write_reg( RX_ADDR_P2, ADDR_R2 )
        write_reg( RX_ADDR_P3, ADDR_R3 )
        write_reg( RX_ADDR_P4, ADDR_R4 )
        write_reg( RX_ADDR_P5, ADDR_R5 )
        write_buf( WR_TX_PLOAD, dat )
        write_reg( EN_AA, 0x07F )
        write_reg( EN_RXADDR, 0x7F )
        write_reg( SETUP_RETR, 0x1A )  # delay 500us, 10 retry
        write_reg( RF_CH, 40 )
        write_reg( RX_PW_P0, RX_PLOAD_WIDTH )
        write_reg( RX_PW_P1, RX_PLOAD_WIDTH )
        write_reg( RX_PW_P2, RX_PLOAD_WIDTH )
        write_reg( RX_PW_P3, RX_PLOAD_WIDTH )
        write_reg( RX_PW_P4, RX_PLOAD_WIDTH )
        write_reg( RX_PW_P5, RX_PLOAD_WIDTH )
        write_reg( RF_SETUP, 0x08 )
        write_reg( CONFIG, 0x0E )
        chip_enable( True )
        time.sleep(0.1)
        chip_enable( False )
        
        
    def set_rx_mode():
        power_off()
        chip_enable( False )
        s.spi( [FLUSH_RX] )
        write_reg( SETUP_AW, 0x03 )
        write_reg( TX_ADDR, ADDR_T )
        write_reg( RX_ADDR_P0, ADDR_R0 )
        write_reg( RX_ADDR_P1, ADDR_R1 )
        write_reg( RX_ADDR_P2, ADDR_R2 )
        write_reg( RX_ADDR_P3, ADDR_R3 )
        write_reg( RX_ADDR_P4, ADDR_R4 )
        write_reg( RX_ADDR_P5, ADDR_R5 )
        write_reg( EN_AA, 0x07F )
        write_reg( EN_RXADDR, 0x7F )
        write_reg( RF_CH, 40 )
        write_reg( RX_PW_P0, RX_PLOAD_WIDTH )
        write_reg( RX_PW_P1, RX_PLOAD_WIDTH )
        write_reg( RX_PW_P2, RX_PLOAD_WIDTH )
        write_reg( RX_PW_P3, RX_PLOAD_WIDTH )
        write_reg( RX_PW_P4, RX_PLOAD_WIDTH )
        write_reg( RX_PW_P5, RX_PLOAD_WIDTH )
        write_reg( RF_SETUP, 0x08 )
        write_reg( CONFIG, 0x0F )
        write_reg( STATUS, 0xFF )
        chip_enable( True )
       
      
    time.sleep(0.5)
    print_regs()

    if WR:
        count = 0
        while True:
            print_regs()
            dat = [i%0xFF for i in range(count, count+TX_PLOAD_WIDTH)]
            #dat = [0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00]    # less-bit-flipping packets
            set_tx_mode( dat )
            print 'write', dat
            time.sleep(1)
            count += TX_PLOAD_WIDTH

            if read_status() & STA_MARK_RX:
                rdat = read_buf( RD_RX_PLOAD, RX_PLOAD_WIDTH )
                print 'read', rdat
                s.spi( [FLUSH_RX] )
                write_reg( STATUS, STA_MARK_RX )
 
    else:
        set_rx_mode()
        while True:
            #print_regs()

            if read_status() & STA_MARK_RX:
                rdat = read_buf( RD_RX_PLOAD, RX_PLOAD_WIDTH )
                print 'read', rdat
                s.spi( [FLUSH_RX] )
                write_reg( STATUS, STA_MARK_RX )
            #time.sleep(0.5)

    

if __name__ == '__main__':
    main(sys.argv)

