#!/usr/bin/env python
import os
import sys
import time
import random
from mcush import *

MALLOC_LIMIT = int(os.getenv('MALLOC_LIMIT', 1*1024))
NO_FREE = Env.getenv_bool("NO_FREE")
REALLOC_SUPPORT = Env.getenv_bool("REALLOC_SUPPORT")

def main(argv=None):

    allocated_size_sum = 0
    mem = []
    mfill = {}
    msize = {}
    malloc_counter = 0
    realloc_counter = 0
    free_counter = 0
    segment_counter = 0
    fill_err_counter = 0

    s = Mcush.Mcush()
    #s.reboot()
    print( 'uptime: %s'% s.uptime() )

    def info( message ):
        print( 'SUM=%d, NUM=%d, (%d/%d/%d/%d) %s'% (allocated_size_sum, segment_counter, malloc_counter, realloc_counter, free_counter, fill_err_counter, message) )
  
    try: 
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
                allocated_size_sum += size
                info( 'malloc %d 0x%08X'% (size, m) )
                s.fillMem( m, pat, 1, size )
            # 2. if malloc failed, try to use realloc from existing one
            elif mem and REALLOC_SUPPORT:
                em = random.choice(mem)
                ems = msize[em]
                m = s.realloc( em, size )
                if m:
                    mem.remove(em)
                    mem.append(m)
                    realloc_counter += 1
                    allocated_size_sum += size - ems
                    info( 'realloc %d 0x%08X -> %d 0x%08X'% (msize[em], em, size, m) )
                    s.fillMem( m, pat, 1, size )
                    mfill[m] = pat 
                    msize[m] = size
            # 3. if still not allocated, try free one 
            if (not m) and mem:
                m = random.choice(mem)
                ms = msize[m]
                mem.remove(m)
                if not s.testMem( m, mfill[m], 1, msize[m] ):
                    fill_err_counter += 1
                    info( 'memory fill test failed @ 0x%08X'% (m) )
                s.free( m )
                mfill.pop(m)
                msize.pop(m)
                free_counter += 1
                segment_counter -= 1
                allocated_size_sum -= ms
                info( 'free 0x%08X'% (m) )

    except KeyboardInterrupt:
        s.disconnect()
        if not mem:
            return
        if NO_FREE:
            return
        # free all buffer
        print( '\nfree all memory...' )
        s.connect() 
        while True:
            if not mem:
                break
            m = mem.pop()
            ms = msize[m]
            s.free( m )
            segment_counter -= 1
            allocated_size_sum -= ms
            free_counter += 1
            info( 'free 0x%08X'% (m) )
        



if __name__ == '__main__':
    main(sys.argv)

