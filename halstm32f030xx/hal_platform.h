#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

#define HAL_LED_NUM  1
#define HAL_LED_PORTS  { 1 }
#define HAL_LED_PINS  { 1 }


#define HAL_UART_DEFINE  1
#define HAL_UART_RCC_GPIO_ENABLE_CMD    RCC_AHBPeriphClockCmd
#define HAL_UART_RCC_GPIO_ENABLE_BIT    RCC_AHBPeriph_GPIOA
#define HAL_UART_RCC_USART_ENABLE_CMD   RCC_APB2PeriphClockCmd
#define HAL_UART_RCC_USART_ENABLE_BIT   RCC_APB2Periph_USART1
#define HAL_UARTx                       USART1
#define HAL_UARTx_TX_PORT               GPIOA
#define HAL_UARTx_TX_PIN                GPIO_Pin_9
#define HAL_UARTx_TX_PINSRC             GPIO_PinSource9
#define HAL_UARTx_RX_PORT               GPIOA
#define HAL_UARTx_RX_PIN                GPIO_Pin_10
#define HAL_UARTx_RX_PINSRC             GPIO_PinSource10
#define HAL_UARTx_AF                    GPIO_AF_USART1
#define HAL_UARTx_IRQn                  USART1_IRQn
#define HAL_UARTx_IRQHandler            USART1_IRQHandler
#define HAL_UARTx_BAUDRATE              9600
#define HAL_UART_QUEUE_RX_LEN           128
#define HAL_UART_QUEUE_TX_LEN           128

#endif
