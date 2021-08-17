#!/usr/bin/env python
import sys
import random
from mcush import *

def main(argv=None):
    random.seed()
    s = Mcush.Mcush( check_idn=False )
    #print( "formating..." )
    #s.spiffsFormat()
    for i in xrange( Env.RETRY):
        name = '/s/test.%d'% random.randint( Env.TEST_MINID, Env.TEST_MAXID) 
        size = random.randint( 1, Env.TEST_MAXSIZE )
        #size = Env.TEST_MAXSIZE
        print( "[%d] %s, %d"% (i, name, size) )
        if not Utils.testSingleRandomFile( s, name, size ):
            quit( 1 )      
            break
    s.disconnect()
   
if __name__ == '__main__':
    main(sys.argv)
