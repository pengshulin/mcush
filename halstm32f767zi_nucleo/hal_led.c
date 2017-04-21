#include "hal.h"
#include "hal_led.def"


void hal_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    int i;

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    for( i=0; i<led_num; i++ )
    {
        GPIO_InitStructure.Pin = led_pins[i];
        HAL_GPIO_Init(led_ports[i], &GPIO_InitStructure);
        HAL_GPIO_WritePin(led_ports[i], led_pins[i], GPIO_PIN_SET);
    }
}

int hal_led_get_num(void)
{
    return led_num;
}

void hal_led_set(int index)
{
    HAL_GPIO_WritePin(led_ports[index], led_pins[index], GPIO_PIN_SET);
}

void hal_led_clr(int index)
{
    HAL_GPIO_WritePin(led_ports[index], led_pins[index], GPIO_PIN_RESET);
}

void hal_led_toggle(int index)
{
    HAL_GPIO_TogglePin(led_ports[index], led_pins[index]);
}

int hal_led_get(int index)
{
    return (HAL_GPIO_ReadPin(led_ports[index], led_pins[index])==GPIO_PIN_SET) ? 1 : 0;
}


