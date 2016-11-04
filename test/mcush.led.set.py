#!/usr/bin/env python
import sys
import time
import Env
import Mcush

CMD = {
'-1': -1,
'1': 1,
'0': 0,
's': 1,
'S': 1,
'set': 1,
'SET': 1,
'on': 1,
'ON': 1,
'c': 0,
'C': 0,
'clr': 0,
'CLR': 0,
'off': 0,
'OFF': 0,
't': -1,
'T': -1,
'toggle': -1,
'TOGGLE': -1,
}


def main(argv=None):
    if len(argv) == 2:
        # read status
        idx = int(argv[1])
        cmd = None
        s = Mcush.Mcush()
        print s.led( idx )
    elif len(argv) == 3: 
        # set/clr/toggle
        idx = int(argv[1])
        cmd = CMD[argv[2].strip()]
        s = Mcush.Mcush()
        if cmd == -1:
            s.led( idx, toggle=True )
        else:
            s.led( idx, on=bool(cmd==1) )
    else:
        return
   



if __name__ == '__main__':
    main(sys.argv)

