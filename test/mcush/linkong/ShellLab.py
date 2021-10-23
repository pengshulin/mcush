# coding:utf8
__doc__ = 'Shell Lab Series'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Mcush, Instrument, Utils, Env
import binascii
import time


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
        command = 'daq'
        if cmd is not None:
            command += ' -c %s'% cmd
        if index is not None:
            command += ' -i %d'% index
        if value is not None:
            command += ' -v %d'% value
        return self.writeCommand( command )

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

    def measure( self, cmd=None, index=None, value=None, fvalue=None ):
        command = 'measure'
        if cmd is not None:
            command += ' -c %s'% cmd
        if index is not None:
            command += ' -i %d'% index
        if value is not None:
            command += ' -v %d'% value
        if fvalue is not None:
            command += ' -V %f'% fvalue
        return self.writeCommand( command )

    def measure_start( self ):
        self.measure( 'start' )

    def measure_stop( self ):
        self.measure( 'stop' )

    def measure_read( self, channel ):
        ret = self.measure( 'read', index=channel )
        dat = []
        for l in ret:
            for v in l.strip().split(','):
                dat.append( float(v) )
        return dat

    measureStart = measure_start
    measureStop = measure_stop
    measureRead = measure_read


COLOR_TAB = {
'black'  : 0x000000,
'red'    : 0xFF0000,
'green'  : 0x00FF00,
'blue'   : 0x0000FF,
'yellow' : 0xFFFF00,
'cyan'   : 0x00FFFF,
'purple' : 0xFF00FF,
'white'  : 0xFFFFFF,
}


class ShellLabLamp(Mcush.Mcush):
    DEFAULT_NAME = 'ShellLabLamp'
    DEFAULT_IDN = re_compile( 'ShellLab-L1[a-zA-Z]*,([0-9]+\.[0-9]+.*)' )

    def lamp( self, color=None, red=None, green=None, blue=None, freq=None, mode=None, count=None ):
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
            cmd += ' -f %s'% freq
        if mode is not None:
            cmd += ' -m %d'% mode
        if count is not None:
            cmd += ' -C %d'% count
        return self.writeCommand(cmd)

    def alarm( self, count=None, freq=None ):
        cmd = 'alarm'
        if count is not None:
            cmd += ' -c %d'% count
        if freq is not None:
            cmd += ' -f %d'% freq
        return self.writeCommand(cmd)

    def reset( self, lamp_freq=1, alarm_freq=1, mode=None ):
        self.lamp( 0, freq=lamp_freq, mode=mode )
        self.alarm( 0, freq=alarm_freq )

    def mode( self, mode ):
        if isinstance(mode, str):
            if mode in ['blink', 'reset']:
                mode = 0
            elif mode in ['clock', 'clockwise', 'rotate']:
                mode = 1
            elif mode in ['anti', 'anticlockwise']:
                mode = 2
            else:
                raise Exception('Unknown mode name %s'% mode)
        self.lamp(mode=mode)

    def color( self, c, freq=None, count=None ):
        if isinstance(c, str):
            if not c in COLOR_TAB:
                raise Exception('Unknown color name %s'% c)
            c = COLOR_TAB[c]
        self.lamp( c, freq=freq, count=count ) 


class ShellLabStrip(Mcush.Mcush):
    DEFAULT_NAME = 'ShellLabStrip'
    DEFAULT_IDN = re_compile( 'ShellLab-L2[a-zA-Z]*,([0-9]+\.[0-9]+.*)' )

    def __init__( self, *args, **kwargs ):
        Instrument.Instrument.__init__( self, *args, **kwargs )
        self.length = kwargs.get('length', None)
        if self.length is not None:
            self.strapLength( self.length ) 

    def strapLength( self, length ):
        cmd = 'strap -l%d'% length
        self.writeCommand(cmd)

    def strap( self, color=None, red=None, green=None, blue=None, freq=None, count=None ):
        cmd = 'strap'
        if color is not None:
            cmd += ' -c 0x%X'% color
        if red is not None:
            cmd += ' -r %d'% red
        if green is not None:
            cmd += ' -g %d'% green
        if blue is not None:
            cmd += ' -b %d'% blue
        if freq is not None:
            cmd += ' -f %s'% freq
        if count is not None:
            cmd += ' -C %d'% count
        return self.writeCommand(cmd)

    strip = strap

    def reset( self, freq=1 ):
        self.strap( 0, freq=freq )

    def color( self, c, freq=None, count=None ):
        if isinstance(c, str):
            if not c in COLOR_TAB:
                raise Exception('Unknown color name %s'% c)
            c = COLOR_TAB[c]
        self.strap( c, freq=freq, count=count ) 

    def beep( self, freq=None, duration=0.05, times=1 ):
        # not supported
        pass

    
class ShellLabStaticStrip(ShellLabStrip):
    def __init__( self, *args, **kwargs ):
        ShellLabStrip.__init__(self, *args, **kwargs)
        setting = Utils.parseKeyValueLines(self.strap())
        if float(setting['freq']) > 0:
            self.strap( freq=0 )
        self._color = int(setting['color'], 16)
        self._brightness = kwargs.get('brightness', 1.0)
        
    def reset( self ):
        self.setColor( 0 )

    def calcDimColor( self, c ):
        r, g, b = (c>>16)&0xFF, (c>>8)&0xFF, c&0xFF
        r = int(r*self._brightness)
        g = int(g*self._brightness)
        b = int(b*self._brightness)
        return ((r&0xFF)<<16)|((g&0xFF)<<8)|(b&0xFF)

    def color( self, c, brightness=None ):
        self._color = c
        if brightness is not None:
            self._brightness = brightness
        if c in COLOR_TAB:
            c = COLOR_TAB[c]
        self.strap(self.calcDimColor(c))

    def brightness( self, b ):
        self._brightness = b
        c = self._color
        if c in COLOR_TAB:
            c = COLOR_TAB[c]
        self.strap(self.calcDimColor(c))
            
    setColor = color
    setBrightness = brightness

# for compatibility
ShellLabStrap = ShellLabStrip
ShellLabStaticStrap = ShellLabStaticStrip


class ShellLabSpotLight(Mcush.Mcush):
    DEFAULT_NAME = 'ShellLabSpotLight'
    DEFAULT_IDN = re_compile( 'ShellLab-SpotLight,([0-9]+\.[0-9]+.*)' )

    def light( self, index=None, power=None, freq=None, count=None, reset=None, sync=None ):
        cmd = 's'
        if index is not None:
            cmd += ' -i %d'% index
        if power is not None:
            cmd += ' -p %d'% power
        if freq is not None:
            cmd += ' -f %s'% freq
        if count is not None:
            cmd += ' -c %d'% count
        if reset:
            cmd += ' -r'
        if sync:
            cmd += ' -s'
        return self.writeCommand(cmd)

    def reset( self ):
        self.light( reset=True )

    def sync( self ):
        self.light( sync=True )


class ShellLabTowerLamp(ShellLabSpotLight):
    # TODO: optimize tower lamp APIs 
    pass


class ShellLabCAN(ShellLab):
    DEFAULT_NAME = 'ShellLabCAN'
    DEFAULT_IDN = re_compile( 'ShellLab-C[0-9a-zA-Z\-]*,([0-9]+\.[0-9]+.*)' )

# CANopen function codes
NMT   = 0x0<<7
SYNC  = 0x1<<7
TIME  = 0x2<<7
TPDO1 = 0x3<<7
RPDO1 = 0x4<<7
TPDO2 = 0x5<<7
RPDO2 = 0x6<<7
TPDO3 = 0x7<<7
RPDO3 = 0x8<<7
TPDO4 = 0x9<<7
RPDO4 = 0xA<<7
TSDO  = 0xB<<7
RSDO  = 0xC<<7
NDGRD = 0xE<<7
LSS   = 0xF<<7
# NMT commands
NMT_START = 1
NMT_STOP = 2
NMT_PRE_OPER = 0x80
NMT_RESET = 0x81
NMT_RESET_COMM = 0x82
# NODE status
STATUS_BOOT_UP = 0
STATUS_CONNECTING = 2
STATUS_STOPPED = 4
STATUS_OPERATIONAL = 5
STATUS_PRE_OPERATIONAL = 0x7F

def _v2l( val ):
    if isinstance(val, list):
        return val
    else:
        return map(ord,val)[:8]

class ShellLabCANopen(ShellLabCAN):
    DEFAULT_SDO_TIMEOUT = 5

    # SYNC command
    def sync( self ):
        self.canWrite( SYNC )

    # NODE_GUARD command
    def writeNodeGuardRequest( self, id ):
        self.canWrite( NDGRD+(id&0x7F), rtr=True )

    # NMT command
    def writeNMTCommand( self, cmd, id ):
        self.canWrite( NMT, [cmd, id&0x7F] )
        
    def resetNode( self, id ):
        self.writeNMTCommand( NMT_RESET, id )

    def startNode( self, id ):
        self.writeNMTCommand( NMT_START, id )

    def stopNode( self, id ):
        self.writeNMTCommand( NMT_STOP, id )

    def preOperNode( self, id ):
        self.writeNMTCommand( NMT_PRE_OPER, id )

    # RPDO commands 
    def writeRPDO1( self, id, val ):
        self.canWrite( RPDO1+(id&0x7F), _v2l(val) )

    def writeRPDO2( self, id, val ):
        self.canWrite( RPDO2+(id&0x7F), _v2l(val) )

    def writeRPDO3( self, id, val ):
        self.canWrite( RPDO3+(id&0x7F), _v2l(val) )

    def writeRPDO4( self, id, val ):
        self.canWrite( RPDO4+(id&0x7F), _v2l(val) )

    # TPDO commands 
    def writeTPDO1Request( self, id ):
        self.canWrite( TPDO1+(id&0x7F), rtr=True  )

    def writeTPDO2Request( self, id ):
        self.canWrite( TPDO2+(id&0x7F), rtr=True  )

    def writeTPDO3Request( self, id ):
        self.canWrite( TPDO3+(id&0x7F), rtr=True  )

    def writeTPDO4Request( self, id ):
        self.canWrite( TPDO4+(id&0x7F), rtr=True  )

    # Object dictionaries
    def readObject( self, id, index, subindex ):
        self.canWrite( RSDO+id, [0x40, index&0xFF, (index>>8)&0xFF, subindex, 0, 0, 0, 0] )
        bytes_required = None
        t0 = time.time()
        responsed = None
        while not responsed:
            if time.time() > t0 + self.DEFAULT_SDO_TIMEOUT:
                raise Exception("SDO upload request timeout")
            for cid, ext, rtr, dat in self.canRead(block_ms=0):
                #self.logger.debug( 'id=0x%X, ext=%d, rtr=%d, dat=%s'% (cid, ext, rtr, binascii.hexlify(dat)) )
                if (cid != TSDO+id) or rtr or (len(dat) != 8):
                    continue
                if (Utils.s2H(dat[1:3]) != index) or (Utils.s2B(dat[3]) != subindex):
                    continue
                m = Utils.s2B(dat[0])
                transfer_type = bool(m & 0x02)
                size_indicator = bool(m & 0x01)
                #self.logger.debug( 'transfer_type: %d  size_indicator: %d'% (transfer_type, size_indicator) )
                if transfer_type:
                    # expedited
                    not_used = (m>>2)&3
                    #self.logger.debug( 'SDO read %d bytes'% (4-not_used) )
                    if size_indicator and not_used:
                        return dat[4:-not_used]
                    else:
                        return dat[4:]
                else:
                    # normal
                    if size_indicator:
                        bytes_required = Utils.s2I(dat[4:])
                    else:
                        bytes_required = 0xFFFFFF  # unknown length
                    #self.logger.debug( 'SDO bytes_required=%X'% (bytes_required) )
                    responsed = True
                    break
        if not bytes_required:
            return
        # continue to read
        read = []
        read_bytes = 0
        control = 0x60
        while read_bytes < bytes_required:
            #self.logger.debug( 'SDO Segment Req control=%X'% (control) )
            self.canWrite( RSDO+id, [control, 0, 0, 0, 0, 0, 0, 0] )
            t0 = time.time()
            responsed = None
            while not responsed:
                if time.time() > t0 + self.DEFAULT_SDO_TIMEOUT:
                    raise Exception("SDO upload segment Timeout")
                for cid, ext, rtr, dat in self.canRead(block_ms=0):
                    #self.logger.debug( 'SDO Segment id=0x%X, ext=%d, rtr=%d, dat=%s'% (cid, ext, rtr, binascii.hexlify(dat)) )
                    if (cid==(TSDO+id)) and (not rtr) and (len(dat)==8):
                        m = Utils.s2B(dat[0])
                        if m & 0x80:
                            raise Exception("SDO Abort")
                        not_used = (m>>1)&7
                        no_more = m&1
                        if not_used:
                            read.append( dat[1:-not_used] )
                        else:
                            read.append( dat[1:] )
                        read_bytes += 7-not_used
                        if no_more:
                            bytes_required = read_bytes
                        #self.logger.debug( 'SDO read_len=%s, no_more=%d'% (read_bytes, no_more) )
                        responsed = True
                        break
            control ^= 0x10
        return Env.EMPTY_BYTE.join(read)
     
    def writeObject( self, id, index, subindex, val ):
        val_len = len(val)
        control = 0x20
        bytes_written = 0
        if val_len <= 4:
            # expedited mode
            control |= 0x02
            if Env.PYTHON_V3:
                v = list(val)
            else:
                v = map(ord, val)
            if len(v) < 4:
                control |= 0x01  # indicated 
                control |= (4-len(v)) << 2 # not used bytes
                while len(v) < 4:
                    v.append( 0 )
            self.canWrite( RSDO+id, [control, index&0xFF, (index>>8)&0xFF, subindex] + v )
            bytes_written = val_len
        else:
            # normal mode
            control |= 0x01  # indicated 
            self.canWrite( RSDO+id, [control, index&0xFF, (index>>8)&0xFF, subindex, 
                           val_len&0xFF, (val_len>>8)&0xFF, (val_len>>16)&0xFF, 0] )
        # initial download response
        t0 = time.time()
        responsed = None
        while not responsed:
            if time.time() > t0 + self.DEFAULT_SDO_TIMEOUT:
                raise Exception("SDO download request timeout")
            for cid, ext, rtr, dat in self.canRead(block_ms=0):
                #self.logger.debug( 'id=0x%X, ext=%d, rtr=%d, dat=%s'% (cid, ext, rtr, binascii.hexlify(dat)) )
                if (cid!=TSDO+id) or rtr or (len(dat)!=8):
                    continue
                m = Utils.s2B(dat[0])
                if m & 0x80:
                    raise Exception("SDO Abort")
                if (Utils.s2H(dat[1:3]) != index) or (Utils.s2B(dat[3]) != subindex):
                    continue
                responsed = True
                break
        # remaining segment request
        control = 0x00
        while bytes_written < val_len:
            if Env.PYTHON_V3:
                v = list(val[bytes_written:bytes_written+7])
            else:
                v = map(ord, val[bytes_written:bytes_written+7])
            bytes_written += len(v)
            if bytes_written >= val_len:
                control |= 0x01  # no more flag
            if len(v) < 7:
                control |= (7-len(v))<<1  # not used bytes
                while len(v) < 7:
                    v.append( 0 )
            self.canWrite( RSDO+id, [control] + v )
            t0 = time.time()
            responsed = None
            while not responsed:
                if time.time() > t0 + self.DEFAULT_SDO_TIMEOUT:
                    raise Exception("SDO download segment timeout")
                for cid, ext, rtr, dat in self.canRead(block_ms=0):
                    #self.logger.debug( 'id=0x%X, ext=%d, rtr=%d, dat=%s'% (cid, ext, rtr, binascii.hexlify(dat)) )
                    if (cid!=TSDO+id) or rtr or (len(dat)!=8):
                        continue
                    m = Utils.s2B(dat[0])
                    if m & 0x80:
                        raise Exception("SDO Abort")
                    responsed = True
                    break
            control ^= 0x10
        
    def readUINT32( self, id, index, subindex ):
        val = self.readObject( id, index, subindex )
        return Utils.s2I(val[:4])

    def readINT32( self, id, index, subindex ):
        val = self.readObject( id, index, subindex )
        return Utils.s2i(val[:4])

    def readUINT16( self, id, index, subindex ):
        val = self.readObject( id, index, subindex )
        return Utils.s2H(val[:2])

    def readINT16( self, id, index, subindex ):
        val = self.readObject( id, index, subindex )
        return Utils.s2h(val[:2])

    def readUINT8( self, id, index, subindex ):
        val = self.readObject( id, index, subindex )
        return Utils.s2B(val[:1])

    def readINT8( self, id, index, subindex ):
        val = self.readObject( id, index, subindex )
        return Utils.s2b(val[:1])

    def readFLOAT32( self, id, index, subindex ):
        val = self.readObject( id, index, subindex )
        return Utils.s2f(val[:4])

    def writeUINT32( self, id, index, subindex, val ):
        dat = Utils.I2s(val)
        self.writeObject( id, index, subindex, dat )

    def writeINT32( self, id, index, subindex, val ):
        dat = Utils.i2s(val)
        self.writeObject( id, index, subindex, dat )

    def writeUINT16( self, id, index, subindex, val ):
        dat = Utils.H2s(val)
        self.writeObject( id, index, subindex, dat )

    def writeINT16( self, id, index, subindex, val ):
        dat = Utils.h2s(val)
        self.writeObject( id, index, subindex, dat )

    def writeUINT8( self, id, index, subindex, val ):
        dat = Utils.B2s(val)
        self.writeObject( id, index, subindex, dat )

    def writeINT8( self, id, index, subindex, val ):
        dat = Utils.b2s(val)
        self.writeObject( id, index, subindex, dat )

    def writeFLOAT32( self, id, index, subindex, val ):
        dat = Utils.f2s(val)
        self.writeObject( id, index, subindex, dat )




class ShellLabStepperMotor(Mcush.Mcush):
    DEFAULT_NAME = 'ShellLabStepperMotor'
    DEFAULT_IDN = re_compile( 'ShellLab(-[A-Z][0-9a-zA-Z\-]*)?,([0-9]+\.[0-9]+.*)' )

    def motor( self, cmd, index=None, value=None ):
        cmd = 'M -c%s'% cmd
        if index is not None:
            cmd += ' -i%d'% index
        if value is not None:
            cmd += ' -v%s'% value
        return self.writeCommand( cmd )

    def start( self, index=None ):
        self.motor( 'start', index=index )

    def stop( self, index=None ):
        self.motor( 'stop', index=index )

    def isStopped( self, index=None ):
        return bool(int(self.motor('stopped', index=index)[0]))

    def isStable( self, index=None ):
        return bool(int(self.motor('stable', index=index)[0]))

    def setPPR( self, ppr, index=None ):
        self.motor( 'ppr', index=index, value=ppr )

    def setSpeed( self, speed, index=None ):
        self.motor( 'speed', index=index, value=speed )

    def setDir( self, direction, index=None ):
        self.motor( 'dir', index=index, value=direction )

    def setFree( self, free, index=None ):
        self.motor( 'free', index=index, value=free )

    def waitForStable( self, index=None ):
        while True:
            if self.isStable(index=index):
                break
            time.sleep(0.1)

    def waitForStopped( self, index=None ):
        while True:
            if self.isStopped(index=index):
                break
            time.sleep(0.1)


class ShellLabSegmentDisplay(Mcush.Mcush):
    DEFAULT_NAME = 'ShellLabSegmentDisplay'
    DEFAULT_IDN = re_compile( 'ShellLab-L3[a-zA-Z]*,([0-9]+\.[0-9]+.*)' )

    def __init__( self, *args, **kwargs ):
        Instrument.Instrument.__init__( self, *args, **kwargs )
        self.digits = int(self.disp('digits')[0])

    def disp( self, cmd=None, idx=None, val=None, msg=None ):
        command = 'd'
        if cmd is not None:
            command += ' -c%s'% cmd
        if idx is not None:
            command += ' -i%d'% idx
        if val is not None:
            command += ' -v%d'% val
        if msg is not None:
            command += ' -m"%s"'% msg
        return self.writeCommand( command ) 

    def brightness(self, new_brightness=7):
        self.disp('brightness', val=new_brightness)

    def clrscr(self):
        self.disp('clrscr')

    def blink(self, on, idx=None):
        self.disp('blink', idx=idx, val=on)

    def raw(self, val, idx):
        self.disp('raw', idx=idx, val=val)
   
    def hex(self, val, idx):
        self.disp('hex', idx=idx, val=val)

    def ascii(self, val, idx):
        self.disp('ascii', idx=idx, val=val)

    def int(self, val, idx=None):
        self.disp('int', idx=idx, val=val)

    def msg(self, msg, idx=None):
        self.disp(idx=idx, msg=msg)

    def dispString(self, string, align='left'):
        if align == 'right':
            msg_len = len(string.replace('.','').replace(':',''))
            self.msg( string, idx=self.digits-msg_len )
        else:
            self.msg( string )

    def dispInteger(self, val, align='right'):
        self.dispString( '%d'% val, align=align )

    def dispHex(self, val, align='right'):
        self.dispString( '%X'% val, align=align )

    def dispFloat(self, val, precesion=1, align='right'):
        fmt = '%%.%df'% precesion
        output_str = fmt% val
        if align == 'right':
            output_len = len(output_str)-1
            if output_len <= self.digits:
                self.dispString( output_str, align='right' )
            else:
                self.dispString( output_str, align='left' )
        else:
            self.dispString( output_str, align='left' )


class ShellLabSensor(Mcush.Mcush):
    DEFAULT_NAME = 'ShellLab-Sensor'
    DEFAULT_IDN = re_compile( 'ShellLab-Sensor-[A-Z][0-9a-zA-Z]*,([0-9]+\.[0-9]+.*)' )

    def measure( self, cmd=None, idx=None, val=None ):
        command = 'm'
        if cmd is not None:
            command += ' -c%s'% cmd
        if idx is not None:
            command += ' -i%d'% idx
        if val is not None:
            command += ' -v%d'% val
        return self.writeCommand( command ) 


class ShellLabSensorT(ShellLabSensor):
    '''DS18B20 based temperature sensor'''
    DEFAULT_NAME = 'ShellLab-Sensor-T'
    DEFAULT_IDN = re_compile( 'ShellLab-Sensor-T,([0-9]+\.[0-9]+.*)' )

    def getT( self ):
        ret = self.measure()
        k,v = ret[0].split(':')
        if k == 'T':
            return float(v)
        return None 


class ShellLabSensorTH(ShellLabSensor):
    '''SHT30 based temperature/humidity sensor'''
    DEFAULT_NAME = 'ShellLab-Sensor-TH'
    DEFAULT_IDN = re_compile( 'ShellLab-Sensor-TH,([0-9]+\.[0-9]+.*)' )

    def configMPS( self, mps=0.5 ):
        if mps >= 10.0:
            val = 4
        elif mps > 4.0:
            val = 3
        elif mps > 2.0:
            val = 2
        elif mps > 1.0:
            val = 1
        else:
            val = 0
        self.measure( 'mps', val=val )
    
    def configRepeatability( self, repeatability='high' ):
        if repeatability == 'low':
            val = 2
        elif repeatability == 'medium':
            val = 1
        else:
            val = 0
        self.measure( 'repeatability', val=val )

    def getTH( self ):
        ret = self.measure()
        a, b = ret[0].split(' ')
        k1,v1 = a.split(':')
        k2,v2 = b.split(':')
        if k1 == 'T' and k2 == 'H':
            return (float(v1), float(v2))
        return None 


class ShellLabSensorEncoder(ShellLabSensor):
    '''Rotary encoder'''
    DEFAULT_NAME = 'ShellLab-Sensor-Encoder'
    DEFAULT_IDN = re_compile( 'ShellLab-Sensor-Encoder,([0-9]+\.[0-9]+.*)' )

    def setPos( self, pos ):
        self.measure('pos', val=pos)

    def getPos( self ):
        return float(self.measure('pos')[0])

    def getSpeed( self ):
        return float(self.measure('speed')[0])

    def getVal( self ):
        pos, val = self.measure()[0].split()
        return float(pos), float(val)


class ShellLabSensorPH(ShellLabSensor):
    '''Acid/alkali PH sensor'''
    DEFAULT_NAME = 'ShellLab-Sensor-PH'
    DEFAULT_IDN = re_compile( 'ShellLab-Sensor-PH,([0-9]+\.[0-9]+.*)' )

    def getPH( self ):
        return None 


class ShellLabSensorPressure(ShellLabSensor):
    '''Pressure sensor'''
    DEFAULT_NAME = 'ShellLab-Sensor-Pressure'
    DEFAULT_IDN = re_compile( 'ShellLab-Sensor-Pressure,([0-9]+\.[0-9]+.*)' )

    def getPressure( self ):
        return None 


class ShellLabSensorWeight(ShellLabSensor):
    '''Weight sensor'''
    DEFAULT_NAME = 'ShellLab-Sensor-Weight'
    DEFAULT_IDN = re_compile( 'ShellLab-Sensor-Weight,([0-9]+\.[0-9]+.*)' )

    def getWeight( self ):
        return None 


