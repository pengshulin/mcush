# coding:utf8
__doc__ = 'UT61B/C/D/E digital multimeter'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from re import compile as re_compile
from .. import Instrument
from .. import Env

FUNCTIONS = {
0x3B: 'Voltage',
0x3D: 'Auto uA Current',
0x3F: 'Auto mA Current',
0x30: '22A Current',
0x39: 'Manual A Current',
0x33: 'Ohm',
0x35: 'Continuity',
0x31: 'Diode',
0x32: 'Frequency',
0x36: 'Capacitance',
0x34: 'Temperature',
0x3E: 'ADP',
}


RANGE_VOLT = {
0x30: (2.2, 0.0001, 'V'),
0x31: (22.0, 0.001, 'V'),
0x32: (220.0, 0.01, 'V'),
0x33: (2200.0, 0.1, 'V'),
0x34: (220.0, 0.01, 'mV'),
}

RANGE_AUTO_UAMP = {
0x30: (2.20, 0.01, 'uA'),
0x31: (22.0, 0.1, 'uA'),
}

RANGE_AUTO_MAMP = {
0x30: (2.200, 0.001, 'mA'),
0x31: (22.00, 0.01, 'mA'),
}

RANGE_22AMP = {
0x30: (22.0, 0.001, 'A'),
}

RANGE_220AMP = {
0x30: (22.0, 0.001, 'A'),
0x31: (220.0, 0.01, 'A'),
}

RANGE_2200AMP = {
0x30: (220.0, 0.01, 'A'),
0x31: (2200.0, 0.1, 'A'),
}


RANGE_MANUAL_AMP = {
0x30: (2.2, 0.0001, 'A'),
0x31: (22.0, 0.001, 'A'),
0x32: (220.0, 0.01, 'A'),
0x33: (2200.0, 0.1, 'A'),
0x34: (22000.0, 1.0, 'A'),
}

RANGE_OHM = {
0x30: (220.0, 0.01, 'Ohm'),
0x31: (2.2, 0.0001, 'kOhm'),
0x32: (22.0, 0.001, 'kOhm'),
0x33: (220.0, 0.01, 'kOhm'),
0x34: (2.2, 0.0001, 'MOhm'),
0x35: (22.0, 0.001, 'MOhm'),
0x36: (220.0, 0.01, 'MOhm'),
}

RANGE_FREQ = {
0x30: (22.0, 0.01, 'Hz'),
0x31: (220.0, 0.1, 'Hz'),
0x33: (22.0, 0.001, 'kHz'),
0x34: (220.0, 0.01, 'kHz'),
0x35: (2.2, 0.0001, 'MHz'),
0x36: (22.0, 0.001, 'MHz'),
0x37: (220.0, 0.01, 'MHz'),
}

RANGE_CAP = {
0x30: (22.0, 0.001, 'nF'),
0x31: (220.0, 0.01, 'nF'),
0x32: (2.2, 0.0001, 'uF'),
0x33: (22.0, 0.001, 'uF'),
0x34: (220.0, 0.01, 'uF'),
0x35: (2.2, 0.0001, 'mF'),
0x36: (22.0, 0.001, 'mF'),
0x37: (220.0, 0.01, 'mF'),
}

FORMATTER = {
1.0: "%.0f",
0.1: "%.1f",
0.01: "%.2f",
0.001: "%.3f",
0.0001: "%.4f",
0.00001: "%.5f",
}


class UT61E( Instrument.SerialInstrument ):
    DEFAULT_NAME = 'UT61'
    DEFAULT_PROMPTS = re_compile( '\x0A' )
    DEFAULT_IDN = re_compile( '.*' )
    DEFAULT_TERMINAL_RESET = False
    DEFAULT_CHECK_IDN = False

    def __init__( self, *args, **kwargs ):
        kwargs['baudrate'] = 19200  # this is fixed
        Instrument.SerialInstrument.__init__( self, *args, **kwargs ) 

    def connect( self ):
        self.port.connect()
        if not self.port.connected:
            return
        self.port.ser.setRTS( 0 )  # power up the opto-isolated circuit

    def writeCommand( self, cmd ):
        # UT61X only receives serial packets
        raise Exception("Not supported")

    def formatReading( self, reading, precision ):
        return FORMATTER[precision]% reading

    def getReading( self ):
        # read measure value, function, range, flags
        while True:
            line = self.readLine( eol='\x0D', decode_utf8=False )
            if len(line) == 0:
                raise Instrument.CommandTimeoutError()
            # convert into integer list
            if Env.PYTHON_V3:
                line = list(line)
            else:
                line = map( ord, list(line) )
            if (len(line) != 13) or ((line[0]&0x7F) != 0x0A):
                continue  # ignore invalid packet
            # parse packet
            function = line[7] & 0x7F 
            if function in FUNCTIONS:
                function = FUNCTIONS[function]
            else:
                continue  # ignore invalid function
            rng = line[1] & 0x7F 
            reading = ''.join([chr(d&0x7F) for d in line[2:7]])
            status = line[8] & 0x7F 
            opt1 = line[9] & 0x7F 
            opt2 = line[10] & 0x7F 
            opt3 = line[11] & 0x7F 
            opt4 = line[12] & 0x7F 
            # search for valid range list 
            rng_lst = None
            vahz = opt3 & 0x01
            if vahz:
                rng_list = RANGE_FREQ
            elif function == 'Voltage':
                rng_list = RANGE_VOLT
            elif function == 'Auto uA Current':
                if opt4 & 0x04:
                    rng_list = RANGE_220AMP
                else:
                    rng_list = RANGE_AUTO_UAMP
            elif function == 'Auto mA Current':
                if opt4 & 0x04:
                    rng_list = RANGE_2200AMP
                else:
                    rng_list = RANGE_AUTO_MAMP
            elif function == '22A Current':
                rng_list = RANGE_22AMP
            elif function == 'Manual A Current':
                rng_list = RANGE_MANUAL_AMP
            elif function in ['Ohm', 'Continuity', 'Diode']:
                rng_list = RANGE_OHM
            elif function == 'Frequency':
                rng_list = RANGE_FREQ
            elif function == 'Capacitance':
                rng_list = RANGE_CAP
            try:
                full_range, precision, unit = rng_list[rng]
            except:
                continue  # ignore invalid range
            try:
                reading = int(reading) * precision
            except:
                continue  # ignore invalid reading 
            # many flag bits (not all)
            flags = []
            if status & 0x4: # sign bit
                reading = -reading
                flags.append("SIGN")
            if status & 0x1:
                flags.append("OL")
            if opt1 & 0x02:
                flags.append("REL")
            if opt3 & 0x08:
                flags.append("DC")
            if opt3 & 0x04:
                flags.append("AC")
            if opt3 & 0x02:
                flags.append("AUTO")
            if opt3 & 0x01:
                flags.append("VAHZ")
            if opt4 & 0x02:
                flags.append("HOLD")
            return function, reading, precision, unit, flags


