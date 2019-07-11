__doc__ = 'Shell Kit class'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Mcush


class ShellKit(Mcush.Mcush):
    DEFAULT_NAME = 'ShellKit'
    DEFAULT_IDN = re_compile( 'ShellKit[0-9]*,([0-9]+\.[0-9]+.*)' )

    def scpiRst( self ):
        if self.checkCommand("*rst"):
            self.writeCommand('*rst')
        else:
            self.errno( 0 )
            # clear all leds
            for i in range(self.getLedNumber()):
                self.led(i, False)
            # stop sgpio
            self.sgpioStop()

