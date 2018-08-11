#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

#define HAL_LED_NUM   2
#define HAL_LED_PORTS   { 1, 1 }
#define HAL_LED_PINS  { 7, 14 }

/* Hardware connection:
   ----------------------------- 
   (MCU)                  (PC)
   PD9  USART3_TX ------> RXD
   PD8  USART3_RX <-----> TXD 
   ----------------------------- 
 */
#define HAL_UART_DEFINE 
#define HAL_UART_RCC_GPIO_ENABLE_CMD    RCC_AHB1PeriphClockCmd
#define HAL_UART_RCC_GPIO_ENABLE_BIT    RCC_AHB1Periph_GPIOD
#define HAL_UART_RCC_USART_ENABLE_CMD   RCC_APB1PeriphClockCmd
#define HAL_UART_RCC_USART_ENABLE_BIT   RCC_APB1Periph_USART3
#define HAL_UARTx                       USART3
#define HAL_UARTx_TX_PORT               GPIOD
#define HAL_UARTx_TX_PIN                GPIO_Pin_9
#define HAL_UARTx_TX_PINSRC             GPIO_PinSource9
#define HAL_UARTx_RX_PORT               GPIOD
#define HAL_UARTx_RX_PIN                GPIO_Pin_8
#define HAL_UARTx_RX_PINSRC             GPIO_PinSource8
#define HAL_UARTx_AF                    GPIO_AF_USART3
#define HAL_UARTx_IRQn                  USART3_IRQn
#define HAL_UARTx_IRQHandler            USART3_IRQHandler
#define HAL_UARTx_BAUDRATE              9600
#define HAL_UART_QUEUE_RX_LEN           128
#define HAL_UART_QUEUE_TX_LEN           128

#endif
