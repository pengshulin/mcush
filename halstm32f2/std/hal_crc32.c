#include "hal.h"


int hal_crc32_calc(int *buf, int len)
{
    static uint8_t inited=0;

    if( ! inited )
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
        inited = 1;
    }

    CRC_ResetDR();
    return CRC_CalcBlockCRC( (uint32_t*)buf, len );
}


