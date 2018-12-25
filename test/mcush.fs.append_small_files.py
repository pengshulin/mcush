#!/usr/bin/env python
import sys
import random
from mcush import *

def main(argv=None):
    random.seed()
    s = Mcush.Mcush()
    #print( "formating..." )
    #s.spiffsFormat()
    counter = 1
    while True:
        name = '/s/test.%d'% counter
        contents = 'This is the contents of test.%d'% counter
        print( "[%d] %s, %d"% (counter, name, len(contents)) )
        open(Env.TEST_WR, 'w+').write(contents)
        s.putFile( name, Env.TEST_WR )
        s.getFile( name, Env.TEST_RD )
        if not Utils.diffTestFiles():
            break 
        counter += 1
    s.disconnect()
   
if __name__ == '__main__':
    main(sys.argv)
