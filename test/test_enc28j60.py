#!/usr/bin/env python
# coding: utf8
import os
import sys
import time
import random
import Env
import Mcush
import Utils

macaddr = [0x12, 0x34, 0x56, 0x78, 0x90, 0xAB]  

ADDR_MASK       = 0x1F
BANK_MASK       = 0x60
SPRD_MASK       = 0x80
# All-bank registers
EIE             = 0x1B
EIR             = 0x1C
ESTAT           = 0x1D
ECON2           = 0x1E
ECON1           = 0x1F
# Bank 0 registers
ERDPTL          = (0x00|0x00)
ERDPTH          = (0x01|0x00)
EWRPTL          = (0x02|0x00)
EWRPTH          = (0x03|0x00)
ETXSTL          = (0x04|0x00)
ETXSTH          = (0x05|0x00)
ETXNDL          = (0x06|0x00)
ETXNDH          = (0x07|0x00)
ERXSTL          = (0x08|0x00)
ERXSTH          = (0x09|0x00)
ERXNDL          = (0x0A|0x00)
ERXNDH          = (0x0B|0x00)
ERXRDPTL        = (0x0C|0x00)
ERXRDPTH        = (0x0D|0x00)
ERXWRPTL        = (0x0E|0x00)
ERXWRPTH        = (0x0F|0x00)
EDMASTL         = (0x10|0x00)
EDMASTH         = (0x11|0x00)
EDMANDL         = (0x12|0x00)
EDMANDH         = (0x13|0x00)
EDMADSTL        = (0x14|0x00)
EDMADSTH        = (0x15|0x00)
EDMACSL         = (0x16|0x00)
EDMACSH         = (0x17|0x00)
# Bank 1 registers
EHT0            = (0x00|0x20)
EHT1            = (0x01|0x20)
EHT2            = (0x02|0x20)
EHT3            = (0x03|0x20)
EHT4            = (0x04|0x20)
EHT5            = (0x05|0x20)
EHT6            = (0x06|0x20)
EHT7            = (0x07|0x20)
EPMM0           = (0x08|0x20)
EPMM1           = (0x09|0x20)
EPMM2           = (0x0A|0x20)
EPMM3           = (0x0B|0x20)
EPMM4           = (0x0C|0x20)
EPMM5           = (0x0D|0x20)
EPMM6           = (0x0E|0x20)
EPMM7           = (0x0F|0x20)
EPMCSL          = (0x10|0x20)
EPMCSH          = (0x11|0x20)
EPMOL           = (0x14|0x20)
EPMOH           = (0x15|0x20)
EWOLIE          = (0x16|0x20)
EWOLIR          = (0x17|0x20)
ERXFCON         = (0x18|0x20)
EPKTCNT         = (0x19|0x20)
# Bank 2 registers
MACON1          = (0x00|0x40|0x80)
MACON2          = (0x01|0x40|0x80)
MACON3          = (0x02|0x40|0x80)
MACON4          = (0x03|0x40|0x80)
MABBIPG         = (0x04|0x40|0x80)
MAIPGL          = (0x06|0x40|0x80)
MAIPGH          = (0x07|0x40|0x80)
MACLCON1        = (0x08|0x40|0x80)
MACLCON2        = (0x09|0x40|0x80)
MAMXFLL         = (0x0A|0x40|0x80)
MAMXFLH         = (0x0B|0x40|0x80)
MAPHSUP         = (0x0D|0x40|0x80)
MICON           = (0x11|0x40|0x80)
MICMD           = (0x12|0x40|0x80)
MIREGADR        = (0x14|0x40|0x80)
MIWRL           = (0x16|0x40|0x80)
MIWRH           = (0x17|0x40|0x80)
MIRDL           = (0x18|0x40|0x80)
MIRDH           = (0x19|0x40|0x80)
# Bank 3 registers
MAADR1          = (0x00|0x60|0x80)
MAADR0          = (0x01|0x60|0x80)
MAADR3          = (0x02|0x60|0x80)
MAADR2          = (0x03|0x60|0x80)
MAADR5          = (0x04|0x60|0x80)
MAADR4          = (0x05|0x60|0x80)
EBSTSD          = (0x06|0x60)
EBSTCON         = (0x07|0x60)
EBSTCSL         = (0x08|0x60)
EBSTCSH         = (0x09|0x60)
MISTAT          = (0x0A|0x60|0x80)
EREVID          = (0x12|0x60)
ECOCON          = (0x15|0x60)
EFLOCON         = (0x17|0x60)
EPAUSL          = (0x18|0x60)
EPAUSH          = (0x19|0x60)
# PHY registers
PHCON1          = 0x00
PHSTAT1         = 0x01
PHHID1          = 0x02
PHHID2          = 0x03
PHCON2          = 0x10
PHSTAT2         = 0x11
PHIE            = 0x12
PHIR            = 0x13
PHLCON          = 0x14
# ENC28J60 ERXFCON Register Bit Definitions
ERXFCON_UCEN    = 0x80
ERXFCON_ANDOR   = 0x40
ERXFCON_CRCEN   = 0x20
ERXFCON_PMEN    = 0x10
ERXFCON_MPEN    = 0x08
ERXFCON_HTEN    = 0x04
ERXFCON_MCEN    = 0x02
ERXFCON_BCEN    = 0x01
# ENC28J60 EIE Register Bit Definitions
EIE_INTIE       = 0x80
EIE_PKTIE       = 0x40
EIE_DMAIE       = 0x20
EIE_LINKIE      = 0x10
EIE_TXIE        = 0x08
EIE_WOLIE       = 0x04
EIE_TXERIE      = 0x02
EIE_RXERIE      = 0x01
# ENC28J60 EIR Register Bit Definitions
EIR_PKTIF       = 0x40
EIR_DMAIF       = 0x20
EIR_LINKIF      = 0x10
EIR_TXIF        = 0x08
EIR_WOLIF       = 0x04
EIR_TXERIF      = 0x02
EIR_RXERIF      = 0x01
# ENC28J60 ESTAT Register Bit Definitions
ESTAT_INT       = 0x80
ESTAT_LATECOL   = 0x10
ESTAT_RXBUSY    = 0x04
ESTAT_TXABRT    = 0x02
ESTAT_CLKRDY    = 0x01
# ENC28J60 ECON2 Register Bit Definitions
ECON2_AUTOINC   = 0x80
ECON2_PKTDEC    = 0x40
ECON2_PWRSV     = 0x20
ECON2_VRPS      = 0x08
# ENC28J60 ECON1 Register Bit Definitions
ECON1_TXRST     = 0x80
ECON1_RXRST     = 0x40
ECON1_DMAST     = 0x20
ECON1_CSUMEN    = 0x10
ECON1_TXRTS     = 0x08
ECON1_RXEN      = 0x04
ECON1_BSEL1     = 0x02
ECON1_BSEL0     = 0x01
# ENC28J60 MACON1 Register Bit Definitions
MACON1_LOOPBK   = 0x10
MACON1_TXPAUS   = 0x08
MACON1_RXPAUS   = 0x04
MACON1_PASSALL  = 0x02
MACON1_MARXEN   = 0x01
# ENC28J60 MACON2 Register Bit Definitions
MACON2_MARST    = 0x80
MACON2_RNDRST   = 0x40
MACON2_MARXRST  = 0x08
MACON2_RFUNRST  = 0x04
MACON2_MATXRST  = 0x02
MACON2_TFUNRST  = 0x01
# ENC28J60 MACON3 Register Bit Definitions
MACON3_PADCFG2  = 0x80
MACON3_PADCFG1  = 0x40
MACON3_PADCFG0  = 0x20
MACON3_TXCRCEN  = 0x10
MACON3_PHDRLEN  = 0x08
MACON3_HFRMLEN  = 0x04
MACON3_FRMLNEN  = 0x02
MACON3_FULDPX   = 0x01
# ENC28J60 MICMD Register Bit Definitions
MICMD_MIISCAN   = 0x02
MICMD_MIIRD     = 0x01
# ENC28J60 MISTAT Register Bit Definitions
MISTAT_NVALID   = 0x04
MISTAT_SCAN     = 0x02
MISTAT_BUSY     = 0x01
# ENC28J60 PHY PHCON1 Register Bit Definitions
PHCON1_PRST     = 0x8000
PHCON1_PLOOPBK  = 0x4000
PHCON1_PPWRSV   = 0x0800
PHCON1_PDPXMD   = 0x0100
# ENC28J60 PHY PHSTAT1 Register Bit Definitions
PHSTAT1_PFDPX   = 0x1000
PHSTAT1_PHDPX   = 0x0800
PHSTAT1_LLSTAT  = 0x0004
PHSTAT1_JBSTAT  = 0x0002
# ENC28J60 PHY PHCON2 Register Bit Definitions
PHCON2_FRCLINK  = 0x4000
PHCON2_TXDIS    = 0x2000
PHCON2_JABBER   = 0x0400
PHCON2_HDLDIS   = 0x0100
# ENC28J60 Packet Control Byte Bit Definitions
PKTCTRL_PHUGEEN = 0x08
PKTCTRL_PPADEN  = 0x04
PKTCTRL_PCRCEN  = 0x02
PKTCTRL_POVERRIDE = 0x01
# SPI operation codes
ENC28J60_READ_CTRL_REG      = 0x00
ENC28J60_READ_BUF_MEM       = 0x3A
ENC28J60_WRITE_CTRL_REG     = 0x40
ENC28J60_WRITE_BUF_MEM      = 0x7A
ENC28J60_BIT_FIELD_SET      = 0x80
ENC28J60_BIT_FIELD_CLR      = 0xA0
ENC28J60_SOFT_RESET         = 0xFF

# The RXSTART_INIT should be zero. See Rev. B4 Silicon Errata
# buffer boundaries applied to internal 8K ram
# the entire available packet buffer space is allocated
#
# start with recbuf at 0/
RXSTART_INIT    = 0x0
# receive buffer end
RXSTOP_INIT     = (0x1FFF-0x0600-1)
# start TX buffer at 0x1FFF-0x0600, pace for one full ethernet frame (~1500 bytes)
TXSTART_INIT    = (0x1FFF-0x0600)
# stp TX buffer at end of mem
TXSTOP_INIT     = 0x1FFF
# max frame length which the conroller will accept:
MAX_FRAMELEN    = 1500        # (note: maximum ethernet frame length would be 1518)



class ENC28J60(Mcush.Mcush):
    RST = '0.4'
    cur_bank = 0
    def init(self):
        self.spi_init(delay=1)
        self.gpio( self.RST, o=True )
        self.gpio( self.RST, c=True )
        time.sleep(0.1)
        self.gpio( self.RST, s=True )

    def bank(self, addr):
        bank = addr & BANK_MASK
        if self.cur_bank != bank:
            self.cur_bank = bank
            self.bfc( ECON1, (ECON1_BSEL1|ECON1_BSEL0) )
            self.bfs( ECON1, bank>>5 )

    def rcr(self, addr):
        r = self.spi( [addr & ADDR_MASK, 0], read=True )
        return r[1] 

    def rbm(self, length):
        r = self.spi( [ENC28J60_READ_BUF_MEM] + [0]*length, read=True )
        return r[1:] 

    def wcr(self, addr, dat):
        self.spi( [ENC28J60_WRITE_CTRL_REG + addr & ADDR_MASK, dat] )
        
    def wbm(self, dat):
        self.spi( [ENC28J60_WRITE_BUF_MEM] + list(dat) )
 
    def bfs(self, addr, dat):
        self.spi( [ENC28J60_BIT_FIELD_SET + addr & ADDR_MASK, dat] )
 
    def bfc(self, addr, dat):
        self.spi( [ENC28J60_BIT_FIELD_CLR + addr & ADDR_MASK, dat] )
 
    def sc(self):
        self.spi( [ENC28J60_SOFT_RESET] )
    
    def write(self, addr, data):
        self.bank(addr)
        self.wcr(addr, data)
 
    def read(self, addr):
        self.bank(addr)
        return self.rcr(addr)
 
def main(argv=None):
    s = ENC28J60()
    s.init()
    s.sc()  # soft reset
     
	######################################################
    # BANK0  
    s.write( ERXSTL, RXSTART_INIT&0xFF )  # rx start
    s.write( ERXSTH, RXSTART_INIT>>8 )
    s.write( ERXRDPTL, RXSTART_INIT&0xFF )  # set receive ptr addr
    s.write( ERXRDPTH, RXSTART_INIT>>8 )
    s.write( ERXNDL, RXSTOP_INIT&0xFF )  # rx end
    s.write( ERXNDH, RXSTOP_INIT>>8 )
    s.write( ETXSTL, TXSTART_INIT&0xFF )  # tx start
    s.write( ETXSTH, TXSTART_INIT>>8 )
    s.write( ETXNDL, TXSTOP_INIT&0xFF )  # tx end
    s.write( ETXNDH, TXSTOP_INIT>>8 )
	######################################################
    # BANK1
    s.write( ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN )
    s.write( EPMM0, 0x3f ) 
    s.write( EPMM1, 0x30 )
    s.write( EPMCSL, 0xf9 )
    s.write( EPMCSH, 0xf7 )
	######################################################
    # BANK2
	# enable MAC receive
    s.write( MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS )
    # bring MAC out of reset
    s.write( MACON2, 0x00 )
    # enable automatic padding to 60bytes and CRC operations
    s.bfs( MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX )
    # set inter-frame gap (non-back-to-back)
    s.write( MAIPGL, 0x12 )
    s.write( MAIPGH, 0x0C )
    # set inter-frame gap (back-to-back)
    s.write( MABBIPG, 0x12 )
    # Set the maximum packet size which the controller will accept
        # Do not send packets longer than MAX_FRAMELEN:
    s.write( MAMXFLL, MAX_FRAMELEN&0xFF )	
    s.write( MAMXFLH, MAX_FRAMELEN>>8 )
    ######################################################
    # BANK 3 
    # write MAC address
    # NOTE: MAC address in ENC28J60 is byte-backward
    s.write( MAADR5, macaddr[0] )	
    s.write( MAADR4, macaddr[1] )
    s.write( MAADR3, macaddr[2] )
    s.write( MAADR2, macaddr[3] )
    s.write( MAADR1, macaddr[4] )
    s.write( MAADR0, macaddr[5] )
    
    mac_read = [s.read(i) for i in [MAADR5, MAADR4, MAADR3, MAADR2, MAADR1, MAADR0]] 
    print 'mac addr:', ':'.join(['%02X'%i for i in mac_read])
    s.write( PHCON1, PHCON1_PDPXMD )
    # no loopback of transmitted frames
    s.write( PHCON2, PHCON2_HDLDIS )
    # switch to bank 0
    s.bank( ECON1 )
    # enable interrutps
    s.bfs( EIE, EIE_INTIE|EIE_PKTIE )
    # enable packet reception
    s.bfs( ECON1, ECON1_RXEN )
	
    counter = 1
    while True:
        for b in range(4):
            regs = [s.read(b*32+i) for i in range(32)]
            print 'BANK%d'%b, regs

        #print s.rbm(10)


        counter += 1




if __name__ == '__main__':
    main(sys.argv)

