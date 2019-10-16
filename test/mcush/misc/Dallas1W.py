# coding: utf8
__doc__ = 'Dallas 1-wire bus controller'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils
import crcmod
import crcmod.predefined


class Dallas1W():
        
    def __init__( self, controller, pin=None ):
        self.controller = controller
        self.crc = crcmod.predefined.mkPredefinedCrcFun('crc-8-maxim')
        if pin:
            self.controller.writeCommand('ds1w -I -p%s'% pin)
        else:
            self.controller.writeCommand('ds1w -I')

    def readBit( self ):
        r = self.controller.writeCommand('ds1w -z')
        return int(r[0])

    def read( self, num=1 ):
        if num == 1:
            return int(self.controller.writeCommand('ds1w -r')[0])
        else:
            r = []
            for i in range(num):
                r.append( int(self.controller.writeCommand('ds1w -r')[0]) )
            return r

    def write0( self ):
        self.controller.writeCommand('ds1w -0') 

    def write1( self ):
        self.controller.writeCommand('ds1w -1') 

    def writeBit( self, bit ):
        if bit:
            self.write1()
        else:
            self.write0()

    def write( self, byte ):
        self.controller.writeCommand('ds1w -w%d'% byte) 

    def reset( self ):
        self.controller.writeCommand('ds1w -R') 

    def scan(self):
        result = []
        path = []
        branch = []
        while True:
            self.reset()
            self.write( 0xF0 )
            for b in path:
                self.readBit()
                self.readBit()
                self.writeBit(b)
            depth = len(path)
            for i in range(len(path), 64):
                a = self.readBit()
                b = self.readBit()
                if (a==0) and (b==0):  # both 0/1
                    branch.append(i)
                    path.append(0)
                    self.writeBit(0)  # search the left branch
                    depth += 1
                elif (a==0) or (b==0):  # only 0/1
                    path.append(a)
                    self.writeBit(a)
                    depth += 1
                else:  # no device
                    break
            if depth == 64:  # found
                r = []
                for i in range(8):
                    v = 0
                    for j in range(8):
                        if path[i*8+j]:
                            v |= 1<<j
                    r.append(v)
                result.append(r)
            if branch:
                b = branch.pop()
                path = path[:b]
                path.append(1)  # search the right branch
            else:  # stop
                break
        # sn filter 
        sn = []
        for r in result:
            if r[0] != self.FAMILY_CODE:
                continue
            crc = self.crc(''.join([chr(i) for i in r[:-1]]))
            if crc == r[-1]:
                sn.append( r[1:-1] )
                sn[-1].reverse()
        return [''.join(['%02X'%i for i in j]) for j in sn]
                 
    def readRom( self ):
        # ignore multi-slave, read only the first one
        self.reset()
        self.write( 0x33 )
        r = self.read(8)
        crc_calc = self.crc(''.join([chr(i) for i in r[:7]]))
        if r[7] != crc_calc:
            code = '-'.join(['%02X'%i for i in r])
            raise Exception("Crc error:%s"% code )
        if r[0] != self.FAMILY_CODE:
            raise Exception("Family code (0x%X) not match: 0x%X"% (self.FAMILY_CODE, r[0]))
        serial_num = r[1:7]
        serial_num.reverse()
        return ''.join(['%02X'%i for i in serial_num])

    getSerialNumber = readRom

    def scanSerialNumber( self ):
        return self.scan()

    def writeROM( self, sn ):
        self.write( self.FAMILY_CODE )
        sn2i = [int('0x'+sn[i*2:i*2+2], 16) for i in range(5,-1,-1)]
        for i in sn2i:
            self.write( i )
        crc = self.crc(''.join([chr(i) for i in [self.FAMILY_CODE]+sn2i]))
        self.write( crc )



class DS18B20( Dallas1W ):
    FAMILY_CODE = 0x28
    parasite = None

    def getTemperature( self, sn=None ):
        # read temperature from scratchpad, ignore remaining (including CRC)
        self.reset()
        if sn is not None:
            self.write( 0x55 )
            self.writeROM( sn )
        else:
            self.write( 0xCC )
        self.write( 0xBE )
        t = Utils.s2h(chr(self.read()) + chr(self.read()))
        return round(t*0.0625, 1)

    def getScratchpad( self, sn=None ):
        # read all from scratchpad
        self.reset()
        if sn is not None:
            self.write( 0x55 )
            self.writeROM( sn )
        else:
            self.write( 0xCC )
        self.write( 0xBE )
        r = self.read(9)
        t = Utils.s2h(chr(r[0]) + chr(r[1]))
        th = Utils.s2b(chr(r[2]))
        tl = Utils.s2b(chr(r[3]))
        res = 9 + ((Utils.s2B(chr(r[4])) >> 5) & 0x03)
        crc_calc = self.crc(''.join([chr(i) for i in r[:8]]))
        if r[8] != crc_calc:
            raise Exception("Crc error")
        return [round(t*0.0625, 1), th, tl, res]

    getAll = getScratchpad

    def setScratchpad( self, sn=None, th=0, tl=0, res=12 ):
        if res not in [9, 10, 11, 12]:
            raise Exception("Resolution must between 9~12")
        self.reset()
        if sn is not None:
            self.write( 0x55 )
            self.writeROM( sn )
        else:
            self.write( 0xCC )
        self.write( 0x4E )
        self.write( th & 0xFF )
        self.write( tl & 0xFF )
        self.write( ((res-9) & 0x3) << 5 )
    
    config = setScratchpad
 
    def convert( self, sn=None ):
        self.reset()
        if sn is not None:
            self.write( 0x55 )
            self.writeROM( sn )
        else:
            self.write( 0xCC )
        self.write( 0x44 )

    def checkParasite( self, sn=None ):
        self.reset()
        if sn is not None:
            self.write( 0x55 )
            self.writeROM( sn )
        else:
            self.write( 0xCC )
        self.write( 0xB4 )
        self.parasite = bool(self.readBit()==0)
        return self.parasite
            

class DS1990( Dallas1W ):
    FAMILY_CODE = 0x01
 
