# coding:utf8
__doc__ = 'mcush controller'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import os
import re
import sys
import time
import base64
import binascii
import logging
from . import Env
from . import Utils
from . import Instrument
from . import Register



class Mcush( Instrument.SerialInstrument ):
    '''Mcush core'''
    DEFAULT_NAME = 'MCUSH'
    DEFAULT_IDN = re.compile( 'mcush,([0-9]+\.[0-9]+.*)' )

    regs_by_name = {}
    regs_by_addr = {}

    def addReg( self, r ):
        '''add register'''
        self.regs_by_name[r.name] = r
        self.regs_by_addr[r.address] = r
        
    def setReg( self, regname, value ):
        '''set register'''
        r = self.regs_by_name[regname]
        w, b, fget, fset = Register.REGISTER_TYPE[r.reg_type]
        m = fset(value)
        self.writeMem( r.address, m )
    
    def getReg( self, regname ):
        '''get register'''
        r = self.regs_by_name[regname]
        w, b, fget, fset = Register.REGISTER_TYPE[r.reg_type]
        m = self.readMem( r.address, length=b )
        return fget(m)

    def led( self, idx, on=None, toggle=None ):
        '''led control'''
        if on is None and toggle is None:
            # read
            r = self.writeCommand( 'led -i %d'% (idx) )
            return bool(r[0].strip() == '1')
        else:
            # set 
            cmd = 'led -i %d'% (idx)
            if toggle is not None:
                cmd += ' -t'
            else:
                cmd += ' -s' if on else ' -c'
            self.writeCommand( cmd )

    def gpio( self, port, i=None, o=None, s=None, c=None, t=None ):
        '''gpio control'''
        cmd = 'gpio -p %s'% str(port)
        if i is not None:
            if type(i) is int:
                cmd += ' -i 0x%x'% i
            elif type(i) is bool and i:
                cmd += ' -i'
        if o is not None:
            if type(o) is int:
                cmd += ' -o 0x%x'% o
            elif type(o) is bool and o:
                cmd += ' -o'
        if s is not None:
            if type(s) is int:
                cmd += ' -s 0x%x'% s
            elif type(s) is bool and s:
                cmd += ' -s'
        if c is not None:
            if type(c) is int:
                cmd += ' -c 0x%x'% c 
            elif type(c) is bool and c:
                cmd += ' -c'
        if t is not None:
            if type(t) is int:
                cmd += ' -t 0x%x'% t 
            elif type(t) is bool and t:
                cmd += ' -t'
        ret = self.writeCommand( cmd )
        if ret:
            return eval(ret[0])

    def parseMemLine( self, line, compact_mode=False ):
        '''parse memory line which has been stripped'''
        # format(standard):  XXXXXXXX: xx xx xx xx xx ... xx    (no ascii mode, final space stripped)
        # format(compact):   xxxxxxxxxxxx...xx
        #print( type(line), line )
        line_len = len(line)
        line_len_err = bool(line_len % 2) if compact_mode else bool((line_len-9) % 3)
        if line_len_err:
            raise CommandExecuteError('memory line width error, length=%s'% line_len)
        if compact_mode:
            m = [binascii.unhexlify(line[i:i+2]) for i in range(0,line_len,2)]
        else:
            m = [binascii.unhexlify(line[i:i+2]) for i in range(10,line_len,3)]
        ret = (b'' if Env.PYTHON_V3 else '').join(m)
        #print( type(ret), len(ret), ret )
        return ret
       
    def fillMem( self, addr, pattern, width, length ):
        '''fill memory with specific pattern'''
        cmd = 'mfill -b 0x%X -w %d -p 0x%X -l %d'% ( addr, width, pattern, length )
        self.writeCommand( cmd )
 
    def testMem( self, addr, pattern, width, length ):
        '''test filled memory with specific pattern'''
        cmd = 'mfill -t -b 0x%X -w %d -p 0x%X -l %d'% ( addr, width, pattern, length )
        try:
            self.writeCommand( cmd )
            return True
        except CommandExecuteError:
            return False
        
    def malloc( self, length ):
        cmd = 'mapi -m -l %d'% length
        ret = self.writeCommand(cmd)
        addr = eval(ret[0])
        return addr

    def realloc( self, addr, length ):
        cmd = 'mapi -r -b 0x%08X -l %d'% (addr, length)
        ret = self.writeCommand(cmd)
        addr = eval(ret[0])
        return addr

    def free( self, addr ):
        cmd = 'mapi -f -b 0x%08X'% addr
        self.writeCommand( cmd )
 
    def readMem( self, addr, length=1, compact_mode=False, retry=None ):
        '''get memory'''
        cmd = 'x -b 0x%X -l %d'% ( addr, length )
        if compact_mode:
            cmd += ' -c'
        if retry:
            ret = self.writeCommandRetry( cmd, retry )
        else:
            ret = self.writeCommand( cmd )
        assert 1 <= len(ret)
        for line in ret:
            self.logger.info( line )
        mem = (b'' if Env.PYTHON_V3 else '').join( \
               [self.parseMemLine(line.strip(), compact_mode) for line in ret])
        #print( type(mem), len(mem), mem )
        return mem[:length]

    MAX_ITEMS_PER_WRITE = { 1: 16, 2: 8, 4: 4  }

    def writeMem( self, addr, data, width=1 ):
        '''write memory'''
        length = len(data)
        written = 0
        while written < length:
            cmd = 'w -b 0x%X -w%d '% ( addr, width )
            left = min(length-written, self.MAX_ITEMS_PER_WRITE[width])
            if width == 1:
                cmd += ' '.join([str(ord(c)) for c in data[written:written+left]])
            elif width == 2:
                cmd += ' '.join([hex(ord(data[i])+(ord(data[i+1])<<8)) for i in range(written, written+left*2, 2)])
            elif width == 4:
                cmd += ' '.join([hex(ord(data[i])+(ord(data[i+1])<<8)+(ord(data[i+2])<<16)+(ord(data[i+3])<<24)) for i in range(written, written+left*4, 4)])
            self.writeCommand( cmd )
            written += left*width
            addr += left*width

    def dumpMem( self, addr, length=1 ):
        '''dump memory'''
        Utils.dumpMem( self.readMem(addr, length) )

    def uptime( self ):
        return self.writeCommand('uptime')[0].strip()

    def sysTask( self ):
        ret = self.writeCommand('sys t')
        return ret

    def sysQueue( self ):
        ret = self.writeCommand('sys q')
        return ret

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
        #print( pathname )
        pathname = unicode(pathname)
        namelen = len(os.path.basename(pathname).encode('utf8'))
        if namelen > 28:
            raise Exception( "File name length too long %d"% namelen )
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
            if b64:
                ret = base64.decodestring( '\n'.join(ret) )
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
        
               
    def i2c_init( self, addr, scl=None, sda=None, delay=None ):
        cmd = 'i2c -a0x%X -i' % (addr)
        if scl:
            cmd += ' --scl=%s'% scl
        if sda:
            cmd += ' --sda=%s'% sda
        if delay:
            cmd += ' --delay=%s'% delay
        self.writeCommand( cmd )


    def i2c( self, write=[], read_count=None ):
        cmd = 'i2c'
        if read_count:
            cmd += ' -r%d'% read_count
        if write:
            cmd += ' %s'% (' '.join(['0x%X'%i for i in write])) 
        ret = self.writeCommand( cmd )
        if read_count:
            r = []
            for l in ret:
                for v in l.strip().split():
                    r.append(eval(v))
            return r
        else:
            return None

    def spi_init( self, sdi=None, sdo=None, sck=None, cs=None, cpol=None, cpha=None, width=None, lsb=None, delay=None ):
        cmd = 'spi -i'
        if sdi:
            cmd += ' --sdi=%s'% sdi
        if sdo:
            cmd += ' --sdo=%s'% sdo
        if sck:
            cmd += ' --sck=%s'% sck
        if cs:
            cmd += ' --cs=%s'% cs
        if cpol:
            cmd += ' --cpol'
        if cpha:
            cmd += ' --cpha'
        if lsb:
            cmd += ' --lsb'
        if width:
            cmd += ' --width=%d'% width
        if delay:
            cmd += ' --delay=%s'% delay
        self.writeCommand( cmd )

    def spi( self, write=[], read=None ):
        cmd = 'spi'
        if read:
            cmd += ' -r'
        for d in write:
            cmd += ' %d'% d 
        ret = self.writeCommand( cmd )
        if read:
            r = []
            for l in ret:
                for v in l.strip().split():
                    r.append(eval(v))
            return r
        else:
            return None

    def rtcSync( self ):
        # update to current time
        t = time.localtime()
        cmd = 'rtc -s %d-%d-%d %d:%d:%d'% (t.tm_year, t.tm_mon, t.tm_mday,
                                           t.tm_hour, t.tm_min, t.tm_sec )
        self.writeCommand( cmd )
   
    def rtcRead( self ): 
        try:
            r = self.writeCommand( 'rtc' )
            d, t = r[0].split(' ')
        except Instrument.CommandExecuteError:
            return None
        YEAR, MONTH, MDAY = d.split('-')
        HOUR, MIN, SEC = t.split(':')
        return (int(YEAR), int(MONTH), int(MDAY), int(HOUR), int(MIN), int(SEC), 0, 1, -1)

 
