#!/usr/bin/env python3
# read serial number from sn.txt file by line
# calculate mac address and print
import binascii

SN=[l.strip() for l in open('sn.txt','rb').readlines()]
#print(SN)

for sn in SN:
    h=0x534F4654
    for i, v in enumerate(sn):
        #h = (h * 521) ^ ((h>>8)&0xFFFF)
        #h ^= v<<(8*(i%3))
        h ^= (h<<3)+v+(h>>2)
        h &= 0xFFFFFFFF
        #print( sn.decode(), '->', hex(h) )
        
    print( sn.decode(), '->', '%06X'% (h&0xFFFFFF) )


