#!/usr/bin/env python
import sys
import time
import random
from mcush import *


def main(argv=None):
    '''random size'''
    s = Mcush.Mcush()
    counter = 0
    while True:
        r = s.writeCommand( 'mapi -t' )
        total = int(r[-1].split(': ')[-1])
        print( "[%d] %d Bytes"% (counter, total) )
        counter += 1
    s.disconnect()
        
   
if __name__ == '__main__':
    main(sys.argv)
