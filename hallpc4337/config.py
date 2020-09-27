from Arm.Lpc import *

env = Lpc4337()
env.setLinkfile( '/ld/lpc4337_m4.ld' )

env.appendDefineFlags( [ 'NO_BOARD_LIB' ] )


