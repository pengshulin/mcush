#!/usr/bin/env python
# generate .build_signature file that contains build date/time stamp
# file will be included in romfs.c and integrated into firmware
# usually this will automatically be done by MCUSH SCons cripts
# so this script is still kept for special IDEs(eg. KEIL-MDK)
# MCUSH designed by Peng Shulin, all rights reserved.
from time import strftime
from os import path
import sys

def generateBuildSignatureFile( dirname=None, fname='.build_signature' ):
    if dirname is not None:
        if path.isdir(dirname):
            fname = path.join(dirname, fname)
        else:
            raise Exception( '%s not a directory'% str(dirname) )
    f = open(fname,'w+')
    f.write('/* automatically generated, do not modify */\n')
    f.write('__signature const char build_signature[] = "%s";\n'% strftime("%Y%m%d-%H%M%S"))
    f.close()

if __name__ == '__main__':
    if len(sys.argv) == 2:
        generateBuildSignatureFile( dirname=sys.argv[1] )
    else:
        generateBuildSignatureFile()

