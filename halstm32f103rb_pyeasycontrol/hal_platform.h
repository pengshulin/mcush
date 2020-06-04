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
//(hal)
#define HAL_BEEP_RCC_GPIO_ENABLE_CMD    LL_APB2_GRP1_EnableClock
#define HAL_BEEP_RCC_GPIO_ENABLE_BIT    LL_APB1_GRP1_PERIPH_GPIOA
#define HAL_BEEP_RCC_TIMER_ENABLE_CMD   LL_APB1_GRP1_EnableClock
#define HAL_BEEP_RCC_TIMER_ENABLE_BIT   LL_APB1_GRP1_PERIPH_TIM2
#define HAL_BEEP_TIMER                  TIM2
#define HAL_BEEP_TIMER_CHANNEL          LL_TIM_CHANNEL_CH3
#define HAL_BEEP_TIMER_OC_SETCMP_CMD    LL_TIM_OC_SetCompareCH3
#define HAL_BEEP_PORT                   GPIOA
#define HAL_BEEP_PIN                    GPIO_PIN_2
//(std)
//#define HAL_BEEP_RCC_GPIO_ENABLE_CMD    RCC_APB2PeriphClockCmd
//#define HAL_BEEP_RCC_GPIO_ENABLE_BIT    RCC_APB2Periph_GPIOA
//#define HAL_BEEP_RCC_TIMER_ENABLE_CMD   RCC_APB1PeriphClockCmd
//#define HAL_BEEP_RCC_TIMER_ENABLE_BIT   RCC_APB1Periph_TIM2
//#define HAL_BEEP_TIMER                  TIM2
//#define HAL_BEEP_TIMER_OC_INIT_CMD      TIM_OC3Init
//#define HAL_BEEP_TIMER_OC_SETCMP_CMD    TIM_SetCompare3
//#define HAL_BEEP_PORT                   GPIOA
//#define HAL_BEEP_PIN                    GPIO_Pin_2
//#define HAL_BEEP_PINSRC                 GPIO_PinSource2
//#define HAL_BEEP_AF                     GPIO_AF_TIMER2
 

#define HAL_POWER_PORT  1
#define HAL_POWER_PIN   6
void hal_power_set(int enable);
int hal_is_power_set(void);


#define CMD_I2C_SDA_PORT  2
#define CMD_I2C_SDA_PIN  0
#define CMD_I2C_SCL_PORT  2
#define CMD_I2C_SCL_PIN  1

#define CMD_SPI_SDI_PORT  2
#define CMD_SPI_SDI_PIN  0
#define CMD_SPI_SDO_PORT  2
#define CMD_SPI_SDO_PIN  1
#define CMD_SPI_SCK_PORT  2
#define CMD_SPI_SCK_PIN  2
#define CMD_SPI_CS_PORT  2
#define CMD_SPI_CS_PIN  3
    
#define HAL_USB_EN_PORT  0
#define HAL_USB_EN_PIN   8

//#define HAL_RESET_USB_PINS  1



#endif
