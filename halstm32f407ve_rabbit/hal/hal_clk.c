/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"


static void LL_Init(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* System interrupt init*/
    /* MemoryManagement_IRQn interrupt configuration */
    NVIC_SetPriority(MemoryManagement_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
    /* BusFault_IRQn interrupt configuration */
    NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
    /* UsageFault_IRQn interrupt configuration */
    NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
    /* SVCall_IRQn interrupt configuration */
    NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
    /* DebugMonitor_IRQn interrupt configuration */
    NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
    /* PendSV_IRQn interrupt configuration */
    NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
    /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);

    if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_5)
    {
        while(1);//Error_Handler();
    }
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

    LL_RCC_HSE_Enable();

    /* Wait till HSE is ready */
    while(LL_RCC_HSE_IsReady() != 1)
    {

    }

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 336, LL_RCC_PLLP_DIV_2);

    LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 336, LL_RCC_PLLQ_DIV_7);

    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while(LL_RCC_PLL_IsReady() != 1)
    {

    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);

    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {

    }
    LL_Init1msTick(168000000);

    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);

    LL_SetSystemCoreClock(168000000);

    /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
}


void _test_clk_output(void)
{
    LL_GPIO_InitTypeDef gpio_init;

    LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOA );

    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init.Pull = LL_GPIO_PULL_NO;
    gpio_init.Alternate = LL_GPIO_AF_0;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init.Pin = LL_GPIO_PIN_8;
    LL_GPIO_Init( GPIOA, &gpio_init);

    LL_RCC_ConfigMCO( LL_RCC_MCO1SOURCE_HSE, LL_RCC_MCO1_DIV_1 );
}

void hal_clk_init(void)
{
    LL_Init();
    SystemClock_Config();
#if HAL_TEST_CLK_OUTPUT
    _test_clk_output();
#endif
}


/* replace the original implementation of ST/HAL driver */
void HAL_Delay(uint32_t Delay)
{
    hal_delay_ms( Delay );
}


/* replace the original implementation of ST/HAL driver */
uint32_t HAL_GetTick( void )
{
    return xTaskGetTickCount()*1000/configTICK_RATE_HZ;
}
