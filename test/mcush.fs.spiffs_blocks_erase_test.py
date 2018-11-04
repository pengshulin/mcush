#!/usr/bin/env python
# repeately erase block, write random data and verify
import sys
import random
import math
from mcush import *

def main(argv=None):
    random.seed()
    s = Mcush.Mcush()
    # get chip size
    try:
        s.spiffsMount()
        info = s.spiffsInfo()
        s.spiffsUmount()
    except:
        info = {'total':8*1024*1024}
    chip_size = int(math.ceil(info['total']/1024.0/1024.0)*1024*1024)
    page_size = 256
    pages = chip_size / page_size
    print( "chip_size: %d\npages: %d"% (chip_size, pages) )

    block_id = 0
    block_size = 0x10000
    block_num = chip_size / block_size

    # verify each page 
    counter = 0
    while True:
        block_id = random.randint(0, block_num-1)
        print( '[%d] testing block %d...'% (counter, block_id) )
        # erase
        s.spiffsErase( block_id * block_size )
        # write random data
        Utils.generateRandomFile( page_size ) 
        testmem = open(Env.TEST_WR, 'rb').read()
        testmem2 = [ Utils.s2H(testmem[2*j:2*(j+1)]) for j in range(page_size/2) ]
        s.spiffsWritePage( block_id * block_size / 256, testmem2 )
        mem = s.spiffsReadPage( block_id * block_size / 256 )
        for j in range(page_size):
            if ord(mem[j]) != ord(testmem[j]):
                print( 'verify failed' )
                s.disconnect()
                return
        counter += 1
                 

    s.disconnect()
   
if __name__ == '__main__':
    main(sys.argv)
