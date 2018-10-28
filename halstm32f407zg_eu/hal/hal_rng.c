#include "hal.h"


void hal_rng_init(void)
{
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_RNG);
    LL_RNG_Enable(RNG);
}

uint32_t hal_rng_get(void)
{
    return LL_RNG_ReadRandData32(RNG);
}


