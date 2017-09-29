# coding:utf8
__doc__ = 'mcush controller on Android'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import os
import re
import sys
import time
import base64
import logging
from . import Env
from . import Utils
from . import Instrument
from . import Mcush


class QPythonBluetoothPort(Instrument.Port):
        
    UUID = '00001101-0000-1000-8000-00805F9B34FB'

    def __init__( self, parent, *args, **kwargs ):
        Instrument.Port.__init__( self, parent, *args, **kwargs )
        import androidhelper
        self.androidhelper = androidhelper
        self.android = androidhelper.Android()
 
    def connect( self ):
        if self.connected:
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
        


class JavaBluetoothPort(Instrument.Port):
        
    UUID = '00001101-0000-1000-8000-00805F9B34FB'

    def __init__( self, parent, *args, **kwargs ):
        Instrument.Port.__init__( self, parent, *args, **kwargs )
        from jnius import autoclass
        self.autoclass = autoclass
        self.BluetoothAdapter = autoclass('android.bluetooth.BluetoothAdapter')
        self.BluetoothDevice = autoclass('android.bluetooth.BluetoothDevice')
        self.BluetoothSocket = autoclass('android.bluetooth.BluetoothSocket')
        self.UUID = autoclass('java.util.UUID')
 
    def connect( self ):
        if self.connected:
            return
        self._connected = False
        paired_devices = self.BluetoothAdapter.getDefaultAdapter().getBondedDevices().toArray()
        socket = None
        for device in paired_devices:
            if device.getName() == name:
                self.socket = device.createRfcommSocketToServiceRecord(
                    self.UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))
                self.recv_stream = socket.getInputStream()
                self.send_stream = socket.getOutputStream()
                self._connected = True
                break
        if self._connected:
            socket.connect()

    def disconnect( self ):
        self._connected = False
    
    def read( self, read_bytes=1 ):
        return self.recv_stream.read(read_bytes)

    def write( self, buf ):
        self.send_stream.write(buf)
        self.send_stream.flush()
 
    def flush( self ):
        self.send_stream.flush()


class KMcush( Mcush.Mcush ):
    '''Mcush on Android/Kivy'''
    PORT_TYPE = JavaBluetoothPort


