#!/usr/bin/env python
# coding: utf-8
__doc__ = 'GUI toolkit for mcush debug and test'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import os
import sys
import time
from time import *
import keyword
import random
import subprocess
try:
    import ConfigParser
except ImportError:
    import configparser
import wx
import wx.lib.newevent
import wx.stc as stc
import StringIO
from copy import deepcopy
from multiprocessing import Queue, freeze_support
from threading import Thread
from mcush import *
from mcush.Mcush import *
from mcush.Utils import *
from mcush.misc import *
from McushDebugDlg import *
from re import compile as re_compile
import base64
import pprint

(UpdateEvent, EVT_UPDATE) = wx.lib.newevent.NewEvent()
import gettext
_ = gettext.gettext

PRESET = [ \
["Output impulse counter", '''\
pin = '0.0'  # PA0
s = Mcush(PORT)
s.errnoStop()  # stop errno blink
s.gpio( pin, o=True )
counter = 0
while True:
    info( 'Counter: %d'% counter )
    s.gpio( pin, s=True )
    s.led(0, True)
    sleep(0.1)
    s.gpio( pin, c=True )
    s.led(0, False)
    sleep(0.5)
    counter += 1
'''],

["LED blink", '''\
s = Mcush(PORT)
s.errnoStop()  # stop errno blink
while True:
    for i in range(3):
        s.led(0, True)
        s.led(1, True)
        sleep(0.1)
        s.led(0, False)
        s.led(1, False)
        sleep(0.1)
    for i in range(3):
        s.led(2, True)
        s.led(3, True)
        sleep(0.1)
        s.led(2, False)
        s.led(3, False)
        sleep(0.1)
'''],

["Beep player", '''\
bp = BeepPlayer.BeepPlayer(Mcush(PORT))
bp.play( BeepPlayer.ALISE )
'''],

["Beep morse code", '''\
bp = MorseCodeBeeper.MorseCodeBeeper(Mcush(PORT))
#bp.set_beeper( bp.beeper_led1 )
while True:
    bp.BeepString( 'SOS' )
    time.sleep(5)
'''],


["SGPIO output", '''\
# PA[0:5]
s = Mcush(PORT)
port = 0  # PA
pins = 0x3F  # [0:5]
freq = 1
buf = range(64)
s.sgpio( port, pins, buf, freq, loop=True )
'''],

]

def stop(message=''):
    raise StopError(message)

task_obj = None
def info(message=''):
    global task_obj
    task_obj.info( message )

STOP = stop
INFO = info

PORT = None

class MyTask(AppUtils.Task):
    def addResult( self, info=None ):
        if info is None:
            self.queue.put( ('add_result', '') )
        else:
            self.queue.put( ('add_result', str(info)) )

    def clearResult( self ):
        self.queue.put( ('clear_result', None) )

    def setReading( self, reading ):
        self.queue.put( ('reading', reading) )

    def updateDeviceInfo( self, model, version, name, time1, time2 ):
        self.queue.put( ('update_device_info', (model, version, name, time1, time2)) )

    def verifyDeviceInfo( self, model, version, name, time1, time2 ):
        self.queue.put( ('verify_device_info', (model, version, name, time1, time2)) )

    def updateInfo( self, card_type, firmware_ver ):
        self.queue.put( ('update_info', (card_type, firmware_ver)) )


class TestPortTask(MyTask):
    def target( self, args ):
        (port) = args
        self.info( u'%s (%s)'% (_('Testing port...'), port) )
        s = Mcush(port)
        model = s.getModel()
        version = s.getVersion()
        sn = s.getSerialNumber()
        info = u'%s %s'% (_('Model'), model)
        info += u', %s %s'% (_('Version'), version)
        if sn:
            info += u', %s %s'% (_('SN'), sn)
        self.info( info, 'ok' )
        for i in range(10): 
            try:
                s.led(0, toggle=True)
            except Exception as e:
                print( e )
                break
            time.sleep(0.05)
        s.disconnect()

class ResetTask(MyTask):
    def target( self, args ):
        (port) = args
        self.info( u"%s (%s)"% (_('Open port...'), port) )
        s = Mcush(port)
        self.info( _("Reset") )
        s.scpiRst()
        self.info( _("Done") )

class RebootTask(MyTask):
    def target( self, args ):
        (port) = args
        self.info( u"%s (%s)"% (_('Open port...'), port) )
        s = Mcush(port)
        self.info( _("Reboot") )
        s.port.write('reboot\n')
        s.port.flush()
        s.disconnect()
        del s
        self.info( _("Done") )


class ExecuteScriptTask(MyTask):
    def target( self, args ):
        global PORT, task_obj
        self.info( _("Executing script...") )
        (port, script) = args
        task_obj = self
        PORT = port
        code = compile(script, '', 'exec')
        exec( code )
        self.info( _("Done") )


class QueryTask(MyTask):
    def target( self, args ):
        (port) = args
        self.info( u"%s (%s)"% (_('Open port...'), port) )
        s = Mcush(port)
        self.info( u"querying..." )
        try:
            lst = s.list( )
            self.queue.put( ('update_filelist', lst) )
            self.info( _("Done") )
        except:
            lst = []
            self.info( u"Failed to query file list", 'error' )


class RemoveTask(MyTask):
    def target( self, args ):
        (port, fname) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        self.info( u"removing %s..."% fname )
        try:
            s.remove( fname )
            self.queue.put( ('remove_file', fname) )
            self.info( _("Done") )
        except Instrument.CommandExecuteError as e:
            self.info( "Failed to remove %s"% fname, 'error' )


class RenameTask(MyTask):
    def target( self, args ):
        (port, oldname, newname) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        self.info( u"rename %s to %s..."% (oldname, newname) )
        try:
            s.rename( oldname, newname )
            self.queue.put( ('rename_file', (oldname, newname)) )
            self.info( _("Done") )
        except Instrument.CommandExecuteError as e:
            self.info( u"Failed to rename %s to %s..."% (oldname, newname), 'error' )


class ViewTask(MyTask):
    def target( self, args ):
        (port, fname) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        self.info( u"download file %s..."% (fname) )
        try:
            r = s.cat( fname, b64=True )
            self.queue.put( ('view_file', (fname, r)) )
            self.info( _("Done") )
        except Instrument.CommandExecuteError as e:
            self.info( u"Failed to download file %s"% (fname), 'error' )


class GetFileTask(MyTask):
    def target( self, args ):
        (port, fname, savename) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        self.info( u"download file %s..."% (fname) )
        try:
            r = s.cat( fname, b64=True )
        except Instrument.CommandExecuteError as e:
            self.info( u"Failed to download file %s"% (fname), 'error' )
        self.info( 'save as %s...'% savename )
        try:
            open(savename, 'w+').write(r)
            self.info( "File %s saved, size %d."% (savename, len(r)) )
        except Exception as e:
            self.info( u"Failed to save file %s"% (savename), 'error' )


PUTFILE_SEGMENT_SIZE = 1024

class PutFileTask(MyTask):
    def target( self, args ):
        (port, srcname, fname) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        self.info( u"read file %s..."% (srcname) )
        try:
            raw = open(srcname, 'r').read()
            raw_len = len(raw)
        except:
            self.info( 'read file %s error'% srcname, 'error' )
            return
        self.info( u"write file %s..."% (fname) )
        raw_split= []
        i = 0
        while True:
            seg = raw[PUTFILE_SEGMENT_SIZE*i:PUTFILE_SEGMENT_SIZE*(i+1)]
            if seg:
                raw_split.append( seg )
            else:
                break
            i += 1
        try:
            cnt = 0
            buf0 = raw_split.pop(0)
            self.info( u"Transfering file %s, 0 / %d bytes (0 %%)"% (fname, raw_len) )
            s.cat( fname, write=True, b64=True, buf=buf0 )
            cnt += len(buf0)
            for buf in raw_split:
                self.info( u"Transfering file %s, %d / %d bytes (%d %%)"% (fname, cnt, raw_len, int(cnt*100/raw_len)) )
                s.cat( fname, write=True, append=True, b64=True, buf=buf )
                cnt += len(buf)
            self.info( u"File %s written, size %d."% (fname, len(raw)) )
            self.queue.put( ('update_filelist', s.list()) )
        except Instrument.CommandExecuteError as e:
            self.info( u"Failed to download file %s"% (fname), 'error' )


class FormatTask(MyTask):
    def target( self, args ):
        (port, path) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        self.info( u"Formating %s..."% (path) )
        s.setTimeout( 30 )
        s.writeCommand( 's -c format' )
        s.setTimeout()
        self.info( _("Done") )


class FsInfoTask(MyTask):
    def target( self, args ):
        (port, path) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        self.info( u"Formating %s..."% (path) )
        a, b = s.writeCommand( 's -c info' )[0].split('  ')
        total = int(a.split(':')[1].strip())
        used = int(b.split(':')[1].strip())
        self.info( 'Total: %d, used: %d (%.1f%%)'% (total, used, float(used)/total*100) )


class I2cScanTask(MyTask):
    def target( self, args ):
        (port, config) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        pin_sda = config['sda']
        pin_scl = config['scl']
        delay_us = int(config['delay_us'])
        s.i2c_init( 0, scl=pin_scl, sda=pin_sda, delay=delay_us )
        result = []
        for addr in range(128):
            self.info( 'Scan address: 0x%02X'% (addr) )
            s.i2c_init( addr )
            try:
                s.i2c( [0] )
                result.append( addr )
            except Instrument.CommandExecuteError:
                pass
        if result:
            self.info( 'Responed address: %s'% (' '.join(['0x%02X'% i for i in result])) )
        else:     
            self.info( 'No address responsed' )

        
class I2cReadTask(MyTask):
    def target( self, args ):
        (port, config) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        addr = int(eval(config['address'])) & 0x7F
        pin_sda = config['sda']
        pin_scl = config['scl']
        delay_us = int(config['delay_us'])
        reg_from = int(eval(config['reg_from']))
        reg_to = int(eval(config['reg_to']))
        if reg_to > 255:
            reg_to = 255
        if reg_to >= reg_from:
            length = reg_to - reg_from + 1
        else:
            length = 1
        s.i2c_init( addr, scl=pin_scl, sda=pin_sda, delay=delay_us )
        result = []
        self.info( _("Reading...") )
        r = s.i2c( [reg_from], read_count=length )
        self.queue.put( ('i2c_data', r) )
        self.info( '%d bytes read'% length )
        

class I2cWriteTask(MyTask):
    def target( self, args ):
        (port, config, data) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        addr = int(eval(config['address'])) & 0x7F
        pin_sda = config['sda']
        pin_scl = config['scl']
        delay_us = int(config['delay_us'])
        reg_from = int(eval(config['reg_from']))
        reg_to = int(eval(config['reg_to']))
        if reg_to > 255:
            reg_to = 255
        #if reg_to >= reg_from:
        #    length = reg_to - reg_from + 1
        #else:
        #    length = 1
        s.i2c_init( addr, scl=pin_scl, sda=pin_sda, delay=delay_us )
        if reg_from + len(data) > 256:
            # cut 
            data = data[:256-reg_from]
        self.info( _("Writing...") )
        addr = reg_from
        wr = [addr]
        while True:
            wr.append( data.pop(0) )
            addr += 1
            if len(wr) > 16:
                s.i2c( wr )
                wr = [addr]
            if not data:
                if len(wr) > 1:
                    s.i2c( wr )
                break
        self.info( _("Done") )
        











class MainFrame(MyFrame):
    def __init__(self, *args, **kwds):
        self.preset_popup_menu_id = {}
        self.plot_spectrum_log_mode = False
        MyFrame.__init__(self, *args, **kwds )

        dirs = wx.StandardPaths.Get()
        self.config_dir = dirs.GetUserDataDir()
        self.config_file = os.path.join( self.config_dir, 'config.conf' )

        #self.Bind(wx.EVT_TIMER, self.OnTimer)
        self.Bind(wx.EVT_CLOSE, self.OnClose)
        self.Bind(EVT_UPDATE, self.OnUpdate)
                
        self.tree_ctrl_fs.Bind(wx.EVT_LEFT_DCLICK, self.OnView)
        self.tree_ctrl_fs.Bind(wx.EVT_TREE_ITEM_RIGHT_CLICK, self.OnFsMenu)
        self.popupID_query = wx.NewId()
        self.popupID_remove = wx.NewId()
        self.popupID_rename = wx.NewId()
        self.popupID_view = wx.NewId()
        self.popupID_get = wx.NewId()
        self.popupID_put = wx.NewId()
        self.popupID_format = wx.NewId()
        self.popupID_fsinfo = wx.NewId()
        self.Bind(wx.EVT_MENU, self.OnQuery, id=self.popupID_query)
        self.Bind(wx.EVT_MENU, self.OnRemove, id=self.popupID_remove)
        self.Bind(wx.EVT_MENU, self.OnRename, id=self.popupID_rename)
        self.Bind(wx.EVT_MENU, self.OnView, id=self.popupID_view)
        self.Bind(wx.EVT_MENU, self.OnGetFile, id=self.popupID_get)
        self.Bind(wx.EVT_MENU, self.OnPutFile, id=self.popupID_put)
        self.Bind(wx.EVT_MENU, self.OnFormat, id=self.popupID_format)
        self.Bind(wx.EVT_MENU, self.OnFsInfo, id=self.popupID_fsinfo)
        
    
        self.init_text_ctrl_script()
        self.loadConfig()
        ports = Utils.enumPorts()
        self.combo_box_port.AppendItems( ports )
        self.msgq = Queue()
        self.listeningThread = Thread( target=self.listener )
        self.listeningThread.setDaemon( 1 )
        self.listeningThread.start()
    
        # images/icons
        self.imgs = self.loadImages('img', 
            {'empty.png': ['empty', 'none'],
             'start.png': ['start', 'run'],
             'dialog-ok-apply.png': ['ok', 'done'],
             'failed.png': ['failed', 'error', 'halt'],
             'drive-harddisk.png': ['drive'],
             'folder.png': ['folder'],
             'folder-documents.png': ['file'],
             'system-shutdown.png': ['quit', 'exit', 'poweroff', 'shutdown'],
             'view-refresh.png': ['refresh', 'reset'],
             'edit-find.png': ['search', 'find'],
             'message.png': ['info', 'message']} )

        self.button_stop.SetBitmap(self.imgs['halt'], wx.LEFT)
        self.button_exit.SetBitmap(self.imgs['exit'], wx.LEFT)
        self.button_run.SetBitmap(self.imgs['run'], wx.LEFT)
        self.button_reset.SetBitmap(self.imgs['reset'], wx.LEFT)
        self.button_reboot.SetBitmap(self.imgs['poweroff'], wx.LEFT)
        self.button_query.SetBitmap(self.imgs['find'], wx.LEFT)

        imglst = wx.ImageList( 24, 24 )
        self.img_id_drive = imglst.Add( self.imgs['drive'] )
        self.img_id_folder = imglst.Add( self.imgs['folder'] )
        self.img_id_file = imglst.Add( self.imgs['file'] )
        self.tree_ctrl_fs.SetImageList(imglst)
        self._imglst = imglst

        self.info('')
        
        #w,h = self.GetSize()
        #self.SetSize((w, 700))
        

    def init_text_ctrl_script(self):
        ctrl = self.text_ctrl_script
        faces = { 'times': 'Courier New',
                  'mono' : 'Courier New',
                  'helv' : 'Courier New',
                  'other': 'Courier New',
                  'size' : 12,
                  'size2': 10,
                }
        KEYWORDS = ['Mcush', 'Env', 'Utils', 'Instrument', 'PORT', 'True', 'False']
        ctrl.SetLexer(wx.stc.STC_LEX_PYTHON)
        ctrl.SetKeyWords(0, " ".join(keyword.kwlist + KEYWORDS))
        ctrl.SetProperty("tab.timmy.whinge.level", "1")
        ctrl.SetMargins(0,0)
        ctrl.SetViewWhiteSpace(False)
        ctrl.Bind(wx.stc.EVT_STC_UPDATEUI, self.OnUpdateUI)
        ctrl.StyleSetSpec(wx.stc.STC_STYLE_DEFAULT,     "face:%(helv)s,size:%(size)d" % faces)
        ctrl.StyleClearAll()
        ctrl.StyleSetSpec(wx.stc.STC_STYLE_DEFAULT,     "face:%(helv)s,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_STYLE_LINENUMBER,  "back:#C0C0C0,face:%(helv)s,size:%(size2)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_STYLE_CONTROLCHAR, "face:%(other)s" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_STYLE_BRACELIGHT,  "fore:#FFFFFF,back:#0000FF,bold")
        ctrl.StyleSetSpec(wx.stc.STC_STYLE_BRACEBAD,    "fore:#000000,back:#FF0000,bold")
        ctrl.StyleSetSpec(wx.stc.STC_P_DEFAULT, "fore:#000000,face:%(helv)s,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_COMMENTLINE, "fore:#007F00,face:%(other)s,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_NUMBER, "fore:#007F7F,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_STRING, "fore:#7F007F,face:%(helv)s,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_CHARACTER, "fore:#7F007F,face:%(helv)s,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_WORD, "fore:#00007F,bold,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_TRIPLE, "fore:#7F0000,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_TRIPLEDOUBLE, "fore:#7F0000,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_CLASSNAME, "fore:#0000FF,bold,underline,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_DEFNAME, "fore:#007F7F,bold,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_OPERATOR, "bold,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_IDENTIFIER, "fore:#000000,face:%(helv)s,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_COMMENTBLOCK, "fore:#7F7F7F,size:%(size)d" % faces)
        ctrl.StyleSetSpec(wx.stc.STC_P_STRINGEOL, "fore:#000000,face:%(mono)s,back:#E0C0E0,eol,size:%(size)d" % faces)
        ctrl.SetCaretForeground("BLACK")
        ctrl.SetMarginType(1, wx.stc.STC_MARGIN_NUMBER)
        ctrl.SetMarginWidth(1, 40)

        self.SetAcceleratorTable(wx.AcceleratorTable([  \
            (wx.ACCEL_NORMAL, wx.WXK_F5,  self.button_run.GetId()),  # run
            ]))

    
    def OnUpdateUI(self, evt):
        ctrl = self.text_ctrl_script
        # check for matching braces
        braceAtCaret = -1
        braceOpposite = -1
        charBefore = None
        caretPos = ctrl.GetCurrentPos()

        if caretPos > 0:
            charBefore = ctrl.GetCharAt(caretPos - 1)
            styleBefore = ctrl.GetStyleAt(caretPos - 1)

        # check before
        if charBefore and (32 < charBefore < 128):
            if chr(charBefore) in "[]{}()" and styleBefore == wx.stc.STC_P_OPERATOR:
                braceAtCaret = caretPos - 1

        # check after
        if braceAtCaret < 0:
            charAfter = ctrl.GetCharAt(caretPos)
            styleAfter = ctrl.GetStyleAt(caretPos)
            if charAfter and (32 < charAfter < 128):
                if chr(charAfter) in "[]{}()" and styleAfter == wx.stc.STC_P_OPERATOR:
                    braceAtCaret = caretPos

        if braceAtCaret >= 0:
            braceOpposite = ctrl.BraceMatch(braceAtCaret)
        if braceAtCaret != -1  and braceOpposite == -1:
            ctrl.BraceBadLight(braceAtCaret)
        else:
            ctrl.BraceHighlight(braceAtCaret, braceOpposite)



    def loadImages(self, img_dir, img_config):
        ret = {}
        for f in img_config:
            img = wx.Bitmap(os.path.join(img_dir, f), wx.BITMAP_TYPE_ANY)
            for name in img_config[f]:
                ret[name] = img
        return ret

    def doExit(self):
        self.saveConfig()
        self.StopSubProcess()
        self.Destroy()

    def OnExit(self, event):
        self.doExit()        
        event.Skip()

    def OnClose(self, event):
        self.saveConfig()
        self.StopSubProcess()
        event.Skip()

    def loadConfig( self ):
        try:
            cfgfile = ConfigParser.ConfigParser()
            cfgfile.read( self.config_file )
            self.combo_box_port.SetValue( unicode(cfgfile.get( 'config', 'port' ), encoding='utf-8'))
            self.text_ctrl_i2c_address.SetValue( unicode(cfgfile.get( 'config', 'i2c_address' ), encoding='utf-8'))
            self.text_ctrl_i2c_pin_sda.SetValue( unicode(cfgfile.get( 'config', 'i2c_pin_sda' ), encoding='utf-8'))
            self.text_ctrl_i2c_pin_scl.SetValue( unicode(cfgfile.get( 'config', 'i2c_pin_scl' ), encoding='utf-8'))
            self.text_ctrl_i2c_delay_us.SetValue( unicode(cfgfile.get( 'config', 'i2c_delay_us' ), encoding='utf-8'))
            self.text_ctrl_i2c_reg_from.SetValue( unicode(cfgfile.get( 'config', 'i2c_reg_from' ), encoding='utf-8'))
            self.text_ctrl_i2c_reg_to.SetValue( unicode(cfgfile.get( 'config', 'i2c_reg_to' ), encoding='utf-8'))
        except Exception as e:
            print( 'loadconfig failed' )
            mode = False

    def saveConfig( self ):
        cfgfile = ConfigParser.ConfigParser()
        cfgfile.add_section( 'config' )
        try:
            cfgfile.set( 'config', 'port', self.combo_box_port.GetValue().encode('utf-8') ) 
        except:
            pass
        try:
            cfgfile.set( 'config', 'i2c_address', self.text_ctrl_i2c_address.GetValue().encode('utf-8') ) 
        except:
            pass
        try:
            cfgfile.set( 'config', 'i2c_pin_sda', self.text_ctrl_i2c_pin_sda.GetValue().encode('utf-8') ) 
        except:
            pass
        try:
            cfgfile.set( 'config', 'i2c_pin_scl', self.text_ctrl_i2c_pin_scl.GetValue().encode('utf-8') ) 
        except:
            pass
        try:
            cfgfile.set( 'config', 'i2c_delay_us', self.text_ctrl_i2c_delay_us.GetValue().encode('utf-8') ) 
        except:
            pass
        try:
            cfgfile.set( 'config', 'i2c_reg_from', self.text_ctrl_i2c_reg_from.GetValue().encode('utf-8') ) 
        except:
            pass
        try:
            cfgfile.set( 'config', 'i2c_reg_to', self.text_ctrl_i2c_reg_to.GetValue().encode('utf-8') ) 
        except:
            pass
        try:
            if not os.path.isdir( self.config_dir ):
                os.mkdir( self.config_dir )
            cfgfile.write(open( self.config_file, 'w+'))
        except:
            pass
        

    def getPorts( self ):
        return self.combo_box_port.GetValue().encode('utf-8')
 
    def listener( self ):
        '''thread to monitor and forward subprocess event'''
        while True:
            try:
                cmd, val = self.msgq.get()
            except EOFError:
                break
            wx.PostEvent( self, UpdateEvent( cmd=cmd, val=val ) )
             
    def info(self, info, info_type='info'):
        if not info:
            info, info_type = '', 'empty'
        self.label_info.SetLabel(info)
        icon = self.imgs[info_type] 
        if self.bitmap_info.GetBitmap() != icon:
            self.bitmap_info.SetBitmap(icon)
  
    def OnUpdate( self, event ):
        if event.cmd == 'info':
            info, info_type = event.val
            self.info(info, info_type)
        elif event.cmd == 'lock':
            lock = event.val
            self.combo_box_port.Enable( not lock )
            self.button_test_port.Enable( not lock )
            #self.button_run.Enable( not lock )
            #self.button_reset.Enable( not lock )
            #self.button_query.Enable( not lock )
            #self.button_remove.Enable( not lock )
            #self.button_rename.Enable( not lock )
            #self.button_view.Enable( not lock )
            #self.button_put_file.Enable( not lock )
            #self.button_get_file.Enable( not lock )
            #self.tree_ctrl_fs.Enable( not lock )
            self.notebook_1.Enable( not lock )
            self.button_stop.Show( lock )
            self.Layout()
        elif event.cmd == 'update_filelist':
            lst = event.val
            self.tree_ctrl_fs.DeleteAllItems()
            root = self.tree_ctrl_fs.AddRoot('/')
            self.tree_ctrl_fs.SetItemImage(root, self.img_id_drive, wx.TreeItemIcon_Normal) 
            subroot = {}
            for p, rn, s in lst:
                n = None if rn is None else rn.decode('utf8', errors='ignore')
                print( p, pprint.pformat(rn), n, s )
                if not subroot.has_key( p ):
                    subroot[p] = self.tree_ctrl_fs.AppendItem(root, p)
                    self.tree_ctrl_fs.SetItemImage(subroot[p], self.img_id_drive, wx.TreeItemIcon_Normal) 
                    self.tree_ctrl_fs.SetPyData(subroot[p], (p, n, s))
                if n is None:
                    continue
                item = self.tree_ctrl_fs.AppendItem(subroot[p], n + ' (%d)'%s)
                self.tree_ctrl_fs.SetPyData(item, (p, n, s))
                self.tree_ctrl_fs.SetItemImage(item, self.img_id_file, wx.TreeItemIcon_Normal) 
            self.tree_ctrl_fs.ExpandAll()
        elif event.cmd == 'view_file':
            (fname, contents) = event.val
            dlg = MyViewFileDialog(self)
            #print type(contents), contents
            dlg.text_ctrl_data.SetValue( contents )
            dlg.SetTitle( '%s'% fname )
            dlg.ShowModal()
        elif event.cmd == 'remove_file':
            fname = event.val
            try:
                item = self.tree_ctrl_fs.GetFocusedItem()
                self.tree_ctrl_fs.Delete( item ) 
            except:
                return
        elif event.cmd == 'rename_file':
            (oldname, newname) = event.val
            try:
                item = self.tree_ctrl_fs.GetFocusedItem()
                p, n, s = self.tree_ctrl_fs.GetPyData( item )
                self.tree_ctrl_fs.SetItemText( item, '%s (%d)'% (newname, s) ) 
                self.tree_ctrl_fs.SetPyData(item, (p, newname, s))
            except Exception as e:
                return
        elif event.cmd == 'i2c_data':
            data = event.val
            lines, l = [], []
            while True:
                l.append( '0x%02X'% data.pop(0) )
                if len(l) >= 16:
                    lines.append( ' '.join(l) )
                    l = []
                if not data:
                    lines.append( ' '.join(l) )
                    break
            self.text_ctrl_i2c_data.SetValue( '\n'.join(lines) )
        event.Skip()
 
    def OnStop(self, event):
        self.status = 'manually_stop'
        self.StopSubProcess()
        event.Skip()

    def StopSubProcess(self):
        try:
            #safe = True if self.task.__class__ == MeasureTest else False
            #self.task.terminate( safely=safe )
            self.task.terminate()
        except:
            pass

    def OnTestPort(self, event):
        port = self.combo_box_port.GetValue()
        if port:
            self.task = TestPortTask( (port), self.msgq )
        event.Skip()

    def OnMenuExit(self, event):
        self.doExit()
        #event.Skip()

    def OnTimer( self, event ):
        event.Skip()
        
    def OnRun( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info(_("port error"), wx.ICON_ERROR)
            return
        script = self.text_ctrl_script.GetValue().rstrip()
        if len(script) > 0:
            self.task = ExecuteScriptTask( (port, script), self.msgq )
        event.Skip()

    def OnReset( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info(_("port error"), wx.ICON_ERROR)
            return
        self.task = ResetTask( (port), self.msgq )
        event.Skip()

    def OnReboot( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info(_("port error"), wx.ICON_ERROR)
            return
        self.task = RebootTask( (port), self.msgq )
        event.Skip()

    def OnLoadExample( self, event ):
        global PRESET
        menu = wx.Menu()
        for name, content in PRESET:
            if not self.preset_popup_menu_id.has_key(name):
                newid = wx.NewId()
                self.preset_popup_menu_id[name] = newid
                self.Bind(wx.EVT_MENU, self.OnLoadPreset, id=newid)
            else:
                newid = self.preset_popup_menu_id[name] 
            menu.Append( newid, name )
        self.PopupMenu(menu)
        event.Skip()
 
    def OnLoadPreset( self, event ):
        id = event.GetId()
        for name in self.preset_popup_menu_id.keys():
            if self.preset_popup_menu_id[name] == id:
                for k,v in PRESET:
                    if k == name:
                        self.text_ctrl_script.SetValue(v)
        event.Skip()
 
    def OnLoadFile( self, event ):
        dlg = wx.FileDialog( self, message=_("Choose python script file"), 
                defaultDir=os.getcwd(), #self.config_dir, 
                defaultFile='', wildcard="Python script file (*.py)|*.py", style=wx.OPEN|wx.CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            self.text_ctrl_script.LoadFile( dlg.GetPath().strip() )
        event.Skip()

    def OnSaveAs( self, event ):
        dlg = wx.FileDialog( self, message=_("Choose python script file"), defaultDir=self.config_dir, 
                defaultFile='', wildcard="Python script file (*.py)|*.py", style=wx.SAVE )
        if dlg.ShowModal() == wx.ID_OK:
            open( dlg.GetPath().strip(), 'w+' ).write( self.text_ctrl_script.GetValue().encode('utf-8') )
        event.Skip()
 
    def OnQuery( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info(_("port error"), 'error')
            return
        self.task = QueryTask( (port), self.msgq )
        event.Skip()

    def OnSelectFileItem( self, event ):
        try:
            item = self.tree_ctrl_fs.GetFocusedItem()
            p, n, s = self.tree_ctrl_fs.GetPyData( item )
            self.info( "File %s, size %d"% (p+'/'+n, s) )
        except:
            self.info( "" )

    def OnRemove( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info(_("port error"), 'error')
            return
        try:
            item = self.tree_ctrl_fs.GetFocusedItem()
            p, n, s = self.tree_ctrl_fs.GetPyData( item )
            fname = p + '/' + n
            self.task = RemoveTask( (port, fname), self.msgq )
        except:
            pass
        event.Skip()

    def OnRename( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info(_("port error"), 'error')
            return
        try:
            item = self.tree_ctrl_fs.GetFocusedItem()
            p, n, s = self.tree_ctrl_fs.GetPyData( item )
            oldname = p + '/' + n
        except:
            return
        dlg = wx.TextEntryDialog( self, 'Rename %s to:'% oldname, 'Rename')
        dlg.SetValue( n )
        if dlg.ShowModal() == wx.ID_OK:
            newname = os.path.basename( dlg.GetValue() )
        else:
            return
        if n != newname :
            self.task = RenameTask( (port, oldname, newname), self.msgq )
        event.Skip()

    def OnView( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info(_("port error"), 'error')
            return
        try:
            item = self.tree_ctrl_fs.GetFocusedItem()
            p, n, s = self.tree_ctrl_fs.GetPyData( item )
            fname = p + '/' + n
            self.task = ViewTask( (port, fname), self.msgq )
        except:
            return
        event.Skip()

    def OnGetFile( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info(_("port error"), 'error')
            return
        try:
            item = self.tree_ctrl_fs.GetFocusedItem()
            p, n, s = self.tree_ctrl_fs.GetPyData( item )
            fname = p + '/' + n
        except:
            return
        dlg = wx.FileDialog( self, message=_("Choose destination file"), defaultDir=self.config_dir, 
                defaultFile=n, wildcard="File (*.*)|*.*", style=wx.SAVE )
        if dlg.ShowModal() == wx.ID_OK:
            savename = dlg.GetPath().strip()
        else:
            return
        self.task = GetFileTask( (port, fname, savename), self.msgq )
        event.Skip()

    def OnPutFile( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info(_("port error"), 'error')
            return
        try:
            item = self.tree_ctrl_fs.GetFocusedItem()
            p, n, s = self.tree_ctrl_fs.GetPyData( item )
            if n is not None:
                self.info(_("not a directory"), 'error')
                return
        except:
            return
        dlg = wx.FileDialog( self, message="Choose source file", defaultDir=self.config_dir, 
                defaultFile='', wildcard="File (*.*)|*.*", style=wx.OPEN )
        if dlg.ShowModal() == wx.ID_OK:
            srcname = dlg.GetPath().strip()
        else:
            return
        fname = p + '/' + os.path.basename(srcname)
        self.task = PutFileTask( (port, srcname, fname), self.msgq )
        event.Skip()

    def OnFsMenu( self, event ):
        item = self.tree_ctrl_fs.GetFocusedItem()
        p, n, s = self.tree_ctrl_fs.GetPyData( item )
        menu = wx.Menu()
        if n is None:
            # mount point
            item1 = wx.MenuItem(menu, self.popupID_query, _("Update"))
            menu.AppendItem(item1)
            if not p.startswith('/r'):
                item2 = wx.MenuItem(menu, self.popupID_put, _("Put file"))
                menu.AppendItem(item2)
                item3 = wx.MenuItem(menu, self.popupID_format, _("Format"))
                menu.AppendItem(item3)
                item4 = wx.MenuItem(menu, self.popupID_fsinfo, u"Filesystem info")
                menu.AppendItem(item4)
        else:
            # file
            if not p.startswith('/r'):
                item1 = wx.MenuItem(menu, self.popupID_remove, _("Remove"))
                menu.AppendItem(item1)
                item2 = wx.MenuItem(menu, self.popupID_rename, _("Rename"))
                menu.AppendItem(item2)
            item3 = wx.MenuItem(menu, self.popupID_view, _("View"))
            menu.AppendItem(item3)
            item4 = wx.MenuItem(menu, self.popupID_get, _("Get file"))
            menu.AppendItem(item4)
        self.PopupMenu(menu)
        menu.Destroy()
        event.Skip()
         
    def OnFormat( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info(_("port error"), 'error')
            return
        item = self.tree_ctrl_fs.GetFocusedItem()
        p, n, s = self.tree_ctrl_fs.GetPyData( item )
        if n is None:
            self.task = FormatTask( (port, p), self.msgq )
        event.Skip()

    def OnFsInfo( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info(_("port error"), 'error')
            return
        item = self.tree_ctrl_fs.GetFocusedItem()
        p, n, s = self.tree_ctrl_fs.GetPyData( item )
        if n is None:
            self.task = FsInfoTask( (port, p), self.msgq )
        event.Skip()

    def loadI2cConfig(self):
        config = {}
        config['address'] = str(self.text_ctrl_i2c_address.GetValue().strip())
        config['sda'] = str(self.text_ctrl_i2c_pin_sda.GetValue().strip())
        config['scl'] = str(self.text_ctrl_i2c_pin_scl.GetValue().strip())
        config['delay_us'] = str(self.text_ctrl_i2c_delay_us.GetValue().strip())
        config['reg_from'] = str(self.text_ctrl_i2c_reg_from.GetValue().strip())
        config['reg_to'] = str(self.text_ctrl_i2c_reg_to.GetValue().strip())
        return config
    
    def OnI2cScan(self, event):
        port = self.combo_box_port.GetValue()
        config = self.loadI2cConfig()
        if port and config:
            self.task = I2cScanTask( (port, config), self.msgq )
        event.Skip()

    def OnI2cRead(self, event):
        port = self.combo_box_port.GetValue()
        config = self.loadI2cConfig()
        if port and config:
            self.task = I2cReadTask( (port, config), self.msgq )
        event.Skip()

    def OnI2cWrite(self, event):
        port = self.combo_box_port.GetValue()
        config = self.loadI2cConfig()
        data = []
        for l in self.text_ctrl_i2c_data.GetValue().strip().splitlines():
            for d in l.strip().split():
                data.append( int(eval(d.strip())) )
        if port and config and data:
            self.task = I2cWriteTask( (port, config, data), self.msgq )
        event.Skip()

    def OnI2cLoad(self, event):
        dlg = wx.FileDialog( self, message=_("Choose data file"), defaultDir=self.config_dir, 
                defaultFile='', wildcard="Data file (*.txt)|*.txt", style=wx.OPEN )
        if dlg.ShowModal() == wx.ID_OK:
            self.text_ctrl_i2c_data.LoadFile( dlg.GetPath().strip() )
        event.Skip()

    def OnI2cSave(self, event):
        dlg = wx.FileDialog( self, message=_("Choose data file"), defaultDir=self.config_dir, 
                defaultFile='', wildcard="Data file (*.txt)|*.txt", style=wx.SAVE )
        if dlg.ShowModal() == wx.ID_OK:
            open( dlg.GetPath().strip(), 'w+' ).write( self.text_ctrl_i2c_data.GetValue().encode('utf-8') )
        event.Skip()


def main():
    gettext.install( "messages", "locale", unicode=True, codeset='utf8' )
    freeze_support()
    app = wx.App(0)
    #app.locale = wx.Locale(wx.LANGUAGE_CHINESE_SIMPLIFIED)
    app.locale = wx.Locale(wx.LANGUAGE_DEFAULT)
    app.SetAppName( 'McushDebugApp' )
    frame_1 = MainFrame(None, wx.ID_ANY, "")
    app.SetTopWindow(frame_1)
    frame_1.Show()
    app.MainLoop()


if __name__ == "__main__":
    main()



