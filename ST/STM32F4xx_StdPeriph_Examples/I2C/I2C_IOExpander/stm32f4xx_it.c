/**
  ******************************************************************************
  * @file    I2C/I2C_IOExpander/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2C_IOExpander
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s/startup_stm32f429x.s).  */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(Blue); 
  
  if(EXTI_GetITStatus(WAKEUP_BUTTON_EXTI_LINE) != RESET)
  {
    /* Toggle LED3 */
    STM_EVAL_LEDToggle(LED3);

    LCD_DisplayStringLine(Line4, (uint8_t *)"IT:  WAKEUP Pressed       ");

    EXTI_ClearITPendingBit(WAKEUP_BUTTON_EXTI_LINE);
  }
}

/**
  * @brief  This function handles External lines 9 to 5 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
  if(EXTI_GetITStatus(IOE16_IT_EXTI_LINE) != RESET)
  {  
#ifdef IOE_INTERRUPT_MODE 
    __IO uint16_t tmpsr = 0;  
    static JOY_State_TypeDef JoyState = JOY_NONE;
    static TS_STATE* TS_State;
    
    /* Get the interrupt status register */
    tmpsr = IOE16_GetITStatus();
    
    /* Check Touch screen interrupt event occurred */
    if((tmpsr & IOE16_TS_IT) != 0)
    {
      /* Update the structure with the current position */
      TS_State = IOE_TS_GetState();  
    
      if ((TS_State->TouchDetected) && (TS_State->Y < 92) && (TS_State->Y > 52))
      {
        if ((TS_State->X > 60) && (TS_State->X < 120))
        {
          LCD_SetTextColor(LCD_COLOR_GREEN);   
          LCD_DisplayStringLine(LCD_LINE_10, (uint8_t *)"     LED1               ");
          STM_EVAL_LEDOn(LED1);
        }
        else if ((TS_State->X > 140) && (TS_State->X < 200))
        {
          LCD_SetTextColor(LCD_COLOR_YELLOW); 
          LCD_DisplayStringLine(LCD_LINE_10, (uint8_t *)"          LED2          ");
          STM_EVAL_LEDOn(LED2);
        }
        else if ((TS_State->X > 220) && (TS_State->X < 280))
        {
          LCD_SetTextColor(LCD_COLOR_RED); 
          LCD_DisplayStringLine(LCD_LINE_10, (uint8_t *)"               LED3     ");
          STM_EVAL_LEDOn(LED3);
        }     
        else if ((TS_State->X > 300) && (TS_State->X < 360))
        {
          LCD_SetTextColor(LCD_COLOR_BLUE); 
          LCD_DisplayStringLine(LCD_LINE_10, (uint8_t *)"                    LED4");
          STM_EVAL_LEDOn(LED4);
        }
      }
      else
      {
        STM_EVAL_LEDOff(LED1);
        STM_EVAL_LEDOff(LED2);
        STM_EVAL_LEDOff(LED3);
        STM_EVAL_LEDOff(LED4);
      }    
      
      /* Clear the interrupt pending bits */    
      IOE_ClearGITPending(IOE16_TS_IT);     
    }
    
    /* Check joystick interrupt event occurred */
    if((tmpsr & IOE16_JOY_IT) != 0 )
    {
      /* Get the joystick State */
      JoyState = IOE16_JoyStickGetState();
      
      /* Set the LCD Text Color */
      LCD_SetTextColor(Blue); 
  
      switch (JoyState)
      {
        case JOY_NONE:
          LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"  JOY:     ----          ");
          break;
        case JOY_UP:
          LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"  JOY:     UP            ");
          break;     
        case JOY_DOWN:
          LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"  JOY:    DOWN           ");
          break;          
        case JOY_LEFT:
          LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"  JOY:    LEFT           ");
          break;         
        case JOY_RIGHT:
          LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"  JOY:    RIGHT          ");
          break;                 
        case JOY_CENTER:
          LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"  JOY:   CENTER          ");
          break; 
        default:
          LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"  JOY:   ERROR           ");
          break;         
      } 
    }
#endif /* IOE_INTERRUPT_MODE */

    /* Clear all pending bits */
    EXTI_ClearITPendingBit(IOE16_IT_EXTI_LINE);
    IOE_ClearGITPending(ALL_GIT);
    IOE16_GetITStatus();  
  }  
}

/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  /* Set the LCD Text Color */
  LCD_SetTextColor(Blue); 
  
  if(EXTI_GetITStatus(TAMPER_BUTTON_EXTI_LINE) != RESET)
  {
    /* Toggle LED2 */
    STM_EVAL_LEDToggle(LED2);

    LCD_DisplayStringLine(Line4, (uint8_t *)"IT: TAMPER/KEY Pressed       ");
   
    EXTI_ClearITPendingBit(TAMPER_BUTTON_EXTI_LINE);
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
