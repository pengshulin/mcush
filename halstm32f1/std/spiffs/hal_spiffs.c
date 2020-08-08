/* MCUSH designed by Peng Shulin, all rights reserved. */
#include <stdarg.h>
#include "hal.h"
#include "spi_flash.h"
#include "mcush.h"
#include "mcush_vfs.h"
#if MCUSH_SPIFFS

/* NOTE:
   in mcush_vfs_spiffs.c, lock/unlock api uses mutex locker
   and the flash is not seperated to multi partitations,
   so the locker here is ignored now */
//#define USE_LOCK   1

#if USE_LOCK
SemaphoreHandle_t semaphore_spiflash;
#endif

void hal_spiffs_flash_init(void)
{
#if USE_LOCK
    semaphore_spiflash = xSemaphoreCreateMutex();
    if( !semaphore_spiflash )
        halt("spiflash semphr create"); 
#endif
    sFLASH_Init();
}

int hal_spiffs_flash_read_id(void)
{
    int id;
#if USE_LOCK
    xSemaphoreTake( semaphore_spiflash, portMAX_DELAY );
#endif
    id = (int)sFLASH_ReadID();
#if USE_LOCK
    xSemaphoreGive( semaphore_spiflash );
#endif
    return id;
}


int hal_spiffs_flash_read_status(void)
{
    int status;
#if USE_LOCK
    xSemaphoreTake( semaphore_spiflash, portMAX_DELAY );
#endif
    status = (int)sFLASH_ReadStatus();
#if USE_LOCK
    xSemaphoreGive( semaphore_spiflash );
#endif
    return status;
}


void hal_spiffs_flash_lock(int lock)
{
#if USE_LOCK
    xSemaphoreTake( semaphore_spiflash, portMAX_DELAY );
#endif
    if( lock )
        sFLASH_Lock();
    else
        sFLASH_Unlock();
#if USE_LOCK
    xSemaphoreGive( semaphore_spiflash );
#endif
}


s32_t *hal_spiffs_flash_read(u32_t addr, u32_t size, u8_t *dst)
{
#if USE_LOCK
    xSemaphoreTake( semaphore_spiflash, portMAX_DELAY );
#endif
    sFLASH_ReadBuffer(dst, addr, (uint8_t)size);
#if USE_LOCK
    xSemaphoreGive( semaphore_spiflash );
#endif
    return SPIFFS_OK;
}


s32_t *hal_spiffs_flash_write(u32_t addr, u32_t size, u8_t *src)
{
#if USE_LOCK
    xSemaphoreTake( semaphore_spiflash, portMAX_DELAY );
#endif
    sFLASH_WriteBuffer(src, addr, (uint8_t)size);
#if USE_LOCK
    xSemaphoreGive( semaphore_spiflash );
#endif
    return SPIFFS_OK;
}


s32_t *hal_spiffs_flash_erase(u32_t addr, u32_t size)
{
    (void)size;
#if USE_LOCK
    xSemaphoreTake( semaphore_spiflash, portMAX_DELAY );
#endif
    sFLASH_EraseSector(addr);
#if USE_LOCK
    xSemaphoreGive( semaphore_spiflash );
#endif
    return SPIFFS_OK;
}


#endif
