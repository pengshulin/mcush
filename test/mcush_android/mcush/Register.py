# coding:utf8
__doc__ = 'mcu registers'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'
from . import Utils


REGISTER_TYPE = {
# c_type_name: (bit_width, bytes, get_func, set_func)
'int8':   ( 8,  1, Utils.s2b, Utils.b2s ),
'uint8':  ( 8,  1, Utils.s2B, Utils.B2s ),
'int16':  ( 16, 2, Utils.s2h, Utils.h2s ),
'uint16': ( 16, 2, Utils.s2H, Utils.H2s ),
'int32':  ( 32, 4, Utils.s2i, Utils.i2s ),
'uint32': ( 32, 4, Utils.s2I, Utils.I2s ),
'int64':  ( 64, 8, Utils.s2q, Utils.q2s ),
'uint64': ( 64, 8, Utils.s2Q, Utils.Q2s ),
'float':  ( 32, 4, Utils.s2f, Utils.f2s ),
'double': ( 64, 8, Utils.s2d, Utils.q2s ),
}

class Register:
    '''mcu register'''
    REG_TYPE = None

    def __init__( self, name, address, description, reg_type=None ):
        self.name = str(name)
        self.address = int(address)
        self.description = str(description)
        self.reg_type = str(reg_type) if reg_type is not None else self.REG_TYPE
        assert( self.reg_type in REGISTER_TYPE )

    def __str__( self ):
        return self.name

class Int8(Register):
    REG_TYPE = 'int8'

class UInt8(Register):
    REG_TYPE = 'uint8'

class Int16(Register):
    REG_TYPE = 'int16'

class UInt16(Register):
    REG_TYPE = 'uint16'

class Int32(Register):
    REG_TYPE = 'int32'

class UInt32(Register):
    REG_TYPE = 'uint32'

class Int64(Register):
    REG_TYPE = 'int64'

class UInt64(Register):
    REG_TYPE = 'uint64'

class Float(Register):
    REG_TYPE = 'float'

class Double(Register):
    REG_TYPE = 'double'


