#if MCUSH_SPIFFS
#include <stdarg.h>
#include "hal.h"
#include "spi_flash.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "mcush.h"
#include "mcush_vfs.h"

SemaphoreHandle_t semaphore_spiffs;

void hal_spiffs_flash_init(void)
{
    semaphore_spiffs = xSemaphoreCreateMutex();
    if( !semaphore_spiffs )
        halt("spiffs semphr create"); 
    sFLASH_Init();
}

int hal_spiffs_flash_read_id(void)
{
    return (int)sFLASH_ReadID();
}


void hal_spiffs_flash_lock(int lock)
{
    if( lock )
        sFLASH_Lock();
    else
        sFLASH_Unlock();
}


s32_t *hal_spiffs_flash_read(u32_t addr, u32_t size, u8_t *dst)
{
    xSemaphoreTake( semaphore_spiffs, portMAX_DELAY );
    sFLASH_ReadBuffer(dst, addr, size);
    xSemaphoreGive( semaphore_spiffs );
    return SPIFFS_OK;
}


s32_t *hal_spiffs_flash_write(u32_t addr, u32_t size, u8_t *src)
{
    xSemaphoreTake( semaphore_spiffs, portMAX_DELAY );
    sFLASH_WriteBuffer(src, addr, size);
    xSemaphoreGive( semaphore_spiffs );
    return SPIFFS_OK;
}


s32_t *hal_spiffs_flash_erase(u32_t addr, u32_t size)
{
    xSemaphoreTake( semaphore_spiffs, portMAX_DELAY );
    sFLASH_EraseSector(addr);
    xSemaphoreGive( semaphore_spiffs );
    return SPIFFS_OK;
}


#endif
