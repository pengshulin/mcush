# coding:utf8
__doc__ = 'Shell Lab Series'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Mcush


class ShellLab(Mcush.Mcush):
    DEFAULT_NAME = 'ShellLab'
    DEFAULT_IDN = re_compile( 'ShellLab(-[A-Z][0-9a-zA-Z\-]*)?,([0-9]+\.[0-9]+.*)' )

    def scpiRst( self ):
        if self.checkCommand("*rst"):
            self.writeCommand('*rst')
        else:
            self.errno( 0 )
            # clear all leds
            for i in range(self.getLedNumber()):
                self.led(i, False)

    def daq( self, cmd, index=None, value=None ):
        cmd = 'daq -c %s'% cmd
        if index is not None:
            cmd += ' -i %d'% index
        if value is not None:
            cmd += ' -v %d'% value
        return self.writeCommand( cmd )

    def daq_init( self, freq=None, length=None, channels=[], channel_mask=None ):
        if freq is not None:
            self.daq( 'freq', value=freq )
        if length is not None:
            self.daq( 'length', value=length )
        if channel_mask is not None:
            self.daq( 'channel_mask', value=channel_mask )
            channels = []
            for i in range(8):
                if channel_mask & (1<<i):
                    channels.append(i)
            self.channels = channels
        else:
            mask = 0
            for c in channels:
                mask |= 1<<c
            self.daq( 'channel_mask', value=mask )
            self.channels = channels
        self.daq( 'init' )
        self.vref = float(self.daq('vref')[0])

    def daq_deinit( self ):
        self.daq( 'deinit' )

    def daq_start( self ):
        self.daq( 'start' )

    def daq_stop( self ):
        self.daq( 'stop' )

    def daq_reset( self ):
        self.daq( 'reset' )

    def daq_done( self ):
        return int(self.daq( 'done' )[0])

    def daq_read( self, channel ):
        ret = self.daq( 'read', index=channel )
        dat = []
        for l in ret:
            for v in l.strip().split(','):
                dat.append( int(v, 16) * self.vref / 4096 )
        return dat

    daqInit = daq_init
    daqDeinit = daq_deinit
    daqStart = daq_start
    daqStop = daq_stop
    daqReset = daq_reset
    daqDone = daq_done
    daqRead = daq_read

