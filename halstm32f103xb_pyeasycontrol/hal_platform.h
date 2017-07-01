#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__


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



#endif
