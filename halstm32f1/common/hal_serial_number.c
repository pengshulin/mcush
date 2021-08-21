#include "hal.h"
#include "mcush.h"


/* return the length of serial number (in bytes) */
int hal_get_serial_number( char *buf )
{
    hexlify( (const char*)UNIQUE_ID0, buf, 12, 1 );
    return strlen(buf);
}


