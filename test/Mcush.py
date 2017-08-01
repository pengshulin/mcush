#!/usr/bin/env python
# coding:utf8
__doc__ = 'mcush controller'
__author__ = 'Peng Shulin <trees_peng@163.com>'
import os
import re
import sys
import time
import logging
import Env
import Utils
import Instrument
from Instrument import reg

class Mcush( Instrument.SerialInstrument ):
    '''Mcush core'''
    DEFAULT_NAME = 'MCUSH'
    DEFAULT_IDN = re.compile( 'mcush,([0-9]+\.[0-9]+.*)' )

    def errno( self, new=None, stop=False ):
        '''set/get error number'''
        if stop:
            self.writeCommand( 'error -s' )
            return 
        if new is None: 
            ret = self.writeCommand( 'error' )
            return int(ret[0].strip())
        else:
            assert isinstance(new, int)
            self.writeCommand( 'error %d'% new )

    def beep( self, freq=None, duration=0.05, times=1 ):
        cmd = 'beep'
        if freq:
            cmd += ' -f %d'% freq
        if duration:
            cmd += ' %d'% (int(duration*1000))
        while times:
            self.writeCommand(cmd)
            if times <= 1:
                break
            times -= 1
            time.sleep( duration )
    
    def disp( self, integer=None, buf=None ):
        if integer is not None:
            cmd = 'disp -i %d'% integer
        elif buf is not None:
            cmd = 'disp -- "%s"'% buf
        self.writeCommand(cmd)



if __name__ == '__main__':
    m = Mcush()
    print m.getModel(), m.getVersion()
    while 1 :
        print m.uptime()
    m.disconnect()

