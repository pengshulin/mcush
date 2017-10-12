# coding:utf8
__doc__ = 'kivy on Android'
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



class JavaBluetoothPort(Instrument.Port):
        
    def __init__( self, parent, *args, **kwargs ):
        Instrument.Port.__init__( self, parent, *args, **kwargs )
        from jnius import autoclass
        self.UUID = autoclass('java.util.UUID')
        self.BluetoothAdapter = autoclass('android.bluetooth.BluetoothAdapter')
        self.BluetoothDevice = autoclass('android.bluetooth.BluetoothDevice')
        self.BluetoothSocket = autoclass('android.bluetooth.BluetoothSocket')
        
    def connect( self ):
        if self._connected:
            return
        paired_devices = self.BluetoothAdapter.getDefaultAdapter().getBondedDevices().toArray()
        for device in paired_devices:
            if device.getName() == self.port:
                self.socket = device.createRfcommSocketToServiceRecord(self.UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))
                #print( type(self.socket), self.socket )
                self.recv = self.socket.getInputStream()
                self.send = self.socket.getOutputStream()
                print( 'JavaBluetoothPort.connect', 'recv', type(self.recv), self.recv, 'send',  type(self.send), self.send )
                self.socket.connect()
                self._connected = True
                break

    def disconnect( self ):
        self._connected = False
        self.socket.close()
    
    def read( self, read_bytes=1 ):
        if self._connected:
            ret = []
            for i in range(read_bytes):
                r = self.recv.read()
                if r:
                    ret.append( chr(r) )
                else:
                    break
            #print( 'read', ret )
            return ''.join(ret)

    def write( self, buf ):
        if self._connected:
            self.send.write(buf)
 
    def flush( self ):
        if self._connected:
            self.send.flush()


class KMcush( Mcush.Mcush ):
    '''Mcush on Android/Kivy'''
    PORT_TYPE = JavaBluetoothPort

    def writeCommand( self, cmd ):
        try:
            ret = Mcush.Mcush.writeCommand( self, cmd )
            print( 'cmd: ' + cmd )
            print( 'ret: ' + '\n'.join(ret) )
            return ret
        except Instrument.CommandSyntaxError:
            print( 'command syntax err: ' + cmd )
            return []

    def vibrate( self, duration=0.05 ):
        try:
            self.vibrator
        except AttributeError:
            from plyer import vibrator
            self.vibrator = vibrator
        self.vibrator.vibrate( duration )

