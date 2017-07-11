#include "hal.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

extern uint32_t SystemCoreClock;


extern const uint8_t port_num;
extern const GPIO_TypeDef * const ports[];

sgpio_cfg_t sgpio_cfg;



int hal_sgpio_init(void)
{
    TIM_TimeBaseInitTypeDef timbase;
    TIM_OCInitTypeDef timoc;
   
    memset( &sgpio_cfg, 0, sizeof(sgpio_cfg_t) );
    RCC_APB1PeriphClockCmd(	RCC_APB1Periph_TIM4, ENABLE );
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA1, ENABLE );

    // Tick Timer
    timbase.TIM_CounterMode = TIM_CounterMode_Up;
    timbase.TIM_ClockDivision = TIM_CKD_DIV1;
    timbase.TIM_Prescaler = 0;
    timbase.TIM_Period = 72-1; // default freq: 1MHz
    timbase.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit( TIM4, &timbase );
    
    // Set Compare_x clock for OUT/IN
    timoc.TIM_OCMode = TIM_OCMode_Active;
    timoc.TIM_Pulse = 0;
    timoc.TIM_OutputState = TIM_OutputState_Enable;
    timoc.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init( TIM4, &timoc );
    TIM_OC2Init( TIM4, &timoc );
    //TIM_DMAConfig( TIM4, TIM_DMABase_CCR1, TIM_DMABurstLength_1Transfer );
    //TIM_DMACmd( TIM4, TIM_DMA_CC1, ENABLE );
    //TIM_DMACmd( TIM4, TIM_DMA_CC2, ENABLE );
    return 1;
}


int hal_sgpio_setup( int loop_mode, int port, int output_mode, int input_mode, void *buf_out, void *buf_in, int buf_len, float freq )
{
	DMA_InitTypeDef dma;

    hal_sgpio_stop();
    if( sgpio_cfg.inited )
    {
        if( sgpio_cfg.buf_out );
            free(sgpio_cfg.buf_out);
        if( sgpio_cfg.buf_in );
            free(sgpio_cfg.buf_in);
        sgpio_cfg.buf_out = sgpio_cfg.buf_in = 0;
    }

    sgpio_cfg.loop_mode = loop_mode;
    sgpio_cfg.port = port;
    sgpio_cfg.output_mode = output_mode;
    sgpio_cfg.input_mode = input_mode;
    sgpio_cfg.buf_out = buf_out;
    sgpio_cfg.buf_in = buf_in;
    sgpio_cfg.buf_len = buf_len;
    sgpio_cfg.run = 0;
    sgpio_cfg.inited = 1;

    if( ! buf_len )
        return 0;
    if( !output_mode && !input_mode  )
        return 0;

    if( output_mode )
        hal_gpio_set_output( port, output_mode );
    if( input_mode )
        hal_gpio_set_input( port, input_mode );
    hal_sgpio_set_freq( freq );

	// DMA config
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma.DMA_Priority = DMA_Priority_High;
    dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
    dma.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
    // output
	if( output_mode && buf_out )
    {
	    dma.DMA_PeripheralBaseAddr = (uint32_t)&(ports[sgpio_cfg.port]->ODR);
	    dma.DMA_Memory0BaseAddr = (uint32_t)buf_out;
	    dma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	    dma.DMA_BufferSize = sgpio_cfg.buf_len;
	    dma.DMA_Mode = sgpio_cfg.loop_mode ? DMA_Mode_Circular : DMA_Mode_Normal;
	    dma.DMA_Channel = DMA_Channel_2;
        DMA_ClearFlag( DMA1_Stream0, DMA_FLAG_FEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_TEIF0 );
	    DMA_Init( DMA1_Stream0, &dma );
	    DMA_Cmd( DMA1_Stream0, ENABLE );
        if( DMA_GetCmdStatus( DMA1_Stream0 ) == DISABLE )
            return 0; 
    }

    // input
	if( input_mode && buf_in )
    {
	    dma.DMA_PeripheralBaseAddr = (uint32_t)&(ports[sgpio_cfg.port]->IDR);
	    dma.DMA_Memory0BaseAddr = (uint32_t)buf_in;
	    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
	    dma.DMA_BufferSize = buf_len;
	    dma.DMA_Mode = sgpio_cfg.loop_mode ? DMA_Mode_Circular : DMA_Mode_Normal;
	    dma.DMA_Channel = DMA_Channel_2;
        DMA_ClearFlag( DMA1_Stream3, DMA_FLAG_FEIF3 | DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 );
	    DMA_Init( DMA1_Stream3, &dma );
	    DMA_Cmd( DMA1_Stream3, ENABLE );
        if( DMA_GetCmdStatus( DMA1_Stream3 ) == DISABLE )
            return 0; 
    }

    return 1;
}


int hal_sgpio_set_freq( float freq )
{
    short val;
    float div;

    // modify TIM4 counter to change the frequency
    div = SystemCoreClock / freq;
    if( div < 16384.0 )
    {
    	TIM4->CNT = 0;
    	TIM4->ARR = div-1;
    	TIM_PrescalerConfig( TIM4, 0, TIM_PSCReloadMode_Immediate );
    }
    else
    {
    	val = sqrt( SystemCoreClock / freq );
    	TIM4->CNT = 0;
    	TIM4->ARR = val;
    	TIM_PrescalerConfig( TIM4, val, TIM_PSCReloadMode_Immediate );
    }
    sgpio_cfg.freq = freq;
    return 1;
}


int hal_sgpio_start( void )
{
    if( ! sgpio_cfg.inited )
        return 0; 
	if( ! sgpio_cfg.buf_len )
        return 0;
	
	DMA_Cmd( DMA1_Stream0, ENABLE );
	DMA_Cmd( DMA1_Stream3, ENABLE );
    TIM_Cmd( TIM4, ENABLE );
    TIM_DMACmd( TIM4, TIM_DMA_CC1, ENABLE );
    TIM_DMACmd( TIM4, TIM_DMA_CC2, ENABLE );

    sgpio_cfg.run = 1;
    return 1;
}


void hal_sgpio_stop( void )
{
    if( ! sgpio_cfg.inited )
        return; 

	DMA_Cmd( DMA1_Stream0, DISABLE );
	DMA_Cmd( DMA1_Stream3, DISABLE );
	TIM_Cmd( TIM4, DISABLE );
    TIM_DMACmd( TIM4, TIM_DMA_CC1, DISABLE );
    TIM_DMACmd( TIM4, TIM_DMA_CC2, DISABLE );
    sgpio_cfg.run = 0;
}


sgpio_cfg_t *hal_sgpio_info( void )
{
    return &sgpio_cfg;
}


