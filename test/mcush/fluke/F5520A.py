# coding:utf8
__doc__ = 'Fluke 5520A multi-product calibrator'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Instrument
from .. import Env
import time
import re


class CommandSequenceError( Exception ):
    pass

class CommandOperationError( Exception ):
    pass

# eg: Fault 1300: Bad syntax (REM)
PATTEN_FAULT = re.compile('Fault ([0-9]+): .+')


class F5520A( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'F5520A'
    DEFAULT_TIMEOUT = 10
    DEFAULT_PROMPTS = re.compile( '([0-9]+?)> ' )
    DEFAULT_IDN = re.compile( 'FLUKE,(5520A|5522A),.+?' )
    promptIndex = None

    def checkReturnedPrompt( self, ret ):
        # check prompt if sequence is continuous
        match = self.DEFAULT_PROMPTS.match( ret[-1] )
        indexRet = int(match.group(1))
        if self.promptIndex is None:
            self.promptIndex = 2  # Ctrl-C will clear the sequence number
        if indexRet != self.promptIndex:
            err = 'sequence should be %d but returned %d'% (self.promptIndex, indexRet)
            raise CommandSequenceError(err)
        self.promptIndex += 1

    def connect( self ):
        Instrument.SerialInstrument.connect( self )
        self.setStandby()
        self.setRemote( True )

    def checkFaultLine( self, line ):
        match = re.match( PATTEN_FAULT, line )
        if match:
            #raise CommandOperationError( match.group(1))  # error code only
            raise CommandOperationError( line.strip() )  # complete message

    def isOperationCompleted( self ):
        ret = self.writeCommand( '*opc?' )
        if len(ret) >= 1:
            self.checkFaultLine( ret[0] ) 
        return ret[-1].strip() == '1'

    def waitForOperationCompleted( self, timeout=5 ):
        # wait for last command is completed
        # NOTE: command *wai does this without timeout mechanism 
        t0 = time.time()
        while True:
            if self.isOperationCompleted():
                break
            if time.time() > t0 + timeout:
                raise CommandOperationError()

    def setStandby( self ):
        # set standby mode
        self.writeCommand( 'STBY' )
        self.waitForOperationCompleted()

    def setOutput( self, mode ):
        # set output mode
        self.writeCommand( '*CLS' )
        ret = self.writeCommand( 'OUT ' + mode )
        if len(ret) >= 1:
            print( ret )
            self.checkFaultLine( ret[-1] )
        self.writeCommand( 'OPER' )
        self.waitForOperationCompleted()

    def getOutput( self ):
        # get output mode
        ret = self.writeCommand( 'OUT?' )
        return ret[0]

    def setRemote( self, remote=True ):
        # set remote/local mode
        self.writeCommand( 'remote' if remote else 'local' )
    
    def setLocal( self ):
        # set local mode
        self.setRemote( False )

    def stop( self ):
        # stop safely
        self.setStandby()
        self.setOutput( '0 V, 0 Hz' )

    def rtcSync( self ):
        # update to current time
        t = time.localtime()
        cmd = 'CLOCK %d,%d,%d,%d,%d,%d'% (t.tm_year, t.tm_mon, t.tm_mday,
                                          t.tm_hour, t.tm_min, t.tm_sec )
        self.writeCommand( cmd )

    def getRange( self ):
        # query range
        self.writeCommand( 'RANGE?' )
        return ret[0]

    def outputVolt( self, volt=0.0, amp=None, freq=None ):
        cmd = '%s V'% volt
        if amp is not None:
            cmd += ', %s A'% amp
        if freq is not None:
            cmd += ', %s Hz'% freq
        self.setOutput( cmd )

    def outputOhm( self, ohm=100.0 ):
        cmd = '%s Ohm'% ohm
        self.setOutput( cmd )


