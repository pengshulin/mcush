#!/usr/bin/env python
# erase chip, test each sector
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
    pages = chip_size / 256
    print( "chip_size: %d\npages: %d"% (chip_size, pages) )

    # erase
    s.spiffsUmount()
    print( 'erase chip...' )
    s.spiffsErase()

    # verify each page is blank
    for i in range(pages):
        print( 'page %d / %d (%d %%)'% (i, pages, float(i)/pages*100) )
        mem = s.spiffsReadPage( i )
        for m in mem:
            if ord(m) != 0xFF:
                print( 'verify failed at page %s'% (i) )
                break


    s.disconnect()
   
if __name__ == '__main__':
    main(sys.argv)
