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
    DEFAULT_MULTILINE_INPUT_LINE_LIMIT = 50

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

    def getLedNumber(self):
        r = self.writeCommand( 'led -n' )
        return int(r[0])
    
    def led( self, idx, on=None, toggle=None ):
        '''led control'''
        if on is None and toggle is None:
            # read
            r = self.writeCommand( 'led -i%d'% (idx) )
            return bool(r[0].strip() == '1')
        else:
            # set 
            cmd = 'led -i%d'% (idx)
            if toggle is not None:
                cmd += ' -t'
            else:
                cmd += ' -s' if on else ' -c'
            self.writeCommand( cmd )

    def ledSetValue( self, idx, val ):
        if isinstance(idx, list):
            for led in dx:
                self.led( led, on=val )
        else:
            self.led( idx, on=val )

    def ledOn( self, idx ):
        if isinstance(idx, list):
            for led in dx:
                self.led( led, on=True )
        else:
            self.led( idx, on=True )

    def ledOff( self, idx ):
        if isinstance(idx, list):
            for led in dx:
                self.led( led, on=False )
        else:
            self.led( idx, on=False )

    def ledToggle( self, idx ):
        if isinstance(idx, list):
            for led in dx:
                self.led( led, toggle=True )
        else:
            self.led( idx, toggle=True )

    ledNumber = getLedNumber
    ledNum = getLedNumber
    ledSetVal = ledSetValue
    ledSet = ledOn
    ledClr = ledOff
    ledReset = ledOff
    ledClear = ledOff

    # raw gpio command
    def gpio( self, port, i=None, o=None, s=None, c=None, t=None ):
        '''gpio control'''
        cmd = 'gpio -p%s'% str(port)
        if i is not None:
            if type(i) is int:
                cmd += ' -i0x%x'% i
            elif type(i) is bool and i:
                cmd += ' -i'
        if o is not None:
            if type(o) is int:
                cmd += ' -o0x%x'% o
            elif type(o) is bool and o:
                cmd += ' -o'
        if s is not None:
            if type(s) is int:
                cmd += ' -s0x%x'% s
            elif type(s) is bool and s:
                cmd += ' -s'
        if c is not None:
            if type(c) is int:
                cmd += ' -c0x%x'% c 
            elif type(c) is bool and c:
                cmd += ' -c'
        if t is not None:
            if type(t) is int:
                cmd += ' -t0x%x'% t 
            elif type(t) is bool and t:
                cmd += ' -t'
        ret = self.writeCommand( cmd )
        if ret:
            return eval(ret[0])

    # some partial functions for gpio manipulating
    def pinInput( self, pin ):
        if isinstance(pin, list):
            for p in pin:
                self.gpio( p, i=True )
        else:
            self.gpio( pin, i=True )

    def pinOutput( self, pin ):
        if isinstance(pin, list):
            for p in pin:
                self.gpio( p, o=True )
        else:
            self.gpio( pin, o=True )

    def pinOutputHigh( self, pin ):
        if isinstance(pin, list):
            for p in pin:
                self.gpio( p, o=True, s=True )
        else:
            self.gpio( pin, o=True, s=True )

    def pinOutputLow( self, pin ):
        if isinstance(pin, list):
            for p in pin:
                self.gpio( p, o=True, c=True )
        else:
            self.gpio( pin, o=True, c=True )

    def pinSetVal( self, pin, val ):
        if isinstance(pin, list):
            for p in pin:
                if val:
                    self.gpio( p, s=True )
                else:
                    self.gpio( p, c=True )
        else:
            if val:
                self.gpio( pin, s=True )
            else:
                self.gpio( pin, c=True )

    pinSetValue = pinSetVal

    def pinSet( self, pin ):
        if isinstance(pin, list):
            for p in pin:
                self.gpio( p, s=True )
        else:
            self.gpio( pin, s=True )

    pinSetHigh = pinSet

    def pinClr( self, pin ):
        if isinstance(pin, list):
            for p in pin:
                self.gpio( p, c=True )
        else:
            self.gpio( pin, c=True )

    pinSetLow = pinClr
    pinReset = pinClr

    def pinToggle( self, pin ):
        if isinstance(pin, list):
            for p in pin:
                self.gpio( p, t=True )
        else:
            self.gpio( pin, t=True )

    def pinRead( self, pin ):
        if isinstance(pin, list):
            return [self.gpio(p) for p in pin]
        else:
            return self.gpio( pin )

    def pinIsHigh( self, pin ):
        return bool(self.gpio( pin ))

    pinIsSet = pinIsHigh

    def pinIsLow( self, pin ):
        return not bool(self.gpio( pin ))

    pinIsClear = pinIsLow
    pinIsReset = pinIsLow



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
        ret = Env.EMPTY_BYTE.join(m)
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
        except Instrument.CommandExecuteError:
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

    def mkbuf( self, value_list, float_mode=False ):
        cmd = 'mkbuf -f' if float_mode else 'mkbuf'
        self.setPrompts( self.DEFAULT_PROMPTS_MULTILINE )
        self.writeCommand( cmd )
        line = ''
        for v in value_list:
            if float_mode:
                line += '%f '% v
            else:
                line += '%d '% v
            if len(line) > self.DEFAULT_MULTILINE_INPUT_LINE_LIMIT:
                self.writeCommand( line.rstrip() )
                line = ''
        if line:
            self.writeCommand( line.rstrip() )
        self.setPrompts()
        r = Utils.parseKeyValueLines(self.writeCommand(''))
        return int(r['address'], 16)
            
 
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
        mem = Env.EMPTY_BYTE.join( \
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

    def errnoStop( self ):
        '''stop errno blink'''
        self.writeCommand( 'error -s' )
        
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
            cmd += ' -f%d'% freq
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

    def checkFileExist( self, pathname ):
        exist = False
        for p, f, s in self.list( pathname ):
            if s is not None:
                exist = True
        return exist

    def checkFileSize( self, pathname ):
        for p, f, s in self.list( pathname ):
            if s is not None:
                return s
        return None

    def convPathname( self, pathname ):
        if not Env.PYTHON_V3:
            pathname = unicode(pathname)
        else:
            pathname = str(pathname)
        namelen = len(os.path.basename(pathname).encode('utf8'))
        if namelen > 28:
            raise Exception( "File name length too long %d"% namelen )
        if pathname.find(' ') != -1:
            pathname = '\"' + pathname + '\"'
        return pathname

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
            ret = '\n'.join(self.writeCommand( cmd ))
            if b64:
                if Env.PYTHON_V3 and isinstance(ret, str):
                     ret = ret.encode('utf8')
                ret = base64.decodestring( ret )
            return ret

    def writeFile( self, pathname, buf='' ):
        self.cat( pathname, b64=True, write=True, append=False, buf=buf )

    def appendFile( self, pathname, buf='' ):
        self.cat( pathname, b64=True, write=True, append=True, buf=buf )

    def remove( self, pathname ):
        pathname = self.convPathname(pathname)
        cmd = 'rm ' + pathname
        self.writeCommand( cmd )

    def rename( self, old_pathname, new_name ):
        old_pathname = self.convPathname(old_pathname)
        new_name = self.convPathname(new_name)
        cmd = 'rename %s %s'% (old_pathname, new_name)
        self.writeCommand( cmd )

    def getFile( self, pathname, local_pathname ):
        # read remote_file and save locally
        dat = self.cat( pathname, b64=True )
        open( local_pathname, 'wb+' ).write(dat)

    def putFile( self, pathname, local_pathname, segment_size=512, segment_done_callback=None ):
        # split local_file into pieces and write
        dat = open(local_pathname, 'rb').read()
        dat_size = len(dat)
        if dat_size == 0:
            return
        dat_segments = int((dat_size-1) / segment_size) + 1
        # write the first
        d = dat[0:segment_size]
        self.cat( pathname, b64=True, write=True, buf=d )
        sent = len(d)
        if segment_done_callback:
            segment_done_callback(1, dat_segments, sent, dat_size)
        # append the remainings
        for i in range(1, dat_segments):
            d = dat[segment_size*i:segment_size*(i+1)]
            self.cat( pathname, b64=True, write=True, append=True, buf=d )
            sent += len(d) 
            if segment_done_callback:
                segment_done_callback(1+i, dat_segments, sent, dat_size)
 
    def spiffs( self, command, value=None, addr=None, compact_mode=None ):
        cmd = 'spiffs -c %s'% command
        if value is not None:
            cmd += ' %s'% value
        if addr is not None:
            cmd += ' -b 0x%X'% addr
        if compact_mode:
            cmd += ' --compact'
        return self.writeCommand( cmd )

    def spiffsID( self ):
        return self.spiffs('id')[0]

    def spiffsMount( self ):
        self.spiffs('mount')

    def spiffsUmount( self ):
        self.spiffs('umount')

    def spiffsRemount( self ):
        self.spiffs('remount')

    def spiffsFormat( self ):
        oldtimeout = self.setTimeout( 60 )
        self.spiffs( 'format' )
        self.setTimeout( oldtimeout )

    def spiffsErase( self, addr=None ):
        oldtimeout = self.setTimeout( 60 )
        if addr is not None:
            self.spiffs( 'erase', addr=addr )
        else:
            self.spiffs( 'erase' )
        self.setTimeout( oldtimeout )

    def spiffsInfo( self ):
        var = Utils.parseKeyValueLines( self.spiffs( 'info' ) )
        var['total'] = int(var['total'])
        var['used'] = int(var['used'])
        var['free'] = var['total'] - var['used']
        return var

    def spiffsReadPage( self, page, pagesize=256 ):
        compact_mode = True
        ret = self.spiffs( "read", addr=page*pagesize, compact_mode=compact_mode ) 
        mem = Env.EMPTY_BYTE.join( \
               [self.parseMemLine(line.strip(), compact_mode=compact_mode) for line in ret])
        return mem
 
    def spiffsWritePage( self, page, buf, pagesize=256 ):
        self.setPrompts( self.DEFAULT_PROMPTS_MULTILINE )
        ret = self.spiffs( "write", addr=page*pagesize ) 
        line = ''
        for item in buf:
            if isinstance(item, int):
                line += '%d '% item
            elif isinstance(item, str):
                line += '%d '% ord(item)
            if len(line) > self.DEFAULT_MULTILINE_INPUT_LINE_LIMIT:
                self.writeCommand( line.rstrip() )
                line = ''
        if line:
            self.writeCommand( line.rstrip() )
        self.setPrompts()
        self.writeCommand( '' )

    def sgpioStop( self ):
        self.writeCommand( 'sgpio --stop' )

    def sgpioStart( self ):
        self.writeCommand( 'sgpio --start' )

    def sgpioOutput( self, port, pins, buf, freq=10, loop=True, start=True ):
        self.sgpioStop()
        cmd = 'sgpio -p%s -o0x%X -f%d'% (int(port), int(pins), int(freq))
        if loop:
            cmd += ' -l'
        self.setPrompts( self.DEFAULT_PROMPTS_MULTILINE )
        self.writeCommand( cmd )
        line = ''
        for item in buf:
            line += '%d '% item
            if len(line) > self.DEFAULT_MULTILINE_INPUT_LINE_LIMIT:
                self.writeCommand( line.rstrip() )
                line = ''
        if line:
            self.writeCommand( line.rstrip() )
        self.setPrompts()
        self.writeCommand( '' )
        if start:
            self.sgpioStart()

    def sgpioInput( self, port, pins, freq, length ):
        self.sgpioStop()
        cmd = 'sgpio -p%s -i0x%X -f%d --input_len=%d'% (int(port), int(pins), int(freq), int(length))
        self.writeCommand( cmd )
        self.sgpioStart()
        while True:
            r = self.writeCommand( 'sgpio --info' )
            if r[0].startswith("stop"):
                addr = int(r[1].split('  ')[1].split(':')[-1], 16)
                return addr
            time.sleep(0.1)
 
    # emulated i2c bus control
    def i2c_init( self, addr, scl=None, sda=None, lsb=None, delay=None ):
        cmd = 'i2c -a0x%X -I' % (addr)
        if scl:
            cmd += ' --scl=%s'% scl
        if sda:
            cmd += ' --sda=%s'% sda
        if lsb:
            cmd += ' --lsb'
        if delay:
            cmd += ' --delay=%s'% delay
        self.writeCommand( cmd )

    i2cInit = i2c_init

    def i2c( self, write=[], read=None, addr=None, no_stop_bit=None ):
        cmd = 'i2c'
        if addr is not None:
            cmd += ' -a0x%X'% addr
        if read:
            cmd += ' -r%d'% read
        if no_stop_bit:
            cmd += ' -n'
        if write:
            cmd += ' %s'% (' '.join([str(i) for i in write])) 
        ret = self.writeCommand( cmd )
        if read:
            r = []
            for l in ret:
                for v in l.strip().split():
                    r.append(eval(v))
            return r
        else:
            return None

    # emulated spi bus control
    def spi_init( self, sdi=None, sdo=None, sck=None, cs=None, cpol=None, cpha=None, width=None, lsb=None, delay=None ):
        cmd = 'spi -I'
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
    
    def spi_update( self, sdi=None, sdo=None, sck=None, cs=None, cpol=None, cpha=None, width=None, lsb=None, delay=None ):
        cmd = 'spi -U'
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
 
    def spi_deinit( self ):
        self.writeCommand( 'spi -D' )
 
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
    
    def spi_write( self, write=[] ):
        if isinstance(write, list):
            return self.spi( write )
        else:
            return self.spi( [write] )

    def spi_read( self, write=[] ):
        if isinstance(write, list):
            return self.spi( write, read=True )
        else:
            return self.spi( [write], read=True )

    spiInit = spi_init
    spiUpdate = spi_update
    spiDeinit = spi_deinit
    spiWrite = spi_write
    spiRead = spi_read

    # emulated pulse generator
    def pulse_init( self, pin='0.0', delay_us=None, invert=None ):
        cmd = 'pulse -p%s -I'% pin
        if delay_us is not None:
            cmd += ' --delay=%d'% delay_us
        if invert:
            cmd += ' --invert'
        self.writeCommand( cmd )

    def pulse_deinit( self ):
        self.writeCommand( 'pulse -D' )
        
    def pulse( self, number ):
        self.writeCommand( 'pulse %d'% number )

    pulseInit = pulse_init
    pulseDeinit = pulse_deinit

    def pwm_init( self, freq=None, range_value=None, init_value=None ):
        cmd = 'pwm -I'
        if freq is not None:
            cmd += ' -f%s'% int(freq)
        if range_value is not None:
            cmd += ' -r%s'% int(range_value)
        if init_value is not None:
            cmd += ' -v%s'% int(init_value)
        self.writeCommand( cmd )

    pwmInit = pwm_init

    # hal layer pwm control
    def pwm( self, index=None, value=50 ):
        cmd = 'pwm'
        if index is not None:
            cmd += ' -i%d'% index
        cmd += ' -v%d'% value
        self.writeCommand( cmd )

    def adc_init( self ):
        self.writeCommand( "adc -I" )
    
    adcInit = adc_init

    def adc( self, channel=None ):
        if channel is None:
            ret = self.writeCommand( "adc" )
            return [float(v) for v in ret[0].split(',')]
        else:
            ret = self.writeCommand( "adc -i%d"% channel )
            return float(ret[0])

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

    def log( self, message=None, enable=None ):
        cmdline = 'log'
        if enable is not None:
            cmdline += ' -e' if enable else ' -d'
        if message is not None:
            cmdline += ' -m \"%s\"'% str(message)
        return self.writeCommand( cmdline )

    def logTail( self ):
        cmd = 'log -t'
        r = self.writeCommand( cmd )
        return '\n'.join(r[1:]) + '\n'

    def logEnter( self ):
        self.writeLine('log')
        self.port.read(4)

    def logExit( self ):
        self.port.write( self.DEFAULT_TERMINATOR_RESET )
        self.port.flush()
        self.readUntilPrompts()

    def logReadline( self ):
        return self.readLine()

    def ping( self, host, times=10, silent=True ):
        cmd = 'ping %s'% host
        self.writeLine(cmd)
        self.port.read(len(cmd)+1) 
        send, recv, ms = 0, 0, 0
        while True:
            line = self.readLine()
            if ('failed' in line) or ('timeout' in line):
                self.readUntilPrompts()
                return (0, 0, 0)
            if 'ping: send' in line:
                send += 1
                if send > times:
                    break
            elif 'ping: recv' in line:
                recv += 1
                # parse response line in format:
                # example: ping: recv 10.168.1.1  8 ms
                ms += int(line.split(' ')[-2])
                if send >= times:
                    break
            if not silent:
                print( line )
        if recv:
            ms /= recv
        self.port.write( self.DEFAULT_TERMINATOR_RESET )
        self.port.flush()
        self.readUntilPrompts()
        return (send, recv, ms)

    def netstat( self ):
        r = self.writeCommand( "netstat" )
        return Utils.parseKeyValueLines(r)

    def setNewIP( self, ip, netmask, gateway ):
        self.setPrompts( self.DEFAULT_PROMPTS_MULTILINE )
        self.writeCommand('netstat -c ip')
        self.writeCommand( str(ip) )
        self.writeCommand( str(netmask) )
        self.writeCommand( str(gateway) )
        self.setPrompts()
        self.writeCommand('')
        

    DEFAULT_TIMEOUT_UPRADE = 20
    def upgrade( self, upgrade_file='/s/upgrade.bin' ):
        command = 'upgrade -f %s'% upgrade_file
        try:
            self.setTimeout( self.DEFAULT_TIMEOUT_UPRADE )
            self.writeCommand( command )
        except Instrument.CommandTimeoutError:
            pass

    def wget( self, url, local_file, timeout=30 ):
        command = 'wget -u %s -f %s'% (url, local_file)
        oldtimeout = self.setTimeout( timeout )
        try:
            ret = self.writeCommand( command )
        except Instrument.CommandTimeoutError:
            self.port.write( self.DEFAULT_TERMINATOR_RESET )
            self.port.flush()
            self.readUntilPrompts()
            ret = None
        self.setTimeout( oldtimeout )
        if ret is None:
            return False
        if len(ret):
            if 'bytes saved' in ret[-1]:
                return True
        return False 

    def crc( self, pathname ):
        command = 'crc %s'% pathname 
        ret = self.writeCommand( command )
        return int(ret[0], 16)
        
 
    def luaReset( self ):
        self.writeCommand( 'lua -r' )

    def luaStop( self ):
        self.writeCommand( 'lua -s' )
 
    def luaCommand( self, cmd ):
        ret = self.writeCommand( 'lua -c "%s"'% cmd )
        return ret[:-1]
 
