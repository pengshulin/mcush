#!/usr/bin/env python3
# WS2812 LED Strap visualizer in posix simulator mode
# MCUSH designed by Peng Shulin, all rights reserved.
# Peng Shulin <trees_peng@163.com> 2022
import os
import sys
import time
import threading
import wx
import wx.lib.newevent
import dbus
from gi.repository import GLib
from dbus.mainloop.glib import DBusGMainLoop

LAMP_SIZE = 20
LAMP_CORNOR = 4
MARGIN = int(LAMP_SIZE / 2)
INTERVAL = int(LAMP_SIZE / 5)
GRID = int(INTERVAL + LAMP_SIZE)


def dbus_signal_handler(*args, **kwargs):
    global dialog_main
    cmd = kwargs['Type']
    #print( args, kwargs )
    if cmd == 'color':
        if (len(args) != 2) or (not isinstance(args[0], int)) or (not isinstance(args[1], int)):
            return
    elif cmd == 'update':
        if (len(args) != 1) or not isinstance(args[0], int):
            return
    else:
        return
    try:
        wx.PostEvent( dialog_main, UpdateEvent( cmd=cmd, val=args ) )
    except RuntimeError:
        pass


(UpdateEvent, EVT_UPDATE) = wx.lib.newevent.NewEvent()

class MyDialog(wx.Dialog):
    def __init__(self, *args, **kwds):
        kwds["style"] = kwds.get("style", 0) | wx.CAPTION | wx.RESIZE_BORDER | wx.CLOSE_BOX | wx.MINIMIZE_BOX | wx.SYSTEM_MENU 
        wx.Dialog.__init__(self, *args, **kwds)

        self.color = []
        self.total_len = 0
        
        self.__set_properties()
        self.__do_layout()

        #self.DBusMonitorThread = threading.Thread( target=self.DBusMonitor )
        #self.DBusMonitorThread.setDaemon(1)
        #self.DBusMonitorThread.start()

        self.lamp_rect = wx.Rect(0, 0, LAMP_SIZE, LAMP_SIZE)

        DBusGMainLoop(set_as_default=True)
        bus = dbus.SessionBus()
        bus.add_signal_receiver(dbus_signal_handler, path='/mcush/posix/sim/ws2812', member_keyword='Type')

        self.Bind(wx.EVT_CLOSE, self.OnClose)
        self.Bind(EVT_UPDATE, self.OnUpdate)

        self.Bind(wx.EVT_PAINT, self.OnPaint)
    
    def __set_properties(self):
        self.SetTitle("WS2812 Emulator")

    def __do_layout(self):
        sizer_whole = wx.BoxSizer(wx.HORIZONTAL)
        sizer_vcenter = wx.BoxSizer(wx.VERTICAL)
        sizer_whole.Add((20, 20), 0, 0, 0)
        sizer_vcenter.Add((20, 20), 0, 0, 0)
        sizer_vcenter.Add((500, 500), 0, 0, 0)
        sizer_vcenter.Add((20, 20), 0, 0, 0)
        sizer_whole.Add(sizer_vcenter, 1, wx.EXPAND, 0)
        sizer_whole.Add((20, 20), 0, 0, 0)
        self.SetSizer(sizer_whole)
        sizer_whole.Fit(self)
        self.Layout()
        self.Centre()

    def OnClose(self,event):
        wx.Exit()
    
    def OnUpdate(self,event):
        #print('%s, %s'% (event.cmd, event.val))
        if event.cmd == 'color':
            offset, color = event.val
            #print('%s, %s'% (offset, color))
            try:
                self.color[offset] = color
            except IndexError:
                self.color.append(color)
        elif event.cmd == 'update':
            total_len = event.val[0]
            if len(self.color) != total_len:
                self.color = self.color[:total_len]
            self.Refresh()
        event.Skip()

    def OnPaint(self, evt):
        pdc = wx.PaintDC(self)
        try:
            dc = wx.GCDC(pdc)
        except:
            dc = pdc
        width, height = dc.GetSize()
        LINE_NUM = int((width-MARGIN*2)/GRID)
        if LINE_NUM == 0:
            return
        for idx, color in enumerate(self.color):
            r, g, b = (color>>16)&0xFF, (color>>8)&0xFF, (color)&0xFF
            dc.SetPen(wx.BLACK_PEN)
            dc.SetBrush(wx.Brush(wx.Colour(r, g, b, 0xFF)))
            idx_x = idx % LINE_NUM 
            idx_y = int(idx / LINE_NUM)
            self.lamp_rect.SetPosition( (idx_x*GRID+MARGIN, idx_y*GRID+MARGIN) )
            dc.DrawRoundedRectangle(self.lamp_rect, LAMP_CORNOR)


    #def DBusMonitor( self ):
        #loop = GLib.MainLoop()
        #loop.run()

    
if __name__ == "__main__":
    app = wx.App(0)
    wx.InitAllImageHandlers()
    dialog_main = MyDialog(None, wx.ID_ANY, "")
    app.SetTopWindow(dialog_main)
    dialog_main.Show()
    app.MainLoop()
