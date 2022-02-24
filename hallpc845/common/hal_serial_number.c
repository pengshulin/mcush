#include "hal.h"
#include "mcush.h"

static int uid[4];
static uint8_t uid_inited=0;

int hal_get_serial_number( char *buf )
{
    unsigned int iap_cmd[5], iap_ret[5];

    if( uid_inited == 0 )
    {
        iap_cmd[0] = IAP_CMD_READ_UID;
        os_enter_critical();
        iap( iap_cmd, iap_ret );
        os_exit_critical();
        if( iap_ret[0] == IAP_RET_CMD_SUCCESS )
            memcpy( &uid[0], &iap_ret[1], 16 );
        else
            memset( &uid[0], 0, 16 );
        uid_inited = 1;
    }
    hexlify( (const char*)uid, buf, 16, 1 );
    return strlen(buf);
}

