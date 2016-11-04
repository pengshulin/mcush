#!/usr/bin/env python
import sys
import time
import Env
import Mcush


def main(argv=None):
    s = Mcush.Mcush()
    for i in range(7):
        print 'Port %d: 0x%08X'% (i, s.gpio(i))
   



if __name__ == '__main__':
    main(sys.argv)

