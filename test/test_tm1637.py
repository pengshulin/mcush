#!/usr/bin/env python
# coding: utf8
# test tm1637 module
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *
from mcush.misc import Tm1637



 
def main(argv=None):
    s = Mcush.Mcush()
    d = Tm1637.LED4()
    d.bind(s)
    d.init()
    string = '    abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 01234567890 \'`~-_=[]{}<>,.^/    '
    for i in range(len(string)-4):
        d.display_string( string[i:i+4] )
        time.sleep(0.5)
    counter = -100
    while True:
        print counter
        d.display_int( counter )
        counter += 1
        #time.sleep(0.1)
	


if __name__ == '__main__':
    main(sys.argv)

