#include "mcush.h"
#include "task_1wire.h"

TaskHandle_t  task_1wire;
QueueHandle_t queue_1wire;

        'SearchRom':  0xF0,
        'ReadRom':  0x33,
        'MatchRom':  0x55,
        'SkipRom':  0xCC,

 'AlarmSearch':  0xEC,
 'ConvertT':  0x44,
 'WriteScratchpad':  0x4E,
 'ReadScratchpad':  0xBE,
 'CopyScratchpad':  0x48,
 'RecallEE':  0xB8,
 'ReadPowerSupply':  0xB4,


int cmd_1wire( int argc, char *argv[] )
{
    int n = -1;
    if( argc == 1 )
    {
        //shell_printf( "%d\n", _errno );
        return 0;
    }
    else if( argc == 2 )
    {
        n = atol(argv[1]);
        return 0;
    } 
    else
        return -1; 
}

static shell_cmd_t cmd_tab[] = {
{   0 , "1wire",  cmd_1wire, 
    "control 1wire bus",
    "1wire command"  },
{   0,  0,  0,  0  } };

/* temporarily test */
#define PORT GPIOG
#define DIN  GPIO_Pin_9
#define DOUT GPIO_Pin_11
void hal_1wire_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = DOUT;
    GPIO_Init(PORT, &GPIO_InitStructure);
    GPIO_ResetBits(PORT, DOUT);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin = DIN;
    GPIO_Init(PORT, &GPIO_InitStructure);
}




void task_1wire_entry(void *p)
{
    shell_add_cmd_table( cmd_tab );
    hal_1wire_init();
    while( 1 )
    {
        hal_led_toggle(0);
        vTaskDelay(1000*configTICK_RATE_HZ/1000);
    }
}


void task_1wire_init(void)
{
    xTaskCreate(task_1wire_entry, (const char *)"1wire", TASK_1WIRE_STACK_SIZE, NULL, TASK_1WIRE_PRIORITY, &task_1wire);
    if( !task_1wire )
        halt("create 1wire task");
    mcushTaskAddToRegistered((void*)task_1wire);
}


