#!/usr/bin/env python
# coding:utf8
__doc__ = 'mcush controller'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import os
import re
import sys
import time
import base64
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

    def list( self, pathname=None ):
        if pathname:
            cmd = 'ls %s'% pathname
        else:
            cmd = 'ls'
        ret = self.writeCommand(cmd)
        flist = []
        path = ''
        for l in ret:
            if l.startswith('/'):
                path = l.strip().rstrip(':')
                flist.append( (path, None, None) )
            else:
                a, b = l.lstrip().split('  ')
                flist.append( (path, b, int(a)) )
        return flist


    def convPathname( self, pathname ):
        print pathname
        pathname = unicode(pathname)
        if pathname.find(' ') != -1:
            pathname = '\"' + pathname + '\"'
        return pathname.encode('utf8')

    def cat( self, pathname, b64=False, write=False, append=False, buf='' ):
        pathname = self.convPathname(pathname)
        if write or append:
            if append:
                cmd = 'cat -a '
            else:
                cmd = 'cat -w '
            if b64:
                cmd += '-b '
            cmd += pathname
            self.setPrompts( self.DEFAULT_PROMPTS_MULTILINE )
            self.writeCommand( cmd )
            if b64:
                buf = base64.encodestring(buf)
            buf = buf.splitlines()
            for l in buf:
                self.writeCommand( l.rstrip() )
            self.setPrompts()
            self.writeCommand( '' )
        else:
            if b64:
                cmd = 'cat -b '
            else:
                cmd = 'cat '
            cmd += pathname
            ret = self.writeCommand( cmd )
            return ret

    def remove( self, pathname ):
        pathname = self.convPathname(pathname)
        cmd = 'rm ' + pathname
        self.writeCommand( cmd )

    def rename( self, old_pathname, new_name ):
        old_pathname = self.convPathname(old_pathname)
        new_name = self.convPathname(new_name)
        cmd = 'rename %s %s'% (old_pathname, new_name)
        self.writeCommand( cmd )

    def spiffs( self, command, value=None ):
        cmd = 'spiffs -c %s'% command
        if value is not None:
            cmd += ' %s'% value
        return self.writeCommand( cmd )


    def sgpio( self, port, pins, buf, freq=10, loop=True, start=True ):
        self.writeCommand( 'sgpio --stop' )
        cmd = 'sgpio -p%s -o0x%X -f%d'% (int(port), int(pins), int(freq))
        if loop:
            cmd += ' -l'
        self.setPrompts( self.DEFAULT_PROMPTS_MULTILINE )
        self.writeCommand( cmd )
        line = ''
        for item in buf:
            line += '%d '% item
            if len(line) >= 60:
                self.writeCommand( line.rstrip() )
                line = ''
        if line:
            self.writeCommand( line.rstrip() )
        self.setPrompts()
        self.writeCommand( '' )
        if start:
            self.writeCommand( 'sgpio --start' )
        
                
