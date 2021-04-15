# coding:utf8
__doc__ = 'beep morse code with beep command (or customized function)'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
# refer to http://en.wikipedia.org/wiki/Morse_code
import sys
import time
from .. import Mcush


MorseCode = {
'a' : '.-', 
'b' : '-...', 
'c' : '-.-.', 
'd' : '-..', 
'e' :'.',
'f' : '..-.', 
'g' : '--.', 
'h' : '....', 
'i' : '..', 
'j' : '.---',
'k' : '-.-', 
'l' : '.-..', 
'm' : '--', 
'n' : '-.', 
'o' : '---',
'p' : '.--.', 
'q' : '--.-', 
'r' : '.-.', 
's' : '...', 
't' : '-',
'u' : '..-', 
'v' : '...-', 
'w' : '.--', 
'x' : '-..-', 
'y' : '-.--',
'z' : '--..', 
'0' : '-----', 
'1' : '.----', 
'2' : '..---', 
'3' : '...--',
'4' : '....-', 
'5' : '.....', 
'6' : '-....', 
'7' : '--...', 
'8' : '---..',
'9' : '----.', 
'/' : '-..-.', 
'+' : '.-.-.', 
'=' : '-...-', 
'.' : '.-.-.-',
',' : '--..--', 
'?' : '..--..', 
'(' : '-.--.', 
')' : '-.--.-', 
'-' : '-....-',
'\"' : '.-..-.', 
'_' : '..--.-', 
'\'' : '.----.', 
':' : '---...', 
';' : '-.-.-.', 
'$' : '...-..-', 
'&' : '.-...', 
'@' : '.--.-.', 
} 


class MorseCodeBeeper():

    duration_dot = 0.1
    duration_dash = 0.1 * 3  # equal to 3 dot
    pause_bit = 0.1  # equal to 1 dot
    pause_char = 0.1 * 3  # equal to 3 dots
    pause_word = 0.1 * 7  # equal to 7 dots

    beeper = None

    def __init__( self, controller ):
        self.controller = controller

    def beeper_led( self, index, duration ):
        self.controller.led( index, True )
        time.sleep( duration )
        self.controller.led( index, False )

    def beeper_led0( self, duration ):
        self.beeper_led( 0, duration )

    def beeper_led1( self, duration ):
        self.beeper_led( 1, duration )

    def beeper_beep( self, duration ):
        self.controller.beep( duration=duration )
        time.sleep( duration )

    def beep_function( self, duration ):
        if self.beeper:
            self.beeper( duration )
        else:
            self.beeper_beep( duration )

    def set_beeper( self, func ):
        self.beeper = func

    def BeepChar( self, char ):
        if not char in MorseCode:
            return
        for bit in list(MorseCode[char]):
            duration = self.duration_dot if bit == '.' else self.duration_dash
            self.beep_function(duration)
            time.sleep(self.pause_bit)

    def BeepWord( self, word ):
        '''Beep word'''
        word = word.lower().strip()
        for i in range(len(word)):
            sys.stdout.write( word[i] )
            sys.stdout.flush()
            self.BeepChar( word[i] )
            time.sleep(self.pause_char)
            
    def BeepString( self, string ):
        '''Beep ASCII string'''
        for word in string.strip().split(' '):
            word = word.strip()
            if word:
                self.BeepWord(word)
                time.sleep(self.pause_word)

    def BeepFile( self, filename ):
        '''Read and beep ASCII file'''
        for string in open(filename,'r').readlines():
            string = string.strip()
            if string:
                self.BeepString( string )


    beepString = BeepString
    beepStr = BeepString
    beepChar = BeepChar
    beepWord = BeepWord
    beepFile = BeepFile

