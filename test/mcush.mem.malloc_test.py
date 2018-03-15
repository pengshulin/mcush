#!/usr/bin/env python
import os
import sys
import time
import random
from mcush import *

MALLOC_LIMIT = 50000

def main(argv=None):

    mem = []
    mfill = {}
    msize = {}
    malloc_counter = 0
    realloc_counter = 0
    free_counter = 0
    segment_counter = 0
    fill_err_counter = 0

    s = Mcush.Mcush()
    s.reboot()

    def info( message ):
        print '[%d] (%d/%d/%d/%d) %s'% (segment_counter, malloc_counter, realloc_counter, free_counter, fill_err_counter, message)
   
    while True:

        size = random.randint(1, MALLOC_LIMIT)
        pat = random.randint(0, 255)
        
        # 1. use malloc
        m = s.malloc( size )
        if m:
            mem.append(m)
            mfill[m] = pat 
            msize[m] = size
            malloc_counter += 1
            segment_counter += 1
            info( 'malloc %d 0x%08X'% (size, m) )
            s.fillMem( m, pat, 1, size )
        # 2. if malloc failed, try to use realloc from existing one
        elif mem:
            em = random.choice(mem)
            m = s.realloc( em, size )
            if m:
                mem.remove(em)
                mem.append(m)
                realloc_counter += 1
                info( 'realloc %d 0x%08X -> %d 0x%08X'% (msize[em], em, size, m) )
                s.fillMem( m, pat, 1, size )
                mfill[m] = pat 
                msize[m] = size
        # 3. if still not allocated, try free one 
        if (not m) and mem:
            m = random.choice(mem)
            mem.remove(m)
            if not s.testMem( m, mfill[m], 1, msize[m] ):
                fill_err_counter += 1
                info( 'memory fill test failed @ 0x%08X'% (m) )
            s.free( m )
            mfill.pop(m)
            msize.pop(m)
            free_counter += 1
            segment_counter -= 1
            info( 'free 0x%08X'% (m) )
        



if __name__ == '__main__':
    main(sys.argv)

