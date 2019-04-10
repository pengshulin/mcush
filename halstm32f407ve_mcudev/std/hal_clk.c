#include "mcush.h"


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


void hal_clk_init(void)
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
        //RCC_PLLConfig(RCC_PLLSource_HSE, HSE_VALUE/1000000, 336, 2, 7);   /* 1M * 336 / 2 = 168M */
        RCC_PLLConfig(RCC_PLLSource_HSE, HSE_VALUE/1000000, 320, 2, 7);   /* 1M * 320 / 2 = 160M */
        //RCC_PLLConfig(RCC_PLLSource_HSE, HSE_VALUE/1000000, 300, 2, 7);   /* 1M * 300 / 2 = 150M */
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
#if HAL_TEST_CLK_OUTPUT
    _test_clk_output();
#endif


    /* Enable CRC module (needed by STemWin) */
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_CRC, ENABLE );
    
    /* Enable ADC & SYSCFG clocks */
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_SYSCFG , ENABLE);

    /* NVIC_PriorityGroup_4: 4 bits for pre-emption priority, 0 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

