#include "hal.h"


#ifndef HAL_POWER_PORT
    #define HAL_POWER_PORT  GPIOB
#endif
#ifndef HAL_POWER_PIN
    #define HAL_POWER_PIN   GPIO_Pin_1
#endif
static uint8_t _power_set;

int hal_is_power_set(void)
{
    return _power_set;
}

void hal_power_set(int enable)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = HAL_POWER_PIN;
    if( enable )
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init((GPIO_TypeDef*)HAL_POWER_PORT, &GPIO_InitStructure);
        GPIO_ResetBits((GPIO_TypeDef*)HAL_POWER_PORT, HAL_POWER_PIN);
        _power_set = 1;
    }
    else
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init((GPIO_TypeDef*)HAL_POWER_PORT, &GPIO_InitStructure);
        _power_set = 0;
    }
}

