#!/usr/bin/env python
# coding:utf8
import os
import re
import sys
import time
import binascii
from mcush import *

HOSTNAME = Env.getenv( 'HOSTNAME', "www.baidu.com" )
while True:
    response = os.system("ping -q -c 1 " + HOSTNAME)
    if response == 0:
        print HOSTNAME, 'is up!'
    else:
        print HOSTNAME, 'is down!'
        try:
            Mcush.Mcush().beep()
        except Exception as e:
            print str(e)

    time.sleep(Env.DELAY)
