#!/usr/bin/env python3
# coding: utf8
# test jy901s module
# Peng Shulin <trees_peng@163.com>
import os
import sys
import time
import random
from mcush import *

 
def main(argv=None):
    wt = WitMotion.JY901()
    #wt.setLED(True)
    wt.syncRTC()
    #wt.setRSW( quad=True )
    #wt.setRSW( port=True, acc=False,gyo=False,mag=False,ang=False )
    #wt.setRSW( time=True )
    #wt.setRSW( time=True, acc=False, gyo=False, mag=False, ang=False )
    #wt.save()
    #wt.lock()
    #while True:
    #    wt.setPort( 0, 2 )  # high output
    #    time.sleep(1)
    #    wt.setPort( 0, 3 )  # low output
    #    time.sleep(1)
   
    while True:
        for p in wt.readPackets():
            print( p.getType(), p.getData() )

if __name__ == '__main__':
    main(sys.argv)

