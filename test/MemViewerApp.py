#!/usr/bin/env python
# coding:utf8
__doc__ = 'Memory Viewer App'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
import os
import sys
import wx
import wx.lib.newevent
from multiprocessing import Queue, freeze_support
import Env
import Utils
import Instrument
from Instrument import reg
from AppUtils import *
from MemViewerDlg import *
from Widgets import SegmentMemoryPanel

ABOUT_INFO = '''\
Memory Viewer Application for MCUSH

URL: https://github.com/pengshulin/mcush
Peng Shullin <trees_peng@163.com> 2017
'''


(UpdateEvent, EVT_UPDATE) = wx.lib.newevent.NewEvent()


class MainFrame(MemViewerFrame):

    def __init__(self, *args, **kwargs):
        MemViewerFrame.__init__( self, *args, **kwargs )
        for i in range(10):
            m = SegmentMemoryPanel(self.panel_main, -1)
            self.sizer_container.Add(m, 0, 0)
            self.sizer_container.Add((20, 5), 0, 0, 0)
            l = wx.StaticLine(self.panel_main, wx.ID_ANY)
            self.sizer_container.Add( l, 0, wx.EXPAND, 0 )
            self.sizer_container.Add((20, 5), 0, 0, 0)
        self.sizer_container.FitInside(self.panel_main)
        self.sizer_container.Fit(self)
        _w, _h = self.panel_main.GetSize()
        self.panel_main.SetMinSize( (_w+80, min(500, _h)) )
        self.sizer_main.Fit(self)
        self.Center()

    def OnAbout(self, event):
        dlg = wx.MessageDialog(self, ABOUT_INFO, 'About', wx.OK)
        dlg.ShowModal()
        dlg.Destroy()


def main():
    gettext.install("app")
    freeze_support()
    app = wx.App(0)
    app.locale = wx.Locale(wx.LANGUAGE_CHINESE_SIMPLIFIED)
    app.SetAppName( 'MemoryViewerApp' )
    frame_1 = MainFrame(None, wx.ID_ANY, "")
    app.SetTopWindow(frame_1)
    frame_1.Show()
    app.MainLoop()


if __name__ == "__main__":
    main()

