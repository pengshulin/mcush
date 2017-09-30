import time
from mcush import *

m = Android.QMcush( '98:D3:32:20:C2:69' )
m.printInfo()
    
mem = m.readMem( 0x20000000, length=512 )
Utils.dumpMem( mem, method=1 )
m.disconnect()


