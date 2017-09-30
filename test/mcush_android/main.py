# Same as before, with a kivy-based UI
 
#'''
#Bluetooth/Pyjnius example
#=========================
#This was used to send some bytes to an arduino via bluetooth.
#The app must have BLUETOOTH and BLUETOOTH_ADMIN permissions (well, i didn't
#tested without BLUETOOTH_ADMIN, maybe it works.)
#Connect your device to your phone, via the bluetooth menu. After the
#pairing is done, you'll be able to use it in the app.
#'''
# 
#from jnius import autoclass
# 
#BluetoothAdapter = autoclass('android.bluetooth.BluetoothAdapter')
#BluetoothDevice = autoclass('android.bluetooth.BluetoothDevice')
#BluetoothSocket = autoclass('android.bluetooth.BluetoothSocket')
#UUID = autoclass('java.util.UUID')
# 
#def get_socket_stream(name):
#    paired_devices = BluetoothAdapter.getDefaultAdapter().getBondedDevices().toArray()
#    socket = None
#    for device in paired_devices:
#        if device.getName() == name:
#            socket = device.createRfcommSocketToServiceRecord(
#                UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))
#            recv_stream = socket.getInputStream()
#            send_stream = socket.getOutputStream()
#            break
#    socket.connect()
#    return recv_stream, send_stream
 
if __name__ == '__main__':
    kv = '''
BoxLayout:
    Button:
        text: 'connect'
        on_release: app.connect()
    ToggleButton:
        id: b1
        text: 'led -i0 -t'
        on_release: app.command(self.text)
    ToggleButton:
        id: b2
        text: 'led -i1 -t'
        on_release: app.command(self.text)
    ToggleButton:
        id: b3
        text: '*idn?'
        on_release: app.command(self.text)
    ToggleButton:
        id: b4
        text: 'beep'
        on_release: app.command(self.text)
    ToggleButton:
        id: b5
        text: '*rst'
        on_release: app.command(self.text)
    '''
    from kivy.lang import Builder
    from kivy.app import App
 
    class Bluetooth(App):
        def build(self):
            from mcush import Android
            self.m = Android.KMcush('mcush', connect=False)
            return Builder.load_string(kv)

        def connect(self):
            self.m.connect()
 
        def command(self, cmd):
            self.m.writeCommand(cmd)
 
        def reset(self, btns):
            for btn in btns:
                btn.state = 'normal'
            self.command('reboot')
 
    Bluetooth().run()
