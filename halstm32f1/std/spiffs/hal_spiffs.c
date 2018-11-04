/* MCUSH designed by Peng Shulin, all rights reserved. */
#include <stdarg.h>
#include "hal.h"
#include "spi_flash.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mcush.h"
#include "mcush_vfs.h"


#if MCUSH_SPIFFS


void hal_spiffs_flash_init(void)
{
    sFLASH_Init();
}

int hal_spiffs_flash_read_id(void)
{
    return sFLASH_ReadID();
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
    //sFLASH_ReadBuffer(dst, addr, size);
    return SPIFFS_OK;
}


s32_t *hal_spiffs_flash_write(u32_t addr, u32_t size, u8_t *src)
{
    //sFLASH_WriteBuffer(src, addr, size);
    return SPIFFS_OK;
}


s32_t *hal_spiffs_flash_erase(u32_t addr, u32_t size)
{
    //sFLASH_EraseSector(addr);
    return SPIFFS_OK;
}


#endif

