#!/usr/bin/env python
# umount/mount spiffs
import sys
from mcush import *

def main(argv=None):
    s = Mcush.Mcush()
    s.spiffsUmount()
    s.spiffsMount()
    s.disconnect()
   
if __name__ == '__main__':
    main(sys.argv)
