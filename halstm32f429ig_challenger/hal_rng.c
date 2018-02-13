#include "hal.h"


void hal_rng_init(void)
{
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
    RNG_Cmd(ENABLE);
}

uint32_t hal_rng_get(void)
{
    return RNG_GetRandomNumber();
}


