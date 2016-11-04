#!/usr/bin/env python
import sys
import time
import Env
import Mcush



def main(argv=None):
    s = Mcush.Mcush()
    s.addReg( Mcush.reg( 'PORTA_CRL', 0x40010800, 'PORTA control (low)' ) )
    s.addReg( Mcush.reg( 'PORTA_CRH', 0x40010804, 'PORTA control (high)' ) )
    s.addReg( Mcush.reg( 'PORTA_IDR', 0x40010808, 'PORTA input data' ) )
    s.addReg( Mcush.reg( 'PORTA_ODR', 0x40010808, 'PORTA output data' ) )
    print hex(s.getReg('PORTA_CRL'))
    print hex(s.getReg('PORTA_CRH'))
    print hex(s.getReg('PORTA_IDR'))
    print hex(s.getReg('PORTA_ODR'))
    s.setReg('PORTA_ODR', 0x12345678)


if __name__ == '__main__':
    main(sys.argv)

