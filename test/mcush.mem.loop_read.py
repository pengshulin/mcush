#!/usr/bin/env python
import os
import sys
import time
import Env
import Mcush
import Utils


def main(argv=None):
    s = Mcush.Mcush()
    count = 1
    while True:
        print time.strftime('%H:%M:%S'), count
        mem = s.readMem( 0x20000000, 20*1024 )
        #Utils.dumpMem( mem ) 
        count += 1


if __name__ == '__main__':
    main(sys.argv)

