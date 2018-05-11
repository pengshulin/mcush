#!/usr/bin/env python
import sys
import random
from mcush import *


def main(argv=None):
    '''random size'''
    random.seed()
    s = Mcush.Mcush()
    while True:
        name = '/s/test.wl'
        size = 64*1024
        print '%s %d bytes'% (name, size)
        if not Utils.testSingleRandomFile( s, name, size ):
            break
    s.disconnect()
        
   
if __name__ == '__main__':
    main(sys.argv)
