# coding:utf8
__doc__ = 'Products from WitMotion Shenzhen Co.'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import re
import binascii
from struct import unpack, pack
import time
from .. import Env, Instrument


PACKET_TYPES = {
    0x50: 'time',
    0x51: 'acc',
    0x52: 'gyo',
    0x53: 'ang',
    0x54: 'mag',
    0x55: 'port',
    0x56: 'pressure_height',
    0x57: 'gps_lon_lat',
    0x58: 'gps_height_yaw_v',
    0x59: 'quat',
    0x5A: 'gps_info',
    }


REGS = {
    0x00: 'SAVE',
    0x01: 'CALSW',
    0x02: 'RSW',
    0x03: 'RATE',
    0x04: 'BAUD',
    0x05: 'AXOFFSET',
    0x06: 'AYOFFSET',
    0x07: 'AZOFFSET',
    0x08: 'GXOFFSET',
    0x09: 'GYOFFSET',
    0x0A: 'GZOFFSET',
    0x0B: 'HXOFFSET',
    0x0C: 'HYOFFSET',
    0x0D: 'HZOFFSET',
    0x0E: 'D0MODE',
    0x0F: 'D1MODE',
    0x10: 'D2MODE',
    0x11: 'D3MODE',
    0x12: 'D0PWMH',
    0x13: 'D1PWMH',
    0x14: 'D2PWMH',
    0x15: 'D3PWMH',
    0x16: 'D0PWMT',
    0x17: 'D1PWMT',
    0x18: 'D2PWMT',
    0x19: 'D3PWMT',
    0x1A: 'IICADDR',
    0x1B: 'LEDOFF',
    0x1C: 'GPSBAUD',
    0x30: 'YYMM',
    0x31: 'DDHH',
    0x32: 'MMSS',
    0x33: 'MS',
    0x34: 'AX',
    0x35: 'AY',
    0x36: 'AZ',
    0x37: 'GX',
    0x38: 'GY',
    0x39: 'GZ',
    0x3A: 'HX',
    0x3B: 'HY',
    0x3C: 'HZ',
    0x3D: 'Roll',
    0x3E: 'Pitch',
    0x3F: 'Yaw',
    0x40: 'TEMP',
    0x41: 'D0Status',
    0x42: 'D1Status',
    0x43: 'D2Status',
    0x44: 'D3Status',
    0x45: 'PressureL',
    0x46: 'PressureH',
    0x47: 'HeightL',
    0x48: 'HeightH',
    0x49: 'LonL',
    0x4A: 'LonH',
    0x4B: 'LatL',
    0x4C: 'LatH',
    0x4D: 'GPSHeight',
    0x4E: 'GPSYaw',
    0x4F: 'GPSVL',
    0x50: 'GPSVH',
    0x51: 'Q0',
    0x52: 'Q1',
    0x53: 'Q2',
    0x54: 'Q3',
    }
REGS_REV = {}
for k,v in REGS.items():
    REGS_REV[v] = k


class WitPacket():
    def __init__( self, raw_bytes ):
        self.data = raw_bytes
        #print(binascii.hexlify(raw_bytes))
        # NOTE: parse data lazily 

    def getType( self ):
        try:
            return self.type
        except AttributeError:
            pass
        if len(self.data) != 11:
            raise Exception('length error (%d)'% len(self.data))
        if self.data[0] != 0x55:
            print(self.data[0])
            raise Exception('SOF error')
        if self.data[1] in PACKET_TYPES:
            self.type = PACKET_TYPES[self.data[1]]
        else:
            self.type = 'unknown'
        return self.type

    def getData( self, data_type=None ):
        if data_type is None:
            data_type = self.getType()
        else:
            if data_type != self.getType():
                raise Exception('type dismatch')
        # checksum
        ck = sum(list(self.data[:10])) & 0xFF
        if ck != self.data[10]:
            raise Exception('packet checksum error 0x%02X (0x%02X required)'% (self.data[10], ck))
        dat = self.data[2:-1]
        if data_type == 'time':
            y,mon,d,h,m,s,ms = unpack(b'BBBBBBH', dat)
            return 2000+y,mon,d,h,m,s,ms
        elif data_type == 'acc':
            x,y,z,t = unpack(b'hhhh', dat)
            return x*16.0/32768, y*16.0/32768, z*16.0/32768, t/100.0
        elif data_type == 'gyo':
            x,y,z,t = unpack(b'hhhh', dat)
            return x*2000.0/32768, y*2000.0/32768, z*2000.0/32768, t/100.0
        elif data_type == 'ang':
            r,p,y,t = unpack(b'hhhh', dat)
            return r*180.0/32768, p*180.0/32768, y*180.0/32768, t/100.0
        elif data_type == 'mag':
            x,y,z,t = unpack(b'hhhh', dat)
            return x, y, z, t/100.0
        elif data_type == 'port':
            d0,d1,d2,d3 = unpack(b'hhhh', dat)
            #return d0*3.3/1024, d1*3.3/1024, d2*3.3/1024, d3*3.3/1024
            # NOTE: seems 12-bits instead of 10-bits (ref to module datasheet)
            return d0*3.3/4096, d1*3.3/4096, d2*3.3/4096, d3*3.3/4096
        elif data_type == 'pressure_height':
            p,h = unpack(b'II', dat)
            return p, h/100.0  # unit: pa, meter
        elif data_type == 'gps_lon_lat':
            lon,lat = unpack(b'II', dat)
            return lon/10000000.0, lat/10000000.0
        elif data_type == 'gps_height_yaw_v':
            h,y,v = unpack(b'hhI', dat)
            return h/10.0, y/100.0, v/1000.0  # unit: meter, degree, km/h
        elif data_type == 'quat':
            q0,q1,q2,q3 = unpack(b'hhhh', dat)
            return q0/32768.0, q1/32768.0, q2/32768.0, q3/32768.0 
        elif data_type == 'gps_info':
            sn, pdop, hdop, vdop = unpack(b'HHHH', dat)
            return sn, pdop/100.0, hdop/100.0, vdop/100.0
        else: 
            return None

    def getTime( self ):
        return self.getData( 'time' )

    def getAcc( self ):
        return self.getData( 'acc' )

    def getGyo( self ):
        return self.getData( 'gyo' )

    def getMag( self ):
        return self.getData( 'mag' )

    def getAng( self ):
        return self.getData( 'ang' )




class JY901( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'JY901'
    DEFAULT_TERMINAL_RESET = False
    DEFAULT_READ_UNTIL_PROMPTS = False
    DEFAULT_CHECK_IDN = False
    DEFAULT_CHECK_RETURN_COMMAND = False

    pending_bytes = []
    read_bytes = 0
    synced = None

    def __init__( self, *args, **kwargs ):
        kwargs['timeout'] = 0.2  # to be more real-time
        Instrument.SerialInstrument.__init__( self, *args, **kwargs ) 

    def readPackets( self, packets_limit=None ):
        '''read and split into packets until timeout'''
        packets = []
        while True:
            if self.port.ser.in_waiting < 11:
                return packets  # buffered packets not ready
            byte = self.port.read(1)
            if not byte:
                return packets  # timeout, stop
            if not self.synced:
                if byte == b'\x55':
                    self.synced = True
                    self.pending_bytes = [byte]
                    self.read_bytes = 1
            else:
                #print(self.read_bytes, self.pending_bytes)
                if (self.read_bytes == 1) and (not ord(byte) in PACKET_TYPES):
                    self.synced = False
                    self.pending_bytes = []
                    self.read_bytes = 0
                else:
                    self.pending_bytes.append(byte)
                    self.read_bytes += 1
                    if self.read_bytes == 11:
                        packets.append(WitPacket(b''.join(self.pending_bytes)))
                        self.pending_bytes = []
                        self.read_bytes = 0
                        self.synced = False
                        if packets_limit is not None:
                            if len(packets) >= packets_limit:
                                return packets
           
    def lock( self ):
        pkt = bytes([0xFF, 0xAA, 0x69, 0x77, 0xA5])
        self.port.write(pkt)
        time.sleep(0.1)

    def unlock( self ):
        pkt = bytes([0xFF, 0xAA, 0x69, 0x88, 0xB5])
        self.port.write(pkt)
        time.sleep(0.1)

    def writeReg( self, addr, val ):
        if isinstance(addr, str):
            addr = REGS_REV[addr]
        pkt = bytes([0xFF, 0xAA, addr, val&0xFF, (val>>8)&0xFF])
        self.unlock()
        self.port.write(pkt)
        time.sleep(0.1)

    def save( self, load_default=False ):
        self.writeReg('SAVE', int(load_default) )

    def loadDefault( self ):
        self.writeReg('SAVE', 1 )

    def calsw( self, mode=0 ):
        self.writeReg('CALSW', mode ) 

    def setRSW( self, time=False, acc=True, gyo=True, ang=True, mag=True, port=False, pressure_height=False, gps_lon_lat=False, gps_height_yaw_v=False, quat=False, gps_info=False ):
        val = 0
        if time:
            val |= 1<<0
        if acc:
            val |= 1<<1
        if gyo:
            val |= 1<<2
        if ang:
            val |= 1<<3
        if mag:
            val |= 1<<4
        if port:
            val |= 1<<5
        if pressure_height:
            val |= 1<<6
        if gps_lon_lat:
            val |= 1<<7
        if gps_height_yaw_v:
            val |= 1<<8
        if quat:
            val |= 1<<9
        if gps_info:
            val |= 1<<10
        self.writeReg('RSW', val ) 
    
    def setRate( self, rate=0x06 ):
        self.writeReg('RATE', rate ) 

    def setBaud( self, baud=0x02 ):
        self.writeReg('BAUD', baud  ) 
   
    def syncRTC( self ):
        tm = time.localtime()
        self.writeReg('YYMM', (tm.tm_year%100)+(tm.tm_mon<<8) )
        self.writeReg('DDHH', (tm.tm_mday)+(tm.tm_hour<<8) )
        self.writeReg('MMSS', (tm.tm_min)+(tm.tm_sec<<8) )

    def setAccOffset( self, x, y, z ):
        self.writeReg('AXOFFSET', x )
        self.writeReg('AYOFFSET', y )
        self.writeReg('AZOFFSET', z )

    def setGyoOffset( self, x, y, z ):
        self.writeReg('GXOFFSET', x )
        self.writeReg('GYOFFSET', y )
        self.writeReg('GZOFFSET', z )

    def setMagOffset( self, x, y, z ):
        self.writeReg('HXOFFSET', x )
        self.writeReg('HYOFFSET', y )
        self.writeReg('HZOFFSET', z )
    
    def setPort( self, index, mode=0 ):
        self.writeReg('D%dMODE'%index, mode)

    def setPWM( self, index, high=None, low=None, total=None ):
        if high is not None:
            self.writeReg('D%dPWMH'%index, high)
        if low is not None:
            self.writeReg('D%dPWML'%index, low)
        if total is not None:
            self.writeReg('D%dPWMT'%index, total)

    def setIIC( self, addr ):
        self.writeReg('IICADDR', addr)

    def setLED( self, on ):
        self.writeReg('LEDOFF', int(not on))
            
    def setGPSBaud( self, baud=0x02 ):
        self.writeReg('GPSBAUD', baud)



