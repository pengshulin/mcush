#!/usr/bin/env python
# coding: utf8
# test pulse generator
# Peng Shulin <trees_peng@163.com>
import sys
import time
from mcush import *


def main(argv=None):
    s = Mcush.Mcush( check_idn=False )
    s.pulseInit( pin='0.0' )

    counter = 10
    while True:
        s.pulse( counter ) 
        #counter += 1
        #if counter > 100:
        #    counter = 1

if __name__ == '__main__':
    main(sys.argv)
    
