#include "hal.h"
#include <stdio.h>


int hal_get_serial_number( char *buf )
{
    sprintf( buf, "%08X%08X%08X", 
        *(unsigned int *)UNIQUE_ID0,
        *(unsigned int *)UNIQUE_ID1,
        *(unsigned int *)UNIQUE_ID2 );

    return 1;
}


