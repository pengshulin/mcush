# coding:utf8
__doc__ = 'Vibration Analysis Platform Series'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Mcush


class VAP( Mcush.Mcush ):
    '''VAP 100/200'''
    DEFAULT_NAME = 'VAP'
    DEFAULT_IDN = re_compile( 'VAP[0-9]+.*,([0-9]+\.[0-9]+.*)' )

    def getDeviceName( self ):
        ret = self.writeCommand( '*idn?' )
        try:
            self.device_name = ret[2].strip()
            self.logger.info( 'DeviceName:%s', str(self.device_name) )
            return self.device_name
        except IndexError:
            self.device_name = None
            return ''
        
    def parseEnv( self, info ):
        var = {}
        for l in info:
            ab = l.split(':')
            if len(ab) == 2:
                a, b = ab
            elif len(ab) == 1:
                a, b = ab[0], ''
            var[a.strip()] = b.strip()
        return var
        
    def env( self ):
        return self.parseEnv( self.writeCommand('env') )
 
    def ad( self, command, index=None, value=None ):
        cmd = 'ad7768 -c %s'% command
        if index is not None:
            cmd += ' -i %d'% index
        if value is not None:
            if value & 0xFFFF0000:
                cmd += ' -v 0x%X'% value
            else:
                cmd += ' -v %d'% value
        ret = self.writeCommand( cmd )
        return ret

    def ad_mem( self ):
        a, b = self.ad('mem')[0].split(' ')
        return (int(a,16), int(b,16))
    
    def ad_stop( self ):
        self.ad( 'stop' )


    def daq( self, command, index=None, value=None ):
        cmd = 'daq -c %s'% command
        if index is not None:
            cmd += ' -i %d'% index
        if value is not None:
            if value & 0xFFFF0000:
                cmd += ' -v 0x%X'% value
            else:
                cmd += ' -v %d'% value
        ret = self.writeCommand( cmd )
        return ret

    def daq_lock( self ):
        self.daq('lock')

    def daq_unlock( self ):
        self.daq('unlock')

    def daq_enable( self ):
        self.daq('enable')

    def daq_disable( self ):
        self.daq('disable')

    def daq_busy( self ):
        return bool(int(self.daq('busy')[0]))

    def daq_tacho( self, info=None ):
        val = None
        if info is None:
            info = self.daq('info')
        for l in info:
            if l.startswith('tac:'):
                val = float(l.split(': ')[1].strip())
                break
        return val
    
    def daq_channel_info( self, info=None ):
        ret = {}
        if info is None:
            info = self.daq('info')
        for l in info:
            if l.startswith('#'):
                a, b, c, d = l.split('  ')
                idx = int(a.lstrip('#'))
                cnt = int(b.split(': ')[-1]) 
                avg = float(c.split(': ')[-1]) 
                rms = float(d.split(': ')[-1]) 
                ret[idx] = {'cnt': cnt, 'avg': avg, 'rms': rms}
        return ret
                 
