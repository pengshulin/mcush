#!/usr/bin/env python
# coding:utf8
from mcush import Env
if Env.platform == 'linux2':
    from mcush import Mcush
    CONTROLLER = Mcush.Mcush
    PORT = '/dev/ttyACM0'
else:
    import mcush.android
    from mcush import Kivy
    CONTROLLER = Kivy.KMcush
    PORT = 'mcush'
from kivy.lang import Builder
from kivy.app import App
from kivy.clock import Clock
 
 
kv = '''
BoxLayout:
    orientation: 'vertical'
    spacing: 2

    Button:
        text: 'connect'
        on_release: app.connect()
    Button:
        id: btn_led0
        text: 'led 0'
        on_press: app.command('led -i0 -s')
        on_release: app.command('led -i0 -c')
    Button:
        id: btn_led1
        text: 'led 1'
        on_press: app.command('led -i1 -s')
        on_release: app.command('led -i1 -c')
    Button:
        id: btn_idn
        text: '*idn?'
        on_release: info.text = app.command('*idn?')
    Button:
        id: btn_gpio
        text: 'gpio -p0'
        on_release: info.text = app.command('gpio -p0')
    Button:
        id: btn_uptime
        text: 'uptime'
        on_release: info.text = app.command('uptime')
    Button:
        id: btn_beep
        text: 'beep'
        on_release: app.command('beep')
    Button:
        id: btn_reboot
        text: 'reboot'
        on_release: app.command('reboot')
    Label:
        id: info
        text: ''
        text_size: self.size
        halign: 'center'
        valign: 'middle'

'''

class BluetoothApp(App):
    def build(self):
        self.m = CONTROLLER(PORT, connect=False)
        Clock.schedule_interval(self.timer, 1)
        return Builder.load_string(kv)

    def connect(self):
        print( 'BluetoothApp.connect', dir(self) )
        print( 'BluetoothApp.connect', dir(self.root) )
        print( 'BluetoothApp.connect', dir(self.root.children) )
        self.m.connect()
        if self.m.connected:
            try:
                self.m.vibrate()
            except:
                self.m.beep()

    def timer(self, dt):
        if not self.m.connected:
            self.connect()
        else:
            self.uptime = self.m.writeCommand('uptime')[0]
            print( "uptime %s"% self.uptime )
            #self.root.info.text = "uptime %s"% self.uptime

    def command(self, cmd):
        if self.m.connected:
            try:
                self.m.vibrate()
            except:
                self.m.beep()
            return ','.join(self.m.writeCommand(cmd))
        else:
            return 'not connected'


BluetoothApp().run()

