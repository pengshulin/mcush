#!/usr/bin/env python
# coding:utf8
# Morse code beeper
# refer to http://en.wikipedia.org/wiki/Morse_code
# designed by Peng Shulin <trees_peng@163.com>
import sys
import time
from mcush import *


MorseCode = {
'a' : '.-', 'b' : '-...', 'c' : '-.-.', 'd' : '-..', 'e' :'.',
'f' : '..-.', 'g' : '--.', 'h' : '....', 'i' : '..', 'j' : '.---',
'k' : '-.-', 'l' : '.-..', 'm' : '--', 'n' : '-.', 'o' : '---',
'p' : '.--.', 'q' : '--.-', 'r' : '.-.', 's' : '...', 't' : '-',
'u' : '..-', 'v' : '...-', 'w' : '.--', 'x' : '-..-', 'y' : '-.--',
'z' : '--..', '0' : '-----', '1' : '.----', '2' : '..---', '3' : '...--',
'4' : '....-', '5' : '.....', '6' : '-....', '7' : '--...', '8' : '---..',
'9' : '----.', '/' : '-..-.', '+' : '.-.-.', '=' : '-...-', '.' : '.-.-.-',
',' : '--..--', '?' : '..--..', '(' : '-.--.', ')' : '-.--.-', '-' : '-....-',
'\"' : '.-..-.', '_' : '..--.-', '\'' : '.----.', ':' : '---...', 
';' : '-.-.-.', '$' : '...-..-', '&' : '.-...', '@' : '.--.-.', } 

class MorseCodeBeeper( Mcush.Mcush ):

    freq = 500
    duration_dot = 0.1
    duration_dash = 0.1 * 3  # equal to 3 dot
    pause_bit = 0.1  # equal to 1 dot
    pause_char = 0.1 * 3  # equal to 3 dots
    pause_word = 0.1 * 7  # equal to 7 dots

    def beep_function( self, duration ):
        self.led( 1, True )
        time.sleep( duration )
        self.led( 1, False )

    def BeepChar( self, char ):
        if not MorseCode.has_key( char ):
            return
        #print '%c'%char,
        for bit in list(MorseCode[char]):
            duration = self.duration_dot if bit == '.' else self.duration_dash
            self.beep_function(duration)
            time.sleep(self.pause_bit)

    def BeepWord( self, word ):
        '''Beep word'''
        word = word.lower().strip()
        for i in range(len(word)):
            #print word[i],
            sys.stdout.write( word[i] )
            sys.stdout.flush()
            self.BeepChar( word[i] )
            time.sleep(self.pause_char)
            
    def BeepString( self, string ):
        '''Beep ASCII string'''
        for word in string.strip().split(' '):
            word = word.strip()
            if word:
                #print word
                self.BeepWord(word)
                time.sleep(self.pause_word)

    def BeepFile( self, filename ):
        '''Read and beep ASCII file'''
        for string in open(filename,'r').readlines():
            string = string.strip()
            if string:
                self.BeepString( string )



if __name__ == '__main__':
    m = MorseCodeBeeper()
    m.BeepString( 'abcdefghijklmnopqrstuvwxyz' )
    m.BeepString( 'SOS' )
    #m.BeepFile('Readme.txt')

