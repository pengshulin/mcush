#!/usr/bin/env python
# erase chip, write each sector with random data and verify
import sys
import random
import math
from mcush import *

def main(argv=None):
    random.seed()
    s = Mcush.Mcush()
    s.spiffsMount()

    # get chip size
    info = s.spiffsInfo()
    chip_size = int(math.ceil(info['total']/1024.0/1024.0)*1024*1024)
    page_size = 256
    pages = chip_size / page_size
    print( "chip_size: %d\npages: %d"% (chip_size, pages) )

    # erase
    s.spiffsUmount()
    print( 'erase chip...' )
    s.spiffsErase()

    # verify each page 
    for i in range(pages):
        print( 'page %d / %d (%d %%)'% (i, pages, float(i)/pages*100) )
        # write random data
        Utils.generateRandomFile( page_size ) 
        testmem = open(Env.TEST_WR, 'rb').read()
        testmem2 = [ Utils.s2H(testmem[2*j:2*(j+1)]) for j in range(page_size/2) ]
        s.spiffsWritePage( i, testmem2 )
        mem = s.spiffsReadPage( i )
        for j in range(page_size):
            if ord(mem[j]) != ord(testmem[j]):
                print( 'verify failed at page %s'% (i) )
                s.disconnect()
                return
                 

    s.disconnect()
   
if __name__ == '__main__':
    main(sys.argv)
