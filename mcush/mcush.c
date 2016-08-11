/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"


TaskHandle_t  task_mcush;
QueueHandle_t queue_mcush;

uint8_t mcush_inited = 0;
uint8_t mcush_echo = 1;
int mcush_errno = 0;

char mcush_tx_buf[MCUSH_TX_BUF_SIZE];
char mcush_rx_buf[MCUSH_RX_BUF_SIZE];

int mcush_argc;
char *mcush_argv[MCUSH_MAX_ARG_NUM];

void mcush_feed_char(char c)
{
}



void mcush_write_char(unsigned char c)
{
    hal_uart_putc((char)c, portMAX_DELAY);
}


void mcush_read_char(unsigned char *c)
{
    if( hal_uart_getc( (char*)c, portMAX_DELAY) != pdTRUE )
    {
        *c = 0;
    }
}


void mcush_set_errno(int errno)
{
    mcush_errno = errno;
}

void mcush_set_echo(int echo)
{
    mcush_echo = echo;
}


const char *mcush_get_prompt(void)
{
#ifdef MCUSH_GET_PROMPT_HOOK
    return mcush_get_prompt_hook(void);
#else
    return MCUSH_DEFAULT_PROMPT;
#endif
}


int mcush_proc_char(char c)
{

    return 0;
}



void mcush_task_entry(void *p)
{
    //mcush_message_t msg;
    //char *command, *last;
    //struct cmd *cmd;
    //int rc;
    char c;

    hal_uart_enable( 1 );
    while(1)
    {
        if( hal_uart_getc( (char*)&c, portMAX_DELAY) == pdTRUE )
            hal_uart_putc((char)c, portMAX_DELAY);
        //if( pdPASS != xQueueReceive( queue_mcush, &msg, portMAX_DELAY ) )
        //    halt( "mcush queue receive" );

        //if( msg.type == MCUSH_MESSAGE_TYPE_CHAR )
        //{
        //    if( mcush_proc_char((char)msg.val) )
        //    {
        //    }
        //}
        //else if( msg.type == MCUSH_MESSAGE_TYPE_RESET )
        //{

        //}
        //else if( msg.type == MCUSH_MESSAGE_TYPE_ECHO )
        //{
        //    mcush_set_echo( msg.val );
        //}

        //diag_printf("\n");
        //diag_printf(m_mcush_prompts[g_cmd_errno]);
        //rc = mcush_gets( mcush_rx_buf, sizeof(mcush_rx_buf) );
        //if (rc == _GETS_CTRLC)
        //{
        //    hal_uart_reset();
        //    mcush_write_char( '\r' );
        //    mcush_write_char( '\n' );
        //}
        //if (console_echo == false) {
        //    mcush_write_char('\r');
        //    mcush_write_char('\n');
        //}
        //command = (char *)&mcush_rx_buf;
        //g_cmd_errno = CMD_ERR_NONE;
        //while (strlen(command) > 0)
        //{                    
        //    last = command;
        //    if ((cmd = parse(&command, &argc, &argv[0])) != (struct cmd *)0) {
        //        (cmd->fun)(argc, argv);
        //       if(g_cmd_errno != CMD_ERR_NONE){
        //           break;
        //       }
        //    } else {
        //        diag_printf("** Error: Illegal command: \"");
        //        diag_printf(last);    
        //        diag_printf("\"\n");    
        //        g_cmd_errno = CMD_ERR_PROC;
        //    }
        //}

    } 
}


void mcush_init(void)
{
    if( mcush_inited )
        return;

    if( !hal_init() )
        halt("hal init");

    queue_mcush = xQueueCreate(MCUSH_QUEUE_SIZE, (unsigned portBASE_TYPE)sizeof(mcush_message_t));
    if( !queue_mcush )
        halt("mcush queue create");

    xTaskCreate(mcush_task_entry, (const char *)"mcush", MCUSH_STACK_SIZE / sizeof(portSTACK_TYPE), NULL, MCUSH_PRIORITY, &task_mcush);
    if( !task_mcush )
        halt("mcush task create");
}


void mcush_start(void)
{
    if( !mcush_inited )
        mcush_init();
    vTaskStartScheduler();
    halt("mcush_stopped");
}

