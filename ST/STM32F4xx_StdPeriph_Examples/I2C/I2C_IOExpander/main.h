/**
  ******************************************************************************
  * @file    I2C/I2C_IOExpander/main.h 
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
#include "stm324x9i_eval.h"
#include "stm324x9i_eval_lcd.h"
#include "stm324x9i_eval_ioe8.h"
#include "stm324x9i_eval_ioe16.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below if you will use the Buttons in polling mode */
/* #define BUTTON_POLLING_MODE */
/* Uncomment the line below if you will use the Buttons in interrupt mode */
#define BUTTON_INTERRUPT_MODE     

/* Uncomment the line below if you will use the IOE in polling mode */
#define IOE_POLLING_MODE
/* Uncomment the line below if you will use the IOE in interrupt mode */
/* #define IOE_INTERRUPT_MODE */

#ifdef BUTTON_POLLING_MODE
  #define BUTTON_MODE  BUTTON_MODE_GPIO
#else
  #define BUTTON_MODE  BUTTON_MODE_EXTI
#endif 	/* BUTTON_POLLING_MODE */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
