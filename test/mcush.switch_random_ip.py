#!/usr/bin/env python
import sys
import time
import random
from mcush import *

def get_random_ip():
    r = random.randint(50,99)
    return r

def main(argv=None):
    '''check current ip/subnet and change random ip'''
    s = Mcush.Mcush()
    ns = s.netstat()
    if 'ip' in ns:
        ip = ns['ip']
        netmask = ns['netmask']
        gateway = ns['gateway']
        ips = ip.split('.')
        old = int(ips[-1])
        while True:
            newip = '.'.join( ips[:3]+[str(get_random_ip())] )
            if newip != ip:
                break
        print ip, '-->', newip
        s.setNewIP( newip, netmask, gateway )
        s.log( "switch ip %s -> %s"% (ip, newip) )

    s.disconnect()
        
   
if __name__ == '__main__':
    main(sys.argv)
