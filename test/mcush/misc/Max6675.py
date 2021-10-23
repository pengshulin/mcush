# coding: utf8
__doc__ = 'MAX6675 SPI Thermometer'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from .. import Mcush, Utils

class Max6675():
        
    def __init__( self, controller, sdi=None, sdo=None, sck=None, cs=None ):
        self.controller = controller
        self.controller.spi_init( sdi, sdo, sck, cs, width=16 )

    def getTemperature( self ):
        v = self.controller.spiRead([0])[0]
        t = ((v>>3)&0xFFF)/4.0  # in degree
        return t

