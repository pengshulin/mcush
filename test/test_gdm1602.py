#!/usr/bin/env python
# coding: utf8
# test gdm1602 lcd module
# Peng Shulin <trees_peng@163.com>
import sys
import time
from mcush import *


def main(argv=None):
    s = Mcush.Mcush('/dev/ttyACM0')

    DP = '0'
    D0 = '0.0'
    D1 = '0.1'
    D2 = '0.2'
    D3 = '0.3'
    D4 = '0.4'
    D5 = '0.5'
    D6 = '0.6'
    D7 = '0.7'
    E = '0.8'
    RW = '0.9'
    RS = '0.10'

    s.pinOutputLow( [E, RW, RS, D0, D1, D2, D3, D4, D5, D6, D7] )

    def write( rs, rw, dat ):
        s.pinSetVal( RS, rs )
        s.pinSetVal( RW, rw )
        s.gpio( DP, o=0xFF, s=dat&0xFF, c=(~dat)&0xFF )
        s.pinSet( E )
        s.pinClr( E )
       
    def read( rs, rw ): 
        s.pinSetVal( RS, rs )
        s.pinSetVal( RW, rw )
        s.gpio( DP, i=0xFF )
        s.pinSet( E )
        r = s.gpio( DP ) & 0xFF
        s.pinClr( E )
        return r

    def readBusyAddress():
        r = read( 0, 1 ) 
        return bool(r&0x80), (r&0x7F)

    

    # init
    write( 0, 0, 1 )  # Clear screen
    write( 0, 0, 2 )  # Return home
    write( 0, 0, 6 )  # Increment + Shift mode
    write( 0, 0, 0x0F )  # Display On, Cursor On, Blink On
    write( 0, 0, 0x3C )  # 8-bit bus, 2-line display
    write( 0, 0, 0x40 )  # CGRAM: 0
    
    def writeLine( line, offset, string ):
        if line == 1:
            addr = 0x00
        else:
            addr = 0x40
        addr += offset
        # write DDRAM address
        write( 0, 0, 0x80+(addr&0x7F) )
        # write data
        for c in string:
            write( 1, 0, ord(c) )
            time.sleep(0.1)

    while True: 
        write( 0, 0, 1 )  # Clear screen
        write( 0, 0, 2 )  # Return home
        writeLine( 1, 0, 'Hardware control' )
        writeLine( 2, 0, '  with Python' )
        time.sleep(10)
     
    #while True:
    #    busy, addr = readBusyAddress()
    #    print( 'Busy: %d  Address: 0x%02X'% (busy, addr) )
           
 
if __name__ == '__main__':
    main(sys.argv)
    
