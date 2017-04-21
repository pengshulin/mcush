#include "hal.h"

const char gpio_start='a', gpio_stop='i';

GPIO_TypeDef *ports[] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI };



static void _set_dir( int port, int bits, uint32_t mode, uint32_t pull )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    int i;
        
    GPIO_InitStructure.Mode = mode;
    GPIO_InitStructure.Pull = pull;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    for( i=0; i<32; i++ )
    {
        if( bits & (1<<i) )
        { 
            GPIO_InitStructure.Pin = 1<<i;
            HAL_GPIO_Init(ports[port], &GPIO_InitStructure);
        }
    }
}


int hal_gpio_get_port_num(void)
{
    return gpio_stop - gpio_start + 1;
}


void hal_gpio_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
}


void hal_gpio_set_input(int port, int bits)
{
    _set_dir( port, bits, GPIO_MODE_INPUT, GPIO_PULLUP );
}


void hal_gpio_set_input_pull(int port, int bits, int pull)
{
    _set_dir( port, bits, GPIO_MODE_INPUT, pull ? GPIO_PULLUP : GPIO_PULLDOWN );
}


void hal_gpio_set_output(int port, int bits)
{
    _set_dir( port, bits, GPIO_MODE_OUTPUT_PP, 0 );
}


void hal_gpio_set(int port, int bits)
{
    HAL_GPIO_WritePin(ports[port], bits, GPIO_PIN_SET);
}


void hal_gpio_clr(int port, int bits)
{
    HAL_GPIO_WritePin(ports[port], bits, GPIO_PIN_RESET);
}


void hal_gpio_toggle(int port, int bits)
{
    HAL_GPIO_TogglePin(ports[port], bits);
}


int hal_gpio_get(int port, int bits)
{
    return HAL_GPIO_ReadPin(ports[port], bits);
}



