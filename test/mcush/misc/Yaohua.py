# coding:utf8
__doc__ = 'products from Shanghai Yaohua Weighing System'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Instrument
from .. import Env

FUNCTIONS = {
'w': 'Weight',
'n': 'Net-weight',
't': 'Tare-weight'
}

PRECESION = [1.0, 0.1, 0.01, 0.001, 0.0001, 0.00001]

FORMATTER = {
1.0: "%.0f",
0.1: "%.1f",
0.01: "%.2f",
0.001: "%.3f",
0.0001: "%.4f",
0.00001: "%.5f",
}


# serial data demo:
# ww000.000lb
# wn-0000.1kg  (negative)
# wt000.000kg

class XK3190A12( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'XK3190'
    DEFAULT_PACKET_FORMAT = re_compile( 'w[wnt]-?[0-9]*.[0-9]*(kg|lb)' )
    DEFAULT_CHECK_IDN = False
    DEFAULT_TERMINAL_RESET = False
    DEFAULT_READ_UNTIL_PROMPTS = False
    DEFAULT_CHECK_RETURN_COMMAND = False

    def writeCommand( self, cmd ):
        if cmd not in ['R', 'T', 'Z']:
            raise Exception('Invalid command')
        Instrument.SerialInstrument.writeCommand( self, cmd )

    def formatReading( self, reading, precision ):
        return FORMATTER[precision]% reading

    def getReading( self ):
        # read for one packet and parse result
        while True:
            line = self.readLine( eol='\x0A', timeout=self.DEFAULT_TIMEOUT ).strip()
            if not self.DEFAULT_PACKET_FORMAT.match( line ):
                continue
            # parse packet
            function = line[1]
            if function in FUNCTIONS:
                function = FUNCTIONS[function]
            unit = line[-2:]
            reading_str = line[2:-2]
            try:
                precesion = PRECESION[len(reading_str.split('.')[-1])]
            except IndexError:
                precesion = PRECESION[-1]
            reading = float(reading_str)
            return function, reading, precesion, unit


