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
    name = '/s/test.big'
    while True:
        contents = 'This is the %s append line.\n'% counter
        print( "[%d] %s, %d"% (counter, name, len(contents)) )
        s.cat( name, append=True, buf=contents )
        counter += 1
    s.disconnect()
   
if __name__ == '__main__':
    main(sys.argv)
