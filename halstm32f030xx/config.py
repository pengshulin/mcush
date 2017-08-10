from Arm.Stm32 import *

env = Stm32f030xx()
env.setLinkfile( '/ld/stm32f030x4_min.ld' )
port = 'ARM_CM0'


