#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

#define HAL_LED_NUM   2
#define HAL_LED_PORTS   { GPIOB, GPIOB }
#define HAL_LED_PINS  { GPIO_Pin_7, GPIO_Pin_14 }

/* Hardware connection:
   ----------------------------- 
   (MCU)                  (PC)
   PA2  USART2_TX ------> RXD
   PA3  USART2_RX <-----> TXD 
   ----------------------------- 
 */
#define HAL_UART_DEFINE 
#define HAL_UART_RCC_GPIO_ENABLE_CMD    RCC_APB2PeriphClockCmd
#define HAL_UART_RCC_GPIO_ENABLE_BIT    RCC_APB2Periph_GPIOA
#define HAL_UART_RCC_USART_ENABLE_CMD   RCC_APB2PeriphClockCmd
#define HAL_UART_RCC_USART_ENABLE_BIT   RCC_APB2Periph_USART2
#define HAL_USARTx                      USART2
#define HAL_USARTx_TX_PORT              GPIOA
#define HAL_USARTx_TX_PIN               GPIO_Pin_2
#define HAL_USARTx_TX_PINSRC            GPIO_PinSource2
#define HAL_USARTx_RX_PORT              GPIOA
#define HAL_USARTx_RX_PIN               GPIO_Pin_3
#define HAL_USARTx_RX_PINSRC            GPIO_PinSource3
#define HAL_USARTx_AF                   GPIO_AF_USART2
#define HAL_USARTx_IRQn                 USART2_IRQn
#define HAL_USARTx_IRQHandler           USART2_IRQHandler

#endif
