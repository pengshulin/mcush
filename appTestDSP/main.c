#include "mcush.h"
#include "task_blink.h"
#include "arm_math.h"

#define FFTLEN  4096
int __errno;
//float buf1[FFTLEN];
//float buf2[FFTLEN];
float buf1[FFTLEN] __attribute__((section(".ccmsram")));
float buf2[FFTLEN] __attribute__((section(".ccmsram")));

/* test fft */
int cmd_fft( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          'l', "loop", "lopp times", "loop times" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          's', "size", "fft size", "fft size" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'i', "ifft", 0, "inverse fft" },
        { MCUSH_OPT_NONE } };
    int loop=1;
    int fft_size = FFTLEN;
    int fft_flag = 0;
    arm_rfft_fast_instance_f32 S;
    int i;
    unsigned int t0, t1;
    float t;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "loop" ) == 0 )
            {
                if( opt.value )
                    loop=atol(opt.value);
            }
            else if( strcmp( opt.spec->name, "size" ) == 0 )
            {
                if( opt.value )
                    fft_size=atol(opt.value);
            }
            else if( strcmp( opt.spec->name, "ifft" ) == 0 )
                fft_flag = 1;
        }
        else
             STOP_AT_INVALID_ARGUMENT  
    }

    /* initalize to zero */
    for( i=0; i<fft_size; i++ )
        buf1[i] = 0.0;

    shell_printf("loop=%d, fft_size=%d, fft_flag=%d\n", loop, fft_size, fft_flag);
    arm_rfft_fast_init_f32( &S, fft_size );
    t0 = xTaskGetTickCount();
    for( i=0; i<loop; i++ )
    {
        arm_rfft_fast_f32( &S, buf1, buf1, fft_flag );
        arm_cmplx_mag_f32( buf2, buf1, fft_size );
    }
    t1 = xTaskGetTickCount();
    t = t1 - t0;
    shell_printf("time=%f\n", t/loop/configTICK_RATE_HZ );
    
    return 0;
}



static shell_cmd_t cmd_tab[] = {
{   0, 0, "fft", cmd_fft, 
    "fft test",
    "fft"  },
{   CMD_END  } };

int main(void)
{ 
    float a,b,c;
    a=3.0;b=4.0;c=a*b;

    mcush_init();
    task_blink_init();
    shell_add_cmd_table( cmd_tab );
    mcush_start();
    while(1);
}

 
