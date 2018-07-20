#include "mcush.h"

const unsigned int baudrate=9600;


void hal_delay_us(uint32_t us)
{
    volatile uint32_t a;
    while(us--)
    {
        for(a=16; a; a--); 
    }
}


void hal_delay_ms(uint32_t ms)
{
    while(ms--)
    {
        hal_delay_us(1000);
    }
}


void hal_delay_10ms(uint32_t ms)
{
    hal_delay_ms( ms*10 );
}



void _test_clk_output(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_MCO);

    //RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_1);
    RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
    RCC_MCO2Config(RCC_MCO2Source_SYSCLK, RCC_MCO2Div_1);
}


void hal_rcc_init(void)
{
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    if( RCC_WaitForHSEStartUp() == SUCCESS )
    {
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR, ENABLE);
        PWR->CR |= PWR_CR_VOS;  
        
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div4);
        RCC_PCLK2Config(RCC_HCLK_Div2);
        RCC_PLLConfig(RCC_PLLSource_HSE, 25, 360, 2, 7);   /* 25M / 25 * 360 / 2 = 180M */
        //RCC_PLLConfig(RCC_PLLSource_HSE, 25, 336, 2, 7);   /* 25M / 25 * 336 / 2 = 168M */
        //RCC_PLLConfig(RCC_PLLSource_HSE, 25, 320, 2, 7);   /* 25M / 25 * 320 / 2 = 160M */
        //RCC_PLLConfig(RCC_PLLSource_HSE, 25, 300, 2, 7);   /* 25M / 25 * 300 / 2 = 150M */
        RCC_PLLCmd(ENABLE);
        while( RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) { }
     
        FLASH_SetLatency(FLASH_Latency_5);
        FLASH_PrefetchBufferCmd(ENABLE);
        FLASH_InstructionCacheCmd(ENABLE);
        FLASH_DataCacheCmd(ENABLE);
        //FLASH_InstructionCacheReset();
        //FLASH_DataCacheReset();

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        while( RCC_GetSYSCLKSource() != RCC_CFGR_SWS_PLL ) { }
    }

    SystemCoreClockUpdate();
    _test_clk_output();

    /* Enable CRC module (needed by STemWin) */
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_CRC, ENABLE );
    
    /* Enable ADC & SYSCFG clocks */
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_SYSCFG , ENABLE);

    /* NVIC_PriorityGroup_4: 4 bits for pre-emption priority, 0 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}


void hal_debug_init(void)
{
}

void hal_reset(void)
{
}


int hal_init(void)
{
    hal_wdg_init();
    hal_rcc_init();
    hal_sdram_init();
    hal_debug_init();
    hal_gpio_init();
    hal_sgpio_init();
    hal_led_init();
#if HAL_RNG
    hal_rng_init();
#endif
#if HAL_RTC
    hal_rtc_init();
#endif
#if HAL_LCD
    hal_lcd_init();
#endif
    if( !hal_uart_init(baudrate) )
        return 0;
    return 1;
}



