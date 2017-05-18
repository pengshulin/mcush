#include "mcush.h"
#include "task_disp_led595.h"

TaskHandle_t  task_disp_led595;
QueueHandle_t queue_disp_led595;

#define MAX_DIGITS  16

const int disp_digits=8;
char disp_buf[MAX_DIGITS];
int disp_update_ms=3;


/* 
 *     a
 *   -----
 * f|     | b
 *  |  g  |
 *   ----- 
 * e|     | c
 *  |     |
 *   ----- .h
 *     d
 */
#define SA  0x01
#define SB  0x02
#define SC  0x04
#define SD  0x08
#define SE  0x10
#define SF  0x20
#define SG  0x40
#define SH  0x80

const char CHR_CONV_TAB[] = 
{
    /* 0 x00 */
    0,
    /* 1 x01 */
    0,
    /* 2 x02 */
    0,
    /* 3 x03 */
    0,
    /* 4 x04 */
    0,
    /* 5 x05 */
    0,
    /* 6 x06 */
    0,
    /* 7 x07 */
    0,
    /* 8 x08 */
    0,
    /* 9 x09 */
    0,
    /* 10 x0a */
    0,
    /* 11 x0b */
    0,
    /* 12 x0c */
    0,
    /* 13 x0d */
    0,
    /* 14 x0e */
    0,
    /* 15 x0f */
    0,
    /* 16 x10 */
    0,
    /* 17 x11 */
    0,
    /* 18 x12 */
    0,
    0,
    /* 20 x14 */
    0,
    /* 21 x15 */
    0,
    /* 22 x16 */
    0,
    /* 23 x17 */
    0,
    /* 24 x18 */
    0,
    /* 25 x19 */
    0,
    /* 26 x1a */
    0,
    /* 27 x1b */
    0,
    /* 28 x1c */
    0,
    /* 29 x1d */
    0,
    /* 30 x1e */
    0,
    /* 31 x1f */
    0,
    /* 32 x20 ' ' */
    0,
    /* 33 x21 '!' */
    0,
    /* 34 x22 '"' */
    0,
    /* 35 x23 '#' */
    0,
    /* 36 x24 '$' */
    0,
    /* 37 x25 '%' */
    0,
    /* 38 x26 '&' */
    0,
    /* 39 x27 ''' */
    SB,
    /* 40 x28 '(' */
    SA + SF + SE + SD,
    /* 41 x29 ')' */
    SA + SB + SC + SD,
    /* 42 x2a '*' */
    0,
    /* 43 x2b '+' */
    0,
    /* 44 x2c ',' */
    0,
    /* 45 x2d '-' */
    SG,
    /* 46 x2e '.' */
    SH,
    /* 47 x2f '/' */
    SB + SG + SE,
    /* 48 x30 '0' */
    SA + SB + SC + SD + SE + SF,
    /* 49 x31 '1' */
    SB + SC,
    /* 50 x32 '2' */
    SA + SB + SG + SE + SD,
    /* 51 x33 '3' */
    SA + SB + SG + SC + SD,
    /* 52 x34 '4' */
    SF + SG + SB + SC,
    /* 53 x35 '5' */
    SA + SF + SG + SC + SD,
    /* 54 x36 '6' */
    SA + SF + SE + SD + SC + SG,
    /* 55 x37 '7' */
    SA + SB + SC,
    /* 56 x38 '8' */
    SA + SB + SC + SD + SE + SF + SG,
    /* 57 x39 '9' */
    SG + SF + SA + SB + SC + SD,
    /* 58 x3a ':' */
    0,
    /* 59 x3b ';' */
    0,
    /* 60 x3c '<' */
    0,
    /* 61 x3d '=' */
    SG + SD,
    /* 62 x3e '>' */
    0,
    /* 63 x3f '?' */
    0,
    /* 64 x40 '@' */
    0,
    /* 65 x41 'A' */
    SA + SB + SC + SE + SF + SG,
    /* 66 x42 'B' */
    SF + SE + SD + SC + SG,
    /* 67 x43 'C' */
    SA + SF + SE + SD,
    /* 68 x44 'D' */
    SG + SE + SD + SC + SB,
    /* 69 x45 'E' */
    SA + SF + SG + SE + SD,
    /* 70 x46 'F' */
    SA + SF + SG + SE,
    /* 71 x47 'G' */
    SA + SF + SE + SD + SC,
    /* 72 x48 'H' */
    SF + SE + SG + SB + SC,
    /* 73 x49 'I' */
    SB + SC,
    /* 74 x4a 'J' */
    SB + SC + SD,
    /* 75 x4b 'K' */
    0,
    /* 76 x4c 'L' */
    SF + SE + SD,
    /* 77 x4d 'M' */
    0,
    /* 78 x4e 'N' */
    SE + SF + SA + SB + SC,
    /* 79 x4f 'O' */
    SA + SB + SC + SD + SE + SF,
    /* 80 x50 'P' */
    SA + SB + SG + SF + SE,
    /* 81 x51 'Q' */
    0,
    /* 82 x52 'R' */
    0,
    /* 83 x53 'S' */
    SA + SF + SG + SC + SD,
    /* 84 x54 'T' */
    0,
    /* 85 x55 'U' */
    SB + SC + SD + SE + SF,
    /* 86 x56 'V' */
    0,
    /* 87 x57 'W' */
    0,
    /* 88 x58 'X' */
    0,
    /* 89 x59 'Y' */
    SF + SG + SB + SC + SD,
    /* 90 x5a 'Z' */
    0,
    /* 91 x5b '[' */
    SA + SF + SE + SD,
    /* 92 x5c '\' */
    SF + SG + SC,
    /* 93 x5d ']' */
    SA + SB + SC + SD,
    /* 94 x5e '^' */
    SB + SA + SF,
    /* 95 x5f '_' */
    SD,
    /* 96 x60 '`' */
    0,
    /* 97 x61 'a' */
    0,
    /* 98 x62 'b' */
    0,
    /* 99 x63 'c' */
    SG + SE + SD,
    /* 100 x64 'd' */
    SG + SE + SD + SC + SB,
    /* 101 x65 'e' */
    0,
    /* 102 x66 'f' */
    0,
    /* 103 x67 'g' */
    0,
    /* 104 x68 'h' */
    SF + SE + SG + SC,
    /* 105 x69 'i' */
    0,
    /* 106 x6a 'j' */
    0,
    /* 107 x6b 'k' */
    0,
    /* 108 x6c 'l' */
    0,
    /* 109 x6d 'm' */
    0,
    /* 110 x6e 'n' */
    0,
    /* 111 x6f 'o' */
    SG + SE + SD + SC,
    /* 112 x70 'p' */
    0,
    /* 113 x71 'q' */
    0,
    /* 114 x72 'r' */
    SE + SG,
    /* 115 x73 's' */
    0,
    /* 116 x74 't' */
    SF + SG + SE + SD,
    /* 117 x75 'u' */
    SE + SD + SC,
    /* 118 x76 'v' */
    0,
    /* 119 x77 'w' */
    0,
    /* 120 x78 'x' */
    0,
    /* 121 x79 'y' */
    0,
    /* 122 x7a 'z' */
    0,
    /* 123 x7b '{' */
    0,
    /* 124 x7c '|' */
    SF + SE,
    /* 125 x7d '}' */
    0,
    /* 126 x7e '~' */
    0,
    /* 127 x7f DEL */
    0,
};




int cmd_disp( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, "update", 'u', "update", "update period in ms", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "int", 'i', "int", "disp an integer", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_LITERAL },
        { MCUSH_OPT_ARG, "data", 0, "data", "new data buffer", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int update_ms=-1;
    int integer=-1;
    int has_data=0;
    char fmt_buf[8];

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "update" ) == 0 )
            {
                if( opt.value )
                    update_ms = atol(opt.value);
            }
            else if( strcmp( opt.spec->name, "int" ) == 0 )
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

    if( update_ms != -1 )
        disp_update_ms = update_ms;

    if( integer != -1 )
    {
        sprintf( fmt_buf, "%%%dd", disp_digits );
        snprintf( disp_buf, disp_digits+1, fmt_buf, integer );
        disp_buf[disp_digits] = 0;
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
        shell_printf( "%s\n", disp_buf );
    }
    return 0;
}

static shell_cmd_t cmd_tab[] = {
{   0,  0, "disp",  cmd_disp, 
    "display on led595",
    "disp <new val>"  },
{   0,  0,  0,  0  } };

/* temporarily test */
#define PORT GPIOG
#define DAT  GPIO_Pin_8
#define SCK  GPIO_Pin_9
#define RCK  GPIO_Pin_10
void hal_led595_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = DAT;
    GPIO_Init(PORT, &GPIO_InitStructure);
    GPIO_SetBits(PORT, DAT);
    GPIO_InitStructure.GPIO_Pin = SCK;
    GPIO_Init(PORT, &GPIO_InitStructure);
    GPIO_SetBits(PORT, SCK);
    GPIO_InitStructure.GPIO_Pin = RCK;
    GPIO_Init(PORT, &GPIO_InitStructure);
    GPIO_SetBits(PORT, RCK);
}

void hal_led595_update_single(char bits)
{
    int i;
    for( i=0; i<8; i++ )
    {
        if( bits & (1<<(7-i)) )
            GPIO_SetBits(PORT, DAT);
        else
            GPIO_ResetBits(PORT, DAT);
        GPIO_ResetBits(PORT, SCK);
        GPIO_SetBits(PORT, SCK);
    }
}

void hal_led595_update(char ctl_digit, char ctl_val)
{
    hal_led595_update_single(ctl_digit); 
    hal_led595_update_single(ctl_val); 
    GPIO_ResetBits(PORT, RCK);
    GPIO_SetBits(PORT, RCK);
}


void task_disp_led595_entry(void *p)
{
    int i;
    char c;
    shell_add_cmd_table( cmd_tab );

    strcpy( disp_buf, "12345678" );
    hal_led595_init();
    while( 1 )
    {
        for( i=0; i<disp_digits; i++ )
        {
            hal_led_toggle(0);
            c = disp_buf[i];
            if( c & 0x80 )
                c = ' ';
            c = CHR_CONV_TAB[(int)c];
            //if( !c )   /* skip empty */
            //    continue;
            hal_led595_update(1<<i, ~c);
            vTaskDelay(disp_update_ms*configTICK_RATE_HZ/1000);
        }
    }
}


void task_disp_led595_init(void)
{
    xTaskCreate(task_disp_led595_entry, (const char *)"led595", TASK_DISP_LED595_STACK_SIZE, NULL, TASK_DISP_LED595_PRIORITY, &task_disp_led595);
    if( !task_disp_led595 )
        halt("create disp led595 task");
    mcushTaskAddToRegistered((void*)task_disp_led595);
}


