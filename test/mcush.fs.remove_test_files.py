#!/usr/bin/env python
'''clear swap files'''
import os
import sys
from mcush import *


def main(argv=None):
    s = Mcush.Mcush(check_idn=False)
    fs = s.list('/s')
    #print fs
    for dirname, filename, size in fs:
        if filename and filename.startswith('test.'):
            print( "remove %s, %d Bytes"% (filename, size) )
            s.remove( os.path.join(dirname, filename) )
 
if __name__ == '__main__':
    main(sys.argv)
