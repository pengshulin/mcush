#include "mcush.h"
#include "hal_sdram.h"

/* platform related reset */
void hal_platform_reset(void)
{

}


/* platform related init */
void hal_platform_init(void)
{
    hal_sdram_init();
}
