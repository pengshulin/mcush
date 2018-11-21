# coding: utf8
__doc__ = 'Aosong sensors'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils


class Dht11():
        
    def __init__( self, controller, pin=None ):
        self.controller = controller
        if pin:
            self.controller.writeCommand('dht11 -p%s'% pin)

    def read( self ):
        ret = self.controller.writeCommand('dht11')
        return float(ret[0]), float(ret[1])

