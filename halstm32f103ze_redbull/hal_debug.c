#include "hal.h"

#define DEBUG_LED1_PORT   GPIOF
#define DEBUG_LED2_PORT   GPIOF
#define DEBUG_LED3_PORT   GPIOF
#define DEBUG_LED4_PORT   GPIOF
#define DEBUG_LED5_PORT   GPIOF

#define DEBUG_LED1_PIN    GPIO_Pin_6
#define DEBUG_LED2_PIN    GPIO_Pin_7
#define DEBUG_LED3_PIN    GPIO_Pin_8
#define DEBUG_LED4_PIN    GPIO_Pin_9
#define DEBUG_LED5_PIN    GPIO_Pin_10



void hal_debug_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
  
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF, ENABLE);
   
    GPIO_InitStructure.GPIO_Pin = DEBUG_LED1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_LED1_PORT, &GPIO_InitStructure);
    GPIO_SetBits(DEBUG_LED1_PORT, DEBUG_LED1_PIN);
    GPIO_InitStructure.GPIO_Pin = DEBUG_LED2_PIN;
    GPIO_Init(DEBUG_LED2_PORT, &GPIO_InitStructure);
    GPIO_SetBits(DEBUG_LED2_PORT, DEBUG_LED2_PIN);
    GPIO_InitStructure.GPIO_Pin = DEBUG_LED3_PIN;
    GPIO_Init(DEBUG_LED2_PORT, &GPIO_InitStructure);
    GPIO_SetBits(DEBUG_LED3_PORT, DEBUG_LED3_PIN);
    GPIO_InitStructure.GPIO_Pin = DEBUG_LED4_PIN;
    GPIO_Init(DEBUG_LED2_PORT, &GPIO_InitStructure);
    GPIO_SetBits(DEBUG_LED4_PORT, DEBUG_LED4_PIN);
    GPIO_InitStructure.GPIO_Pin = DEBUG_LED5_PIN;
    GPIO_Init(DEBUG_LED2_PORT, &GPIO_InitStructure);
    GPIO_SetBits(DEBUG_LED5_PORT, DEBUG_LED5_PIN);
}


void hal_debug_led(uint8_t index, int8_t mode)
{
    GPIO_TypeDef *port;
    uint16_t pin;

    switch(index)
    {
    case 0: port = DEBUG_LED1_PORT; pin = DEBUG_LED1_PIN; break;
    case 1: port = DEBUG_LED2_PORT; pin = DEBUG_LED2_PIN; break;
    case 2: port = DEBUG_LED3_PORT; pin = DEBUG_LED3_PIN; break;
    case 3: port = DEBUG_LED4_PORT; pin = DEBUG_LED4_PIN; break;
    case 4: port = DEBUG_LED5_PORT; pin = DEBUG_LED5_PIN; break;
    default:
        return;
    }

    if(mode > 0)
        GPIO_ResetBits(port, pin);
    else if(mode < 0)
    {
        /* toggle, stm32f1 stdlib has no toggle function */
        GPIO_WriteBit(port, pin, GPIO_ReadOutputDataBit(port,pin) ? 0 : 1);
    }
    else
        GPIO_SetBits(port, pin);
}



