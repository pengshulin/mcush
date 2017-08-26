#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__


#define LED_NORMAL  0
#define LED_ERROR   1

#ifndef HAL_LED_NUM
    #define HAL_LED_NUM   2
    #define HAL_LED_PORTS  {GPIOB, GPIOB}
    #define HAL_LED_PINS  {GPIO_Pin_0, GPIO_Pin_1}
#endif


#define USE_CMD_BEEP  1
void hal_beep_init( void );
void hal_beep_on( int freq );
void hal_beep_off( void );


#define USE_CMD_SGPIO  1
#define SGPIO_FREQ_MIN  1.0
#define SGPIO_FREQ_DEF  1000000.0
#define SGPIO_FREQ_MAX  4000000.0

typedef struct _sgpio_cfg_t
{
    uint8_t inited;
    uint8_t run;
    uint8_t loop_mode;
    uint8_t port;
    uint16_t input_mode;
    uint16_t output_mode;
    void *buf_out;
    void *buf_in;
    uint32_t buf_len;
    float freq;
} sgpio_cfg_t;

int hal_sgpio_init( void );
int hal_sgpio_setup( int loop_mode, int port, int output_mode, int input_mode, void *buf_out, void *buf_in, int buf_len, float freq );
int hal_sgpio_start( void );
void hal_sgpio_stop( void );
int hal_sgpio_set_freq( float freq );
sgpio_cfg_t *hal_sgpio_info( void );

#define USE_CMD_POWER  1
void hal_power_set(int enable);
int hal_is_power_set(void);

#define HAL_USE_USB_DISCONNECT 1

#endif
