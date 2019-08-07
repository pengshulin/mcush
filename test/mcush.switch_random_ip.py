#!/usr/bin/env python
import sys
import time
import random
from mcush import *


def main(argv=None):
    '''check current ip/subnet and change random ip'''
    s = Mcush.Mcush()
    ns = s.netstat()
    if ns.has_key('ip'):
        ip = ns['ip']
        netmask = ns['netmask']
        gateway = ns['gateway']
        ips = ip.split('.')
        old = int(ips[-1])
        while True:
            newip = '.'.join( ips[:3]+[str(random.randint(2,254))] )
            if newip != ip:
                break
        print ip, '-->', newip
        s.setNewIP( newip, netmask, gateway )

    s.disconnect()
        
   
if __name__ == '__main__':
    main(sys.argv)
