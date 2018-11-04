#!/usr/bin/env python
import sys
import time
import random
from mcush import *


def main(argv=None):
    '''random size'''
    s = Mcush.Mcush()
    pool = []
    while True:
        try:
            buf = range(random.randint(1, Env.TEST_MAXSIZE))
            addr = s.mkbuf( buf )
            print( 'Create: 0x%08X  length: %d'% (addr, len(buf)) )
            pool.append( (addr, buf) )
        except Instrument.CommandExecuteError:
            (addr, buf) = pool.pop(random.randint(0,len(pool)-1))
            print( 'Free: 0x%08X'% addr )
            mem = s.readMem( addr, length=2*len(buf), compact_mode=True )
            # compare the memory with original buf
            for i in range(len(buf)):
                if buf[i] != Utils.s2h(mem[i*2:(i+1)*2]):
                    print( 'memory check failed at offset %d'% i )
            s.free( addr )
    s.disconnect()
        
   
if __name__ == '__main__':
    main(sys.argv)
