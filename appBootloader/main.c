/* Multi-function bootloader framework
 *
 * MCUSH designed by Peng Shulin, all rights reserved.
 * Peng Shulin <trees_peng@linkongsoft.com> 2020
 */
#include "mcush.h"
#include "flash_config.h"


__signature const char bootloader_signature[] = "<bootloader>";

typedef void (user_app_entry_t)(void);

const bootloader_flag_t *flag = (bootloader_flag_t*)FLASH_BOOTLOADER_FLAG_ADDR_BASE;

extern int upgrade_from_shell(void);
extern int upgrade_from_iflash(void);
extern int upgrade_from_spi_flash(void);
extern int upgrade_from_spiffs(void);
extern int upgrade_from_fatfs(void);
extern int upgrade_from_sdcard(void);

   

__weak void bootloader_led_start(void)
{
    hal_led_set(0);
}

__weak void bootloader_led_program(void)
{
    hal_led_toggle(0);
}

__weak void bootloader_led_succ(void)
{
    hal_led_clr(0);
}

__weak void bootloader_led_fail(void)
{
    int i;

    for( i=0; i<3; i++ )
    {
        hal_led_set( 0 );
        hal_delay_ms(100);
        hal_led_clr( 0 );
        hal_delay_ms(100);
    }
}

__weak void bootloader_led_halt(void)
{
    while( 1 )
    {
        hal_led_set( 0 );
        hal_delay_ms(250);
        hal_led_clr( 0 );
        hal_delay_ms(250);
    }
}


int check_upgrade_size(int max)
{
    if( (flag->size >= 1*1024) && (flag->size <= max) )
        return 1;
    else
        return 0;
}
 
int erase_bootloader_flags(void)
{
    return hal_flash_erase( (void*)FLASH_BOOTLOADER_FLAG_ADDR_BASE, FLASH_BOOTLOADER_FLAG_SIZE );
}

 
int main(void)
{
    uint32_t *sp;

    hal_wdg_init();
    hal_clk_init();
    hal_gpio_init();
    hal_led_init();
    
    bootloader_led_start();

    /* check for bootloader command */
    if( (flag->magic_num != BOOTLOADER_FLAG_MAGIC_NUM) )
        goto run_app;
    
#ifdef BOOTLOADER_SUPPORT_UPGRADE_FROM_SHELL
    if( strcmp( flag->operation, BOOTLOADER_UPGRADE_FROM_SHELL ) == 0 )
    {
        if( upgrade_from_shell() < 0 )
            goto halt;
    }
    else
#endif
#ifdef BOOTLOADER_SUPPORT_UPGRADE_FROM_INTERNAL_FLASH
    if( strcmp( flag->operation, BOOTLOADER_UPGRADE_FROM_INTERNAL_FLASH ) == 0 )
    {
        if( ! check_upgrade_size(FLASH_UPGRADE_SIZE) )
            goto run_app;
        if( upgrade_from_iflash() < 0 )
            goto halt;
    }
    else
#endif
#ifdef BOOTLOADER_SUPPORT_UPGRADE_FROM_SPI_FLASH
    if( strcmp( flag->operation, BOOTLOADER_UPGRADE_FROM_SPI_FLASH ) == 0 )
    {
        if( ! check_upgrade_size(FLASH_APP_SIZE) )
            goto run_app;
        if( upgrade_from_spi_flash() < 0 )
            goto halt;
    }
    else
#endif 
#ifdef BOOTLOADER_SUPPORT_UPGRADE_FROM_SPIFFS
    if( strcmp( flag->operation, BOOTLOADER_UPGRADE_FROM_SPIFFS ) == 0 )
    {
        if( ! check_upgrade_size(FLASH_APP_SIZE) )
            goto run_app;
        if( upgrade_from_spiffs() < 0 )
            goto halt;
    }
    else
#endif
#ifdef BOOTLOADER_SUPPORT_UPGRADE_FROM_FATFS
    if( strcmp( flag->operation, BOOTLOADER_UPGRADE_FROM_FATFS ) == 0 )
    {
        if( ! check_upgrade_size() )
            goto run_app;
        if( upgrade_from_fatfs() < 0 )
            goto halt;
    }
    else
#endif
#ifdef BOOTLOADER_SUPPORT_UPGRADE_FROM_SDCARD
    if( strcmp( flag->operation, BOOTLOADER_UPGRADE_FROM_SDCARD ) == 0 )
    {
        if( ! check_upgrade_size(FLASH_APP_SIZE) )
            goto run_app;
        if( upgrade_from_sdcard() < 0 )
            goto halt;
    }
    else
#endif
    {
        /* un-matched operation, ignore */
        goto run_app;
    }

    bootloader_led_succ();

run_app: 
    /* check user app entry by stack pointer */ 
    sp = (uint32_t*)FLASH_APP_ADDR_BASE;
    if( (*sp & 0x2FF00000) != 0x20000000 )
        goto halt;

    /* jump and run */
    __set_MSP(*sp);
    (*((user_app_entry_t*)(*(uint32_t*)(FLASH_APP_ADDR_BASE+4))))(); 
    while(1);

halt:
    bootloader_led_halt();
}

