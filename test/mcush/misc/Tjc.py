# coding:utf8
__doc__ = 'USART-HMI from Taojingchi Co.'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import re
import binascii
import time
from .. import Env, Instrument


RETURN_CODES = {
    0x00 : 'invalid',
    0x01 : 'ok',
    0x02 : 'control ID err',
    0x03 : 'page ID err',
    0x04 : 'image ID err',
    0x05 : 'font ID err',
    0x11 : 'baudrate err',
    0x12 : 'curve control ID err or channel err',
    0x1A : 'var name err',
    0x1B : 'oper err',
    0x1C : 'set err',
    0x1D : 'EEPROM err',
    0x1E : 'parm num err',
    0x1F : 'IO err',
    0x20 : 'escape char err',
    0x23 : 'var name too long',
    0x24 : 'usart buf overflow',
    0x65 : '',
    0x66 : '',
    0x67 : '',
    0x68 : '',
    0x70 : '',
    0x86 : '',
    0x87 : '',
    0x88 : '',
    0x89 : '',
    0xFD : '',
    0xFE : '',
    }


class TjcHMI( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'TjcHMI'
    DEFAULT_TERMINATOR_WRITE = '\xFF\xFF\xFF'
    DEFAULT_TERMINATOR_RESET = '\xFF\xFF\xFFbkcmd=3\xFF\xFF\xFF'
    DEFAULT_PROMPTS = re.compile( '[^\xFF]+\xff\xff\xff' )
    DEFAULT_CHECK_RETURN_COMMAND = False
    
    def readUntilPrompts( self, check_return_code=True ):
        '''read until prompts'''
        contents = []
        while True:
            byte = self.port.read(1)
            if byte:
                if Env.PYTHON_V3:
                    byte = chr(ord(byte))
                if (len(contents)==0) and (byte=='\xff'):
                    continue  # ignore leading 0xff
                else:
                    contents.append( byte )
            else:
                raise Instrument.CommandTimeoutError( binascii.hexlify(''.join(contents)) )
            ret = ''.join(contents)
            match = self.prompts.match( ret )
            if match:
                self.logger.debug( ret )
                if check_return_code:
                    assert ord(contents[0]) in RETURN_CODES.keys()
                return [''] + list(ret.rstrip('\xff')) + ['']

    def page( self, pageid=0 ):
        cmd = 'page %s'% str(pageid)
        self.writeCommand(cmd)

    def ref( self, obj ):
        cmd = 'ref %s'% str(obj)
        self.writeCommand(cmd)

    def click( self, click ):
        cmd = 'click %s'% str(click)
        self.writeCommand(cmd)

    def get( self, att ):
        cmd = 'get %s'% str(att)
        return self.writeCommand(cmd)

    #def print( self, att ):
    #    cmd = 'print %s'% str(att)
    #    return self.writeCommand(cmd)

    def vis( self, obj, state ):
        cmd = 'vis %s,%s'% [str(obj), str(int(state))]
        self.writeCommand(cmd)

    def sendme( self ):
        cmd = 'sendme'
        return self.writeCommand(cmd)

    def cls( self, color='black' ):
        cmd = 'cls %s'% str(color).upper()
        self.writeCommand(cmd)

    def line( self, x0, y0, x1, y1, color='white' ):
        cmd = 'line %s,%s,%s,%s,%s'% (x0, y0, x1, y1, str(color).upper())
        self.writeCommand(cmd)
    
    def cir( self, x, y, r, color='white' ):
        cmd = 'cir %s,%s,%s,%s'% (x, y, r, str(color).upper())
        self.writeCommand(cmd)

    def set( self, key, val ):
        if isinstance(val, str) or isinstance(val, unicode):
            cmd = '%s=\"%s\"'% (str(key), val.encode('gbk'))
        else:
            cmd = '%s=%s'% (str(key), str(val))
        self.writeCommand(cmd)

    def xstr( self, x,y,w,h,fontid,pointcolor,backcolor,xcenter,ycenter,sta,string ):
        cmd = 'xstr %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,\"'% (str(x),str(y),str(w),str(h),
                    str(fontid),str(pointcolor),str(backcolor),str(xcenter),str(ycenter), str(sta))
        if Env.PYTHON_V3:
            cmd += ''.join( map(chr, string.encode(encoding='gbk')) )
        else:
            cmd += unicode(string).encode(encoding='gbk')
        cmd += '\"'    
        self.writeCommand(cmd)


