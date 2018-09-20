#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

#define HAL_LED_NUM  1

#if defined(BLACK_BOARD)
    /* black board */
    #define HAL_LED_PORTS  { 1 }
    #define HAL_LED_PINS  { 12 }
    #define HAL_LED_REV
#elif defined(BLUE_BOARD)
    /* blue board */
    #define HAL_LED_PORTS  { 2 }
    #define HAL_LED_PINS  { 13 }
    #define HAL_LED_REV
#else
    #error "which board?"
#endif



#define USE_CMD_SGPIO   1

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

 
void hal_pwm_init( int freq, int range, int value );
void hal_pwm_deinit( void );
void hal_pwm_set( int index, int value );
int hal_pwm_get( int index );
int hal_pwm_get_num( void );

 
void hal_adc_init( void );
void hal_adc_deinit( void );
int hal_adc_get_num( void );
float hal_adc_get( int index );

  
#define HAL_USE_USB_DISCONNECT  0


#endif
