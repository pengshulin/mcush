/* Upgrade firmware from file /s/upgrade.bin
 *
 * MCUSH designed by Peng Shulin, all rights reserved.
 * Peng Shulin <trees_peng@linkongsoft.com> 2020
 */
#include "mcush.h"
#include "flash_config.h"
#include "md5.h"

#if BOOTLOADER_SUPPORT_UPGRADE_FROM_SPIFFS

#define SEGMENT_SIZE 8192

extern const bootloader_flag_t *flag;
extern int erase_bootloader_flags(void);
extern void bootloader_led_program(void);
    

/* error return:
 *  > 0  upgrade succ
 *  = 0  upgrade failed
 *  < 0  need to be halted
 */
int upgrade_from_spiffs(void)
{
    int fd, r;
    char buf[SEGMENT_SIZE];
    char *p;
    md5_t md5;
    int md5_output[4];
    int size;

    mcush_mount( "s", &mcush_spiffs_driver );
    fd = mcush_open( "/s/upgrade.bin", "r" );
    if( fd == 0 )
        return 0;

    /* check for file size and md5 */
    md5_init( &md5 );
    size = 0;
    while(1)
    {
        r = mcush_read( fd, buf, SEGMENT_SIZE );
        if( r <= 0 )
            break;
        md5_update( &md5, buf, r );
        size += r;
    }
    md5_finish( &md5, (char*)md5_output );
    if( size != flag->size )
        goto fail;
    if( memcmp( (void*)md5_output, (void*)(flag->md5), 16 ) != 0 )
        goto fail;

    /* read again and program */
    mcush_seek( fd, 0, 0 );  /* back to head */
    /* erase app flash */
    bootloader_led_program();
    if( ! hal_flash_erase( (void*)FLASH_APP_ADDR_BASE, FLASH_APP_SIZE ) )
        goto halt;
    p = (char*)FLASH_APP_ADDR_BASE;
    while(1)
    {
        r = mcush_read( fd, buf, SEGMENT_SIZE );
        if( r <= 0 )
            break;
        bootloader_led_program();
        if( hal_flash_program( (void*)p, (int*)buf, r ) == 0 )
            goto halt;
        p += r; 
    }
    mcush_close(fd);
    /* finally, clear bootloader flags */
    erase_bootloader_flags();
    return 1;
fail:
    mcush_close(fd);
    return 0;
halt:
    mcush_close(fd);
    return -1;
}

#endif

