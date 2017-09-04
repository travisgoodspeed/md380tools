#!/usr/bin/env python2

# File: md380tools/md380remote.pyw
# Purpose: 'Remote screen viewer' and screenshot utility 
#          for MD380, MD390, RT3, RT8, and the like .
#
# Latest modifications:
# 2017-06: Initial version by DL4YHF, using wxPython V4.0.0,
#          and examples from zetcode.com/wxpython/layout/ .
#          Later modified for wxPython V3.something because 
#          installing wxPython V4.0.0a ("Phoenix") on a certain
#          Linux distro was a painful experience .
#      So stick to wxPython V3 until the Phoenix bird can fly.
#
import wx
import array
from md380_tool import *

dfu = None
online = 0


# ----------------------------------------------------------------------
class RemoteScreenPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, size=(160, 128))
        self.Bind(wx.EVT_PAINT, self.OnPaint)

        self.make_offline_bitmap(self.Size.width, self.Size.height)
        self.timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.OnTimer, self.timer )
        # ex: self.timer.StartOnce(500)  # interval in milliseconds, fire ONCE !
        # -> "AttributeError: 'Timer' object has no attribute 'StartOnce'" . Sigh,,
        self.timer.Start(500, wx.TIMER_ONE_SHOT)  # interval in milliseconds, fire ONCE !
        self.n_errors = 0

    def make_offline_bitmap(self, width, height):
        # Make a bitmap using an array of RGB bytes, similar as lcd_driver.c : LCD_ColorGradientTest()
        bpp = 3  # bytes per pixel
        rgb_bytes = array.array('B', [0] * width*height*bpp)
        h = self.Size.height
        w = self.Size.width

        for y in xrange(height):
            for x in xrange(width):
                offset = y*width*bpp + x*bpp
                r = 255 * x / w     # increasing from left to right: RED
                g = 255 - (255*x/w) # increasing from right to left: GREEN
                b = 255 * y / h     # increasing from top to bottom: BLUE
                rgb_bytes[offset + 0] = r
                rgb_bytes[offset + 1] = g
                rgb_bytes[offset + 2] = b

        # ex: self.rgbBmp = wx.BitmapFromBuffer(width, height, bytes) : "deprecated" in wxPython V4
        # ex: self.rgbBmp = wx.Bitmap.FromBuffer(width, height, bytes) : type object 'Bitmap' has no ..'FromBuffer' .
        self.rgbBmp = wx.BitmapFromBuffer(width, height, rgb_bytes) # back to the 'deprecated' stuff. Grrrr.

    def DrawRemoteScreen(self, dc, bmp):
        dc.DrawBitmap(bmp, 0, 0, True)
        if not online:
          dc.DrawText( "offline",60,60 )

    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        self.DrawRemoteScreen(dc, self.rgbBmp)

    def GrabRemoteScreen(self):
        # Grab the LCD framebuffer via USB and convert to a (wx-) bitmap
        # Read image data with 160 pixels per line, 3 bytes per pixel.
        width = 160 # width of the MD380 LCD in pixels
        height= 128 # height of the MD380 LCD in pixels
        bpp = 3  # bytes per pixel
        bytes = array.array('B', [0] * width*height*bpp)
        h = self.Size.height
        w = self.Size.width
        for y in xrange(height):
            buf = dfu.read_framebuf_line(y)
            if len(buf) == bpp*width: # successfully read from framebuffer
               for x in xrange(width):
                   offset = y*width*bpp + x*bpp
                   bytes[offset + 0] = buf[3*x+2] # red
                   bytes[offset + 1] = buf[3*x+1] # green
                   bytes[offset + 2] = buf[3*x+0] # blue
            else: # framebuffer access failed (e.g. LCD driver or controller was busy)
                self.n_errors +=1  # don't worry, the next periodic update will fix it

        self.rgbBmp = wx.BitmapFromBuffer(width, height, bytes) # "deprecated" in wxPython V4
        # ex: self.rgbBmp = wx.Bitmap.FromBuffer(width, height, bytes) # incompatible with wxPython V3


    def OnTimer(self,event):
        if online:
           self.GrabRemoteScreen()
           self.Refresh() # let wxPython call OnPaint() when it's time to..
           # Allow some time to pass BETWEEN two timer-calls,
           # regardless of how long the above operations have taken.
           # Firing the timer event every 200 ms may be asking too much.
           # ex: self.timer.StartOnce(200) # ok for wxPython V4, incompatible with V3
           self.timer.Start(200, wx.TIMER_ONE_SHOT) # interval in milliseconds, fire ONCE !

#----------------------------------------------------------------------
class RemoteKeyboardPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, size=(160, 128))

        # for the keyboard panel, the "grid" sizer is the "main" sizer.
        gridSizer = wx.GridSizer(4, 4) # rows, cols
        for row in (("M", unichr(0x02C4), unichr(0x02C5), "B"),
                    ("1", "2", "3", "*"),
                    ("4", "5", "6", "0"),
                    ("7", "8", "9", "#")):
            for label in row:
                b = wx.Button(self, -1, label, size=(32,28))
                if label=="M":
                   b.SetBackgroundColour('green')
                if label=="B":
                   b.SetBackgroundColour('red')
                b.Bind(wx.EVT_LEFT_DOWN, self.OnLeftBtnDownForRemote )
                b.Bind(wx.EVT_LEFT_UP,   self.OnLeftBtnUpForRemote )
                gridSizer.Add(b)
        self.SetSizer(gridSizer)
        gridSizer.Fit(self)

    def label_to_key(self, label ):
        # For most buttons, the label is the same as the keyboard code
        # used in md380tools/applet/src/app_menu.c, except these two:
        if label==unichr(0x02C4):
           key= 'U' # 'Up' (cursor key)
        elif label==unichr(0x02C5):
           key= 'D' # 'Down' (cursor key)
        else:
           key= label.encode('ascii','ignore')
        return key

    def OnLeftBtnDownForRemote(self, evt):
        # Get the clicked button's label, and convert to single-char 'key'
        key = self.label_to_key( evt.GetEventObject().GetLabel() )
        if online: # send the key as 'remote control' key ?
           dfu.send_keyboard_event( key.encode('ascii','ignore'), 1)
        # from Robin Dunn:
        # > It may depend on the platform, but I've seen cases where
        # > if the LEFT_DOWN handler eats the event then the system
        # > doesn't see it and won't know that a dclick event should be sent
        # > if there is another left-down within the time out period. 7
        # > Calling Skip should allow it to keep working if you need it.
        evt.Skip() # let others (the "system"?) process this event, too

    def OnLeftBtnUpForRemote(self, evt):
        key = self.label_to_key(evt.GetEventObject().GetLabel())
        if online:  # signal 'key RELEASED' to the remotely controlled rig:
            dfu.send_keyboard_event( key.encode('ascii', 'ignore'), 0)
        evt.Skip() # let others (the "system"?) process this event, too


#----------------------------------------------------------------------
class MainFrame ( wx.Frame ):

    def __init__( self ):

        # loosely based on wiki.wxpython.org/BoxSizerTutorial ..
        wx.Frame.__init__ ( self, None, wx.ID_ANY, title = "MD380 Screenshot",
                                  size = wx.Size( 240,300 ),
                                  style= wx.SYSTEM_MENU | wx.CAPTION | wx.CLOSE_BOX )

        # Add a panel so it looks correct on all platforms
        self.panel = wx.Panel(self, wx.ID_ANY)

        # Create the widgets first.. proceed from top to bottom:
        self.imgPanel = RemoteScreenPanel(self.panel)
        keyPanel   = RemoteKeyboardPanel(self.panel)
        captureBtn = wx.Button(self.panel, wx.ID_ANY, 'Capture')
        exitBtn    = wx.Button(self.panel, wx.ID_ANY, 'Exit')

        # Create 'BoxSizers', used here to align and position widgets on the form
        topSizer = wx.BoxSizer(wx.VERTICAL)
        imgSizer = wx.BoxSizer(wx.HORIZONTAL)
        btnSizer = wx.BoxSizer(wx.HORIZONTAL)

        # Let the 'BoxSizers' know what's inside, from top to bottom.
        # mySizer.Add(window, proportion, flag(s), border [, userData] )
        #  border : number of pixels around the widget that's been added
        imgSizer.Add(self.imgPanel, 0, wx.ALL, 5)
        btnSizer.Add(captureBtn, 0, wx.ALL, 5)
        btnSizer.Add(exitBtn, 0, wx.ALL, 5)
        topSizer.Add(imgSizer, 0, wx.CENTER)
        topSizer.Add(keyPanel, 1, wx.CENTER)
        topSizer.Add(wx.StaticLine(self.panel,), 0, wx.ALL|wx.EXPAND, 5)
        topSizer.Add(btnSizer, 0, wx.ALL|wx.CENTER, 5)

        self.panel.SetSizer(topSizer)
        topSizer.Fit(self)

        # Bind widgets to their event handlers
        self.Bind(wx.EVT_BUTTON, self.OnCapture, captureBtn)
        self.Bind(wx.EVT_BUTTON, self.OnExit, exitBtn)

        # Start capture-file sequence at index ONE
        self.CaptureIndex = 0

    def __del__( self ):
        pass

    def OnCapture(self, event):
        # Try to save the currently visible bitmap (imgPanel.rgbBmp)
        # as a file, without asking dumb questions about where to save it.
        self.CaptureIndex = self.CaptureIndex+1
        self.imgPanel.rgbBmp.SaveFile('screenshot_'+str(self.CaptureIndex)+'.png', wx.BITMAP_TYPE_PNG)

    def OnExit(self, event):
        self.Close(True)


if __name__ == '__main__':
    try:
        dfu = init_dfu()  # raises exception when radio not connected
        online = True
    except Exception:
        online = False
    app = wx.App()
    frame = MainFrame()
    frame.Show()
    app.MainLoop()
