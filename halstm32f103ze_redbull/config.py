from Arm.Stm32 import *

env = Stm32f1hd()
env.setLinkfile( '/ld/stm32f103ze_redbull.ld' )
#env.appendDefineFlags( ['DATA_IN_ExtSRAM'] )
port = 'ARM_CM3'

env.appendDefineFlags( [ 'MCUSH_VFS=1', 'MCUSH_ROMFS=1' ] )

#USE_SPIFFS=True
#USE_SPIFFS=False
#
#if USE_SPIFFS:
#    env.appendDefineFlags( [ 'MCUSH_SPIFFS=1' ] )
#    env.appendPath([ '/libspiffs' ])
#    env.appendGlobSource([ '/libspiffs/*.c' ])


