#!/usr/bin/env python
# coding:utf8
import os
import re
import sys
import time
import binascii
from mcush import *


h = Tjc.TjcHMI()
for page in ['main','button','key','guide','box','progressbar','slide']:
    print( 'page %s'% page )
    h.page( page )
    time.sleep(0.5)

h.cls()
for i in range(0,319,10):
    h.line( 0,0,i,239)
    h.cir( i,100,10)

h.page( 'key' )
h.set( 't0.txt', 'hello' )
h.xstr(0,0,200,50,0,'RED','BLUE',1,1,1,u'Hello测试')

time.sleep(0.5)
#h.cls()

 
