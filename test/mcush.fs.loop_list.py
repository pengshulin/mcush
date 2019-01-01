#!/usr/bin/env python
# list spiffs directory
import sys
import time
from mcush import *

def main(argv=None):
    s = Mcush.Mcush()
    count = 1
    # record init files info
    old_files = {}
    t0 = time.time()
    for _, filename, size in s.list('/s'):
        if size is not None:
            old_files[filename] = size
    s.errno( 0 )  # clear errno
    while True:
        if s.errno() == 3:
            print 'errno == 3, stop'
            break
        print( "[%d] listing..."% (count) )
        t = time.time()
        fs = s.list('/s')
        new_files = {}
        for dirname, filename, size in fs:
            if size is None:
                continue
            if filename in old_files:
                # print files with size modification (including data rate)
                dsize = size - old_files[filename]
                dt = t - t0
                if dsize:
                    sample_rate = dsize / dt
                    # in red color
                    print( "  \x1B[31m%6d %s (%.3f kB/s)\x1B[0m"% (size, filename, sample_rate/1000) )
                else:
                    print( "  %6d %s"% (size, filename) )
            else:
                # newly appended files
                print( "  %6d %s"% (size, filename) )
            new_files[filename] = size    
        old_files = new_files
        t0 = t
        count += 1
        
    s.disconnect()
   
if __name__ == '__main__':
    main(sys.argv)
