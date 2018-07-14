from Arm.Lpc import *

env = Lpc4337()
env.setLinkfile( '/ld/lpc4337_m4_min.ld' )


env.appendDefineFlags( [ 'NO_BOARD_LIB' ] )


env.appendDefineFlags( [ 'MCUSH_VFS=1', 'MCUSH_ROMFS=1' ] )

