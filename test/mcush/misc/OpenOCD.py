# coding: utf8
__doc__ = 'OpenOCD Controller'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Instrument, Env, Utils
from re import compile as re_compile
import time

WIDTH_TYPE = {8:'b', 16:'h', 32:'w'}

class OpenOCD( Instrument.SocketInstrument ):
    DEFAULT_NAME = 'OpenOCD'
    DEFAULT_PROMPTS = re_compile( '\x00?\x0d> ' )
    DEFAULT_CHECK_IDN = False
        
    def __init__( self, *args, **kwargs ):
        kwargs['net_addr'] = kwargs.get('net_addr','localhost')
        kwargs['net_port'] = kwargs.get('net_port','4444')
        Instrument.SocketInstrument.__init__( self, *args, **kwargs )

    def checkReturnedCommand( self, ret, cmd ):
        # cut off the reprinted leading prompts
        if ret[0].startswith('\x08\x08  \x08\x08> '):
            ret[0] = ret[0][8:]
        Instrument.SocketInstrument.checkReturnedCommand( self, ret, cmd )

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

    def _mdx( self, width, addr, count ):
        cmd = 'md%s 0x%X'% (WIDTH_TYPE[width], addr)
        if count:
            cmd += ' %d'% count
        ret = self.writeCommand( cmd )
        mem = []
        for line in ret:
            for b in line.split(': ')[1].split(' '):
                intb = int(b,16)
                mem.append( intb & 0xFF )
                if width >= 16:
                    mem.append( (intb>>8) & 0xFF )
                    if width >= 32:
                        mem.append( (intb>>16) & 0xFF )
                        mem.append( (intb>>24) & 0xFF )
        if Env.PYTHON_V3:
            return bytes(mem)
        else:
            return Env.EMPTY_BYTE.join(map(chr,mem))

    def mdb( self, addr, count=None ):
        return self._mdx( 8, addr, count )

    def mdh( self, addr, count=None ):
        return self._mdx( 16, addr, count )

    def mdw( self, addr, count=None ):
        return self._mdx( 32, addr, count )

    def _mwx( self, width, addr, value, count ):
        cmd = 'mw%s 0x%X 0x%X'% (WIDTH_TYPE[width], addr, value)
        if count:
            cmd += ' %d'% count
        self.writeCommand( cmd )

    def mwb( self, addr, value, count=None ):
        self._mwx( 8, addr, value, count )

    def mwh( self, addr, value, count=None ):
        self._mwx( 16, addr, value, count )

    def mww( self, addr, value, count=None ):
        self._mwx( 32, addr, value, count )


    def getReg( self, reg ):
        if isinstance( reg, int ):
            cmd = 'reg %u'% reg
        else:
            cmd = 'reg %s'% reg
        ret = self.writeCommand( cmd )
        return int(ret[0].split(': ')[-1], 16)

    def setReg( self, reg, val ):
        if isinstance( reg, int ):
            cmd = 'reg %u 0x%X'% (reg, val)
        else:
            cmd = 'reg %s 0x%X'% (reg, val)
        self.writeCommand( cmd )

    def program( self, filename, address=None, verify=None, reset=None ):
        cmd = 'program %s'% filename
        if address:
            cmd += ' 0x%X'% address
        if verify:
            cmd += ' verify'
        if reset:
            cmd += ' reset'
        self.writeCommand( cmd )
         
