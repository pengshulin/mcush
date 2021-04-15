#include "hal.h"
#include "mcush.h"


int hal_get_serial_number( char *buf )
{
    int id[2];

    id[0] = *(int*)UNIQUE_ID0;
    id[1] = *(int*)UNIQUE_ID1;

    hexlify( (const char*)id, buf, 8, 1 );
    return 8;
}


