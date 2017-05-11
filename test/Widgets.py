# encoding:utf8
__doc__ = 'widgets'
__author__ = 'Peng Shulin <trees_peng@163.com>'
import wx
from binascii import hexlify
from wx import Panel


class SegmentMemoryPanel(Panel):

    def __init__(self, parent, id=-1, pos=wx.DefaultPosition, size=wx.DefaultSize, 
                style=wx.TAB_TRAVERSAL|wx.NO_BORDER, name='SegmentMemory', 
                has_segment_name=True, demo=True, has_ascii=True, bus_width=8,
                font=None ):

        Panel.__init__( self, parent, id, pos, size, style, name )
        self.SetBackgroundColour(wx.Colour(255,255,255))
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.label_title = wx.StaticText( self, wx.ID_ANY, name )
        self.label_title.SetFont(wx.Font(12, wx.DEFAULT, wx.NORMAL, wx.NORMAL, 0, ""))
        self.sizer.Add( self.label_title, 1, wx.EXPAND)
        self.sizer.Add( (20, 5), 0, 0, 0 )
        self.sizer_dat = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer.Add( self.sizer_dat, 0, 0, 0 )
        self.sizer_addr = wx.BoxSizer(wx.VERTICAL)
        self.sizer_val = wx.BoxSizer(wx.VERTICAL)
        self.sizer_ascii = wx.BoxSizer(wx.VERTICAL)
        self.sizer_dat.Add( self.sizer_addr, 0, wx.EXPAND, 0 )
        self.sizer_dat.Add( (20, 20), 0, 0, 0 )
        self.sizer_dat.Add( self.sizer_val, 0, wx.EXPAND, 0 )
        self.sizer_dat.Add( (20, 20), 0, 0, 0 )
        self.sizer_dat.Add( self.sizer_ascii, 0, wx.EXPAND, 0 )
        self.SetSizer(self.sizer)
        self.sizer.FitInside(self)
        self.sizer.Fit(self)
        self.SetMinSize( (500,10) )
        self.add_line(0x12345678, 'afdasdffasdfasdf' )
        self.add_line(0x12345678, 'afdasdffasdfasdf' )
        
        self.sizer.FitInside(self)
        self.sizer.Fit(self)
        size = self.GetSize()
        self.SetMinSize(size)
        print self.GetSize()
        
 
    def add_line(self, addr, mem, has_ascii=True):
        assert 0 <= len(mem) <= 16
        ctrl_addr = wx.StaticText(self, wx.ID_ANY, "%08X"% addr )
        ctrl_addr.SetFont(wx.Font(9, wx.TELETYPE, wx.NORMAL, wx.BOLD, 0, ""))
        ctrl_addr.SetForegroundColour( wx.Colour(128,128,128) )
        self.sizer_addr.Add(ctrl_addr, 0, 0, 0 )
        lst = [hexlify(c) for c in mem]
        if len(lst) > 8:
            lst.insert(8, '' )
        #print lst
        ctrl_val = wx.StaticText(self, wx.ID_ANY, ' '.join(lst) )
        ctrl_val.SetFont(wx.Font(9, wx.TELETYPE, wx.NORMAL, wx.NORMAL, 0, ""))
        self.sizer_val.Add(ctrl_val, 0, 0, 0 )
        if has_ascii:
            lst = [ c if 32<=ord(c)<=126 else '.' for c in mem ]
            ctrl_ascii = wx.StaticText(self, wx.ID_ANY, ''.join(lst) )
        else:
            ctrl_ascii = wx.StaticText(self, wx.ID_ANY, '' )
        ctrl_ascii.SetFont(wx.Font(9, wx.TELETYPE, wx.NORMAL, wx.NORMAL, 0, ""))
        self.sizer_ascii.Add(ctrl_ascii, 0, 0, 0 )



