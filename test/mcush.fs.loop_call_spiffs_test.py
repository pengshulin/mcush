#!/usr/bin/env python
# call spiffs test command
import sys
import time
from mcush import *

def main(argv=None):
    s = Mcush.Mcush()
    count = 1
    s.errno( 0 )  # clear errno
    if s.checkFileExist( '/s/test.dat' ):
        print( "remove /s/test.dat" )
        s.remove( '/s/test.dat' )
    while True:
        print( "[%d]"% (count) )
        s.spiffs( 'test' )
        count += 1
        if count % 1000 == 0:
            # file is 38*1000 Bytes now
            s.remove( '/s/test.dat' )
            
    s.disconnect()
   
if __name__ == '__main__':
    main(sys.argv)
