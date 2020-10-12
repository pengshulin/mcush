#include "hal.h"



int hal_flash_erase( void *addr, int bytes )
{
    FLASH_Status status;
    int sbytes=FLASH_PAGE_SIZE;
    
    FLASH_ClearFlag( FLASH_FLAG_PGERR );
    FLASH_Unlock();
    while( bytes )
    {
        hal_wdg_clear();
        status = FLASH_ErasePage((uint32_t)addr);
        if( status != FLASH_COMPLETE )
            break;
        if( bytes < sbytes )
            bytes = 0;
        else
            bytes -= sbytes; 
        addr = (void*)( (uint32_t)addr + sbytes );
    }
    FLASH_Lock();
    return bytes ? 0 : 1; 
}


int hal_flash_program( void *addr, int *dat, int bytes )
{
    uint32_t address = (uint32_t)addr;

    FLASH_ClearFlag( FLASH_FLAG_PGERR );
    FLASH_Unlock();
    while( bytes )
    {
        hal_wdg_clear();
        if( FLASH_ProgramWord(address, (uint32_t)*dat) != FLASH_COMPLETE )
            break;
        dat++;
        address += 4;
        bytes -= 4;
    }
    FLASH_Lock();
    return bytes ? 0 : 1; 
}

