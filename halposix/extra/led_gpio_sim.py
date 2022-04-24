#!/usr/bin/env python3
# LED/GPIO visualizer in posix simulator mode
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


LED_NUM = 5
GPIO_NUM = 5

COLOR_PNGS = [
'emblem-colors-blue.png',
'emblem-colors-brown.png',
'emblem-colors-green.png',
'emblem-colors-grey.png',
'emblem-colors-orange.png',
'emblem-colors-red.png',
'emblem-colors-violet.png',
'emblem-colors-white.png',
'emblem-colors-yellow.png']



def dbus_signal_handler(*args, **kwargs):
    global dialog_main
    #print( args, kwargs )
    cmd = kwargs['Type']
    if cmd in ['gpio_set', 'gpio_clr', 'gpio_toggle', 'led_set', 'led_clr', 'led_toggle']:
        try:
            wx.PostEvent( dialog_main, UpdateEvent( cmd=cmd, val=args ) )
        except RuntimeError:
            pass


(UpdateEvent, EVT_UPDATE) = wx.lib.newevent.NewEvent()

class MyDialog(wx.Dialog):
    def __init__(self, *args, **kwds):
        kwds["style"] = kwds.get("style", 0) | wx.CAPTION | wx.CLOSE_BOX | wx.MINIMIZE_BOX | wx.SYSTEM_MENU 
        wx.Dialog.__init__(self, *args, **kwds)
        self.icons = self.loadIcons()
        self.button_leds = []
        self.state_leds = [None for i in range(LED_NUM)]
        for i in range(LED_NUM):
            self.button_leds.append( wx.BitmapButton(self, wx.ID_ANY, self.icons['grey'] ) )
        self.button_gpios = []
        self.output_gpios = [[None]*16 for i in range(GPIO_NUM)]
        self.state_gpios = [[None]*16 for i in range(GPIO_NUM)]
        for i in range(GPIO_NUM):
            lst = []
            for i in range(16):
                lst.append( wx.BitmapButton(self, wx.ID_ANY, self.icons['grey'] ) )
            self.button_gpios.append( lst )
        
        self.__set_properties()
        self.__do_layout()

        #self.DBusMonitorThread = threading.Thread( target=self.DBusMonitor )
        #self.DBusMonitorThread.setDaemon(1)
        #self.DBusMonitorThread.start()

        DBusGMainLoop(set_as_default=True)
        bus = dbus.SessionBus()
        bus.add_signal_receiver(dbus_signal_handler, path='/mcush/posix/sim/gpio', member_keyword='Type')
        bus.add_signal_receiver(dbus_signal_handler, path='/mcush/posix/sim/led', member_keyword='Type')

        self.Bind(wx.EVT_CLOSE, self.OnClose)
        self.Bind(EVT_UPDATE, self.OnUpdate)
    
    def loadIcons( self ):
        icons = {}
        for imgfname in COLOR_PNGS:
            img = wx.Bitmap(os.path.join('img',imgfname), wx.BITMAP_TYPE_ANY)
            name = os.path.splitext(imgfname)[0].split('-')[-1]
            icons[name] = img
        return icons

    def __set_properties(self):
        self.SetTitle("GPIO/LEDs emulator")
        for btn in self.button_leds:
            btn.SetMinSize(btn.GetBestSize())
        for lst in self.button_gpios:
            for btn in self.button_leds:
                btn.SetMinSize(btn.GetBestSize())

    def __do_layout(self):
        sizer_whole = wx.BoxSizer(wx.HORIZONTAL)
        sizer_vcenter = wx.BoxSizer(wx.VERTICAL)
        sizer_whole.Add((20, 20), 0, 0, 0)
        sizer_vcenter.Add((20, 20), 0, 0, 0)
        sizer_leds = wx.BoxSizer(wx.HORIZONTAL)
        for bidx, btn in enumerate(self.button_leds):
            btn.SetToolTip( 'LED #%d'% bidx )
            sizer_leds.Add(btn, 0, wx.ALIGN_CENTER_VERTICAL, 0)
        sizer_vcenter.Add(sizer_leds, 1, wx.EXPAND, 0)
        sizer_vcenter.Add((20, 10), 0, 0, 0)
        self.sizer_gpios = []
        for gidx, glst in enumerate(self.button_gpios):
            sizer_gpio = wx.BoxSizer(wx.HORIZONTAL)
            for bidx, btn in enumerate(glst):
                sizer_gpio.Add(btn, 0, wx.ALIGN_CENTER_VERTICAL, 0)
                btn.SetToolTip( 'GPIO %d.%d'% (gidx, bidx) )
                if bidx % 8 == 7:
                    sizer_gpio.Add((5,5), 0, 0, 0)
            sizer_vcenter.Add(sizer_gpio, 1, wx.EXPAND, 0)
            sizer_vcenter.Add((2,2), 0, 0, 0)
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
        if event.cmd in ['led_set', 'led_clr', 'led_toggle']:
            cmd = event.cmd[4]
            idx = int(event.val[0])
            if cmd == 's':
                self.state_leds[idx] = True
            elif cmd == 'c':
                self.state_leds[idx] = False
            elif cmd == 't':
                self.state_leds[idx] = not self.state_leds[idx]
            color = 'blue' if self.state_leds[idx] else 'grey'
            self.button_leds[idx].SetBitmap( self.icons[color] )
        elif event.cmd in ['gpio_set', 'gpio_clr', 'gpio_toggle']:
            cmd = event.cmd[5]
            port, bits = int(event.val[0]), int(event.val[1])
            for i in range(16):
                if bits & (1<<i):
                    if cmd == 's':
                        self.state_gpios[port][i] = True
                    elif cmd == 'c':
                        self.state_gpios[port][i] = False
                    elif cmd == 't':
                        self.state_gpios[port][i] = not self.state_gpios[port][i]
                    color = 'green' if self.state_gpios[port][i] else 'red'
                    self.button_gpios[port][i].SetBitmap( self.icons[color] )
        event.Skip()

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
