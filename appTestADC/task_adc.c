#include "mcush.h"
#include "task_adc.h"

TaskHandle_t  task_adc;
QueueHandle_t queue_adc;

#define ADC_CHANNEL_NUM  8

const GPIO_TypeDef *adc_ports[] = { GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA };
const int adc_pins[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
const int adc_ins[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

float adc_value[ADC_CHANNEL_NUM];

float adval_to_voltage(int val)
{
    float ret;
    ret = val;
    ret = 3.3 * ret / 4096;
    return ret;
}

int cmd_adc( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, "loop", 'l', 0, "loop mode", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "loop_delay", 0, "loop delay adc_value", "adc_value in ms", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "index", 'i', "channel_index", "select channel", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int loop=0, loop_delay=-1;
    TickType_t tick;
    char c;
    int channel=-1;
    int i;
    
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "loop" ) == 0 )
                loop=1;
            else if( strcmp( opt.spec->name, "loop_delay" ) == 0 )
            {
                if( opt.value )
                    loop_delay=atol(opt.value);
            }
            else if( strcmp( opt.spec->name, "index" ) == 0 )
            {
                if( opt.value )
                    channel=atol(opt.value);
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }; 

    if( loop_delay <= 0 )
        loop_delay = 1000;  /* default: 1 sec */
loop_start:


    if( channel<0 )
    {
        for( i=0; i<ADC_CHANNEL_NUM; i++ )
        {
            shell_write_float( adc_value[i] );
            shell_write_char( i < (ADC_CHANNEL_NUM-1) ? ',' : '\n' );
        }
    }
    else if( channel < ADC_CHANNEL_NUM )
    {
        shell_write_float( adc_value[channel] );
        shell_write_char( '\n' );
    } 
    else
        goto channel_error;

    if( loop )
    {
        tick = xTaskGetTickCount();
        while( xTaskGetTickCount() < tick + loop_delay*configTICK_RATE_HZ/1000 )
        {
            if( shell_driver_read_char_blocked(&c, 1*configTICK_RATE_HZ/1000) != -1 )
                if( c == 0x03 ) /* Ctrl-C for stop */
                    return 0;
        }
        goto loop_start;
    }
 
    return 0;

channel_error:
    shell_write_line("channel error");
    return 1;

}

static const shell_cmd_t cmd_tab[] = {
{   0,  'a',  "adc",  cmd_adc, 
    "adc control",
    "adc [-l] [-i channel_index]"  },
{   CMD_END } };


#define DELAY  10

void task_adc_entry(void *p)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    int i;
    
    shell_add_cmd_table( cmd_tab );

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    for( i=0; i<ADC_CHANNEL_NUM; i++ )
    {
        GPIO_InitStructure.GPIO_Pin = 1<<(adc_pins[i]);
        GPIO_Init((GPIO_TypeDef*)adc_ports[i], &GPIO_InitStructure);
    }
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    ADC_StructInit(&ADC_InitStructure);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;

    while( 1 )
    {
        for( i=0; i<ADC_CHANNEL_NUM; i++ )
        {
            ADC_Cmd( ADC1, DISABLE );
            ADC_InitStructure.ADC_NbrOfChannel = i;
            ADC_Init( ADC1, &ADC_InitStructure);
            ADC_Cmd( ADC1, ENABLE );
            ADC_SoftwareStartConvCmd(ADC1, ENABLE);
            vTaskDelay(DELAY*configTICK_RATE_HZ/1000);
            adc_value[i] = adval_to_voltage( ADC_GetConversionValue(ADC1) );
        }
    }
}


void task_adc_init(void)
{
    xTaskCreate(task_adc_entry, (const char *)"adc", TASK_ADC_STACK_SIZE, NULL, TASK_ADC_PRIORITY, &task_adc);
    if( !task_adc )
        halt("create adc task");
}


