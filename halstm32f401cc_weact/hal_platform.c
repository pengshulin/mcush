#include "mcush.h"

/* platform related reset */
void hal_platform_reset(void)
{
    hal_adc_deinit();
    hal_daq_deinit();
    hal_pwm_deinit();
}


/* platform related init */
void hal_platform_init(void)
{


}
