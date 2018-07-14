#include "mcush.h"
#include "task_disp_lcd1602.h"

TaskHandle_t  task_disp_lcd1602;
QueueHandle_t queue_disp_lcd1602;

#define MAX_DIGITS  16
typedef struct _cb_lcd1602
{
    char disp_buf[MAX_DIGITS+1];
    char disp_buf2[MAX_DIGITS+1];
    uint8_t cursor, line, dat_output;
}cb_lcd1602_t;

cb_lcd1602_t cb;

int cmd_disp( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          'i', "int", "int", "disp an integer" },
        { MCUSH_OPT_LITERAL },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED,
          0, "data", "data", "new data buffer" },
        { MCUSH_OPT_NONE } };
    int integer=-1;
    int has_data=0;
    char fmt_buf[8];

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "int" ) == 0 )
            {
                if( opt.value )
                    integer = atol(opt.value);
            }
            else if( strcmp( opt.spec->name, "data" ) == 0 )
            {
                has_data = 1;
                break; 
            }
        }
        else
             STOP_AT_INVALID_ARGUMENT  
    }

    if( integer != -1 )
    {
        sprintf( fmt_buf, "%%%dd", MAX_DIGITS );
        snprintf( cb.disp_buf, MAX_DIGITS+1, fmt_buf, integer );
        cb.disp_buf[MAX_DIGITS] = 0;
        return 0;
    }

    if( has_data )
    {
        while( parser.idx < argc )
        {
            shell_write_line( argv[parser.idx] );
            parser.idx++;
        }
    }
    else
    {
        shell_printf( "%s\n", cb.disp_buf );
    }
    return 0;
}

static shell_cmd_t cmd_tab[] = {
{   0,  0, "disp",  cmd_disp, 
    "display on lcd1602",
    "disp <new val>"  },
{   0,  0,  0,  0  } };

/* temporarily test */
#define PORT_CTL GPIOC
#define PIN_RS   GPIO_Pin_13
#define PIN_RW   GPIO_Pin_14
#define PIN_E    GPIO_Pin_15
#define PORT_DAT GPIOF
#define PIN_D0   GPIO_Pin_0
#define PIN_D1   GPIO_Pin_1
#define PIN_D2   GPIO_Pin_2
#define PIN_D3   GPIO_Pin_3
#define PIN_D4   GPIO_Pin_4
#define PIN_D5   GPIO_Pin_5
#define PIN_D6   GPIO_Pin_6
#define PIN_D7   GPIO_Pin_7
#define PIN_DALL (PIN_D0 | PIN_D1 | PIN_D2 | PIN_D3 | PIN_D4 | PIN_D5 | PIN_D6 | PIN_D7)
//#define INV_RS
void hal_lcd1602_dat_mode(int output)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = output ? GPIO_Mode_OUT : GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin = PIN_DALL;
    GPIO_Init(PORT_DAT, &GPIO_InitStructure);
}

void hal_lcd1602_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = PIN_RW | PIN_E | PIN_RS;
    GPIO_Init(PORT_CTL, &GPIO_InitStructure);
    GPIO_ResetBits(PORT_CTL, PIN_RW | PIN_RS | PIN_E);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin = PIN_DALL;
    GPIO_Init(PORT_DAT, &GPIO_InitStructure);
}

 
int hal_lcd1602_read(int command_mode)
{
    int val;
    GPIO_SetBits(PORT_CTL, PIN_RW);
#ifdef INV_RS
    if( command_mode )
        GPIO_SetBits(PORT_CTL, PIN_RS);
    else
        GPIO_ResetBits(PORT_CTL, PIN_RS);
#else
    if( command_mode )
        GPIO_ResetBits(PORT_CTL, PIN_RS);
    else
        GPIO_SetBits(PORT_CTL, PIN_RS);
#endif
    if( cb.dat_output )
    {
        hal_lcd1602_dat_mode( 0 );
        cb.dat_output = 0;
    }
    GPIO_SetBits(PORT_CTL, PIN_E);
 
    val = GPIO_ReadInputData(PORT_DAT) & PIN_DALL; 
    GPIO_ResetBits(PORT_CTL, PIN_E);
    return val;
}   


void hal_lcd1602_wait_for_rdy(void)
{
    while( hal_lcd1602_read(1) & 0x80 )
        vTaskDelay(1);
}


void hal_lcd1602_write(int command_mode, int val)
{
    hal_lcd1602_wait_for_rdy();

    GPIO_ResetBits(PORT_CTL, PIN_RW);
#ifdef INV_RS
    if( command_mode )
        GPIO_SetBits(PORT_CTL, PIN_RS);
    else
        GPIO_ResetBits(PORT_CTL, PIN_RS);
#else
    if( command_mode )
        GPIO_ResetBits(PORT_CTL, PIN_RS);
    else
        GPIO_SetBits(PORT_CTL, PIN_RS);
#endif
    if( ! cb.dat_output )
    {
        hal_lcd1602_dat_mode( 1 );
        cb.dat_output = 1;
    }
   
    GPIO_SetBits(PORT_CTL, PIN_E);

    GPIO_SetBits(PORT_DAT, val & PIN_DALL); 
    GPIO_ResetBits(PORT_DAT, (~val) & PIN_DALL); 
    vTaskDelay(1);

    GPIO_ResetBits(PORT_CTL, PIN_E);
}



void hal_lcd1602_write_str( uint8_t x, uint8_t y, char *buf )
{
    hal_lcd1602_write( 1, 0x40 );  /* write address */
    while( *buf )
    {
        hal_lcd1602_write( 0, *buf++ );
    }
}


void task_disp_lcd1602_entry(void *p)
{
    shell_add_cmd_table( cmd_tab );

    memset( &cb, 0, sizeof(cb_lcd1602_t) );
    hal_lcd1602_init();

    //strcpy( cb.disp_buf,  ">>>1234567890<<<" );
    //strcpy( cb.disp_buf2, "ABCDEFGHIJKLMNOP" );

    hal_lcd1602_write( 1, 0x01 );  /* clear disp */
    hal_lcd1602_write( 1, 0x02 );  /* cursor back */
    hal_lcd1602_write( 1, 0x07 );  /* ID=1 S=1 */
    hal_lcd1602_write( 1, 0x0F );  /* D=1 C=1 B=1 */
    hal_lcd1602_write( 1, 0x2C );  /* DL=0 N=1 F=1 */

    while( 1 )
    {
        hal_lcd1602_write_str( 0, 0, "Hello world!" );
        shell_printf("%02X\n", hal_lcd1602_read(1));
        vTaskDelay(1000*configTICK_RATE_HZ/1000);
    }
}


void task_disp_lcd1602_init(void)
{
    xTaskCreate(task_disp_lcd1602_entry, (const char *)"lcd1602", TASK_DISP_LCD1602_STACK_SIZE, NULL, TASK_DISP_LCD1602_PRIORITY, &task_disp_lcd1602);
    if( !task_disp_lcd1602 )
        halt("create disp lcd1602 task");
    mcushTaskAddToRegistered((void*)task_disp_lcd1602);
}


