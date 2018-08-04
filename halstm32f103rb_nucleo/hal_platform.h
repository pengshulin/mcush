#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__


/* Hardware connection:
   ----------------------------- 
   (MCU)                  (PC)
   PA2  USART2_TX ------> RXD
   PA3  USART2_RX <-----> TXD 
   PA5  LED3
   NOTE: ST-LINK-TX/RX conflicts with PA2/PA3 
   ----------------------------- 
 */
#define HAL_LED_NUM   1
#define HAL_LED_PORTS  { GPIOA }
#define HAL_LED_PINS  { GPIO_Pin_5 }

//#define HAL_UART_DEFINE 
//#define HAL_UART_RCC_GPIO_ENABLE_CMD    RCC_APB2PeriphClockCmd
//#define HAL_UART_RCC_GPIO_ENABLE_BIT    RCC_APB2Periph_GPIOA
//#define HAL_UART_RCC_USART_ENABLE_CMD   RCC_APB1PeriphClockCmd
//#define HAL_UART_RCC_USART_ENABLE_BIT   RCC_APB1Periph_USART2
//#define HAL_UARTx                       USART2
//#define HAL_UARTx_TX_PORT               GPIOA
//#define HAL_UARTx_TX_PIN                GPIO_Pin_2
//#define HAL_UARTx_TX_PINSRC             GPIO_PinSource2
//#define HAL_UARTx_RX_PORT               GPIOA
//#define HAL_UARTx_RX_PIN                GPIO_Pin_3
//#define HAL_UARTx_RX_PINSRC             GPIO_PinSource3
//#define HAL_UARTx_AF                    GPIO_AF_USART2
//#define HAL_UARTx_IRQn                  USART2_IRQn
//#define HAL_UARTx_IRQHandler            USART2_IRQHandler



#endif
