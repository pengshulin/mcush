/* Copy firmware from duplicated area of internal flash
 *
 * MCUSH designed by Peng Shulin, all rights reserved.
 * Peng Shulin <trees_peng@linkongsoft.com> 2020
 */
#include "mcush.h"
#include "flash_config.h"
#include "md5.h"

#if BOOTLOADER_SUPPORT_UPGRADE_FROM_INTERNAL_FLASH

extern const bootloader_flag_t *flag;
extern int erase_bootloader_flags(void);
extern void bootloader_led_program(void);

/* error return:
 *  > 0  upgrade succ
 *  = 0  upgrade failed
 *  < 0  need to be halted
 */
int upgrade_from_iflash(void)
{
    int md5_output[4];
    uint32_t app_size;

    /* compare md5 */
    md5( (const char*)FLASH_UPGRADE_ADDR_BASE, flag->size, (char*)md5_output );
    if( memcmp( (void*)md5_output, (void*)(flag->md5), 16 ) != 0 )
        return 0;

    /* erase app flash */
    app_size = FLASH_APP_SIZE;
    if( FLASH_UPGRADE_ADDR_BASE > FLASH_APP_ADDR_BASE )
    {
        if( FLASH_APP_ADDR_BASE + app_size > FLASH_UPGRADE_ADDR_BASE )
            app_size = FLASH_UPGRADE_ADDR_BASE - FLASH_APP_ADDR_BASE;
    }
    bootloader_led_program();
    if( ! hal_flash_erase( (void*)FLASH_APP_ADDR_BASE, app_size ) )
        return -1;  /* app sector may be erased, cannot continue */
    
    /* copy from upgrade flash */
    if( hal_flash_program( (void*)FLASH_APP_ADDR_BASE, (int*)FLASH_UPGRADE_ADDR_BASE, flag->size ) )
    {
        /* finally, clear bootloader flags */
        erase_bootloader_flags();
        return 1;
    }
    else
        return -1;  /* fail to copy, halt */
}

#endif

