# coding: utf8
__doc__ = 'OpenOCD Controller'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Instrument, Env, Utils
from re import compile as re_compile
import time



class OpenOCD( Instrument.SocketInstrument ):
    DEFAULT_PROMPTS = re_compile( '\x00\x0d> ' )
    DEFAULT_TERMINATOR_RESET = '\n'  # new-line
        
    def __init__( self, *args, **kwargs ):
        kwargs['ip'] = kwargs.get('ip','localhost')
        kwargs['port'] = kwargs.get('port','4444')
        self.check_idn = False
        Instrument.SocketInstrument.__init__( self, *args, **kwargs )

    def halt( self, ms=None ):
        cmd = 'halt'
        if ms is not None:
            cmd += ' %d'% ms
        self.writeCommand( cmd )

    def reset( self, mode='run' ):
        # mode: run/halt/init
        self.writeCommand( 'reset %s'% mode )

    def resume( self, addr=None ):
        cmd = 'resume'
        if addr is not None:
            cmd += ' 0x%X'% addr
        self.writeCommand( cmd )

    def step( self ,addr=None ):
        cmd = 'step'
        if addr is not None:
            cmd += ' 0x%X'% addr
        self.writeCommand( cmd )

