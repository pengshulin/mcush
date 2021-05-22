#include "hal.h"

#if HAL_TEST_CLK_OUTPUT
/* PA8 - MCO */
static void _test_clk_output(void)
{
    GPIO_InitTypeDef gpio_init;

    LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_GPIOA );
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //GPIO_StructInit(&gpio_init);
    gpio_init.Pin = GPIO_PIN_8;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pull = GPIO_NOPULL;
    HAL_GPIO_Init( GPIOA, &gpio_init );
    //RCC_MCOConfig(RCC_MCO_SYSCLK);
    __HAL_RCC_MCO1_CONFIG(RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
}
#endif


void hal_clk_init(void)
{  
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* System interrupt init*/
    /* MemoryManagement_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    /* BusFault_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    /* UsageFault_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
    /* SVCall_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
    /* DebugMonitor_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
    /* PendSV_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    /* Disable JTAG, use SWJ only */
    __HAL_AFIO_REMAP_SWJ_NOJTAG();

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
    {
        while(1);  // Error_Handler();  
    }

    LL_RCC_HSE_EnableCSS();
    LL_RCC_HSE_Enable();

    /* Wait till HSE is ready */
    while(LL_RCC_HSE_IsReady() != 1)
    {
    }

#if HSE_VALUE == 8000000
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);   /* 8M / 1 * 9 = 72M */
#elif HSE_VALUE == 12000000
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_6);   /* 12M / 1 * 6 = 72M */
#elif HSE_VALUE == 16000000
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_2, LL_RCC_PLL_MUL_9);   /* 16M / 2 * 9 = 72M */
#else
    #error "HSE_VALUE not defined"
#endif

    LL_RCC_PLL_Enable();

     /* Wait till PLL is ready */
    while(LL_RCC_PLL_IsReady() != 1)
    {
      
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);

    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

     /* Wait till System clock is ready */
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {
    
    }
    LL_Init1msTick(72000000);

    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);

    LL_SetSystemCoreClock(72000000);

    LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL_DIV_1_5);

    /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));

    /* NVIC_PriorityGroup_4: 4 bits for pre-emption priority, 0 bits for subpriority */
    HAL_NVIC_SetPriorityGrouping( NVIC_PRIORITYGROUP_4);

#if HAL_TEST_CLK_OUTPUT
    _test_clk_output();
#endif
}
