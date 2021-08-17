#!/usr/bin/env python
import sys
import random
from mcush import *

Env.TEST_MAXID = 20

def main(argv=None):
    random.seed()
    s = Mcush.Mcush( check_idn=False )
    s.spiffsRemount()
    #print( "formating..." )
    #s.spiffsFormat()
    try:
        size = int(open('%s.log'% argv[0], 'r').read().strip().splitlines()[-1].split()[0]) + 1
    except:
        size = 1
    log = open('%s.log'% argv[0], 'a+') 
    counter = 0
    while True:
        name = '/s/test.%d'% random.randint( Env.TEST_MINID, Env.TEST_MAXID) 
        err = ''
        #size = 43
        try:
            if not Utils.testSingleRandomFile( s, name, size ):
                err = 'verify err'
        except Instrument.CommandExecuteError:
            err = 'io err'
        except Instrument.CommandTimeoutError:
            s.writeCommand('')
            s.readUntilPrompts()
            err = 'comm err'
        print( "[%d] %s, %d  %s"% (counter, name, size, err) )
        if err:
            log.write("%s  %s\n"% (size, err))
            log.flush()
            break  # stop
        size += 1
        counter += 1
        
    s.disconnect()
    log.close()
   
if __name__ == '__main__':
    main(sys.argv)
