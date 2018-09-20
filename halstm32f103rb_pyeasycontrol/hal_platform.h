#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__


#define LED_NORMAL  0
#define LED_ERROR   1

#define HAL_LED_NUM   2
#define HAL_LED_PORTS  { 1, 1 }
#define HAL_LED_PINS  { 0, 1 }


#define USE_CMD_BEEP  1
void hal_beep_init( void );
void hal_beep_on( int freq );
void hal_beep_off( void );

/* Hardware connection:
   ----------------------------- 
   (MCU)                  
   PA2  TIMER2_3 ------> BEEPER
   ----------------------------- 
*/
#define HAL_BEEP_DEFINE 
#define HAL_BEEP_RCC_GPIO_ENABLE_CMD    RCC_APB2PeriphClockCmd
#define HAL_BEEP_RCC_GPIO_ENABLE_BIT    RCC_APB2Periph_GPIOA
#define HAL_BEEP_RCC_TIMER_ENABLE_CMD   RCC_APB1PeriphClockCmd
#define HAL_BEEP_RCC_TIMER_ENABLE_BIT   RCC_APB1Periph_TIM2
#define HAL_BEEP_TIMER                  TIM2
#define HAL_BEEP_TIMER_OC_INIT_CMD      TIM_OC3Init
#define HAL_BEEP_TIMER_OC_SETCMP_CMD    TIM_SetCompare3
#define HAL_BEEP_PORT                   GPIOA
#define HAL_BEEP_PIN                    GPIO_Pin_2
#define HAL_BEEP_PINSRC                 GPIO_PinSource2
#define HAL_BEEP_AF                     GPIO_AF_TIMER2
 

#include "hal_sgpio.h"
    
#define USE_CMD_POWER  1
void hal_power_set(int enable);
int hal_is_power_set(void);

#define HAL_USE_USB_DISCONNECT 1



#endif
