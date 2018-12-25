#!/usr/bin/env python
'''test swap files'''
import os
import sys
import time
from mcush import *


def main(argv=None):
    s = Mcush.Mcush()
    fs = s.list('/s')
    #fs = s.list('/s')[-3:-2]
    #fs = s.list('/s')[:-2]
    #fs = s.list('/s')[-2:]
    #fs = [ ('/s','test.59',1736) ]
    #fs = [ ('/s','test.63',1370) ]
    #time.sleep(1)
    #print fs
    for dirname, filename, size in fs:
        if filename and filename.startswith('test.'):
            print( "test %s, %d Bytes"% (filename, size) )
            #s.remove( os.path.join(dirname, filename) )
            dat = s.cat(os.path.join(dirname, filename), b64=True)
            if len(dat) != size:
                print( "Fail: %s bytes read"% len(dat) )
 
if __name__ == '__main__':
    main(sys.argv)
