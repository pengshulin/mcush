# coding:utf8
__doc__ = 'qpython on android'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import os
import re
import sys
import time
import base64
import logging
from .. import Env
from .. import Utils
from .. import Instrument
from .. import Mcush


class QPythonBluetoothPort(Instrument.Port):
        
    UUID = '00001101-0000-1000-8000-00805F9B34FB'

    def __init__( self, parent, *args, **kwargs ):
        Instrument.Port.__init__( self, parent, *args, **kwargs )
        import androidhelper
        self.androidhelper = androidhelper
        self.android = androidhelper.Android()
 
    def connect( self ):
        if self._connected:
            return
        r = self.android.bluetoothConnect(self.UUID, self.port)
        if r.error:
            raise Instrument.SerialNotFound( self.port )
        r = self.android.bluetoothGetConnectedDeviceName('')
        print("%s Connected"% r.result)
        self._connected = True

    def disconnect( self ):
        self.android.bluetoothStop( '' )
        self._connected = False
    
    def read( self, read_bytes=1 ):
        return self.android.bluetoothRead(read_bytes, connID='').result

    def write( self, buf ):
        self.android.bluetoothWrite( buf, '' )
 
    def flush( self ):
        pass



class QMcush( Mcush.Mcush ):
    '''Mcush on Android/QPython'''
    PORT_TYPE = QPythonBluetoothPort

    def tts( self, msg ):
        self.port.android.ttsSpeak( msg )
        while True:
            r = self.port.android.ttsIsSpeaking()
            if r.result:
                time.sleep(0.5)
            else:
                break

    def vibrate( self, duration=300 ):
        self.port.android.vibrate(duration)

    def smsSend( self, addr, msg ):
        self.port.android.smsSend( addr, msg )
        


