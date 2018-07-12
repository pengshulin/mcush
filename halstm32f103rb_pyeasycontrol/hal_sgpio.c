#include "hal.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

#if USE_CMD_SGPIO

/*
 * TIM1.CC1 -- DMA1.Ch2 -- OUTPUT
 * TIM1.CC2 -- DMA1.CH3 -- INPUT
 */

extern uint32_t SystemCoreClock;


extern const uint8_t port_num;
extern const GPIO_TypeDef * const ports[];

sgpio_cfg_t sgpio_cfg;



int hal_sgpio_init(void)
{
    TIM_TimeBaseInitTypeDef timbase;
    TIM_OCInitTypeDef timoc;
   
    memset( &sgpio_cfg, 0, sizeof(sgpio_cfg_t) );
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_TIM1, ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );

    // Tick Timer
    timbase.TIM_CounterMode = TIM_CounterMode_Up;
    timbase.TIM_ClockDivision = TIM_CKD_DIV1;
    timbase.TIM_Prescaler = 0;
    timbase.TIM_Period = 72-1; // default freq: 1MHz
    timbase.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit( TIM1, &timbase );
    
    // Set Compare_x clock for OUT/IN
    timoc.TIM_OCMode = TIM_OCMode_Active;
    timoc.TIM_Pulse = 0;
    timoc.TIM_OutputState = TIM_OutputState_Enable;
    timoc.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init( TIM1, &timoc );
    TIM_OC2Init( TIM1, &timoc );
    TIM_DMACmd( TIM1, TIM_DMA_CC1, ENABLE );
    TIM_DMACmd( TIM1, TIM_DMA_CC2, ENABLE );
    return 1;
}


int hal_sgpio_setup( int loop_mode, int port, int output_mode, int input_mode, void *buf_out, void *buf_in, int buf_len, float freq )
{
	DMA_InitTypeDef dma;

	TIM_Cmd( TIM1, DISABLE );
	DMA_Cmd( DMA1_Channel2, DISABLE );
	DMA_Cmd( DMA1_Channel3, DISABLE );
	
    if( sgpio_cfg.inited )
    {
        if( sgpio_cfg.buf_out )
            free(sgpio_cfg.buf_out);
        if( sgpio_cfg.buf_in )
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
	dma.DMA_Priority = DMA_Priority_Medium;
	dma.DMA_M2M = DMA_M2M_Disable;
            
    DMA_ClearITPendingBit( DMA_IT_TC );
	
    // output
	if( output_mode && buf_out )
    {
	    dma.DMA_PeripheralBaseAddr = (uint32_t)&(ports[sgpio_cfg.port]->ODR);
	    dma.DMA_MemoryBaseAddr = (uint32_t)buf_out;
	    dma.DMA_DIR = DMA_DIR_PeripheralDST;
	    dma.DMA_BufferSize = sgpio_cfg.buf_len;
	    dma.DMA_Mode = sgpio_cfg.loop_mode ? DMA_Mode_Circular : DMA_Mode_Normal;
        if( ! sgpio_cfg.loop_mode )
        {
            DMA_ITConfig( DMA1_Channel2, DMA_IT_TC, ENABLE );
        }
	    DMA_Init( DMA1_Channel2, &dma );
	    DMA_Cmd( DMA1_Channel2, ENABLE );
    }

    // input
	if( input_mode && buf_in )
    {
	    dma.DMA_PeripheralBaseAddr = (uint32_t)&(ports[sgpio_cfg.port]->IDR);
	    dma.DMA_MemoryBaseAddr = (uint32_t)buf_in;
	    dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	    dma.DMA_BufferSize = buf_len;
	    dma.DMA_Mode = sgpio_cfg.loop_mode ? DMA_Mode_Circular : DMA_Mode_Normal;
        if( ! sgpio_cfg.loop_mode  )
            DMA_ITConfig( DMA1_Channel3, DMA_IT_TC, ENABLE );
        DMA_Init( DMA1_Channel3, &dma );
	    DMA_Cmd( DMA1_Channel3, ENABLE );
    }

    return 1;
}


int hal_sgpio_set_freq( float freq )
{
    short val;
    float div;

    // modify TIM1 counter to change the frequency
    div = SystemCoreClock / freq;
    if( div < 16384.0 )
    {
    	TIM1->CNT = 0;
    	TIM1->ARR = div-1;
    	TIM_PrescalerConfig( TIM1, 0, TIM_PSCReloadMode_Immediate );
    }
    else
    {
    	val = sqrt( SystemCoreClock / freq );
    	TIM1->CNT = 0;
    	TIM1->ARR = val;
    	TIM_PrescalerConfig( TIM1, val, TIM_PSCReloadMode_Immediate );
    }
    sgpio_cfg.freq = freq;
    return 1;
}


int hal_sgpio_start( void )
{
    if( ! sgpio_cfg.inited )
        return 0;

    if( sgpio_cfg.buf_len )
    {
        //DMA_Cmd( DMA1_Channel2, ENABLE );
        //DMA_Cmd( DMA1_Channel3, ENABLE );
        sgpio_cfg.run = 1;
        TIM_Cmd( TIM1, ENABLE );
        return 1;
	}
    return 0;
}


void hal_sgpio_stop( void )
{
    if( ! sgpio_cfg.inited )
        return; 
	//DMA_Cmd( DMA1_Channel2, DISABLE );
	//DMA_Cmd( DMA1_Channel3, DISABLE );
	//DMA_ITConfig( DMA1_Channel2, DMA_IT_TC, DISABLE );
	//DMA_ITConfig( DMA1_Channel3, DMA_IT_TC, DISABLE );
    sgpio_cfg.run = 0;
	TIM_Cmd( TIM1, DISABLE );
}


sgpio_cfg_t *hal_sgpio_info( void )
{
    return &sgpio_cfg;
}

void isr_stop(void)
{
    sgpio_cfg.run = 0;
    DMA_ITConfig( DMA1_Channel2, DMA_IT_TC, DISABLE );
    DMA_ITConfig( DMA1_Channel3, DMA_IT_TC, DISABLE );
}

void DMA1_Channel2_IRQHandler(void)
{
    isr_stop();
}


void DMA1_Channel3_IRQHandler(void)
{
    isr_stop();
}



#endif
