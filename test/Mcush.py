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

    def list( self, pathname='/s' ):
        cmd = 'ls %s'% pathname
        ret = self.writeCommand(cmd)
        flist = []
        path = ''
        for l in ret:
            if l.startswith('/'):
                path = l.strip().rstrip(':')
            else:
                a, b = l.strip().split()
                flist.append( (os.path.join(path, b), int(a)) )
        return flist

    def cat( self, pathname, b64=False, write=False, append=False, buf='' ):
        if write or append:
            if append:
                cmd = 'cat -a '
            else:
                cmd = 'cat -w '
            cmd += pathname
            self.setPrompts( self.DEFAULT_PROMPTS_MULTILINE )
            self.writeCommand( cmd )
            for l in buf.splitlines():
                self.writeCommand( l.rstrip() )
            self.setPrompts()
            self.writeCommand( '' )
        else:
            if b64:
                cmd = 'cat -b %s'% pathname
            else:
                cmd = 'cat %s'% pathname
            ret = self.writeCommand( cmd )
            #for l in ret:
            #    print l
            return ret

    def remove( self, pathname ):
        cmd = 'rm %s'% pathname
        self.writeCommand( cmd )

    def rename( self, old_pathname, new_name ):
        cmd = 'rename %s %s'% (old_pathname, new_name)
        self.writeCommand( cmd )


    def spiffs( self, command, value=None ):
        cmd = 'spiffs -c %s'% command
        if value is not None:
            cmd += ' %s'% value
        return self.writeCommand( cmd )



