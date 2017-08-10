from Arm.Stm32 import *

env = Stm32f767xx()
env.setLinkfile( '/ld/stm32f767zi_min.ld' )
port = 'ARM_CM7'

