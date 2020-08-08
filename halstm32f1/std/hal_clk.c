#include "hal.h"

#if HAL_TEST_CLK_OUTPUT
/* PA8 - MCO */
static void _test_clk_output(void)
{
    GPIO_InitTypeDef init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_StructInit(&init);
    init.GPIO_Pin = GPIO_Pin_8;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &init);
    RCC_MCOConfig(RCC_MCO_SYSCLK);
}
#endif


void hal_clk_init(void)
{  
    RCC_DeInit();
    RCC_ClearFlag();
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR, ENABLE);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    RCC_ClockSecuritySystemCmd(ENABLE);
    RCC_HSEConfig(RCC_HSE_ON);
        
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Disable);
    FLASH_SetLatency(FLASH_Latency_2);

    if( RCC_WaitForHSEStartUp() == SUCCESS )
    {
#if HSE_VALUE == 8000000
    #ifdef RCC_PLLSource_PREDIV1
        RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);   /* 8M / 1 * 9 = 72M */
    #else
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);   /* 8M / 1 * 9 = 72M */
    #endif
#elif HSE_VALUE == 12000000
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);   /* 12M / 1 * 6 = 72M */
#elif HSE_VALUE == 16000000
        RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);   /* 16M / 2 * 9 = 72M */
#else
        #error "HSE_VALUE not defined"
#endif
    }
    else
    {
        RCC_HSEConfig(RCC_HSE_OFF);
        RCC_ClockSecuritySystemCmd(DISABLE);
#ifdef RCC_PLLSource_PREDIV1
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_8);   /* 8M / 2 * 9 = 36M */
#else
        //RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_8);   /* 8M / 2 * 8 = 32M */
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);   /* 8M / 2 * 12 = 48M */
        //RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);   /* 8M / 2 * 16 = 64M (Max) */
#endif
    }
    RCC_PLLCmd(ENABLE);
    while( RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET )
    {
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while( RCC_GetSYSCLKSource() != 0x08 )
    {
    }
    
    SystemCoreClockUpdate();
#if HAL_TEST_CLK_OUTPUT
    _test_clk_output();
#endif

    /* NVIC_PriorityGroup_4: 4 bits for pre-emption priority, 0 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}
