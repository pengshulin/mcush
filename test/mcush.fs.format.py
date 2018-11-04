#!/usr/bin/env python
# format spiffs
import sys
import random
from mcush import *

def main(argv=None):
    random.seed()
    s = Mcush.Mcush()
    try:
        s.spiffsMount()
        print( s.spiffsInfo() )
    except:
        print( "mount error" )

    print( "formating..." )
    s.spiffsFormat()
        
    s.disconnect()
   
if __name__ == '__main__':
    main(sys.argv)
