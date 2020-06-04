#include "hal.h"


int hal_fcfs_erase(int *address)
{
    FLASH_Status status;
    
    FLASH_ClearFlag( FLASH_FLAG_PGERR );
    FLASH_Unlock();
    status = FLASH_ErasePage((uint32_t)address);
    FLASH_Lock();
    return (status == FLASH_COMPLETE) ? 1 : 0;
}


int hal_fcfs_program(int *address, int *buf, int length)
{
    FLASH_ClearFlag( FLASH_FLAG_PGERR );
    FLASH_Unlock();
    while( length )
    {
        if( FLASH_ProgramWord((uint32_t)address, (uint32_t)*buf) != FLASH_COMPLETE )
            break;
        buf++;
        address++;
        length--;
    }
    FLASH_Lock();
    return length ? 0 : 1; 
}

