#include "mcush.h"


/* return the length of serial number (in bytes) */
int hal_get_serial_number( char *buf )
{
    strcpy( buf, "000000000000000000000000" );
    return strlen(buf);
}


