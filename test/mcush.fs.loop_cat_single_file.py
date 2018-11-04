#!/usr/bin/env python
# generate a single binary file, looply read the file and check
import sys
import random
from mcush import *

def main(argv=None):
    random.seed()
    s = Mcush.Mcush()

    print( s.spiffsInfo() )

    #print( "formating..." )
    #Utils.spiffsFormat( s )
        
    name = '/s/test.bin'
    size = Env.TEST_MAXSIZE
    Utils.generateRandomFile( size )
    s.putFile( name, Env.TEST_WR )

    counter = 0
    while True:
        print( "[%d] %s, %d"% (counter, name, size) )
        s.getFile( name, Env.TEST_RD )
        if not Utils.diffTestFiles():
            break
        counter += 1

    s.disconnect()
   
if __name__ == '__main__':
    main(sys.argv)
