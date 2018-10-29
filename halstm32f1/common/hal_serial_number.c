#include "hal.h"
#include "mcush.h"


int hal_get_serial_number( char *buf )
{
    hexlify( (const char*)UNIQUE_ID0, buf, 12, 1 );
    return 1;
}


