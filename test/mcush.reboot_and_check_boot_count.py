#!/usr/bin/env python
import sys
import time
import random
from mcush import *
from mcush.misc import Tm1637


def main(argv=None):
    m = Mcush.Mcush()
    r = Mcush.Mcush('/dev/ttyACM0')
    r.gpio('2.0', o=True, c=True )
    d = Tm1637.LED4( r )
    d.init(scl='2.14', sda='2.15')
    d.clear()

    while True:
        # reset
        r.gpio('2.0', s=True)
        r.gpio('2.0', c=True)
        m.readUntilPrompts()
        boot = int(m.cat('/s/boot'))
        print boot
        d.display_string("%d"% boot)
        #time.sleep(1)

   
if __name__ == '__main__':
    main(sys.argv)
