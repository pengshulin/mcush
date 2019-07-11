#include "mcush.h"

    
extern int set_errno(int num);

/* platform related reset */
void hal_platform_reset(void)
{
    int i;

    for( i=0; i<hal_led_get_num(); i++ )
        hal_led_clr(i);
    hal_pwm_deinit(); 
    hal_adc_deinit(); 
    set_errno(0);
}


/* platform related init */
void hal_platform_init(void)
{

}
