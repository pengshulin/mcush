#include "hal.h"

void EXTI0_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line0);
}

void RTC_WKUP_IRQHandler (void)
{
    //RTC_ClearITPendingBit(RTC_IT_WUT);
    //EXTI_ClearITPendingBit(EXTI_Line20);
}

void WWDG_IRQHandler(void)
{
}

void USB_HP_CAN1_TX_IRQHandler(void)
{
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
}

void USBWakeUp_IRQHandler(void)
{
}

void USART1_IRQHandler(void)
{
}

void USART2_IRQHandler(void)
{
}

//void USART3_IRQHandler(void)
//{
//}

void TIM1_IRQHandler(void)
{
}

void TIM2_IRQHandler(void)
{
}

void TIM3_IRQHandler(void)
{
}

void TIM4_IRQHandler(void)
{
}

void TIM5_IRQHandler(void)
{
}

void TIM6_IRQHandler(void)
{
}

void TIM7_IRQHandler(void)
{
}

void TIM8_IRQHandler(void)
{
}

void TIM9_IRQHandler(void)
{
}

void TIM10_IRQHandler(void)
{
}

void TIM11_IRQHandler(void)
{
}

void TAMPER_STAMP_IRQHandler(void)
{
}




