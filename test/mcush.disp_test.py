#!/usr/bin/env python
import os
import sys
import time
import random
from mcush import *
import subprocess


def main(argv=None):
    s = Mcush.Mcush()
    count = 0
    while True:
        #s.disp( integer=count )
        #count += 1
        ##time.sleep(0.2)

        sensors = subprocess.check_output(['sensors'])
        for l in sensors.splitlines():
            if l.startswith('Core 0:'):
                temp = float(l.split(':')[1].lstrip().split('  ')[0][1:-3])
                s.disp( buf='%.1fc'% temp )
                print temp
        time.sleep(1)
        
 



if __name__ == '__main__':
    main(sys.argv)

