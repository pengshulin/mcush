#!/usr/bin/env python
import os
import sys
import time
import Env
import Mcush
import Utils


def main(argv=None):
    try:
        assert 3 <= len(argv) <= 4
        addr = int(eval(argv[1]))
        length = int(eval(argv[2]))
    except:
        print >> sys.stderr, 'Syntax: %s addr length <file_name>'% argv[0]
        return
    try:
        fname = argv[3]
    except IndexError:
        fname = None
    if Env.VERBOSE:
        print 'addr: 0x%08X, length: %d'% (addr, length)
        print 'output: %s'% ('stdout' if fname is None else fname )

    if length <= 0:
        return

    s = Mcush.Mcush()
    mem = s.readMem( addr, length, compact_mode=Env.COMPACT_MODE )
    if fname is None:
        #Utils.dumpMem( mem, method=1 ) 
        open(Env.TEST_WR, 'w+').write(mem)
        #Utils.dumpFile( Env.TEST_WR )
        sys.stdout.write( mem )  # write binary directly to console
        os.remove( Env.TEST_WR )
    else:
        open(fname, 'w+').write(mem)



if __name__ == '__main__':
    main(sys.argv)

