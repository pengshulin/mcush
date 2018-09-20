from Arm.Stm32 import *

env = Stm32f1hd()
env.setLinkfile( '/ld/stm32f103ze_redbull.ld' )
#env.appendDefineFlags( ['DATA_IN_ExtSRAM'] )

env.appendDefineFlags( [
    'USE_CMD_SGPIO=1',
    'USE_CMD_ADC=1',
    ] )

