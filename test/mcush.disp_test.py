#!/usr/bin/env python
import os
import sys
import time
import random
import Env
import Mcush
import Utils


def main(argv=None):

    s = Mcush.Mcush()

    count = 0
    while True:
        s.disp( integer=count )
        count += 1
        #time.sleep(0.2)
 



if __name__ == '__main__':
    main(sys.argv)

