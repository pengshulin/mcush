#include "mcush.h"
#include "hal.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_istr.h"


//void USB_HP_CAN1_TX_IRQHandler(void)
//{
//}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    USB_Istr();
    portEND_SWITCHING_ISR( pdTRUE );
}

void USBWakeUp_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line18);
}


