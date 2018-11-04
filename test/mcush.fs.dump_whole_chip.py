#!/usr/bin/env python
# dump whole spiffs flash chip
import sys
import random
import math
from mcush import *

def main(argv=None):
    random.seed()
    s = Mcush.Mcush()

    info = s.spiffsInfo()
    chip_size = int(math.ceil(info['total']/1024.0/1024.0)*1024*1024)
    page_size = 256
    pages = chip_size / page_size
    print( "chip_size: %d\npages: %d"% (chip_size, pages) )

    f = open('spiffs.bin', 'wb+') 
    for i in range( pages ):
        print( 'page %d / %d (%d %%)'% (i, pages, float(i)/pages*100) )
        mem = s.spiffsReadPage( i )
        f.write( mem )

    f.close()
    s.disconnect()
   
if __name__ == '__main__':
    main(sys.argv)
