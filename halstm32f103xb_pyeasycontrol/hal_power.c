#include "hal.h"

#define PORT  GPIOB
#define PIN   GPIO_Pin_6

void hal_power_set(int enable)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = PIN;
    if( enable )
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init((GPIO_TypeDef*)PORT, &GPIO_InitStructure);
        GPIO_ResetBits((GPIO_TypeDef*)PORT, PIN);
    }
    else
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init((GPIO_TypeDef*)PORT, &GPIO_InitStructure);
    }
}

