#!/usr/bin/env python3
# coding:utf8
# append checksum at vector offset 7
# Peng Shulin <trees_peng@163.com> 2021
import os
import sys
import intelhex
import mcush
from mcush.Utils import s2I, I2s

def calcCheckSum(raw):
    vectors = [s2I(raw[i*4:i*4+4]) for i in range(7)]
    checksum = (-sum(vectors)) & 0xFFFFFFFF
    return checksum
 
def main(argv=None):
    if len(argv) < 2:
        print('need bin/hex file as parm')
    else:
        inputfile = argv[1].strip()

    if not os.path.isfile( inputfile ):
        print( "%s not exist"% inputfile )
        return 1
   
    if inputfile.endswith('.bin'): 
        f = open(inputfile, 'r+b')
        checksum = calcCheckSum(f.read(7*4))
        f.seek( 7*4 )
        f.write( I2s(checksum) )
        f.flush()
        f.close()
    elif inputfile.endswith('.hex'): 
        f = intelhex.IntelHex(inputfile)
        head_address = f.addresses()[0]
        checksum = calcCheckSum(f.gets(head_address,7*4))
        f.puts( head_address+7*4, I2s(checksum) )
        f.tofile( inputfile, 'hex' )
    print( '%s patched vector checksum: 0x%08X'% (inputfile, checksum) ) 
        


if __name__ == '__main__':
    sys.exit(main(sys.argv))

