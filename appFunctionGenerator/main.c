#include <math.h>
#include "mcush.h"
#include "task_blink.h"

//#define USE_SINGLE_TIMER  1

#define DAC_DHR12R1_ADDRESS    0x40007408
#define DAC_DHR12R2_ADDRESS    0x40007414

extern uint32_t SystemCoreClock;

extern uint16_t dat_sine_1000[];
extern uint16_t dat_sine10_1000[];
extern uint16_t dat_sine100_1000[];
extern uint16_t dat_half_sine_1000[];
extern uint16_t dat_triangle_1000[];
extern uint16_t dat_sawtooth_1000[];
extern uint16_t dat_impulse_1000[];
extern uint16_t dat_impulse10_1000[];
extern uint16_t dat_envelope_1000[];
extern uint16_t dat_random_1000[];
extern uint16_t dat_shock10_1000[];
extern uint16_t dat_shock20_1000[];
extern uint16_t dat_shock50_1000[];
extern uint16_t dat_shock100_1000[];
extern uint16_t dat_shock200_1000[];

typedef struct _fgen_cfg_t
{
    float freq;
    void *buf;
    int len;
    int need_free;
} fgen_cfg_t;

fgen_cfg_t cfg1={
    .freq = 1000000,
    .buf = dat_shock20_1000,
    .len = 1000,
    .need_free = 0,
};

fgen_cfg_t cfg2={
    .freq = 1000000,
    .buf = dat_sine_1000,
    .len = 1000,
    .need_free = 0,
};


int _calc_tim_period( float freq )
{
    float f = SystemCoreClock;
    int i;

    f = round(f/freq/2.0);
    i = f;
    if( i )
        i--;
    if( i > 0xffff )
        i = 0xffff;
    else if( i < 1 )
        i = 1;
    return i;
}
 
void TIM6_Config(float freq)
{
    TIM_TimeBaseInitTypeDef tim_init;

    TIM_Cmd(TIM6, DISABLE);
    /* TIM6 Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    /* Time base configuration */
    TIM_TimeBaseStructInit(&tim_init);
    tim_init.TIM_Period = _calc_tim_period( freq );
    tim_init.TIM_Prescaler = 0;
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_init.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &tim_init);
    /* TIM6 TRGO selection */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
    /* TIM6 enable counter */
    TIM_Cmd(TIM6, ENABLE);

    cfg1.freq = freq;
#if USE_SINGLE_TIMER
    cfg2.freq = freq;
#endif
}


void TIM7_Config(float freq)
{
    TIM_TimeBaseInitTypeDef tim_init;

    TIM_Cmd(TIM7, DISABLE);
    /* TIM7 Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    /* Time base configuration */
    TIM_TimeBaseStructInit(&tim_init);
    tim_init.TIM_Period = _calc_tim_period( freq );
    tim_init.TIM_Prescaler = 0;
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_init.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM7, &tim_init);
    /* TIM7 TRGO selection */
    TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);
    /* TIM7 enable counter */
    TIM_Cmd(TIM7, ENABLE);
    cfg2.freq = freq;
}


void DAC_Ch1_Config( uint16_t *buf, int size )
{
    DMA_InitTypeDef dma_init;
    DAC_InitTypeDef dac_init;

    /* Disable DMA1_Stream5 */
    DMA_Cmd(DMA1_Stream5, DISABLE);
    /* Disable DAC Channel1 */
    DAC_Cmd(DAC_Channel_1, DISABLE);
    /* Disable DMA for DAC Channel1 */
    DAC_DMACmd(DAC_Channel_1, DISABLE);

    /* DAC channel1 Configuration */
    DAC_StructInit( &dac_init );
    dac_init.DAC_Trigger = DAC_Trigger_T6_TRGO;
    dac_init.DAC_WaveGeneration = DAC_WaveGeneration_None;
    dac_init.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &dac_init);

    /* DMA1_Stream5 channel7 configuration **************************************/
    DMA_DeInit(DMA1_Stream5);
    dma_init.DMA_Channel = DMA_Channel_7;
    dma_init.DMA_PeripheralBaseAddr = DAC_DHR12R1_ADDRESS;
    dma_init.DMA_Memory0BaseAddr = (uint32_t)buf;
    dma_init.DMA_BufferSize = size;
    dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma_init.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init.DMA_Mode = DMA_Mode_Circular;
    dma_init.DMA_Priority = DMA_Priority_High;
    dma_init.DMA_FIFOMode = DMA_FIFOMode_Disable;
    dma_init.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    dma_init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dma_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &dma_init);

    /* Enable DMA1_Stream5 */
    DMA_Cmd(DMA1_Stream5, ENABLE);
    /* Enable DAC Channel1 */
    DAC_Cmd(DAC_Channel_1, ENABLE);
    /* Enable DMA for DAC Channel1 */
    DAC_DMACmd(DAC_Channel_1, ENABLE);
}


void DAC_Ch2_Config( uint16_t *buf, int size )
{
    DMA_InitTypeDef dma_init;
    DAC_InitTypeDef dac_init;

    /* Disable DMA1_Stream6 */
    DMA_Cmd(DMA1_Stream6, DISABLE);
    /* Disable DAC Channel2 */
    DAC_Cmd(DAC_Channel_2, DISABLE);
    /* Disable DMA for DAC Channel2 */
    DAC_DMACmd(DAC_Channel_2, DISABLE);

    /* DAC channel2 Configuration */
    DAC_StructInit( &dac_init );
#if USE_SINGLE_TIMER
    dac_init.DAC_Trigger = DAC_Trigger_T6_TRGO;
#else
    dac_init.DAC_Trigger = DAC_Trigger_T7_TRGO;
#endif
    dac_init.DAC_WaveGeneration = DAC_WaveGeneration_None;
    dac_init.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_2, &dac_init);

    /* DMA1_Stream6 channel7 configuration **************************************/
    DMA_DeInit(DMA1_Stream6);
    dma_init.DMA_Channel = DMA_Channel_7;
    dma_init.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR12R2_ADDRESS;
    dma_init.DMA_Memory0BaseAddr = (uint32_t)buf;
    dma_init.DMA_BufferSize = size;
    dma_init.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma_init.DMA_Mode = DMA_Mode_Circular;
    dma_init.DMA_Priority = DMA_Priority_High;
    dma_init.DMA_FIFOMode = DMA_FIFOMode_Disable;
    dma_init.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    dma_init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dma_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream6, &dma_init);

    /* Enable DMA1_Stream6 */
    DMA_Cmd(DMA1_Stream6, ENABLE);
    /* Enable DAC Channel2 */
    DAC_Cmd(DAC_Channel_2, ENABLE);
    /* Enable DMA for DAC Channel2 */
    DAC_DMACmd(DAC_Channel_2, ENABLE);
}


void hal_sync(void)
{
    //DMA_Cmd(DMA1_Stream5, DISABLE);
    //DMA_Cmd(DMA1_Stream6, DISABLE);
    DMA_SetCurrDataCounter( DMA1_Stream5, 0 );
    DMA_SetCurrDataCounter( DMA1_Stream6, 0 );
    //DMA_Cmd(DMA1_Stream5, ENABLE);
    //DMA_Cmd(DMA1_Stream6, ENABLE);
}


void hal_dac_init(void)
{
    GPIO_InitTypeDef gpio_init;

    RCC_APB1PeriphClockCmd(	RCC_APB1Periph_DAC, ENABLE );
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA1, ENABLE );

    gpio_init.GPIO_Mode = GPIO_Mode_AN;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio_init);

    TIM6_Config( cfg1.freq );
#if ! USE_SINGLE_TIMER
    TIM7_Config( cfg2.freq );
#endif
    DAC_Ch1_Config( cfg1.buf, cfg1.len );
    DAC_Ch2_Config( cfg2.buf, cfg2.len );

    hal_sync();
}


int cmd_fgen( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'f', "freq", "trig_freq", "dma trig freq 100~6000000" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          '2', "channel2", 0, "apply to channel 2" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'm', "mode", "waveform_mode", "sine[10|100]|triangle|random|impulse[10]|sawtooth|half_sine|envelope|shock(10|20|50|100|200)|custom" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'i', "info", 0, "print info" },
        { MCUSH_OPT_NONE }
    };
    mcush_opt_parser parser;
    mcush_opt opt;
    char info=0, channel2=0;
    uint16_t *buf=0;
    int buf_len=1000, need_free=0;
    float freq=-1;

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "mode" ) == 0 )
            {
                if( strcmp( opt.value, "sine" ) == 0 )
                    buf = dat_sine_1000;
                else if( strcmp( opt.value, "sine10" ) == 0 )
                    buf = dat_sine10_1000;
                else if( strcmp( opt.value, "sine100" ) == 0 )
                    buf = dat_sine100_1000;
                else if( strcmp( opt.value, "half_sine" ) == 0 )
                    buf = dat_half_sine_1000;
                else if( strcmp( opt.value, "triangle" ) == 0 )
                    buf = dat_triangle_1000;
                else if( strcmp( opt.value, "sawtooth" ) == 0 )
                    buf = dat_sawtooth_1000;
                else if( strcmp( opt.value, "impulse" ) == 0 )
                    buf = dat_impulse_1000;
                else if( strcmp( opt.value, "impulse10" ) == 0 )
                    buf = dat_impulse10_1000;
                else if( strcmp( opt.value, "envelope" ) == 0 )
                    buf = dat_envelope_1000;
                else if( strcmp( opt.value, "random" ) == 0 )
                    buf = dat_random_1000;
                else if( strcmp( opt.value, "shock10" ) == 0 )
                    buf = dat_shock10_1000;
                else if( strcmp( opt.value, "shock20" ) == 0 )
                    buf = dat_shock20_1000;
                else if( strcmp( opt.value, "shock50" ) == 0 )
                    buf = dat_shock50_1000;
                else if( strcmp( opt.value, "shock100" ) == 0 )
                    buf = dat_shock100_1000;
                else if( strcmp( opt.value, "shock200" ) == 0 )
                    buf = dat_shock200_1000;
                else if( strcmp( opt.value, "custom" ) == 0 )
                {
                    if( !shell_make_16bits_data_buffer( (void*)&buf, &buf_len ) )
                        return 1; 
                    need_free = 1;
                    //shell_printf("customed buffer @ 0x%08X\n", buf);
                } 
                else
                    return 1; 
            }
            else if( strcmp( opt.spec->name, "freq" ) == 0 )
            {
                if( ! shell_eval_float(opt.value, &freq) )
                    return 1;
                if( freq < 100.0 || freq > 6000000 )
                {
                    shell_write_line( "freq out of range" );
                    return 1;
                }
            }
            else if( strcmp( opt.spec->name, "channel2" ) == 0 )
                channel2 = 1;
            else if( strcmp( opt.spec->name, "info" ) == 0 )
                info = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT
    }

    if( (buf==0) && (freq<0) )
        info = 1;  /* if nothing is set, display info */

    if( info )
    {
        shell_printf( "DAC1 0x%08X %d %.1f\n", cfg1.buf, cfg1.len, cfg1.freq );
        shell_printf( "DAC2 0x%08X %d %.1f\n", cfg2.buf, cfg2.len, cfg2.freq );
        return 0;
    }

    if( buf && buf_len )
    {
        if( channel2 )
        {
            if( cfg2.need_free )
            {
                free( cfg2.buf );
                cfg2.buf = 0;
                cfg2.len = 0;
                cfg2.need_free = 0;
            }
            DAC_Ch2_Config( buf, buf_len );
            cfg2.buf = buf;
            cfg2.len = buf_len; 
            cfg2.need_free = need_free;
        }
        else
        {
            if( cfg1.need_free )
            {
                free( cfg1.buf );
                cfg1.buf = 0;
                cfg1.len = 0;
                cfg1.need_free = 0;
            }
            DAC_Ch1_Config( buf, buf_len );
            cfg1.buf = buf;
            cfg1.len = buf_len; 
            cfg1.need_free = need_free;
        }
    }

    if( freq > 0 )
    {
        if( channel2 )
        {
            TIM7_Config( freq );
            //cfg2.freq = freq;
        }
        else
        {
            TIM6_Config( freq );
            //cfg1.freq = freq;
        }
    }
    
    return 0;
}


static const shell_cmd_t cmd_tab[] = {
    {   0,  'f', "fgen",  cmd_fgen,
        "function generator",
        "fgen -m <mode> -f <freq>"
    },
    {   CMD_END  }
};


int main(void)
{
    mcush_init();
    task_blink_init();
    hal_dac_init();
    shell_add_cmd_table( cmd_tab );
    mcush_start();
    while(1);
}


