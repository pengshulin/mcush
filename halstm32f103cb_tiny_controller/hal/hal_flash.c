#include "hal.h"



int hal_flash_erase( void *addr, int bytes )
{
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef erase; 
    uint32_t error;
    const int pbytes=FLASH_PAGE_SIZE;
    
    FLASH_WaitForLastOperation(1);
    HAL_FLASH_Unlock();
    while( bytes )
    {
        erase.TypeErase = FLASH_TYPEERASE_PAGES;
        erase.NbPages = 1;
        erase.PageAddress = (uint32_t)addr;
        error = 0;
        hal_wdg_clear();
        status = HAL_FLASHEx_Erase( &erase, &error );
        if( status != HAL_OK )
            break;
        if( pbytes >= bytes )
            bytes = 0;
        else
            bytes -= pbytes;
        addr = (void*)( (uint32_t)addr + pbytes );
    }
    HAL_FLASH_Lock();
    return (status == HAL_OK) ? 1 : 0;
}


int hal_flash_program( void *addr, int *dat, int bytes )
{
    uint32_t address=(uint32_t)addr;

    FLASH_WaitForLastOperation(1);
    HAL_FLASH_Unlock();
    while( bytes > 0 )
    {
        hal_wdg_clear();
        if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, (uint32_t)*dat) != HAL_OK )
            break;
        dat++;
        address += 4;
        bytes -= 4;
    }
    HAL_FLASH_Lock();
    return (bytes > 0) ? 0 : 1; 
}

