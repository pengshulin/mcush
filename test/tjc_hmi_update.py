#!/usr/bin/env python
# coding:utf8
import os
import re
import sys
import time
import binascii
from mcush import *


    
posy = 0
heighty = 20


def main(argv=None):
    if len(argv) < 2:
        print 'Usage: %s [tft_filename]'% argv[0]
        return 1
    filename = argv[1]
    if not os.path.isfile(filename):
        print 'File not exist.'
        return 2
    dat = open(filename).read()
    filesize = len(dat) 
    if filesize <= 0:
        print 'File error'
        return 3
    # split dat into 4k packets
    packets = []
    while True:
        cut = dat[:4096]
        packets.append(cut)
        if len(cut) < 4096:
            break
        dat = dat[4096:]
        if len(dat) == 0:
            break
    print 'File name: %s'% filename
    print 'File size: %d Bytes, splited into %s packets'% (filesize, len(packets))

    # 1. connect using default baudrate (9600) 
    # 2. get device info
    # 3. switch to fast baudrate (115200)
    # 4. draw some info on screen (if available fonts exist)
    # 5. download
    h = Tjc.TjcHMI()
    h.port.write("\xFF\xFF\xFFconnect\xFF\xFF\xFF") 
    h.port.flush()
    ret = h.readUntilPrompts(check_return_code=False)
    info = ''.join(ret)
    print 'Device info:', info
    a,b,c,d,e,f,g = info.split(',')
    aa, ab = a.split(' ')
    assert aa == 'comok'
    print '  touch: %s'% ('yes' if int(ab) else 'no')
    print '  type: %s'% c
    print '  firmware: %s'% d
    print '  controller: %s'% e
    print '  serial num: %s'% f
    print '  flash size: %s'% g
    assert filesize <= int(g)

    def writeline(s):
        global posy
        h.xstr(0,posy,320,posy+heighty,0,'YELLOW','BLUE',0,0,1,s)
        posy += heighty
   
    h.cls(color='BLUE')
    writeline('TJC-HMI TFT FILE UPDATE')
    writeline('Peng Shulin <trees_peng@163.com> 2018')
    writeline(time.strftime('%Y-%m-%d %H:%M:%S'))
   
    baud = h.port.baudrate
    if baud != 115200:
        baud = 115200
        print u'switch baudrate to %d'% baud
        h.port.write("baud=%d\xFF\xFF\xFF"% baud)
        h.port.flush()
        h.port.ser.close()
        h.port.ser.baudrate = baud
        h.port.ser.open()
        #h.port.ser.reset_input_buffer()
        #h.port.ser.reset_output_buffer()
        #time.sleep(0.5)
    writeline('baudrate: %d'% baud)
    writeline('ready to download...')
    for i in [3,2,1]:
        writeline('%d'% i)
        time.sleep(1)
    print 'downloading...'
    h.port.write( 'whmi-wri %s,%s,%s\xff\xff\xff'% (filesize,baud,0) )
    h.port.flush()
    # write packets
    sent = 0
    percent = 0
    while True:
        byte = h.port.read(1)
        if not byte:
            continue
        if byte == '\x05':
            # sync and send packet
            if len(packets):
                p = packets.pop(0)
                h.port.write(p)
                sent += len(p)
                np = int(sent*100/filesize)
                if np != percent:
                    print '%d / %d ( %d %%)...'% (sent, filesize, np)
                    percent = np
            else:
                print 'done'
                break 
        else:
            print 'unknown byte', ord(byte)
            return -1
    return 0
 
if __name__ == '__main__':
    sys.exit(main(sys.argv))


