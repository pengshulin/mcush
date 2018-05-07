#!/usr/bin/env python
# coding: utf8
# test tiny board
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *


def main(argv=None):
    s = Mcush.Mcush()
    s.beep()
    for i in range(5):
        s.led(0, on=True)
        s.led(1, on=True)
        time.sleep(0.1)
        s.led(0, on=False)
        s.led(1, on=False)
        time.sleep(0.1)

    for i in range(5):
        s.led(2, on=True)
        s.led(3, on=True)
        time.sleep(0.1)
        s.led(2, on=False)
        s.led(3, on=False)
        time.sleep(0.1)
    
    s.beep()


if __name__ == '__main__':
    main(sys.argv)

