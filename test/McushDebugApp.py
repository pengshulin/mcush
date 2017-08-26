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
import ConfigParser
import wx
import wx.lib.newevent
import wx.stc as stc
import StringIO
from copy import deepcopy
from multiprocessing import Queue, freeze_support
from threading import Thread
import Env
import Utils
from AppUtils import Task
from Mcush import *
from McushDebugDlg import *
from re import compile as re_compile
import base64


(UpdateEvent, EVT_UPDATE) = wx.lib.newevent.NewEvent()
import gettext

PRESET = [ \
["Output impulse counter", '''\
pin = '0.0'  # PA0
s = Mcush(PORT)
s.gpio( pin, o=True )
counter = 0
while True:
    info( 'Counter: %d'% counter )
    s.gpio( pin, s=True )
    sleep(0.1)
    s.gpio( pin, c=True )
    sleep(0.5)
    counter += 1
'''],

["SGPIO output", '''\
# PA[0:2]
s = Mcush(PORT)
port = 0  # PA
pins = 7  # [0:2]
freq = 1
buf = [0, 1, 2, 3, 4, 5, 6, 7]
s.sgpio( port, pins, buf, freq, loop=True )
'''],

]

def STOP(message=''):
    raise StopError(message)

task_obj = None
def info(message=''):
    global task_obj
    task_obj.info( message )

PORT=None

class MyTask(Task):
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
        self.info( u"Test port %s..."% port )
        s = Mcush(port)
        self.updateInfo( s.getModel(), s.getVersion() )
        s.disconnect()
        self.info( u"Done" )

class ResetTask(MyTask):
    def target( self, args ):
        (port) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        self.info( u"reset..." )
        s.scpiRst()
        self.info( u"Done" )

class RebootTask(MyTask):
    def target( self, args ):
        (port) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        self.info( u"reboot..." )
        s.ser.write('reboot\n')
        s.ser.flush()
        self.info( u"Done" )


class ExecuteScriptTask(MyTask):
    def target( self, args ):
        global PORT, task_obj
        self.info( u"Execute code..." )
        (port, code, script) = args
        task_obj = self
        PORT = port
        exec( code )
        self.info( u"Done" )

class QueryTask(MyTask):
    def target( self, args ):
        (port) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        self.info( u"querying..." )
        try:
            lst = s.list( )
            self.queue.put( ('update_filelist', lst) )
            self.info( u"Done" )
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
            self.info( "Done" )
        except Instrument.CommandExecuteError, e:
            self.info( "Failed to remove %s"% fname, 'error' )

class RenameTask(MyTask):
    def target( self, args ):
        (port, oldname, newname) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        self.info( u"rename %s to %s..."% (oldname, newname) )
        try:
            s.rename( oldname, newname )
            self.info( "Done" )
        except Instrument.CommandExecuteError, e:
            self.info( u"Failed to rename %s to %s..."% (oldname, newname), 'error' )

class ViewTask(MyTask):
    def target( self, args ):
        (port, fname) = args
        self.info( u"Open port %s..."% port )
        s = Mcush(port)
        self.info( u"download file %s..."% (fname) )
        try:
            r = s.cat( fname, b64=True )
            r = base64.b64decode('\n'.join(r))
            self.queue.put( ('view_file', r) )
            self.info( "Done" )
        except Instrument.CommandExecuteError, e:
            self.info( u"Failed to download file %s"% (fname), 'error' )



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
            {'empty.png': ['empty','none'],
             'start.png': ['start'],
             'dialog-ok-apply.png': ['ok', 'done'],
             'failed.png': ['failed', 'error', 'halt'],
             'message.png': ['info', 'message']} )

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
        except Exception, e:
            print 'loadconfig failed'
            mode = False

    def saveConfig( self ):
        cfgfile = ConfigParser.ConfigParser()
        cfgfile.add_section( 'config' )
        try:
            cfgfile.set( 'config', 'port', self.combo_box_port.GetValue().encode('utf-8') ) 
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
            self.button_run.Enable( not lock )
            self.button_reset.Enable( not lock )
            self.button_query.Enable( not lock )
            self.button_remove.Enable( not lock )
            self.button_rename.Enable( not lock )
            self.button_view.Enable( not lock )
            #self.notebook_1.Enable( not lock )
            self.button_stop.Show( lock )
            self.Layout()
        elif event.cmd == 'update_filelist':
            lst = event.val
            self.tree_ctrl_fs.DeleteAllItems()
            root = self.tree_ctrl_fs.AddRoot('/')
            subroot = {}
            for p, n, s in lst:
                print p, n, s
                if not subroot.has_key( p ):
                    subroot[p] = self.tree_ctrl_fs.AppendItem(root, p)
                if n is None:
                    continue
                item = self.tree_ctrl_fs.AppendItem(subroot[p], n + ' (%d)'%s)
                self.tree_ctrl_fs.SetPyData(item, (p, n, s))
            self.tree_ctrl_fs.ExpandAll()
        elif event.cmd == 'view_file':
            contents = event.val
            print contents
            dlg = MyViewFileDialog(self)
            dlg.text_ctrl_data.SetValue( contents )
            dlg.ShowModal()
            
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
            self.info("port error", wx.ICON_ERROR)
            return
        try:
            script = self.text_ctrl_script.GetValue()
            code = compile(script, '', 'exec')
        except Exception as e:
            self.info(unicode(e), wx.ICON_ERROR)
            return
        self.task = ExecuteScriptTask( (port, code, script), self.msgq )
        event.Skip()

    def OnReset( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info("port error", wx.ICON_ERROR)
            return
        self.task = ResetTask( (port), self.msgq )
        event.Skip()

    def OnReboot( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info("port error", wx.ICON_ERROR)
            return
        self.task = RebootTask( (port), self.msgq )
        event.Skip()

    def OnLoad( self, event ):
        global PRESET
        menu = wx.Menu()
        sm = wx.Menu()
        for name, content in PRESET:
            if not self.preset_popup_menu_id.has_key(name):
                newid = wx.NewId()
                self.preset_popup_menu_id[name] = newid
                self.Bind(wx.EVT_MENU, self.OnLoadPreset, id=newid)
            else:
                newid = self.preset_popup_menu_id[name] 
            sm.Append( newid, name )
        menu.AppendMenu(wx.NewId(), u"Load preset", sm)
        menu.AppendSeparator()
        newid = wx.NewId()
        self.Bind(wx.EVT_MENU, self.OnLoadFromFile, id=newid)
        item = wx.MenuItem(menu, newid, u"Load file...")
        menu.AppendItem(item)
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
 
    def OnLoadFromFile( self, event ):
        dlg = wx.FileDialog( self, message="Choose python script file", defaultDir=self.config_dir, 
                defaultFile='', wildcard="Python script file (*.py)|*.py", style=wx.OPEN )
        if dlg.ShowModal() == wx.ID_OK:
            self.text_ctrl_script.LoadFile( dlg.GetPath().strip() )
        event.Skip()

    def OnSaveAs( self, event ):
        dlg = wx.FileDialog( self, message="Choose python script file", defaultDir=self.config_dir, 
                defaultFile='', wildcard="Python script file (*.py)|*.py", style=wx.SAVE )
        if dlg.ShowModal() == wx.ID_OK:
            open( dlg.GetPath().strip(), 'w+' ).write( self.text_ctrl_script.GetValue().encode('utf-8') )
        event.Skip()
 
    def OnQuery( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info("port error", wx.ICON_ERROR)
            return
        self.task = QueryTask( (port), self.msgq )
        event.Skip()

    def OnSelectFileItem( self, event ):
        try:
            item = self.tree_ctrl_fs.GetFocusedItem()
            p, n, s = self.tree_ctrl_fs.GetPyData( item )
            self.info( "File %s, size %d"% (os.path.join(p,n), s) )
        except:
            self.info( "" )

    def OnRemove( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info("port error", wx.ICON_ERROR)
            return
        try:
            item = self.tree_ctrl_fs.GetFocusedItem()
            p, n, s = self.tree_ctrl_fs.GetPyData( item )
            fname = os.path.join(p,n)
            self.task = RemoveTask( (port, fname), self.msgq )
        except:
            pass
        event.Skip()

    def OnRename( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info("port error", wx.ICON_ERROR)
            return
        try:
            item = self.tree_ctrl_fs.GetFocusedItem()
            p, n, s = self.tree_ctrl_fs.GetPyData( item )
            oldname = os.path.join(p,n)
        except:
            return
        dlg = wx.TextEntryDialog( self, 'Rename %s to:'% oldname, 'Rename')
        dlg.SetValue( n )
        if dlg.ShowModal() == wx.ID_OK:
            newname = dlg.GetValue().encode('utf8')
        else:
            return

        self.task = RenameTask( (port, oldname, newname), self.msgq )
        event.Skip()

    def OnView( self, event ):
        port = self.combo_box_port.GetValue()
        if not port:
            self.info("port error", wx.ICON_ERROR)
            return
        try:
            item = self.tree_ctrl_fs.GetFocusedItem()
            p, n, s = self.tree_ctrl_fs.GetPyData( item )
            fname = os.path.join(p,n)
            self.task = ViewTask( (port, fname), self.msgq )
        except:
            return
        event.Skip()



 
def main():
    gettext.install("app") # replace with the appropriate catalog name
    freeze_support()
    app = wx.App(0)
    app.locale = wx.Locale(wx.LANGUAGE_CHINESE_SIMPLIFIED)
    app.SetAppName( 'McushDebugApp' )
    frame_1 = MainFrame(None, wx.ID_ANY, "")
    app.SetTopWindow(frame_1)
    frame_1.Show()
    app.MainLoop()


if __name__ == "__main__":
    main()



