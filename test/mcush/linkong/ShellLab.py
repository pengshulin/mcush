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

    def monitor( self, cmd, index=None, value=None ):
        cmd = 'monitor -c %s'% cmd
        if index is not None:
            cmd += ' -i %d'% index
        if value is not None:
            cmd += ' -v %d'% value
        return self.writeCommand( cmd )

    def monitor_start( self ):
        self.monitor( 'start' )

    def monitor_stop( self ):
        self.monitor( 'stop' )

    def monitor_read( self, channel ):
        ret = self.monitor( 'read', index=channel )
        dat = []
        for l in ret:
            for v in l.strip().split(','):
                dat.append( float(v) )
        return dat

    monitorStart = monitor_start
    monitorStop = monitor_stop
    monitorRead = monitor_read


COLOR_TAB = {
'black'  : 0x000000,
'red'    : 0x0000FF,
'green'  : 0x00FF00,
'yellow' : 0x00FFFF,
'blue'   : 0xFF0000,
'purple' : 0xFF00FF,
'cyan'   : 0xFFFF00,
'white'  : 0xFFFFFF,
}


class ShellLabLamp(Mcush.Mcush):
    DEFAULT_NAME = 'ShellLabLamp'
    DEFAULT_IDN = re_compile( 'ShellLab(-L[0-9a-zA-Z\-]*)?,([0-9]+\.[0-9]+.*)' )

    def lamp( self, color=None, red=None, green=None, blue=None, freq=None ):
        cmd = 'lamp'
        if color is not None:
            cmd += ' -c 0x%X'% color
        if red is not None:
            cmd += ' -r %d'% red
        if green is not None:
            cmd += ' -g %d'% green
        if blue is not None:
            cmd += ' -b %d'% blue
        if freq is not None:
            cmd += ' -f %d'% freq
        self.writeCommand(cmd)

    def reset( self ):
        self.lamp( 0, freq=0 )

    def blink( self, freq=1.0 ):
        self.lamp( freq=freq )

    def color( self, c ):
        if isinstance(c, str):
            if not COLOR_TAB.has_key(c):
                raise Exception('Unknown color name %s'% c)
            c = COLOR_TAB[c]
        self.lamp( c ) 


