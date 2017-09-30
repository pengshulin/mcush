import time
from mcush import *

m = Android.QMcush( '98:D3:32:20:C2:69' )
m.vibrate()
m.printInfo()
for i in range(10):
    print '[%d]'% i
    m.led( 1, toggle=True ) 
    time.sleep(0.5)
m.vibrate()
m.disconnect()


