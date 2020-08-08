#include "hal.h"

#if defined(FCFS_ADDR)

int hal_fcfs_erase(void)
{
    FLASH_Status status;
    
    FLASH_ClearFlag( FLASH_FLAG_PGERR );
    FLASH_Unlock();
    status = FLASH_ErasePage((uint32_t)FCFS_ADDR);
    FLASH_Lock();
    return (status == FLASH_COMPLETE) ? 1 : 0;
}


int hal_fcfs_program(int offset, int *buf, int length)
{
    FLASH_ClearFlag( FLASH_FLAG_PGERR );
    FLASH_Unlock();
    while( length )
    {
        if( FLASH_ProgramWord((uint32_t)(FCFS_ADDR+offset), (uint32_t)*buf) != FLASH_COMPLETE )
            break;
        buf++;
        offset+=4;
        length--;
    }
    FLASH_Lock();
    return length ? 0 : 1; 
}

#endif
