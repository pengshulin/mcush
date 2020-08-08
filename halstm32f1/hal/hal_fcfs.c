#include "hal.h"


int hal_fcfs_erase(void)
{
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef erase; 
    uint32_t error;
    
    __HAL_FLASH_CLEAR_FLAG( FLASH_FLAG_PGERR );
    HAL_FLASH_Unlock();
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = (uint32_t)FCFS_ADDR;
    erase.NbPages = 1;
    error = 0;
    status = HAL_FLASHEx_Erase( &erase, &error );
    HAL_FLASH_Lock();
    return (status == HAL_OK) ? 1 : 0;
}


int hal_fcfs_program(int offset, int *buf, int length)
{
    __HAL_FLASH_CLEAR_FLAG( FLASH_FLAG_PGERR );
    HAL_FLASH_Unlock();
    while( length )
    {
        if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)(FCFS_ADDR+offset), (uint64_t)*buf) != HAL_OK )
            break;
        buf++;
        offset+=4;
        length--;
    }
    HAL_FLASH_Lock();
    return length ? 0 : 1; 
}

