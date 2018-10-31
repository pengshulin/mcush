#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

#define LED_NORMAL  0
#define LED_ERROR   2

#define HAL_LED_NUM   4
#define HAL_LED_PORTS  { 1, 1, 1, 1 }
#define HAL_LED_PINS  { 8, 10, 9, 11 }



#ifndef USE_CMD_BEEP
#define USE_CMD_BEEP  1
#define PORT_BEEP       GPIOB
#define PIN_BEEP        GPIO_Pin_0
#endif

#ifndef USE_CMD_SGPIO
#define USE_CMD_SGPIO  1
#endif



void hal_pwm_init( int freq, int range, int value );
void hal_pwm_deinit( void );
void hal_pwm_set( int index, int value );
int hal_pwm_get( int index );
int hal_pwm_get_num( void );


void hal_adc_init( void );
void hal_adc_deinit( void );
int hal_adc_get_num( void );
float hal_adc_get( int index );


#define USE_HAL_USB_DISCONNECT  0

#define USE_CMD_POWER   0
#define HAL_POWER_PORT  GPIOB
#define HAL_POWER_PIN   GPIO_Pin_1
void hal_power_set(int enable);
int hal_is_power_set(void);


#define HAL_SPIFFS_CHIPID  0xEF4017  // W25P64
#define sFLASH_SPI                           SPI2
#define sFLASH_SPI_CLK                       RCC_APB1Periph_SPI2
#define sFLASH_SPI_CLK_INIT                  RCC_APB1PeriphClockCmd
#define sFLASH_SPI_SCK_PIN                   GPIO_Pin_13
#define sFLASH_SPI_SCK_GPIO_PORT             GPIOB
#define sFLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_SCK_SOURCE                GPIO_PinSource13
#define sFLASH_SPI_SCK_AF                    GPIO_AF_SPI2
#define sFLASH_SPI_MISO_PIN                  GPIO_Pin_14
#define sFLASH_SPI_MISO_GPIO_PORT            GPIOB
#define sFLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MISO_SOURCE               GPIO_PinSource14
#define sFLASH_SPI_MISO_AF                   GPIO_AF_SPI2
#define sFLASH_SPI_MOSI_PIN                  GPIO_Pin_15
#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOB
#define sFLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MOSI_SOURCE               GPIO_PinSource15
#define sFLASH_SPI_MOSI_AF                   GPIO_AF_SPI2
#define sFLASH_CS_PIN                        GPIO_Pin_12
#define sFLASH_CS_GPIO_PORT                  GPIOB
#define sFLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOB


#define HAL_RESET_VCP_PIN  1


#endif
