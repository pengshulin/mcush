#include "mcush.h"
#include "task_logger.h"
#include "task_blink.h"
#include "task_netmon.h"
#include "task_modbus_tcp.h"


int main(void)
{
    mcush_init();
    task_logger_init();
    task_blink_init();
    task_netmon_init();
    task_modbus_tcp_init();
    mcush_start();
    while(1);
}


