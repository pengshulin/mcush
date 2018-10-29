/*
upgrade process 2nd stage:
upgrade firmware to main flash from swap sectors
this small flash program should runs in SRAM

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
New firmware size (in 32bit integer) is stored at the end of swap area.

Peng Shulin <trees_peng@163.com> 2018
*/
#include "stm32f4xx.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_iwdg.h"

#define ADDR_SWAP    (0x08000000+512*1024)
#define ADDR_DST     (0x08000000)
#define ADDR_SIZE    (0x08000000+1024*1024-4)


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




void assert_failed(uint8_t* file, uint32_t line)
{
}


void assert_param(void *p)
{
}


void begin_flash_operation(void)
{
    FLASH_PrefetchBufferCmd( DISABLE );
    FLASH_InstructionCacheCmd( DISABLE );
    FLASH_InstructionCacheReset();
    FLASH_DataCacheCmd( DISABLE );
    FLASH_DataCacheReset();
    FLASH_Unlock();
}

void end_flash_operation(void)
{
    FLASH_Lock();
    FLASH_PrefetchBufferCmd( ENABLE );
    FLASH_SetLatency( FLASH_Latency_4 );
    FLASH_InstructionCacheCmd( ENABLE );
    FLASH_DataCacheCmd( ENABLE );
}

int upgrade(void)
{
    FLASH_Status status;
    uint32_t i, data, addr_src=ADDR_SWAP, addr_dst=ADDR_DST;
    const uint32_t copy_cycle = (*(const unsigned int*)ADDR_SIZE)/4+1;

    IWDG_ReloadCounter();
    begin_flash_operation();
    
    // erase runtime sectors
    for( i=0; i<=7; i++ )
    {
        status = FLASH_EraseSector( sector_tab[i], VoltageRange_3 );
        if( FLASH_COMPLETE != status )
            goto flash_error;
        IWDG_ReloadCounter();
    }

    // copy from swap sectors
    for( i=0; i<copy_cycle; i++ )
    {
        data = *(uint32_t*)addr_src;
        status = FLASH_ProgramWord( addr_dst, data );
        if( FLASH_COMPLETE != status )
            goto flash_error;
        addr_src += 4;
        addr_dst += 4;
        IWDG_ReloadCounter();
    }

    // erase swap sectors
    for( i=8; i<=11; i++ )
    {
        status = FLASH_EraseSector( sector_tab[i], VoltageRange_3 );
        if( FLASH_COMPLETE != status )
            goto flash_error;
        IWDG_ReloadCounter();
    }

    end_flash_operation();
    return 1;

flash_error:
    end_flash_operation();
    return 0;
}


void reboot(void)
{
    NVIC_SystemReset();
}


int main(void)
{
    upgrade();
    reboot();
    while(1);
    return 0;
}

