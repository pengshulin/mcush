/**
  ******************************************************************************
  * @file    ADC/ADC_TripleModeInterleaved/main.h 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#if defined (USE_STM324xG_EVAL)
  #include "stm324xg_eval.h"

#elif defined (USE_STM324x7I_EVAL) 
  #include "stm324x7i_eval.h"

#elif defined (USE_STM324x9I_EVAL) 
  #include "stm324x9i_eval.h"

#else
 #error "Please select first the Evaluation board used in your application (in Project Options)"
#endif
     
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#if defined (USE_STM324xG_EVAL)
  #define ADC_CHANNEL              ADC_Channel_12
  #define ADC1_2_CHANNEL_GPIO_CLK  RCC_AHB1Periph_GPIOC
  #define GPIO_PIN                 GPIO_Pin_2
  #define GPIO_PORT                GPIOC
  #define DMA_CHANNELx             DMA_Channel_0
  #define DMA_STREAMx              DMA2_Stream0
  #define ADC_CDR_ADDRESS          ((uint32_t)0x40012308)

#elif defined (USE_STM324x7I_EVAL)
  #define ADC_CHANNEL              ADC_Channel_12
  #define ADC1_2_CHANNEL_GPIO_CLK  RCC_AHB1Periph_GPIOC
  #define GPIO_PIN                 GPIO_Pin_2
  #define GPIO_PORT                GPIOC
  #define DMA_CHANNELx             DMA_Channel_0
  #define DMA_STREAMx              DMA2_Stream0
  #define ADC_CDR_ADDRESS          ((uint32_t)0x40012308)

#else 
  #define ADC_CHANNEL              ADC_Channel_12
  #define ADC1_2_CHANNEL_GPIO_CLK  RCC_AHB1Periph_GPIOC
  #define GPIO_PIN                 GPIO_Pin_2
  #define GPIO_PORT                GPIOC
  #define DMA_CHANNELx             DMA_Channel_0
  #define DMA_STREAMx              DMA2_Stream0
  #define ADC_CDR_ADDRESS          ((uint32_t)0x40012308)

#endif
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
