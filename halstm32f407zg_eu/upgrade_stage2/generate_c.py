#!/usr/bin/env python
'''generate c file from binary file'''
import FileGenerator, os
NOTES = '''\
This is the 2nd stage upgrade program, compile and link it with the 
main firmware.  After preparing the new firmware in swap flash sectors,
copy the program to sram and run it.'''
BIN_FILE = 'upgrade_stage2.bin'
DST_FILE = 'upgrade_stage2.c'

prog = open(BIN_FILE, 'rb').read()
size = len(prog) 

OUTPUT = FileGenerator.CGenerator( DST_FILE, DST_FILE )
OUTPUT.writeSeperator()
OUTPUT.writeComment( NOTES )
OUTPUT.writeSeperator()
OUTPUT.writeLine()
OUTPUT.writeLine( 'const unsigned int upgrade_prog_length = %d;'% size )
OUTPUT.writeLine( '' )
OUTPUT.writeLine( 'const char upgrade_prog_data[] = {' )
chars = []
for _ in range(size):
    chars.append( '0x%02x, '% ord(prog[_]) )
    if len(chars) > 12:
        OUTPUT.writeLine( ''.join(chars) )
        chars = []
if chars:
    OUTPUT.writeLine( ''.join(chars) )
OUTPUT.writeLine( '};' )
OUTPUT.close()

