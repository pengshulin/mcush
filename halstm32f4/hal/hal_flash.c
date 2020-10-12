#include "hal.h"


static int check_flash_sector( void *addr )
{
    if( (uint32_t)addr < 0x08000000 )
        return -1;
    else if( (uint32_t)addr < 0x08000000 + 64*1024 )
        return FLASH_SECTOR_0 + ((uint32_t)addr-0x08000000)/(16*1024);
    else if( (uint32_t)addr < 0x08000000 + 128*1024 )
        return FLASH_SECTOR_4;
    else
        return FLASH_SECTOR_5 + ((uint32_t)addr-0x08020000)/(128*1024);
}


static int sector_bytes( int sector_id )
{
    if( sector_id <= FLASH_SECTOR_3 )
        return 16*1024;
    else if( sector_id == FLASH_SECTOR_4 )
        return 64*1024;
    else
        return 128*1024;
}


int hal_flash_erase( void *addr, int bytes )
{
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef erase; 
    uint32_t error;
    int sbytes;
    
    FLASH_WaitForLastOperation(1);
    HAL_FLASH_Unlock();
    while( bytes )
    {
        erase.TypeErase = FLASH_TYPEERASE_SECTORS;
        erase.Sector = check_flash_sector(addr);
        erase.NbSectors = 1;
        erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
        error = 0;
        hal_wdg_clear();
        status = HAL_FLASHEx_Erase( &erase, &error );
        if( status != HAL_OK )
            break;
        sbytes = sector_bytes(erase.Sector);
        if( sbytes >= bytes )
            bytes = 0;
        else
            bytes -= sbytes;
        addr = (void*)( (uint32_t)addr + sbytes );
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

