# coding: utf8
__doc__ = 'abstract bitmap canvas with common 2d graphic libs'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from . import Bitmap
from . import Font


# abstract canvas
class Canvas():

    def __init__( self, display ):
        self.display = display
        self.width = display.width
        self.height = display.height

    def flush( self, force=False ):
        # flush screen
        raise NotImplementedError

    def setPixel( self, x, y, color=None, flush=True ):
        # set pixel
        raise NotImplementedError

    def clrScr( self, flush=True ):
        # clear screen
        for y in range(self.height):
            for x in range(self.width):
                self.setPixel( x, y, color=0, flush=False )
        if flush:
            self.flush()

    def drawVLine( self, x, y0, y1, color=None, flush=True ):
        # draw vertical line
        if y1 < y0:
            y0, y1 = y1, y0
        for y in range(y0, y1+1):
            self.setPixel( x, y, color, flush=False )
        if flush:
            self.flush()

    def drawHLine( self, y, x0, x1, color=None, flush=True ):
        # draw horizontal line
        if x1 < x0:
            x0, x1 = x1, x0
        for x in range(x0, x1+1):
            self.setPixel( x, y, color, flush=False )
        if flush:
            self.flush()
 
    def drawRectangle( self, x0, y0, x1, y1, color=None, fill=True, flush=True ):
        # draw rectangle 
        if x0 == x1:
            self.drawVLine( x0, y0, y1, color, flush=False )
        elif y0 == y1:
            self.drawHLine( y0, x0, x1, color, flush=False )
        else:
            self.drawHLine( y0, x0, x1, color, flush=False )
            self.drawHLine( y1, x0, x1, color, flush=False )
            if fill:
                for y in range(y0+1, y1):
                    self.drawHLine( y, x0, x1, color, flush=False )
            else:
                self.drawVLine( x0, y0, y1, color, flush=False )
                self.drawVLine( x1, y0, y1, color, flush=False )
        if flush:
            self.flush()
    
    def drawLine( self, x0, y0, x1, y1, color=None, flush=True ):
        # draw normal line
        if x1 < x0:
            x0, y0, x1, y1 = x1, y1, x0, y0
        if x0 == x1:
            self.drawVLine( x0, y0, y1, color, flush=False )
        elif y0 == y1:
            self.drawHLine( y0, x0, x1, color, flush=False )
        else:
            self.setPixel( x0, y0, color, flush=False )
            self.setPixel( x1, y1, color, flush=False )
            k = float(y1-y0)/(x1-x0)
            if abs(k) < 1.0:
                for x in range(x0+1, x1):
                    y = int(y0+(x-x0)*k+0.5)
                    self.setPixel( x, y, color, flush=False )
            else:
                if k > 0:
                    for y in range(y0+1, y1):
                        x = int(x0+(y-y0)/k+0.5)
                        self.setPixel( x, y, color, flush=False )
                else:
                    for y in range(y0-1, y1, -1):
                        x = int(x0+(y-y0)/k+0.5)
                        self.setPixel( x, y, color, flush=False )
        if flush:
            self.flush()
 
    def drawBitmap( self, x, y, bitmap, mode='normal', flush=True ):
        # draw bitmap
        for i in range(bitmap.height):
            for j in range(bitmap.width):
                c = bitmap.getPixel(j, i)
                if mode == 'normal':
                    self.setPixel( x+j, y+i, int(c), flush=False )
                elif mode == 'transparent':
                    if c:
                        self.setPixel( x+j, x+i, flush=False )
        if flush: 
            self.flush() 

    def drawString( self, x, y, string, mode='normal', font=None, flush=True ):
        # draw text
        if font is None:
            font = Font.DEFAULT_FONT
        for s in string:
            if not s in font:
                s = None
            elif font[s] is None:
                s = '?'
            bitmap = font[s]
            self.drawBitmap( x, y, bitmap, mode, flush=False )
            x += bitmap.width
            if x >= self.width:
                break
        if flush: 
            self.flush() 

    def getStringRenderSize( self, string, font=None ):
        # calculate string render size
        width = 0
        height = 0
        if font is None:
            font = Font.DEFAULT_FONT
        for s in string:
            if not s in font:
                s = None
            elif font[s] is None:
                s = '?'
            bitmap = font[s]
            w, h = bitmap.width, bitmap.height
            width += w
            if h > height:
                height = h
        return (width, height) 

