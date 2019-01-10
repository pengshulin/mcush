#!/usr/bin/env python
import sys
import time
from mcush import *


URL=Env.getenv('URL', 'http://baidu.com/index.html')
FNAME='/s/wget.test'

def main(argv=None):
    m = Mcush.Mcush()
    Utils.writeTitle( 'wget test' )
    print( 'URL: %s'% URL )
    print( 'FNAME: %s'% FNAME )

       
    cnt_succ, cnt_fail = 0, 0
    while True:
        if m.checkFileExist(FNAME):
            m.remove( FNAME )
        r = m.wget( URL, FNAME )
        size = m.checkFileSize( FNAME )
        if r and size:
            cnt_succ += 1
        else:
            cnt_fail += 1
        print( '%s  size: %s  succ: %d  fail: %d'% (Utils.DATETIME(), size if size else '---', cnt_succ, cnt_fail) )

   
if __name__ == '__main__':
    main(sys.argv)
