/* 
upgrade process 1st stage:
prepare firmware to swap sectors from file system

flash sectors for stm32f407xg (1MBytes total)
------------------------------------------
Sector 0,   // 16kbytes         ^ 
Sector 1,   // 16kbytes         |
Sector 2,   // 16kbytes         |
Sector 3,   // 16kbytes   (runtime firmware area)
Sector 4,   // 64kbytes         |
Sector 5,   // 128kbytes        |
Sector 6,   // 128kbytes        |
Sector 7,   // 128kbytes        v
------------------------------------------
Sector 8,   // 128kbytes  <<< swap space
Sector 9,   // 128kbytes  <<< swap space
Sector 10,  // 128kbytes  <<< swap space
Sector 11,  // 128kbytes  <<< swap space
------------------------------------------
 
Peng Shulin <trees_peng@163.com> 2018
*/
#include "mcush.h"
#include "hal.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_iwdg.h"

#define ADDR_SWAP    (0x08000000+512*1024)
#define ADDR_SIZE    (0x08000000+1024*1024-4)
#define UPGRADE_FILE_SIZE_MIN   (10*1024)
#define UPGRADE_FILE_SIZE_MAX   (512*1024-4)

static const uint16_t sector_tab[] = {
    FLASH_Sector_0,
    FLASH_Sector_1,
    FLASH_Sector_2,
    FLASH_Sector_3,
    FLASH_Sector_4,
    FLASH_Sector_5,
    FLASH_Sector_6,
    FLASH_Sector_7,
    FLASH_Sector_8,
    FLASH_Sector_9,
    FLASH_Sector_10,
    FLASH_Sector_11,
};


extern unsigned int upgrade_prog_length;
extern char upgrade_prog_data[];

typedef void (upgrade_prog_t)(void);

static void begin_flash_operation(void)
{
    portENTER_CRITICAL();
    FLASH_PrefetchBufferCmd( DISABLE );
    FLASH_InstructionCacheCmd( DISABLE );
    FLASH_InstructionCacheReset();
    FLASH_DataCacheCmd( DISABLE );
    FLASH_DataCacheReset();
    FLASH_Unlock();
}

static void end_flash_operation(void)
{
    FLASH_Lock();
    FLASH_PrefetchBufferCmd( ENABLE );
    FLASH_SetLatency( FLASH_Latency_4 );
    FLASH_InstructionCacheCmd( ENABLE );
    FLASH_DataCacheCmd( ENABLE );
    portEXIT_CRITICAL();
}

/* read upgrade file from file system and write to swap space */
int hal_upgrade_prepare_swap( const char *filename, int debug_mode )
{
    int fd;
    uint32_t i, data, br, addr=ADDR_SWAP;
    int size;
    uint32_t checksum=0;
    FLASH_Status status;

    if( ! mcush_size( filename, &size ) )
        return 0;  /* file not exist */
    if( size < UPGRADE_FILE_SIZE_MIN  || size > UPGRADE_FILE_SIZE_MAX )
        return 0;  /* file size out of range */
    fd = mcush_open( filename, "rb" );
    if( fd == 0 )
        return 0;  /* open error */
   
    begin_flash_operation();

    IWDG_ReloadCounter();
    // erase sectors
    for( i=8; i<=11; i++ )
    {
        status = FLASH_EraseSector( sector_tab[i], VoltageRange_3 );
        if( FLASH_COMPLETE != status )
            goto flash_error;
        IWDG_ReloadCounter();
    }

    // write size of the new firmware
    status = FLASH_ProgramWord( ADDR_SIZE, size );
    if( FLASH_COMPLETE != status )
        goto flash_error;

    while( size )
    {
        // load data from file
        br = mcush_read( fd, (void*)&data, 4 );
        if( (br > 0) && (br < 4) )
            br = 4;  // addr align
        else if( br == 0 )
            goto file_error;
        // copy data from upgrade file
        status = FLASH_ProgramWord( addr, data );
        if( FLASH_COMPLETE != status )
            goto flash_error;

        addr += 4;
        size -= br;
        IWDG_ReloadCounter();

        // update checksum
        checksum += data;
    }

    mcush_close( fd );
    IWDG_ReloadCounter();
    end_flash_operation();

    // TODO: use safer mechanism
    //if( !debug_mode && checksum )
    //    return 0;
    return 1;

file_error:
flash_error:
    mcush_close( fd );
    end_flash_operation();
    return 0;
}




void hal_upgrade_run_stage2(void)
{
    char *p_src, *p_dst;
    unsigned int i;

    begin_flash_operation();

    // load and call subprogram
    p_src = (char*)upgrade_prog_data;
    p_dst = (char*)0x20000000;  // internal sram
    for( i=0; i<upgrade_prog_length; i++ )
        *p_dst++ = *p_src++;

    IWDG_ReloadCounter();
    (*((upgrade_prog_t*)(*(uint32_t*)0x20000000)))();
    while( 1 );  // should never run to here
}


