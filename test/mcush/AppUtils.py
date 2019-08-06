# coding: utf8
__doc__ = 'subprocess task control'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from time import sleep
try:
    from Queue import Empty
except ImportError:
    from queue import Empty
from threading import Thread
from multiprocessing import Process, Queue
try:
    from termios import error as termios_error
except ImportError:
    termios_error = None
from . import Env
from . import Instrument
from os.path import join, dirname
import gettext
_t = gettext.translation( "mcush", join(dirname(__file__), "locale"), codeset='utf8', fallback=True )
try:
    _getStr = _t.ugettext
except AttributeError:
    _getStr = _t.gettext


class Task():
    STOP_MAX_DELAY = 15
    STOP_CHECK_PERIOD = 0.2

    def __init__( self, args, queue ):
        self.args = args
        self.queue = queue
        self.control_queue = Queue()
        self.p = Process( target=self.monitor )
        self.p.start()
        self.listener = Thread( target=self.listener )
        self.listener.setDaemon( 1 )
        self.listener.start()
        self.manual_stop = False

    def target( self, args ):
        raise NotImplementedError

    def waitStop( self, timeout ):
        try:
            ret = self.control_queue.get( block=True, timeout=timeout  )
            assert ret == 'stop', ret
            self.info( _getStr('Stopping') )
            return True
        except Empty:
            pass 
        return False

    def stop( self ):
        if not self.p.is_alive():
            return True
        self.manual_stop = True
        self.control_queue.put( 'stop' )
        for i in range( int(self.STOP_MAX_DELAY / self.STOP_CHECK_PERIOD) ):
            if not self.p.is_alive():
                return True
            else:
                sleep( self.STOP_CHECK_PERIOD )
        if Env.VERBOSE:
            print( 'subprocess still alive' )
        return False

    def info( self, info, info_type='info' ):
        self.queue.put( ('info', (info, info_type)) )

    def end( self ):
        self.queue.put( ('lock', False) )
        self.queue.put( ('end', False) )

    def monitor( self ):
        try:
            self.queue.put( ('lock', True) )
            self.target( self.args )
        except Instrument.CommandTimeoutError:
            self.info( _getStr('Stopping') )
            self.info( _getStr('Timeout'), 'error' )
        except Instrument.PortNotFound as e:
            self.info( _getStr('Port not exist') + ': ' + unicode(e), 'error' )
        except Instrument.UnknownPortError as e:
            self.info( _getStr('Unknown port error') + ': ' + unicode(e), 'error' )
        except Instrument.IDNMatchError as e:
            self.info( _getStr('IDN not match') + ': ' + unicode(e), 'error' )
        except Exception as e:
            if Env.VERBOSE:
                print( type(e), str(e) )
            info = _getStr('Unknown error') + ': '
            if termios_error and isinstance(e, termios_error):
                a,b = e.args
                info += u'%d %s'% (a, b.decode(encoding='utf8'))
            else:
                #for c in str(e):  # e string may not be Unicode
                #    info += c if ord(c) <= 128 else 'x%X'% ord(c)
                info = str(e).decode(encoding='utf8')
            self.info( info, 'error' )
     
    def listener( self ):
        if self.p.is_alive():
            #print 'listener:alive'
            self.p.join()
            #print 'listener:end'
        self.end()
       
    def isStopped( self ):
        return not self.p.is_alive()
 
    def terminate( self, safely=False ): 
        if safely:
            if self.stop():
                return
        if self.p.is_alive():
            self.p.terminate()
        self.info( _getStr('Stopped') )
        self.end()

    def pause( self, prompt ):
        # clear queue
        while True:
            try:
                self.control_queue.get( block=False )
            except Empty:
                break
        self.queue.put( ('pause', prompt) )
        try:
            ret = self.control_queue.get( block=True, timeout=None )
            if ret == 'continue':
                return True 
        except Empty:
            pass
        return False

    def halt( self ):
        self.queue.put( ('halt', None) )
 
    def done( self ):
        self.queue.put( ('done', None) )


