# coding:utf8
import time
from mcush import *

layout="""<?xml version="1.0" encoding="utf-8"?>
<RelativeLayout android:id="@+id/MainWidget"
    android:layout_width="fill_parent" android:layout_height="fill_parent"
    xmlns:android="http://schemas.android.com/apk/res/android">
    <LinearLayout android:layout_width="fill_parent"
        android:layout_height="fill_parent" android:orientation="vertical">

        <TextView android:id="@+id/idn" android:layout_width="fill_parent"
            android:layout_height="fill_parent" android:textColor="#0bda51"
            android:text="Model/Version/SerialNumber" android:layout_weight="1" android:gravity="center" />

        <LinearLayout android:layout_width="fill_parent"
            android:layout_weight="1" android:layout_height="fill_parent"
            android:orientation="horizontal">
            <TextView android:id="@+id/lb2" android:layout_width="fill_parent"
                android:layout_height="fill_parent" android:textColor="#0bda51"
                android:text="GPIO A:" android:layout_weight="1" android:gravity="center" />
            <TextView android:id="@+id/gpioa" android:layout_width="fill_parent"
                android:layout_height="fill_parent" android:textColor="#0bda51"
                android:text="-----" android:layout_weight="1" android:gravity="center" />
            <TextView android:id="@+id/lb3" android:layout_width="fill_parent"
                android:layout_height="fill_parent" android:textColor="#0bda51"
                android:text="GPIO B:" android:layout_weight="1" android:gravity="center" />
            <TextView android:id="@+id/gpiob" android:layout_width="fill_parent"
                android:layout_height="fill_parent" android:textColor="#0bda51"
                android:text="-----" android:layout_weight="1" android:gravity="center" />
        </LinearLayout>
        <Button android:id="@+id/getstatus" android:layout_width="fill_parent"
            android:layout_weight="1" android:layout_height="fill_parent"
            android:text="Reflash Status" />

        <TextView android:id="@+id/lb5" android:layout_width="fill_parent"
            android:layout_height="fill_parent" android:textColor="#0bda51"
            android:text="LEDs" android:layout_weight="1" android:gravity="center" />
        <LinearLayout android:layout_width="fill_parent" android:layout_weight="1" 
            android:layout_height="fill_parent" android:orientation="horizontal">
            <Button android:id="@+id/LED0" android:layout_width="fill_parent"
                android:layout_weight="1" android:layout_height="fill_parent"
                android:text="LED0" />
            <Button android:id="@+id/LED1" android:layout_width="fill_parent"
                android:layout_weight="1" android:layout_height="fill_parent"
                android:text="LED1" />
        </LinearLayout>

        <TextView android:id="@+id/lb7" android:layout_width="fill_parent"
            android:layout_height="fill_parent" android:textColor="#0bda51"
            android:text="Control" android:layout_weight="1" android:gravity="center" />
        <LinearLayout android:layout_width="fill_parent"
            android:layout_weight="1" android:layout_height="fill_parent"
            android:orientation="horizontal">
            <Button android:id="@+id/reboot" android:layout_width="fill_parent"
                android:layout_weight="1" android:layout_height="fill_parent"
                android:text="Reboot" />
            <Button android:id="@+id/disconnectexit" android:layout_width="fill_parent"
                android:layout_height="fill_parent" android:text="Disconnect + Exit"
                android:layout_weight="1" />
        </LinearLayout>
        <TextView android:id="@+id/info" android:layout_width="fill_parent"
            android:layout_height="fill_parent" android:textColor="#FFFFFF"
            android:text="" android:layout_weight="1" android:gravity="center" />

    </LinearLayout>
</RelativeLayout>
"""

def eventloop( m ):
    droid = m.port.android
    free_slot_index = 0
    while True:
        try:
            event = droid.eventWait(50).result
            if event is None:
                free_slot_index += 1
                if free_slot_index == 1:
                    info( 'uptime: %s'%  m.uptime() )
                elif free_slot_index == 2:
                    droid.fullSetProperty("gpioa","text",'0x%04X'% m.gpio( '0' ) )
                elif free_slot_index == 3:
                    droid.fullSetProperty("gpiob","text",'0x%04X'% m.gpio( '1' ) )
                else:
                    free_slot_index = 0
                continue
            #print(event)
            ename = event["name"]
            edata = event["data"]
            if ename == "key":
                droid.vibrate(30)
                if edata == '4':
                    return
            elif ename == "screen":
                if edata=="destroy":
                    return
            elif ename == "click":
                eid = edata['id']
                if eid == "disconnectexit":
                    return
                elif eid == "LED0":
                    #print("LED0")
                    m.led( 0, toggle=True )
                elif eid == "LED1":
                    #print("LED1")
                    m.led( 1, toggle=True )
                elif eid == "getstatus":
                    droid.fullSetProperty("gpioa","text",'0x%04X'% m.gpio( '0' ) )
                    droid.fullSetProperty("gpiob","text",'0x%04X'% m.gpio( '1' ) )
                elif eid == "reboot":
                    m.reboot()

        except Exception as e:
            print(unicode(e))
            droid.fullSetProperty("info","text","eventloop fail!")


m = Android.QMcush( '98:D3:32:20:C2:69', connect=False )

def info(msg):
    m.port.android.fullSetProperty("info","text",msg)

m.tts( u'启动' )
r = m.port.android.fullShow(layout)
if not r.error:
    info('connecting...')
    m.connect()
    m.port.android.fullSetProperty("idn","text",'%s, %s\n%s'% (m.getModel(), m.getVersion(), m.getSerialNumber()) )
    m.tts( u'蓝牙已连接' )
    info('connected')
    eventloop( m )
    info('quiting...')
    m.disconnect()
    m.port.android.fullDismiss()
m.tts( u'停止' )
m.port.android.exit()


